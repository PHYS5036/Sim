#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"

//---------------------------------------------------------------------------

class AnalysisManager;
class PrimaryGeneratorAction;

//---------------------------------------------------------------------------

class EventAction : public G4UserEventAction
{
  public:
  EventAction( AnalysisManager*, PrimaryGeneratorAction* );
   ~EventAction();

  public:
    void     BeginOfEventAction(const G4Event*);
    void     EndOfEventAction(const G4Event*);

  private:
  AnalysisManager*        fAnaManager;
  PrimaryGeneratorAction* fPGA;

};
#endif

//---------------------------------------------------------------------------

    
