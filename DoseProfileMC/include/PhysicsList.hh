#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4EmConfigurator.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class PhysicsListMessenger;

class PhysicsList: public G4VModularPhysicsList
{
public:

  PhysicsList();
  virtual ~PhysicsList();

  void ConstructParticle();

  void SetCuts();
  void SetCutForGamma(G4double);
  void SetCutForElectron(G4double);
  void SetCutForPositron(G4double);

  void AddPhysicsList(const G4String& name);
  void ConstructProcess();

  void AddPackage(const G4String& name);
  void ConstructPhotoNuclear();

private:

  G4EmConfigurator em_config;

  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;

  G4bool helIsRegistered;
  G4bool bicIsRegistered;
  G4bool biciIsRegistered;
  G4bool locIonIonInelasticIsRegistered;
  G4bool radioactiveDecayIsRegistered;

  G4String                             emName;
  G4VPhysicsConstructor*               emPhysicsList;
  G4VPhysicsConstructor*               decPhysicsList;
  G4VPhysicsConstructor*  raddecayList;

  std::vector<G4VPhysicsConstructor*>  hadronPhys;

  PhysicsListMessenger* pMessenger;
};

#endif
