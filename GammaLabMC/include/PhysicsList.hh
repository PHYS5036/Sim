#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "PhysicsListMessenger.hh"
#include "G4VUserPhysicsList.hh"
#include "globals.hh"

//---------------------------------------------------------------------------

class G4RadioactiveDecayPhysics;

class PhysicsList: public G4VUserPhysicsList
{
  public:
    PhysicsList();
   ~PhysicsList();

  inline void SetEmModel  ( G4String model ) { fModelName   = model; }
  inline void SetEmProcess( G4String proc  ) { fProcessName = proc;  }
  inline void SetRadioactiveDecay(G4bool enabled) { fRadioactiveDecay = enabled; }

  protected:
    void ConstructParticle();
    void ConstructProcess();
    void SetCuts();
    void ConstructGeneral();
    void ConstructEM();
    void AddStepMax();


  G4String fModelName;
  G4String fProcessName;

  PhysicsListMessenger* fPmessenger;
  G4bool fRadioactiveDecay = false;
  G4RadioactiveDecayPhysics* fDecayPhysics = nullptr;

};
#endif

//---------------------------------------------------------------------------


 
