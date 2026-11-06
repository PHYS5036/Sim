#ifndef TrackOriginInfo_h
#define TrackOriginInfo_h 1

#include "G4VUserTrackInformation.hh"

enum class TrackOrigin {
  Gamma,
  Beta
};

class TrackOriginInfo : public G4VUserTrackInformation
{
public:
  explicit TrackOriginInfo(TrackOrigin origin) : fOrigin(origin) {}
  void Print() const override {}
  TrackOrigin GetOrigin() const { return fOrigin; }

private:
  TrackOrigin fOrigin;
};

#endif
