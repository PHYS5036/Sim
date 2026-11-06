#ifndef RadiationOriginTrackingAction_h
#define RadiationOriginTrackingAction_h 1

#include "G4UserTrackingAction.hh"

class RadiationOriginTrackingAction : public G4UserTrackingAction
{
public:
  void PreUserTrackingAction(const G4Track*) override;
  void PostUserTrackingAction(const G4Track*) override;
};

#endif
