#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

//---------------------------------------------------------------------------

class DetectorConstruction;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;

//---------------------------------------------------------------------------

class DetectorMessenger: public G4UImessenger
{
public:
  DetectorMessenger(DetectorConstruction*);
  ~DetectorMessenger();
  
  void SetNewValue(G4UIcommand*, G4String);
    
private:
  DetectorConstruction*        fDetector;

  G4UIdirectory*               fDetectorDir;

  G4UIcmdWithAnInteger*        fTumourOnCmd;
  G4UIcmdWithADouble*          fTumourRadiusCmd;
  G4UIcmdWithADouble*          fTumourHeightCmd;

  G4UIcommand*                 fUpdateCmd;
};

#endif

//---------------------------------------------------------------------------

