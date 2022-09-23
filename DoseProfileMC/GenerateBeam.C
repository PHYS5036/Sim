// -----------------------------------------------------------------------------
//
// This simulation code is written in C++ to be used with CERN ROOT
// It generates a realistic beam of particles.
// It uses a root random number generator (TRandom3) to simulate 
// energy, angle and position for a beam of the users choosing.
// This data is then filled into a ROOT tree (TTree) for input to Geant4 
//
// P423M Lab - David Hamilton
// 
// root -b -q GenerateBeam.C++
//
// -----------------------------------------------------------------------------

#include "TRandom3.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include <iostream>
using namespace std;

void GenerateBeam( const char* particle = "BEAMX", 
		   float energy  = 70.,
		   int   nevents = 10000,
		   const char* outname  = "~/data/Gen_BEAMX_ENERGYY.root")
{

  // -----------------------------------------------------------------------------
  // Define output variables, open root file and set-up a tree for output
  // -----------------------------------------------------------------------------

  float fVx;  // x position of beam origin in cm
  float fVy;  // y position of beam origin in cm
  float fVz;  // z position of beam origin in cm
  float fPx;  // x component of beam momentum in MeV
  float fPy;  // y component of beam momentum in MeV
  float fPz;  // z component of beam momentum in MeV
  float fEn;  // beam energy in MeV
  int   fPDG; // unique PDG code for particle type

  TFile* fROOTFile = new TFile( outname,"RECREATE" );
  TTree* fROOTTree = new TTree( "h1", "Generator Output Tree G4" );
  int    fBasket   = 64000;
  fROOTTree->SetAutoSave();
    
  fROOTTree->Branch("X_vtx",   &fVx, "X_vtx/F",   fBasket );
  fROOTTree->Branch("Y_vtx",   &fVy, "Y_vtx/F",   fBasket );
  fROOTTree->Branch("Z_vtx",   &fVz, "Z_vtx/F",   fBasket );
  
  fROOTTree->Branch("Px_Gam",  &fPx,  "Px_Gam/F", fBasket );
  fROOTTree->Branch("Py_Gam",  &fPy,  "Py_Gam/F", fBasket );
  fROOTTree->Branch("Pz_Gam",  &fPz,  "Pz_Gam/F", fBasket );
  fROOTTree->Branch("En_Gam",  &fEn,  "En_Gam/F", fBasket );

  fROOTTree->Branch("PDG",     &fPDG, "PDG/I",    fBasket );

  // -----------------------------------------------------------------------------
  // Initialise random number generator
  // -----------------------------------------------------------------------------

  TRandom3* fRand = new TRandom3(-1);
  float spread = 1.0;
  const char* orientation = "top"; 

  // -----------------------------------------------------------------------------  
  // Loop over nevents and generate photons by MC methods
  // -----------------------------------------------------------------------------
  
  for( int i =0; i < nevents; i++ ) {

    // -----------------------------------------------------------------------------
    // Assign Variables according to user-entered parameters
    // -----------------------------------------------------------------------------
    
    if( strcmp( particle, "gamma") == 0 )
      fPDG = 22;
    else if( strcmp( particle, "electron") == 0  )
      fPDG = 11;
    else if( strcmp( particle, "proton") == 0 )
      fPDG = 2212;
    else if( strcmp( particle, "ion") == 0) {
      fPDG = 150000;
    }
    else 
      cerr << "ERROR: unknown beam particle" << endl;
    
    // -----------------------------------------------------------------------------
    
    fVx = 0.0; 
    fVy = 0.0; 
    fVz = 0.0; 
    fPx = 0.0; 
    fPy = 0.0; 
    fPz = 0.0; 
    fEn = 0.0;
    
    if( strcmp( orientation, "back" ) == 0 ) {
      fVz = -100.0;
      fPx = 0.0;
      fPy = 0.0;
      fPz = 1.0;
    }
    else if( strcmp( orientation, "top" ) == 0 ) {
      fVy = 100.0; 
      fPx = 0.0;
      fPy = -1.0;
      fPz = 0.0;
    }
    else if( strcmp( orientation, "front" ) == 0 ) {
      fVz = 100.0; 
      fPx = 0.0;
      fPy = 0.0;
      fPz = -1.0;
    }
    else if( strcmp( orientation, "bottom" ) == 0 ) {
      fVy = -100.0; 
      fPx = 0.0;
      fPy = 1.0;
      fPz = 0.0;
    }
    else 
      cerr << "ERROR: unknown beam orientation" << endl;
    
    if( i % 10000 == 0 )
      cout << "Event = " << i << endl;
    
    // Set beam vertex to reflect spread
    float halfs = spread/2.;
    if( fVz != 0.0 ) {  // horizontal beam
      fVx = fRand->Uniform( -halfs, halfs ); 
      fVy = fRand->Uniform( -halfs, halfs ); 
    }
    else {  // horizontal beam
      fVx = fRand->Uniform( -halfs, halfs ); 
      fVz = fRand->Uniform( -halfs, halfs ); 
    }

    // Set momentum components
    fEn = energy;
    fPx = fPx*fEn; 
    fPy = fPy*fEn;  
    fPz = fPz*fEn;  
    
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

