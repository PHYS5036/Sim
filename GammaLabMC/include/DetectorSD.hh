#ifndef DetectorSD_h
#define DetectorSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "globals.hh"
#include "G4VHitsCollection.hh"

#include "DetectorHit.hh"

//---------------------------------------------------------------------------

class DetectorSD : public G4VSensitiveDetector
{
public:
  
  DetectorSD( G4String, G4int );
  ~DetectorSD();
  
  void   Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step* astep,G4TouchableHistory* ROHist);
  G4bool ProcessHits_constStep(const G4Step* astep,G4TouchableHistory*);
  void   EndOfEvent(G4HCofThisEvent*);
  
private:
  
  DetectorHitsCollection*  fCollection;  
  G4int                    fDetID;
  G4int                    fNelements;
  G4int                    fNhits;
  G4int*                   fhitID;
  G4int*                   fHits;

};
#endif

//---------------------------------------------------------------------------






