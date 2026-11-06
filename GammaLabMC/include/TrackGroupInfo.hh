#ifndef TrackGroupInfo_h
#define TrackGroupInfo_h 1

#include "G4VUserTrackInformation.hh"

class G4ParticleDefinition;

class TrackGroupInfo : public G4VUserTrackInformation
{
public:
  TrackGroupInfo(G4int groupID, G4ParticleDefinition* particle)
      : fGroupID(groupID), fParticle(particle) {}

  void Print() const override {}
  G4int GetGroupID() const { return fGroupID; }
  G4ParticleDefinition* GetParticle() const { return fParticle; }

private:
  G4int fGroupID;
  G4ParticleDefinition* fParticle;
};

#endif
