#include "TrackGroupAction.hh"
#include "TrackGroupInfo.hh"
#include "AnalysisManager.hh"

#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"

void TrackGroupAction::PreUserTrackingAction(const G4Track* track)
{
  if (!track->GetUserInformation()) {
    const G4bool intermediateIon =
        track->GetParentID() == 0 &&
        track->GetDefinition()->GetParticleType() == "nucleus";
    fpTrackingManager->SetUserTrackInformation(
        new TrackGroupInfo(track->GetTrackID(), track->GetDefinition(),
                           !intermediateIon));
  }
}

void TrackGroupAction::PostUserTrackingAction(const G4Track* track)
{
  auto* secondaries = fpTrackingManager->GimmeSecondaries();
  if (!secondaries) return;

  const auto* parentInfo =
      dynamic_cast<const TrackGroupInfo*>(track->GetUserInformation());
  const G4bool nuclearDecay =
      track->GetDefinition()->GetParticleType() == "nucleus";

  for (auto* secondary : *secondaries) {
    if (secondary->GetUserInformation()) continue;
    const auto* creator = secondary->GetCreatorProcess();
    const G4bool decayProduct = creator &&
        (creator->GetProcessName().contains("RadioactiveDecay") ||
         creator->GetProcessName() == "PhotonEvaporation");
    if (nuclearDecay && decayProduct) {
      auto* definition = secondary->GetDefinition();
      const G4bool stableNucleus =
          definition->GetParticleType() == "nucleus" &&
          definition->GetPDGStable();
      const G4bool finalProduct =
          definition->GetParticleType() != "nucleus" || stableNucleus;
      if (finalProduct) {
        fAnalysisManager->AddDecayProduct(secondary);
      }
      secondary->SetUserInformation(new TrackGroupInfo(
          secondary->GetTrackID(), definition, finalProduct));
    } else if (parentInfo) {
      secondary->SetUserInformation(new TrackGroupInfo(
          parentInfo->GetGroupID(), parentInfo->GetParticle(),
          parentInfo->IsFinalProduct()));
    } else {
      secondary->SetUserInformation(new TrackGroupInfo(
          track->GetTrackID(), track->GetDefinition()));
    }
  }
}
