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
#include "TTree.h"
#include "TString.h"

#include <iostream>
using namespace std;

void GenerateSpectrum( const char* specname = "macros/SOURCE1.txt", 
		       const char* outname  = "~/data/Gen_SOURCE1.root",
		       int   nevents  = 250000, 
		       float distance = 5.0 )
{

  //  -----------------------------------------------------------------------------
  // Define output variables, open root file and set-up a tree for output
  //-----------------------------------------------------------------------------

  float     fVx = 0.0;         // x position of gamma source in cm
  float     fVy = -1.0;         // y position of gamma source in cm
  float     fVz = -distance;   // z position of gamma source in cm

  float     fGammaPx;          // x component of gamma momentum in MeV
  float     fGammaPy;          // y component of gamma momentum in MeV
  float     fGammaPz;          // z component of gamma momentum in MeV
  float     fGammaEn;          // gamma-ray energy in MeV
  
  TFile* fROOTFile = new TFile( outname,"RECREATE" );
  TTree* fROOTTree = new TTree( "h1", "Generator Output Tree G4" );
  int    fBasket   = 64000;
  fROOTTree->SetAutoSave();
    
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
  // Read input file, get the number of peaks, then energies and branching ratios
  // -----------------------------------------------------------------------------

  FILE *fp = fopen ( specname , "r" );
  if( !fp) {
    cerr << "Input file " << specname << " not found" << endl;
    return;
  }

  int   npeaks;
  char* dummy = new char[100];;  
  fscanf(fp , "%s = %d" , dummy, &npeaks);
  float Epk[npeaks], BRpk[npeaks];
  fscanf(fp, "%s\t%s" , dummy, dummy);
  for (int i=0; i<npeaks; i++)
    fscanf(fp, "%f\t%f" , &Epk[i], &BRpk[i] );

  fclose( fp );

  if( (BRpk[0] + BRpk[1]) > 1 )
    nevents = (Int_t)(nevents * (BRpk[0]+BRpk[1]));
   
  // -----------------------------------------------------------------------------  
  // Loop over nevents and generate photons by MC methods
  // -----------------------------------------------------------------------------
  
  for( int i =0; i < nevents; i++ ) {
    
       if( i % 10000 == 0 )
           cout << "Event = " << i << endl;
    
    // Randomly generate angular distributions (in polar coordinates)
    float ph1     = fRand->Uniform( -TMath::Pi(), TMath::Pi() );
    float costh  = fRand->Uniform( -1, 1 );
    float th     = TMath::ACos( costh );

    if( (BRpk[0] + BRpk[1]) < 1 ) {
      // Randomly generate photon energy spectrum from read-in energies and branching ratios
      float branchseed = fRand->Uniform ( 0., 1.);
      if( branchseed < BRpk[0] )
	fGammaEn = Epk[0];
      else if( branchseed > BRpk[0] && branchseed < (BRpk[0]+BRpk[1]) )
	fGammaEn = Epk[1];
      else
	continue;
    }
    else {
      float branchseed = fRand->Uniform ( 0., (BRpk[0]+BRpk[1]) );
      if( branchseed < BRpk[0] )
	fGammaEn = Epk[0];
      else if( branchseed > BRpk[0] && branchseed < (BRpk[0]+BRpk[1]) )
	fGammaEn = Epk[1];      
    }

    // Set Cartesian momentum components
    fGammaPx = fGammaEn * TMath::Sin( th ) * TMath::Cos( ph1 );
    fGammaPy = fGammaEn * TMath::Sin( th ) * TMath::Sin( ph1 );
    fGammaPz = fGammaEn * TMath::Cos( th );
    
    // Fill output tree for this event
    fROOTTree->Fill();
  }

  // -----------------------------------------------------------------------------  
  // Write output tree to file
  // -----------------------------------------------------------------------------  

  fROOTTree->Write();
  fROOTFile->Close();
}

// -----------------------------------------------------------------------------

