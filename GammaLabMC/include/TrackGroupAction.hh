#ifndef TrackGroupAction_h
#define TrackGroupAction_h 1

#include "G4UserTrackingAction.hh"

class AnalysisManager;

class TrackGroupAction : public G4UserTrackingAction
{
public:
  explicit TrackGroupAction(AnalysisManager* analysisManager)
      : fAnalysisManager(analysisManager) {}
  void PreUserTrackingAction(const G4Track*) override;
  void PostUserTrackingAction(const G4Track*) override;

private:
  AnalysisManager* fAnalysisManager;
};

#endif
