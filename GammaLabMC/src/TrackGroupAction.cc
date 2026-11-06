#include "TrackGroupAction.hh"
#include "TrackGroupInfo.hh"

#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"

void TrackGroupAction::PreUserTrackingAction(const G4Track* track)
{
  if (!track->GetUserInformation()) {
    fpTrackingManager->SetUserTrackInformation(
        new TrackGroupInfo(track->GetTrackID(), track->GetDefinition()));
  }
}

void TrackGroupAction::PostUserTrackingAction(const G4Track* track)
{
  auto* secondaries = fpTrackingManager->GimmeSecondaries();
  if (!secondaries) return;

  const auto* parentInfo =
      dynamic_cast<const TrackGroupInfo*>(track->GetUserInformation());
  const G4bool primaryIon =
      track->GetParentID() == 0 &&
      track->GetDefinition()->GetParticleType() == "nucleus";

  for (auto* secondary : *secondaries) {
    if (secondary->GetUserInformation()) continue;

    const G4int groupID = primaryIon
        ? secondary->GetTrackID()
        : (parentInfo ? parentInfo->GetGroupID() : track->GetTrackID());
    auto* particle = primaryIon
        ? secondary->GetDefinition()
        : (parentInfo ? parentInfo->GetParticle() : track->GetDefinition());
    secondary->SetUserInformation(new TrackGroupInfo(groupID, particle));
  }
}
