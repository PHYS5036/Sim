// -----------------------------------------------------------------------------
//
// This simulation code is written in C++ to be used with CERN ROOT
// It reads in the data produced by the geant4 simulation from the 
// root file (TFile)
// Then it gets the root tree (TTree) in which the data is stored 
// and loops over all events
// For each event where energy was deposited in the detector, the 
// energy deposit is smeared by a root random number generator (TRandom3) 
// according to the resolution function:
// resolution = k * Energy^n, where k and n were obtained from the data
// It outputs the smeared spectrum as a root file so that it can be analysed
//
// P423M Lab - David Hamilton
//
// root -b -q SmearSpectrum.C++
//
// -----------------------------------------------------------------------------

#include "TRandom3.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TStyle.h"

#include <iostream>
#include <fstream>
using namespace std;

string home=getenv("OUTPUT_DIR");
void SmearSpectrum( string finname  = home+"/G4_SOURCE1.root", 
		    float resconst = 3.3,
		    float respower = -0.5)
// Resolution parameters from fit to NaI data: resconst = 3.3, respower = -0.5
// Resolution parameters from fit to GAGG data: resconst = ?, respower = -0.8
{

  // -----------------------------------------------------------------------------
  // Open root file, get root tree and associate local and tree variables
  // -----------------------------------------------------------------------------
  
  TFile *fin = new TFile( finname.c_str() );
  TTree* T   = (TTree*)fin->Get("T");
  
  //Declaration of leaves types
  Float_t         Prim_E;
  Float_t         Prim_Th;
  Float_t         Prim_Ph;
  Float_t         Prim_t;
  Int_t           Prim_pdg;
  Int_t           Detector_Nhits;
  Float_t         Detector_t[1];
  Int_t           Detector_pdg[1];
  Float_t         Detector_E[1];
  Float_t         Detector_Ed[1];

  // Set branch addresses.
  T->SetBranchAddress("Prim_E",&Prim_E);
  T->SetBranchAddress("Prim_Th",&Prim_Th);
  T->SetBranchAddress("Prim_Ph",&Prim_Ph);
  T->SetBranchAddress("Prim_t",&Prim_t);
  T->SetBranchAddress("Prim_pdg",&Prim_pdg);
  T->SetBranchAddress("Detector_Nhits",&Detector_Nhits);
  T->SetBranchAddress("Detector_t",Detector_t);
  T->SetBranchAddress("Detector_pdg",Detector_pdg);
  T->SetBranchAddress("Detector_E",Detector_E);
  T->SetBranchAddress("Detector_Ed",Detector_Ed);
  
  Long64_t nentries = T->GetEntries();

  // -----------------------------------------------------------------------------
  // Open root file, get root tree and associate local and tree variables
  // -----------------------------------------------------------------------------
  
  TRandom3* fRand = new TRandom3(-1);
  TH1F*     hE    = new TH1F("hE","", 512, 0, 1500 );

  for ( Long64_t i = 0; i < nentries; i++ ) {

    T->GetEntry(i);                       // get the ith detector hit
    if( Detector_Ed[0] == 0 ) continue;   // ignore hits that deposit no energy
    
    float gam_energy = Detector_Ed[0] * 1000; //convert to keV
    float resolution = resconst * TMath::Power( gam_energy, respower );
    float fwhm       = resolution * gam_energy;
    float sigma      = fwhm/2.35;
    
    gam_energy   += fRand->Gaus( 0.0, sigma );  // do the smearing
    hE->Fill( gam_energy );              // fill histogram
  }

  // -----------------------------------------------------------------------------
  // Use the histogram of smeared energy to create a simulated root file
  // -----------------------------------------------------------------------------

  gStyle->SetCanvasColor(0);
  gStyle->SetFrameFillColor(0);

  gStyle->SetPadTopMargin(.05);
  gStyle->SetPadLeftMargin(.15);
  gStyle->SetPadRightMargin(.05);
  gStyle->SetPadBottomMargin(.15);

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetStripDecimals(kFALSE);

  TCanvas* c2 = new TCanvas( "c2", "", 800, 600 );

  hE->Draw();
  hE->GetYaxis()->SetTitle("Number of Counts");
  hE->GetXaxis()->SetTitle("Energy (keV)");
  Int_t lastBin = hE->FindLastBinAbove(0);
  Double_t maxEnergy = hE->GetBinLowEdge(lastBin + 1);
  hE->GetXaxis()->SetRangeUser(0, maxEnergy);

  string s1 = (string)finname;
  
  // Extract just the filename without path
  size_t lastSlash = s1.find_last_of("/");
  string filename = (lastSlash != string::npos) ? s1.substr(lastSlash + 1) : s1;
  
  // Remove extension and create output names in home directory
  string basename = filename.substr(0, filename.find(".", 0));
  string outname = home + "/" + basename + ".pdf";
  string outname1 = home + "/" + basename + ".Spe";
  string bgname = home + "/Bg_Sim.Spe";
  
  ofstream obfile, osfile;
  obfile.open(bgname.c_str());
  osfile.open(outname1.c_str());

  cout << "PDF output file saved to: " << outname << endl;
  cout << "Spe output file saved to: " << outname1 << endl;
  cout << "Background file saved to: " << bgname << endl;

  cout << outname1.c_str() << endl;
  for ( Int_t j = 0; j < 512+12+14; j++ ) {  
    if( j < 7 ) {
      obfile << "dummy" << endl;
      osfile << "dummy" << endl;
    }
	else if( j == 7 ) {
      obfile << "$MEAS_TIM:" << endl;
      osfile << "$MEAS_TIM:" << endl;
	}
	else if( j == 8 ) {
      obfile << "600 600" << endl;
      osfile << "600 600" << endl;
    }
    else if( j == 9 ) {
      obfile << "$DATA:" << endl;
      osfile << "$DATA:" << endl;
    }
	else if ( j==10 ) {
      obfile << "0 511" << endl;
      osfile << "0 511" << endl;
	}
    else if( j > (511+12) ) {
      obfile << "dummy" << endl;
      osfile << "dummy" << endl;
    }
    else {
      obfile << "0" << endl;
      osfile << hE->GetBinContent(j-11) << endl;
    }
  }

  obfile.close();
  osfile.close();

  c2->Print(outname.c_str());

}
