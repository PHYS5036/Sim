#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh" 
#include "G4ThreeVector.hh"
#include "globals.hh"


//---------------------------------------------------------------------------

class PrimaryGeneratorMessenger;
class G4ParticleGun;
class G4GeneralParticleSource;
class G4Event;

enum { EPGA_GPS, EPGA_ROOT};

//---------------------------------------------------------------------------

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();    
  ~PrimaryGeneratorAction();

  void GeneratePrimaries (G4Event*);
  void SetMode(G4int mode){ fMode = mode; }
  void SetDumpBeam(G4int dump){ fDump = dump; }

  G4int GetMode()                     { return fMode; }
  G4int GetDumpBeam()                 { return fDump; }
  G4ThreeVector GetVertex()           { return G4ThreeVector(fVx,  fVy,  fVz);  }
  G4ThreeVector GetDirection()        { return G4ThreeVector(fPxp, fPyp, fPzp); }
  G4double      GetEnergy()           { return fEp; }
  G4double      GetTime()             { return fTp; }
  G4ParticleDefinition* GetPrimPDef() { return fPDefinition; }

private:
  PrimaryGeneratorMessenger* fGunMessenger;

  G4ParticleGun*             fParticleGun;    
  G4GeneralParticleSource*   fParticleSource;
  G4ParticleTable*           fParticleTable;
  G4IonTable*                fIonTable; 
  G4ParticleDefinition*      fPDefinition;
  G4int                      fMode;   
  G4int                      fDump;   
  
  int                      fNevent;
  int                      fPDG;
  float                    fVx;
  float                    fVy;
  float                    fVz;
  float                    fPxp;
  float                    fPyp;
  float                    fPzp;
  float                    fEp;
  float                    fTp;

};
#endif

//---------------------------------------------------------------------------


