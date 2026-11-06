#include "PhysicsListMessenger.hh"
#include "PhysicsList.hh"

#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

//---------------------------------------------------------------------------

PhysicsListMessenger::PhysicsListMessenger( PhysicsList* PhysList )
  :fPhysicsList(PhysList)
{
  fPhysicsListDir        = new G4UIdirectory("/GammaLabMC/physics/");
  fPhysicsListDir->SetGuidance("PhysicsList geometry control");

  fEmModelCmd = new G4UIcmdWithAString("/GammaLabMC/physics/SetEmModel",this);
  fEmModelCmd->SetGuidance("Select which EM model to use: Standard, Penelope or Livermore");

  fEmProcessCmd = new G4UIcmdWithAString("/GammaLabMC/physics/SetEmProcess",this);
  fEmProcessCmd->SetGuidance("Select which EM gamma processes to use: All, Photoelectric, Compton or Pair");

  fRadioactiveDecayCmd = new G4UIcmdWithABool("/GammaLabMC/physics/RadioactiveDecay",this);
  fRadioactiveDecayCmd->SetGuidance("Enable ion transport and radioactive decay for a GPS ion source.");
  fRadioactiveDecayCmd->AvailableForStates(G4State_PreInit);
}

//---------------------------------------------------------------------------

PhysicsListMessenger::~PhysicsListMessenger()
{
  delete fPhysicsListDir;

  delete fEmModelCmd;
  delete fEmProcessCmd;
  delete fRadioactiveDecayCmd;
}

//---------------------------------------------------------------------------

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
  if (command == fEmModelCmd )
    fPhysicsList->SetEmModel( newValue );
  else if (command == fEmProcessCmd )
    fPhysicsList->SetEmProcess( newValue );
  else if (command == fRadioactiveDecayCmd)
    fPhysicsList->SetRadioactiveDecay(fRadioactiveDecayCmd->GetNewBoolValue(newValue));
}

//---------------------------------------------------------------------------
