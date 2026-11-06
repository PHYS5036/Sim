#ifndef TrackGroupInfo_h
#define TrackGroupInfo_h 1

#include "G4VUserTrackInformation.hh"

class G4ParticleDefinition;

class TrackGroupInfo : public G4VUserTrackInformation
{
public:
  TrackGroupInfo(G4int groupID, G4ParticleDefinition* particle,
                 G4bool finalProduct = true)
      : fGroupID(groupID), fParticle(particle), fFinalProduct(finalProduct) {}

  void Print() const override {}
  G4int GetGroupID() const { return fGroupID; }
  G4ParticleDefinition* GetParticle() const { return fParticle; }
  G4bool IsFinalProduct() const { return fFinalProduct; }

private:
  G4int fGroupID;
  G4ParticleDefinition* fParticle;
  G4bool fFinalProduct;
};

#endif
