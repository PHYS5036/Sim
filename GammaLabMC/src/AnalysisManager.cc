#include "AnalysisManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "AnalysisMessenger.hh"

#include "G4UnitsTable.hh"
#include "G4RunManager.hh"
#include "G4Point3D.hh"
#include "G4Transform3D.hh"
#include "G4Track.hh"

#include "TROOT.h"
#include "TApplication.h"
#include "TParameter.h"
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
  fOutputType  = TString("detector");
  fGeneratedEventCount = 0;
  fTransportedEventCount = 0;
  fROOTfile = NULL;
  fROOTtree = NULL;

  fAnaMessenger = new AnalysisMessenger(this);
}

//---------------------------------------------------------------------------

AnalysisManager::~AnalysisManager()
{
   if( fROOTfile && fROOTtree ) {
     fROOTfile->cd();
     fROOTtree->Write();
   }
   if( fROOTfile ) fROOTfile->Close();
}

//---------------------------------------------------------------------------

void AnalysisManager::InitOutput()
{ 
  if( fROOTfile ) return;

  TString outDir = gSystem->DirName(fOutFileName);
  if( outDir != "" && outDir != "." ) gSystem->mkdir(outDir, kTRUE);

  fROOTfile = new TFile(fOutFileName,"RECREATE","fROOTfile",1);
  fROOTfile->cd();
  TParameter<Long64_t>("GeneratedEventCount", fGeneratedEventCount).Write();
  TParameter<Long64_t>("TransportedEventCount", fTransportedEventCount).Write();
  fROOTtree = new TTree("T","Output Tree");
  fROOTtree->SetAutoSave();

  // Set Primary Branches
  fROOTtree->Branch("Prim_E",      &fPEne,   "Prim_E/F"      );
  fROOTtree->Branch("Prim_Th",     &fPth,    "Prim_Th/F"     );
  fROOTtree->Branch("Prim_Ph",     &fPph,    "Prim_Ph/F"     );
  fROOTtree->Branch("Prim_t",      &fPTime,  "Prim_t/F"      );
  fROOTtree->Branch("Prim_pdg",    &fPpdg,   "Prim_pdg/I"    );
  fROOTtree->Branch("Decay_N",     &fDecay_N, "Decay_N/I");
  fROOTtree->Branch("Decay_pdg",   &fDecay_pdg);
  fROOTtree->Branch("Decay_E",     &fDecay_E);
  fROOTtree->Branch("Decay_px",    &fDecay_px);
  fROOTtree->Branch("Decay_py",    &fDecay_py);
  fROOTtree->Branch("Decay_pz",    &fDecay_pz);
  fROOTtree->Branch("Decay_x",     &fDecay_x);
  fROOTtree->Branch("Decay_y",     &fDecay_y);
  fROOTtree->Branch("Decay_z",     &fDecay_z);
  fROOTtree->Branch("Decay_t",     &fDecay_t);
  
  // Set Raw FPD Scintillator Hit Branches
  fROOTtree->Branch("Detector_Nhits", &fRAW_Nhits, "Detector_Nhits/I");  
  fROOTtree->Branch("Detector_t",     fRAW_time,   "Detector_t[Detector_Nhits]/F"   );
  fROOTtree->Branch("Detector_pdg",   fRAW_pdg,    "Detector_pdg[Detector_Nhits]/I" );
  fROOTtree->Branch("Detector_xpost",     fRAW_xpost,      "Detector_xpost[Detector_Nhits]/F"  );
  fROOTtree->Branch("Detector_ypost",     fRAW_ypost,      "Detector_ypost[Detector_Nhits]/F"  );
  fROOTtree->Branch("Detector_zpost",     fRAW_zpost,      "Detector_zpost[Detector_Nhits]/F"  );
  fROOTtree->Branch("Detector_E",     fRAW_Energy, "Detector_E[Detector_Nhits]/F"   );
  fROOTtree->Branch("Detector_Ed",    fRAW_Edep,   "Detector_Ed[Detector_Nhits]/F"  );
  
}

//---------------------------------------------------------------------------

