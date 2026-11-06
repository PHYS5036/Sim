#ifndef TrackGroupAction_h
#define TrackGroupAction_h 1

#include "G4UserTrackingAction.hh"

class TrackGroupAction : public G4UserTrackingAction
{
public:
  void PreUserTrackingAction(const G4Track*) override;
  void PostUserTrackingAction(const G4Track*) override;
};

#endif
