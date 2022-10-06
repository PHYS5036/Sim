#ifndef AnalysisManager_h
#define AnalysisManager_h 1

#include "globals.hh"
#include "PrimaryGeneratorAction.hh"
#include "AnalysisMessenger.hh"

#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"


//---------------------------------------------------------------------------

class AnalysisManager {

public:

  AnalysisManager();
  ~AnalysisManager();

  void InitOutput();
  void WriteOutput();

  void ZeroArray();
  void FillArray( int );
  void FillTree();

  void SetOutFileName  ( G4String fname )           { fOutFileName  = fname; }

  void SetPrimaryEnergy   (G4double       ene  )       { fPEne  = ene;  }
  void SetPrimaryTime     (G4double       time )       { fPTime = time; }
  void SetPrimaryPDef     (G4ParticleDefinition* pdef) { fPPDef = pdef; }
  void SetPrimaryDirection(G4ThreeVector  dir  )       { fPdir  = dir;  }

  void SetStepPDef      ( G4ParticleDefinition* sp ) { fSteppdef = sp;    }
  void SetStepPosPre    ( G4ThreeVector  spos )      { fSteppospre  = spos;  }
  void SetStepPosPost   ( G4ThreeVector  spos )      { fSteppospost  = spos;  }
  void SetStepP3        ( G4ThreeVector  smom )      { fStepp3   = smom;  }
  void SetStepTime      ( G4double       stime )     { fSteptime = stime; }
  void SetStepID        ( G4int          sid )       { fStepid   = sid;   }
  void SetStepEdep      ( G4double       sedep)      { fStepedep = sedep; }

  void SetEventN        ( G4int          eid )       { fEventN   = eid;   }


private:
  
  AnalysisMessenger*    fAnaMessenger;
  G4String              fOutFileName = "temp.csv";

  const static int nTypes = 4; //leptons, mesons, baryons, ions
  std::vector<int> pdg_limit = {100,1000,10000};

  const static int nPixX  = 40;
  const static int nPixY  = 400;
  const static int nPixZ  = 40;

  int minX = -10;
  int minY = -100;
  int minZ = -10;
  int maxX = 10;
  int maxY = 100;
  int maxZ = 10;

  int fEventN = 0;
 
  double eDep[nPixX][nPixY][nPixZ][nTypes][2];
  double inputPos[nPixX][nPixY];

  // Primary
  float                 fPEne;
  float                 fPth;
  float                 fPph;
  float                 fPTime;
  G4ParticleDefinition* fPPDef;
  int                   fPpdg;
  G4ThreeVector         fPdir;

  // Step raw
  G4ParticleDefinition* fSteppdef;
  G4ThreeVector         fStepp3;
  G4ThreeVector         fSteppospre;
  G4ThreeVector         fSteppospost;
  G4double              fSteptime;
  G4int                 fStepid;
  G4double              fStepedep;

  static const int    fMaxhits = 50000;

  int                 fRAW_Nhits;
  int                 fRAW_id[fMaxhits];
  float               fRAW_time[fMaxhits];
  float               fRAW_Edep[fMaxhits];
  int                 fRAW_pdg[fMaxhits];
  float               fRAW_mass[fMaxhits];
  float               fRAW_mom[fMaxhits];
  float               fRAW_px[fMaxhits];
  float               fRAW_py[fMaxhits];
  float               fRAW_pz[fMaxhits];
  float               fRAW_xpre[fMaxhits];
  float               fRAW_ypre[fMaxhits];
  float               fRAW_zpre[fMaxhits];
  float               fRAW_xpost[fMaxhits];
  float               fRAW_ypost[fMaxhits];
  float               fRAW_zpost[fMaxhits];
  float               fRAW_Energy[fMaxhits];

  int                 index_x[fMaxhits];
  int                 index_y[fMaxhits];
  int                 index_z[fMaxhits];
  int                 index_pdg[fMaxhits];

};

#endif

//---------------------------------------------------------------------------
