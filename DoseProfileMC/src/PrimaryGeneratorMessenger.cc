#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "Randomize.hh"

#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

//---------------------------------------------------------------------------

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger( PrimaryGeneratorAction* Gun )
:fAction(Gun)
{
  fGunDir = new G4UIdirectory("/DoseProfileMC/generator/");
  fGunDir->SetGuidance("PrimaryGenerator control");

  fSetModeCmd = new G4UIcmdWithAnInteger("/DoseProfileMC/generator/Mode",this);
  fSetModeCmd->SetGuidance("Set the mode of the generator, command line, GPS or ROOT");
  fSetModeCmd->SetParameterName("Mode",false);
  fSetModeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetSeedCmd = new G4UIcmdWithAnInteger("/DoseProfileMC/generator/Seed",this);
  fSetSeedCmd->SetGuidance("Set the random seed for the generator");
  fSetSeedCmd->SetParameterName("Seed",false);
  fSetSeedCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

//---------------------------------------------------------------------------


PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fGunDir;
  delete fSetModeCmd;
  delete fSetSeedCmd;
}

//---------------------------------------------------------------------------


void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 

  if( command == fSetModeCmd )
     { fAction->SetMode(fSetModeCmd->GetNewIntValue(newValue));}

  if( command == fSetSeedCmd )
    { CLHEP::HepRandom::setTheSeed(fSetSeedCmd->GetNewIntValue(newValue));}

}

//---------------------------------------------------------------------------