void AnalysisManager::ClearDecayProducts()
{
  fDecay_N = 0;
  fDecay_pdg.clear();
  fDecay_E.clear();
  fDecay_px.clear();
  fDecay_py.clear();
  fDecay_pz.clear();
  fDecay_x.clear();
  fDecay_y.clear();
  fDecay_z.clear();
  fDecay_t.clear();
}

void AnalysisManager::AddDecayProduct(const G4Track* track)
{
  const auto* definition = track->GetDefinition();
  const auto& momentum = track->GetMomentum();
  const auto& position = track->GetPosition();

  fDecay_pdg.push_back(definition->GetPDGEncoding());
  fDecay_E.push_back(static_cast<Float_t>(track->GetKineticEnergy()));
  fDecay_px.push_back(static_cast<Float_t>(momentum.x()));
  fDecay_py.push_back(static_cast<Float_t>(momentum.y()));
  fDecay_pz.push_back(static_cast<Float_t>(momentum.z()));
  fDecay_x.push_back(static_cast<Float_t>(position.x()));
  fDecay_y.push_back(static_cast<Float_t>(position.y()));
  fDecay_z.push_back(static_cast<Float_t>(position.z()));
  fDecay_t.push_back(static_cast<Float_t>(track->GetGlobalTime()));
  fDecay_N = static_cast<Int_t>(fDecay_pdg.size());
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
  fRAW_Nhits     = 0;
  fFPDpdef   = NULL;
  fFPDp3     = (zero);
  fFPDpospre    = (zero);
  fFPDpospost    = (zero);
  fFPDid     = 0;
  fFPDtime   = 0;
  fFPDedep   = 0;

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
    fRAW_xpre[i]       = 9999;
    fRAW_ypre[i]       = 9999;
    fRAW_zpre[i]       = 9999;
    fRAW_xpost[i]       = 9999;
    fRAW_ypost[i]       = 9999;
    fRAW_zpost[i]       = 9999;
    fRAW_Energy[i]  = 9999;
  }
}

//---------------------------------------------------------------------------

void AnalysisManager::FillArray( Int_t hitn ) 
{
    fRAW_Nhits++;
    fRAW_id[hitn]     = (Int_t)fFPDid;
    fRAW_pdg[hitn]    = (Int_t)fFPDpdef->GetPDGEncoding();
    fRAW_mass[hitn]   = (Float_t)fFPDpdef->GetPDGMass();
    fRAW_time[hitn]   = (Float_t)fFPDtime;                                   
    fRAW_mom[hitn]    = (Float_t)fFPDp3.mag();                             
    fRAW_px[hitn]     = (Float_t)fFPDp3.getX();                             
    fRAW_py[hitn]     = (Float_t)fFPDp3.getY();                             
    fRAW_pz[hitn]     = (Float_t)fFPDp3.getZ();                             
    fRAW_xpre[hitn]      = (Float_t)fFPDpospre.getX();                             
    fRAW_ypre[hitn]      = (Float_t)fFPDpospre.getY();                             
    fRAW_zpre[hitn]      = (Float_t)fFPDpospre.getZ();                             
    fRAW_xpost[hitn]      = (Float_t)fFPDpospost.getX();                             
    fRAW_ypost[hitn]      = (Float_t)fFPDpospost.getY();                             
    fRAW_zpost[hitn]      = (Float_t)fFPDpospost.getZ();                             
    fRAW_Edep[hitn]   = (Float_t)fFPDedep;
    fRAW_Energy[hitn] = 1000*TMath::Sqrt( fRAW_mom[hitn]*fRAW_mom[hitn] 
				     + fRAW_mass[hitn]*fRAW_mass[hitn] );

}

//---------------------------------------------------------------------------

void AnalysisManager::FillTree()
{
  // Primary Variables
  fPTime  = (Float_t)fPTime;
  fPth    = (Float_t)fPdir.getTheta();                         
  fPph    = (Float_t)fPdir.getPhi();                                                      
  fPEne   = (Float_t)fPEne*1000;                         
  fPpdg   = fPPDef ? (Int_t)fPPDef->GetPDGEncoding() : 0;
  
  fROOTtree->Fill();
}

//---------------------------------------------------------------------------
