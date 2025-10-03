#ifndef PhysicsListMessenger_h
#define PhysicsListMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

//---------------------------------------------------------------------------

class PhysicsList;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;

//---------------------------------------------------------------------------

class PhysicsListMessenger: public G4UImessenger
{
public:
  PhysicsListMessenger( PhysicsList* );
  ~PhysicsListMessenger();
  
  void SetNewValue(G4UIcommand*, G4String);
  
private:
  PhysicsList*                 fPhysicsList;

  G4UIdirectory*               fPhysicsListDir;

  G4UIcmdWithAString*          fEmModelCmd;
  G4UIcmdWithAString*          fEmProcessCmd;
};

#endif

//---------------------------------------------------------------------------

