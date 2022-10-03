#include "AnalysisManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "AnalysisMessenger.hh"

#include "G4UnitsTable.hh"
#include "G4RunManager.hh"
#include "G4Point3D.hh"
#include "G4Transform3D.hh"

// #include "TApplication.h"
// #include "TFile.h"
// #include "TSystem.h"
// #include "TTree.h"

//---------------------------------------------------------------------------

AnalysisManager::AnalysisManager()
{
  ZeroArray();

  fOutFileName = G4String("output/out_default.csv");

  fAnaMessenger = new AnalysisMessenger(this);
}

//---------------------------------------------------------------------------

AnalysisManager::~AnalysisManager()
{
//    fROOTtree->Write();
//    fROOTfile->Close();
}

//---------------------------------------------------------------------------

void AnalysisManager::InitOutput()
{ 

  

//   fROOTfile = new TFile(fOutFileName,"RECREATE","fROOTfile",1);
//   fROOTtree = new TTree("T","Output Tree");
//   fROOTtree->SetAutoSave();

//   // Set Primary Branches
//   fROOTtree->Branch("Prim_E",      &fPEne,   "Prim_E/F"      );
//   fROOTtree->Branch("Prim_Th",     &fPth,    "Prim_Th/F"     );
//   fROOTtree->Branch("Prim_Ph",     &fPph,    "Prim_Ph/F"     );
//   fROOTtree->Branch("Prim_pdg",    &fPpdg,   "Prim_pdg/I"    );
  
//   // Set Raw Phantom Step Hit Branches
//   fROOTtree->Branch("Phantom_Nhits", &fRAW_Nhits, "Phantom_Nhits/I");  
//   fROOTtree->Branch("Phantom_pdg",   fRAW_pdg,    "Phantom_pdg[Phantom_Nhits]/I");
//   fROOTtree->Branch("Phantom_id",    fRAW_id,     "Phantom_id[Phantom_Nhits]/I");
//   fROOTtree->Branch("Phantom_x",     fRAW_xpre,   "Phantom_x[Phantom_Nhits]/F"  );
//   fROOTtree->Branch("Phantom_y",     fRAW_ypre,   "Phantom_y[Phantom_Nhits]/F"  );
//   fROOTtree->Branch("Phantom_z",     fRAW_zpre,   "Phantom_z[Phantom_Nhits]/F"  );
//   fROOTtree->Branch("Phantom_Ed",    fRAW_Edep,   "Phantom_Ed[Phantom_Nhits]/F" );
  
}

//---------------------------------------------------------------------------

void AnalysisManager::ZeroArray()
{
  // Primary
  G4ThreeVector zero(0.,0.,0.);
  fPEne   = 9999;
  fPdir   = (zero);
  fPth    = 9999;
  fPph    = 9999;
  fPTime  = 9999;
  fPPDef  = NULL;
  fPpdg   = 9999;

  // Raw Hits
  fRAW_Nhits  = 0;
  fSteppdef    = NULL;
  fStepp3      = (zero);
  fSteppospre  = (zero);
  fSteppospost = (zero);
  fStepid      = 0;
  fSteptime    = 0;
  fStepedep    = 0;

  for ( int i = 0; i < fMaxhits; i++ ) {
    fRAW_id[i]      = 9999;  
    fRAW_time[i]    = 9999;
    fRAW_Edep[i]    = 9999;
    fRAW_pdg[i]     = 9999;
    fRAW_mass[i]    = 9999;
    fRAW_mom[i]     = 9999;
    fRAW_px[i]      = 9999;
    fRAW_py[i]      = 9999;
    fRAW_pz[i]      = 9999;
    fRAW_xpre[i]    = 9999;
    fRAW_ypre[i]    = 9999;
    fRAW_zpre[i]    = 9999;
    fRAW_xpost[i]   = 9999;
    fRAW_ypost[i]   = 9999;
    fRAW_zpost[i]   = 9999;
    fRAW_Energy[i]  = 9999;
  }
}

//---------------------------------------------------------------------------

void AnalysisManager::FillArray( int hitn ) 
{
    fRAW_Nhits++;
    fRAW_id[hitn]     = (int)fStepid;
    fRAW_pdg[hitn]    = (int)fSteppdef->GetPDGEncoding();
    fRAW_mass[hitn]   = (float)fSteppdef->GetPDGMass();
    fRAW_time[hitn]   = (float)fSteptime;                                   
    fRAW_mom[hitn]    = (float)fStepp3.mag();                             
    fRAW_px[hitn]     = (float)fStepp3.getX();                             
    fRAW_py[hitn]     = (float)fStepp3.getY();                             
    fRAW_pz[hitn]     = (float)fStepp3.getZ();                             
    fRAW_xpre[hitn]   = (float)fSteppospre.getX();                             
    fRAW_ypre[hitn]   = (float)fSteppospre.getY();                             
    fRAW_zpre[hitn]   = (float)fSteppospre.getZ();                             
    fRAW_xpost[hitn]  = (float)fSteppospost.getX();                             
    fRAW_ypost[hitn]  = (float)fSteppospost.getY();                             
    fRAW_zpost[hitn]  = (float)fSteppospost.getZ();                             
    fRAW_Edep[hitn]   = (float)fStepedep;
    fRAW_Energy[hitn] = sqrt( fRAW_mom[hitn]*fRAW_mom[hitn] 
				     + fRAW_mass[hitn]*fRAW_mass[hitn] );

    fRAW_xpre[hitn]   = (fRAW_xpre[hitn] + fRAW_xpost[hitn])/2.;
    fRAW_ypre[hitn]   = (fRAW_ypre[hitn] + fRAW_ypost[hitn])/2.;
    fRAW_zpre[hitn]   = (fRAW_zpre[hitn] + fRAW_zpost[hitn])/2.;

}

//---------------------------------------------------------------------------

void AnalysisManager::FillTree()
{
  // Primary Variables
  fPTime  = (float)fPTime;
  fPth    = (float)fPdir.getTheta();                         
  fPph    = (float)fPdir.getPhi();                                                      
  fPEne   = (float)fPEne;                         
  fPpdg   = (int)  fPPDef->GetPDGEncoding();
  
//   fROOTtree->Fill();
}

//---------------------------------------------------------------------------
