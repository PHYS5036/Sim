// -----------------------------------------------------------------------------
//
// This simulation code is written in C++ to be used with CERN ROOT
// It generates a realistic spectrum of gamma rays for a given source
// It reads in the source gamma data (energy and branching ratio) from 
// a text file and uses a root random number generator (TRandom3) to simulate 
// energy, angle and position of source.
// This data is then filled into a ROOT tree (TTree) for input to Geant4 
//
// P423M Lab - David Hamilton and Cole Cohen
// 
// root -b -q GenerateSpectrum.C++
//
// -----------------------------------------------------------------------------

#include "TRandom3.h"
#include "TMath.h"
#include "TFile.h"
#include "TParameter.h"
#include "TTree.h"
#include "TString.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void GenerateSpectrum( const char* specname = "macros/SOURCE1.txt", 
		       const char* outname  = "~/data/Gen_SOURCE1.root",
		       int   nevents  = 250000, 
		       float distance = 0.0,
		       int detectorType = -1,
		       int sourceHolderType = 0,
		       float detectorDistanceMm = 50.0)
{

  if (nevents <= 0) {
    cerr << "Number of events must be positive" << endl;
    return;
  }

  // Read all gamma lines before creating the output file, so malformed input
  // does not leave behind an apparently valid but empty ROOT file.
  ifstream sourceFile(specname);
  string key;
  char equals;
  int npeaks = 0;
  string energyHeader, branchingHeader;
  if (!(sourceFile >> key >> equals >> npeaks) || key != "npeaks"
      || equals != '=' || npeaks <= 0
      || !(sourceFile >> energyHeader >> branchingHeader)) {
    cerr << "Invalid source spectrum header in " << specname << endl;
    return;
  }

  vector<float> energies(npeaks);
  vector<float> branchingRatios(npeaks);
  double totalBranchingRatio = 0.0;
  for (int i = 0; i < npeaks; ++i) {
    if (!(sourceFile >> energies[i] >> branchingRatios[i])
        || !isfinite(energies[i]) || energies[i] <= 0.0f
        || !isfinite(branchingRatios[i]) || branchingRatios[i] < 0.0f) {
      cerr << "Invalid gamma line " << i + 1 << " in " << specname << endl;
      return;
    }
    totalBranchingRatio += branchingRatios[i];
  }
  if (!(totalBranchingRatio > 0.0) || !isfinite(totalBranchingRatio)) {
    cerr << "Source spectrum has no valid gamma emission probability: "
         << specname << endl;
    return;
  }

  // Values may represent total emission probabilities below one or photon
  // yields above one. Preserve the existing per-decay event-yield behavior.
  if (totalBranchingRatio > 1.0) {
    nevents = static_cast<int>(nevents * totalBranchingRatio);
  }

  //  -----------------------------------------------------------------------------
  // Define output variables, open root file and set-up a tree for output
  //-----------------------------------------------------------------------------

  float     fVx = 0.0;         // x position of gamma source in cm
  float     fVy = 0.0;         // y position of gamma source in cm
  float     fVz = distance;   // z position of gamma source in cm

  float     fGammaPx;          // x component of gamma momentum in MeV
  float     fGammaPy;          // y component of gamma momentum in MeV
  float     fGammaPz;          // z component of gamma momentum in MeV
  float     fGammaEn;          // gamma-ray energy in MeV
  
  TFile* fROOTFile = new TFile( outname,"RECREATE" );
  TTree* fROOTTree = new TTree( "h1", "Generator Output Tree G4" );
  int    fBasket   = 64000;
    
  fROOTTree->Branch("X_vtx",   &fVx,        "X_vtx/F",   fBasket );
  fROOTTree->Branch("Y_vtx",   &fVy,        "Y_vtx/F",   fBasket );
  fROOTTree->Branch("Z_vtx",   &fVz,        "Z_vtx/F",   fBasket );
  
  fROOTTree->Branch("Px_Gam",  &fGammaPx,    "Px_Gam/F", fBasket );
  fROOTTree->Branch("Py_Gam",  &fGammaPy,    "Py_Gam/F", fBasket );
  fROOTTree->Branch("Pz_Gam",  &fGammaPz,    "Pz_Gam/F", fBasket );
  fROOTTree->Branch("En_Gam",  &fGammaEn,    "En_Gam/F", fBasket );

  // -----------------------------------------------------------------------------
  // Initialise random number generator
  // -----------------------------------------------------------------------------

  TRandom3* fRand = new TRandom3(-1);

  // -----------------------------------------------------------------------------  
  // Loop over nevents and generate photons by MC methods
  // -----------------------------------------------------------------------------
  
  for( int i =0; i < nevents; i++ ) {
    
       if( i % 10000 == 0 )
           cout << "Event = " << i << endl;
    
    // Randomly generate angular distributions (in polar coordinates)
    float ph1     = fRand->Uniform( -TMath::Pi(), TMath::Pi() );
    
    // Want to boost statistics for attenuation experiment
  

    float costh  = fRand->Uniform( -1, 1 ); 
    float th     = TMath::ACos( costh );

    const double branchSeed = fRand->Uniform(
        0.0, totalBranchingRatio < 1.0 ? 1.0 : totalBranchingRatio);
    double cumulativeBranchingRatio = 0.0;
    bool selectedGammaLine = false;
    for (int line = 0; line < npeaks; ++line) {
      cumulativeBranchingRatio += branchingRatios[line];
      if (branchSeed < cumulativeBranchingRatio) {
        fGammaEn = energies[line];
        selectedGammaLine = true;
        break;
      }
    }
    if (!selectedGammaLine) continue;

    // Set Cartesian momentum components
    fGammaPx = fGammaEn * TMath::Sin( th ) * TMath::Cos( ph1 );
    fGammaPy = fGammaEn * TMath::Sin( th ) * TMath::Sin( ph1 );
    fGammaPz = fGammaEn * TMath::Cos( th );
    
    // Fill output tree for this event
    fROOTTree->Fill();
  }

  // Filter the completed event list by its projected intersection with the
  // detector housing. The housing envelope is intentionally larger than the
  // active crystal, allowing for interactions in the detector casing.
  Long64_t generatedEntries = fROOTTree->GetEntries();
  Long64_t transportedEntries = generatedEntries;
  TTree* outputTree = fROOTTree;

  if (detectorType == 0 || detectorType == 1) {
    const double holderOffsetMm = sourceHolderType == 0 ? 12.0 : 2.0;
    const double detectorFrontZMm = holderOffsetMm + detectorDistanceMm;
    const double housingRadiusMm = detectorType == 0 ? 14.0 : 28.596;
    const double crystalMarginMm = detectorType == 0 ? 5.0 : 10.0;
    const double acceptanceRadiusMm = housingRadiusMm + crystalMarginMm;
    const double sourceZMm = distance * 10.0;

    fROOTTree->SetName("h1_unfiltered");
    outputTree = fROOTTree->CloneTree(0);
    outputTree->SetName("h1");

    for (Long64_t entry = 0; entry < generatedEntries; ++entry) {
      fROOTTree->GetEntry(entry);

      const double px = fGammaPx;
      const double py = fGammaPy;
      const double pz = fGammaPz;
      const double dzMm = detectorFrontZMm - sourceZMm;
      if (pz <= 0.0 || dzMm <= 0.0) continue;

      const double distanceAlongRay = dzMm / pz;
      const double hitXmm = fVx * 10.0 + px * distanceAlongRay;
      const double hitYmm = fVy * 10.0 + py * distanceAlongRay;
      if (hitXmm * hitXmm + hitYmm * hitYmm
          <= acceptanceRadiusMm * acceptanceRadiusMm) {
        outputTree->Fill();
      }
    }

    transportedEntries = outputTree->GetEntries();
    cout << "Geometric pre-transport cut retained " << transportedEntries
         << " of " << generatedEntries << " generated gamma events." << endl;
  }

  // These counts let downstream absolute-efficiency analyses use the full
  // generated population as the denominator after transport has been reduced.
  fROOTFile->cd();
  outputTree->Write("h1");
  TParameter<Long64_t>("GeneratedEventCount", generatedEntries).Write();
  TParameter<Long64_t>("TransportedEventCount", transportedEntries).Write();
  fROOTFile->Close();
}

// -----------------------------------------------------------------------------
