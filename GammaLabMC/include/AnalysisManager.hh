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
#include <vector>

class TTree;
class TFile;
class G4Track;

//---------------------------------------------------------------------------

class AnalysisManager {

public:

  AnalysisManager();
  ~AnalysisManager();

  void InitOutput();
  void SetEventCounts(G4int generated, G4int transported) {
    fGeneratedEventCount = generated;
    fTransportedEventCount = transported;
  }
  void ClearDecayProducts();
  void AddDecayProduct(const G4Track* track);

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
  G4int                 fGeneratedEventCount;
  G4int                 fTransportedEventCount;
  Int_t                 fDecay_N;
  std::vector<Int_t>    fDecay_pdg;
  std::vector<Float_t>  fDecay_E;
  std::vector<Float_t>  fDecay_px;
  std::vector<Float_t>  fDecay_py;
  std::vector<Float_t>  fDecay_pz;
  std::vector<Float_t>  fDecay_x;
  std::vector<Float_t>  fDecay_y;
  std::vector<Float_t>  fDecay_z;
  std::vector<Float_t>  fDecay_t;
  
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
