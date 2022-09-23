#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4THitsCollection.hh"

#include "DetectorSD.hh"
#include "DetectorHit.hh"

using namespace CLHEP;

//---------------------------------------------------------------------------

DetectorSD::DetectorSD(G4String name, G4int)
  :G4VSensitiveDetector(name)
{
  collectionName.insert(G4String("SDHits")+name);
  fCollection = NULL;
  fNelements  = 10000;
  fNhits      = 0;
  fhitID      = new G4int[fNelements];
  fHits       = new G4int[fNelements];
  for(G4int i=0; i<fNelements; i++) fhitID[i] = -1;
  for(G4int i=0; i<fNelements; i++) fHits[i]  = 0;
}

//---------------------------------------------------------------------------

DetectorSD::~DetectorSD()
{
}

//---------------------------------------------------------------------------

void DetectorSD::Initialize(G4HCofThisEvent* HCE)
{

  fCollection = new DetectorHitsCollection(SensitiveDetectorName,collectionName[0]);
  static G4int HCID = -1;

  if( HCID < 0 )  {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);

  }  
  HCE->AddHitsCollection( HCID, (G4VHitsCollection*)fCollection ); 
}

//---------------------------------------------------------------------------

G4bool DetectorSD::ProcessHits( G4Step* aStep,G4TouchableHistory* )
{ 
  
  G4Track*              aTrack       = aStep->GetTrack();
  G4TouchableHistory*   theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  G4VPhysicalVolume*    volume       = theTouchable->GetVolume();
  G4int                 id           = volume->GetCopyNo();
  G4String              ParticleName = aTrack->GetDefinition()->GetParticleName();
  G4double              edep         = aStep->GetTotalEnergyDeposit();
  
  DetectorHit* Hit = new DetectorHit;
  Hit->SetEnergy(edep);
  Hit->SetMomentum(aTrack->GetMomentum());
  Hit->SetPrePosition(aStep->GetPreStepPoint()->GetPosition());
  Hit->SetPostPosition(aStep->GetPostStepPoint()->GetPosition());
  Hit->SetPDef(aTrack->GetDefinition());
  Hit->SetID(id);
  Hit->SetTime(aStep->GetPostStepPoint()->GetGlobalTime());
  fhitID[id] = fCollection->insert(Hit) -1;
  fHits[fNhits++]=id;
  return true;
}

//---------------------------------------------------------------------------

G4bool DetectorSD::ProcessHits_constStep(G4Step*,G4TouchableHistory*)
{
    return false;
}

//---------------------------------------------------------------------------

void DetectorSD::EndOfEvent(G4HCofThisEvent*)
{

  for (G4int i=0;i<fNhits;i++) {
    fhitID[fHits[i]] = -1;
    fHits[i]         = 0;
  }
  fNhits = 0;
}

//---------------------------------------------------------------------------














