#include "AnalysisManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "AnalysisMessenger.hh"

#include "G4UnitsTable.hh"
#include "G4RunManager.hh"
#include "G4Point3D.hh"
#include "G4Transform3D.hh"

#include "TROOT.h"
#include "TApplication.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TString.h"
#include "TMath.h"

//---------------------------------------------------------------------------

AnalysisManager::AnalysisManager()
{
  ZeroArray();

  fOutFileName = TString("output/out_default.root");

  fAnaMessenger = new AnalysisMessenger(this);
}

//---------------------------------------------------------------------------

AnalysisManager::~AnalysisManager()
{
   fROOTtree->Write();
   fROOTfile->Close();
}

//---------------------------------------------------------------------------

void AnalysisManager::InitOutput()
{ 

  fROOTfile = new TFile(fOutFileName,"RECREATE","fROOTfile",1);
  fROOTtree = new TTree("T","Output Tree");
  fROOTtree->SetAutoSave();

  // Set Primary Branches
  fROOTtree->Branch("Prim_E",      &fPEne,   "Prim_E/F"      );
  fROOTtree->Branch("Prim_Th",     &fPth,    "Prim_Th/F"     );
  fROOTtree->Branch("Prim_Ph",     &fPph,    "Prim_Ph/F"     );
  fROOTtree->Branch("Prim_pdg",    &fPpdg,   "Prim_pdg/I"    );
  
  // Set Raw Phantom Step Hit Branches
  fROOTtree->Branch("Phantom_Nhits", &fRAW_Nhits, "Phantom_Nhits/I");  
  fROOTtree->Branch("Phantom_pdg",   fRAW_pdg,    "Phantom_pdg[Phantom_Nhits]/I");
  fROOTtree->Branch("Phantom_id",    fRAW_id,     "Phantom_id[Phantom_Nhits]/I");
  fROOTtree->Branch("Phantom_x",     fRAW_xpre,   "Phantom_x[Phantom_Nhits]/F"  );
  fROOTtree->Branch("Phantom_y",     fRAW_ypre,   "Phantom_y[Phantom_Nhits]/F"  );
  fROOTtree->Branch("Phantom_z",     fRAW_zpre,   "Phantom_z[Phantom_Nhits]/F"  );
  fROOTtree->Branch("Phantom_Ed",    fRAW_Edep,   "Phantom_Ed[Phantom_Nhits]/F" );
  
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

  for ( Int_t i = 0; i < fMaxhits; i++ ) {
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

void AnalysisManager::FillArray( Int_t hitn ) 
{
    fRAW_Nhits++;
    fRAW_id[hitn]     = (Int_t)fStepid;
    fRAW_pdg[hitn]    = (Int_t)fSteppdef->GetPDGEncoding();
    fRAW_mass[hitn]   = (Float_t)fSteppdef->GetPDGMass();
    fRAW_time[hitn]   = (Float_t)fSteptime;                                   
    fRAW_mom[hitn]    = (Float_t)fStepp3.mag();                             
    fRAW_px[hitn]     = (Float_t)fStepp3.getX();                             
    fRAW_py[hitn]     = (Float_t)fStepp3.getY();                             
    fRAW_pz[hitn]     = (Float_t)fStepp3.getZ();                             
    fRAW_xpre[hitn]   = (Float_t)fSteppospre.getX();                             
    fRAW_ypre[hitn]   = (Float_t)fSteppospre.getY();                             
    fRAW_zpre[hitn]   = (Float_t)fSteppospre.getZ();                             
    fRAW_xpost[hitn]  = (Float_t)fSteppospost.getX();                             
    fRAW_ypost[hitn]  = (Float_t)fSteppospost.getY();                             
    fRAW_zpost[hitn]  = (Float_t)fSteppospost.getZ();                             
    fRAW_Edep[hitn]   = (Float_t)fStepedep;
    fRAW_Energy[hitn] = TMath::Sqrt( fRAW_mom[hitn]*fRAW_mom[hitn] 
				     + fRAW_mass[hitn]*fRAW_mass[hitn] );

    fRAW_xpre[hitn]   = (fRAW_xpre[hitn] + fRAW_xpost[hitn])/2.;
    fRAW_ypre[hitn]   = (fRAW_ypre[hitn] + fRAW_ypost[hitn])/2.;
    fRAW_zpre[hitn]   = (fRAW_zpre[hitn] + fRAW_zpost[hitn])/2.;

}

//---------------------------------------------------------------------------

void AnalysisManager::FillTree()
{
  // Primary Variables
  fPTime  = (Float_t)fPTime;
  fPth    = (Float_t)fPdir.getTheta();                         
  fPph    = (Float_t)fPdir.getPhi();                                                      
  fPEne   = (Float_t)fPEne;                         
  fPpdg   = (Int_t)  fPPDef->GetPDGEncoding();
  
  fROOTtree->Fill();
}

//---------------------------------------------------------------------------
