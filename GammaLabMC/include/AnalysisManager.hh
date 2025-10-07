#ifndef AnalysisManager_h
#define AnalysisManager_h 1

#include "globals.hh"
#include "PrimaryGeneratorAction.hh"
#include "AnalysisMessenger.hh"

#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "Rtypes.h"
#include "TVector3.h"
#include "TString.h"

class TTree;
class TFile;

//---------------------------------------------------------------------------

class AnalysisManager {

public:

  AnalysisManager();
  ~AnalysisManager();

  void InitOutput();

  void ZeroArray();
  void FillArray( Int_t );
  void FillTree();

  TString GetOutputType() { return fOutputType; }
  
  void SetOutFileName  ( TString fname )           { fOutFileName  = fname; }
  void SetOutputType   ( TString tname )           { fOutputType   = tname; }

  void SetPrimaryEnergy   (G4double       ene  )       { fPEne  = ene;  }
  void SetPrimaryTime     (G4double       time )       { fPTime = time; }
  void SetPrimaryPDef     (G4ParticleDefinition* pdef) { fPPDef = pdef; }
  void SetPrimaryDirection(G4ThreeVector  dir  )       { fPdir  = dir;  }

  void SetFPDPDef      ( G4ParticleDefinition* sp ) { fFPDpdef = sp;    }
  void SetFPDPosPre       ( G4ThreeVector  spos )      { fFPDpospre  = spos;  }
  void SetFPDPosPost       ( G4ThreeVector  spos )      { fFPDpospost  = spos;  }
  void SetFPDP3        ( G4ThreeVector  smom )      { fFPDp3   = smom;  }
  void SetFPDTime      ( G4double       stime )     { fFPDtime = stime; }
  void SetFPDID        ( G4int          sid )       { fFPDid   = sid;   }
  void SetFPDEdep      ( G4double       sedep)      { fFPDedep = sedep; }

private:
  
  AnalysisMessenger*    fAnaMessenger;
  TString               fOutFileName;
  TString               fOutputType;
  TFile*                fROOTfile;
  TTree*                fROOTtree;
  
  // Primary
  Float_t               fPEne;
  Float_t               fPth;
  Float_t               fPph;
  Float_t               fPTime;
  G4ParticleDefinition* fPPDef;
  Int_t                 fPpdg;
  G4ThreeVector         fPdir;

  // FPD raw
  G4ParticleDefinition* fFPDpdef;
  G4ThreeVector         fFPDp3;
  G4ThreeVector         fFPDpospre;
  G4ThreeVector         fFPDpospost;
  G4double              fFPDtime;
  G4int                 fFPDid;
  G4double              fFPDedep;

  static const Int_t    fMaxhits = 50000;

  Int_t                 fRAW_Nhits;
  Int_t                 fRAW_id[fMaxhits];
  Float_t               fRAW_time[fMaxhits];
  Float_t               fRAW_Edep[fMaxhits];
  Int_t                 fRAW_pdg[fMaxhits];
  Float_t               fRAW_mass[fMaxhits];
  Float_t               fRAW_mom[fMaxhits];
  Float_t               fRAW_px[fMaxhits];
  Float_t               fRAW_py[fMaxhits];
  Float_t               fRAW_pz[fMaxhits];
  Float_t               fRAW_xpre[fMaxhits];
  Float_t               fRAW_ypre[fMaxhits];
  Float_t               fRAW_zpre[fMaxhits];
  Float_t               fRAW_xpost[fMaxhits];
  Float_t               fRAW_ypost[fMaxhits];
  Float_t               fRAW_zpost[fMaxhits];
  Float_t               fRAW_Energy[fMaxhits];


};

#endif

//---------------------------------------------------------------------------
