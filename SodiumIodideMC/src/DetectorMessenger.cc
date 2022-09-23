#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"

//---------------------------------------------------------------------------

DetectorMessenger::DetectorMessenger(DetectorConstruction* Detect)
  :fDetector(Detect)
{
  fDetectorDir        = new G4UIdirectory("/SodiumIodideMC/detector/");
  fDetectorDir->SetGuidance("Detector geometry control");

  fAbsorberOnCmd        = new G4UIcmdWithAnInteger("/SodiumIodideMC/detector/AbsorberOn",this);
  fAbsorberOnCmd->SetGuidance("Set 1 for absorber on, 0 for off.");

  fAbsorberMaterialCmd = new G4UIcmdWithAString("/SodiumIodideMC/detector/AbsorberMaterial",this);
  fAbsorberMaterialCmd->SetGuidance("Select Absorber Material (G4_Al, G4_Fe or G4_Pb.");

  fAbsorberThicknessCmd = new G4UIcmdWithADouble("/SodiumIodideMC/detector/AbsorberThickness",this);
  fAbsorberThicknessCmd->SetGuidance("Set the thickness of the absorber in cm.");

  fUpdateCmd          = new G4UIcommand("/SodiumIodideMC/detector/update",this);
  fUpdateCmd->SetGuidance("Update the detector geometry with changed values.");
  fUpdateCmd->SetGuidance("Must be run before beamOn if detector has been changed.");  
}

//---------------------------------------------------------------------------

DetectorMessenger::~DetectorMessenger()
{
  delete fDetectorDir;

  delete fAbsorberOnCmd;
  delete fAbsorberThicknessCmd;
  delete fAbsorberMaterialCmd;

  delete fUpdateCmd;
}

//---------------------------------------------------------------------------

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
  if(command == fUpdateCmd)
    fDetector->UpdateGeometry();
  else if (command == fAbsorberOnCmd)
    fDetector->SetAbsorberOn(fAbsorberOnCmd->GetNewIntValue(newValue));
  else if (command == fAbsorberMaterialCmd )
    fDetector->SetAbsorberMaterial( newValue );
  else if (command == fAbsorberThicknessCmd)
    fDetector->SetAbsorberThickness(fAbsorberThicknessCmd->GetNewDoubleValue(newValue));
}

//---------------------------------------------------------------------------
