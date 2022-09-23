#include "PhysicsListMessenger.hh"
#include "PhysicsList.hh"

#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"

//---------------------------------------------------------------------------

PhysicsListMessenger::PhysicsListMessenger( PhysicsList* PhysList )
  :fPhysicsList(PhysList)
{
  fPhysicsListDir        = new G4UIdirectory("/SodiumIodideMC/physics/");
  fPhysicsListDir->SetGuidance("PhysicsList geometry control");

  fEmModelCmd = new G4UIcmdWithAString("/SodiumIodideMC/physics/SetEmModel",this);
  fEmModelCmd->SetGuidance("Select which EM model to use: Standard, Penelope or Livermore");

  fEmProcessCmd = new G4UIcmdWithAString("/SodiumIodideMC/physics/SetEmProcess",this);
  fEmProcessCmd->SetGuidance("Select which EM gamma processes to use: All, Photoelectric, Compton or Pair");
}

//---------------------------------------------------------------------------

PhysicsListMessenger::~PhysicsListMessenger()
{
  delete fPhysicsListDir;

  delete fEmModelCmd;
  delete fEmProcessCmd;
}

//---------------------------------------------------------------------------

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
  if (command == fEmModelCmd )
    fPhysicsList->SetEmModel( newValue );
  else if (command == fEmProcessCmd )
    fPhysicsList->SetEmProcess( newValue );
}

//---------------------------------------------------------------------------
