#ifndef AnalysisMessenger_h
#define AnalysisMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

//---------------------------------------------------------------------------

class AnalysisManager;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;

//---------------------------------------------------------------------------

class AnalysisMessenger: public G4UImessenger
{
  public:
    AnalysisMessenger(AnalysisManager*);
   ~AnalysisMessenger();

    void SetNewValue(G4UIcommand*, G4String);

  private:
    AnalysisManager*      fAnalysisManager;
    G4UIdirectory*        fAnalysisDir;
    G4UIcmdWithAString*   fOutFileCmd;
};
#endif

//---------------------------------------------------------------------------

