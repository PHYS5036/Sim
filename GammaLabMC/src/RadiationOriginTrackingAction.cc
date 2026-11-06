#include "RadiationOriginTrackingAction.hh"
#include "TrackOriginInfo.hh"

#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"

void RadiationOriginTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  if (!track->GetUserInformation()) {
    const G4String& particleName =
        track->GetDefinition()->GetParticleName();
    const TrackOrigin origin = track->GetParentID() == 0 &&
        (particleName == "e-" || particleName == "e+")
        ? TrackOrigin::Beta : TrackOrigin::Gamma;
    fpTrackingManager->SetUserTrackInformation(
        new TrackOriginInfo(origin));
  }
}

void RadiationOriginTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  auto* secondaries = fpTrackingManager->GimmeSecondaries();
  if (!secondaries) return;

  const auto* parentInfo =
      dynamic_cast<const TrackOriginInfo*>(track->GetUserInformation());
  const TrackOrigin parentOrigin =
      parentInfo ? parentInfo->GetOrigin() : TrackOrigin::Gamma;
  const bool parentIsIon =
      track->GetDefinition()->GetParticleType() == "nucleus";

  for (auto* secondary : *secondaries) {
    if (secondary->GetUserInformation()) continue;

    TrackOrigin origin = parentOrigin;
    const G4String& particleName =
        secondary->GetDefinition()->GetParticleName();
    if (particleName == "gamma") {
      origin = TrackOrigin::Gamma;
    } else if (parentIsIon &&
               (particleName == "e-" || particleName == "e+") &&
               secondary->GetCreatorProcess() &&
               secondary->GetCreatorProcess()->GetProcessName() ==
                   "RadioactiveDecay") {
      origin = TrackOrigin::Beta;
    }

    secondary->SetUserInformation(new TrackOriginInfo(origin));
  }
}
