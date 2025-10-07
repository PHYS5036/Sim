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
  fDetectorDir        = new G4UIdirectory("/GammaLabMC/detector/");
  fDetectorDir->SetGuidance("Detector geometry control");

  // Absorber commands
  fAbsorberOnCmd        = new G4UIcmdWithAnInteger("/GammaLabMC/detector/AbsorberOn",this);
  fAbsorberOnCmd->SetGuidance("Set 1 for absorber on, 0 for off.");

  fAbsorberMaterialCmd = new G4UIcmdWithAString("/GammaLabMC/detector/AbsorberMaterial",this);
  fAbsorberMaterialCmd->SetGuidance("Select Absorber Material (G4_Al, G4_Fe or G4_Pb.");

  fAbsorberThicknessCmd = new G4UIcmdWithADouble("/GammaLabMC/detector/AbsorberThickness",this);
  fAbsorberThicknessCmd->SetGuidance("Set the thickness of the absorber in mm.");

  // Detector Setup commands
  fDetectorDistanceCmd = new G4UIcmdWithADouble("/GammaLabMC/detector/DetectorDistance",this);
  fDetectorDistanceCmd->SetGuidance("Set the distance from source to detector front face in mm.");

  fDetectorTypeCmd = new G4UIcmdWithAnInteger("/GammaLabMC/detector/DetectorType",this);
  fDetectorTypeCmd->SetGuidance("Set the type of detector: 0 = Sodium Iodide, 1 = GAGG.");

  fSourceHolderTypeCmd = new G4UIcmdWithAnInteger("/GammaLabMC/detector/SourceHolderType",this);
  fSourceHolderTypeCmd->SetGuidance("Set the type of source holder: 0 = 3D printed plastic with lead lining, 1 = Old pencil source holder (lead block with hole).");

  fUpdateCmd          = new G4UIcommand("/GammaLabMC/detector/update",this);
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

  delete fDetectorDistanceCmd;
  delete fDetectorTypeCmd;
  delete fSourceHolderTypeCmd;

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
  else if (command == fDetectorDistanceCmd)
    fDetector->SetDetectorDistance(fDetectorDistanceCmd->GetNewDoubleValue(newValue));
  else if (command == fDetectorTypeCmd)
    fDetector->SetDetectorType(fDetectorTypeCmd->GetNewIntValue(newValue));
  else if (command == fSourceHolderTypeCmd)
    fDetector->SetSourceHolderType(fSourceHolderTypeCmd->GetNewIntValue(newValue));
}

//---------------------------------------------------------------------------
