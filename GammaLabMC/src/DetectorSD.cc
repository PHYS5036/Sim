#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4THitsCollection.hh"
#include "TrackGroupInfo.hh"
#include <unordered_map>

#include "DetectorSD.hh"
#include "DetectorHit.hh"

//---------------------------------------------------------------------------

DetectorSD::DetectorSD(G4String name, G4int)
  :G4VSensitiveDetector(name)
{
  collectionName.insert(G4String("SDHits")+name);
  fCollection = NULL;
}

//---------------------------------------------------------------------------

DetectorSD::~DetectorSD()
{}

//---------------------------------------------------------------------------

void DetectorSD::Initialize(G4HCofThisEvent* HCE)
{

  fCollection = new DetectorHitsCollection(SensitiveDetectorName,collectionName[0]);
  fTrackHits.clear();
  static G4int HCID = -1;

  if( HCID < 0 )  {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);

  }  
  HCE->AddHitsCollection( HCID, (G4VHitsCollection*)fCollection ); 
}

//---------------------------------------------------------------------------

G4bool DetectorSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{ 
 
  G4Track* aTrack = aStep->GetTrack();
  G4double edep   = aStep->GetTotalEnergyDeposit();
  if (edep <= 0) return false;
  const auto* groupInfo =
      dynamic_cast<const TrackGroupInfo*>(aTrack->GetUserInformation());
  if (groupInfo && !groupInfo->IsFinalProduct()) return false;
  const G4int groupID = groupInfo
      ? groupInfo->GetGroupID() : aTrack->GetTrackID();
  auto found = fTrackHits.find(groupID);
  if (found == fTrackHits.end()) {
    DetectorHit* Hit = new DetectorHit;
    Hit->AddEnergy(edep);
    Hit->SetMomentum(aTrack->GetMomentum());
    Hit->SetPrePosition(aStep->GetPreStepPoint()->GetPosition());
    Hit->SetPostPosition(aStep->GetPostStepPoint()->GetPosition());
    Hit->SetPDef(groupInfo
        ? groupInfo->GetParticle() : aTrack->GetDefinition());
    Hit->SetID(groupID);
    Hit->SetTime(aStep->GetPostStepPoint()->GetGlobalTime());
    fTrackHits.emplace(groupID, Hit);
    fCollection->insert(Hit);
  }
  else {
    found->second->AddEnergy(edep);
  }
  return true;
}

//---------------------------------------------------------------------------

G4bool DetectorSD::ProcessHits_constStep(const G4Step*, G4TouchableHistory*)
{
  return false;
}

//---------------------------------------------------------------------------

void DetectorSD::EndOfEvent(G4HCofThisEvent*)
{
  fTrackHits.clear();
}

//---------------------------------------------------------------------------







