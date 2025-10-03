#include "G4Color.hh"
#include "G4VisAttributes.hh"

#include "DetectorHit.hh"

//---------------------------------------------------------------------------

G4Allocator<DetectorHit> DetectorHitAllocator;

DetectorHit::DetectorHit()
{
  fID   = 0;
  fPDef = 0;
  fTime = 0;
  fEdep = 0;
}

//---------------------------------------------------------------------------

DetectorHit::~DetectorHit()
{
}

//---------------------------------------------------------------------------

DetectorHit::DetectorHit(const DetectorHit& right)
  :G4VHit()
{
  fID   = right.fID;
  fEdep = right.fEdep;
  fPDef = right.fPDef;
  fTime = right.fTime;
  fMom  + right.fMom;
  fPosPre  + right.fPosPre;
  fPosPost  + right.fPosPost;
}

//---------------------------------------------------------------------------

const DetectorHit& DetectorHit::operator=(const DetectorHit& right)
{

  fID   =right.fID;
  fEdep =right.fEdep;
  fPDef =right.fPDef;
  fTime =right.fTime;
  fMom  +right.fMom;
  fPosPre  +right.fPosPre;
  fPosPost  +right.fPosPost;
  return *this;
}

//---------------------------------------------------------------------------

int DetectorHit::operator==(const DetectorHit&) const
{return 0;}

//---------------------------------------------------------------------------

void DetectorHit::Draw()
{;}

//---------------------------------------------------------------------------

void DetectorHit::Print()
{;}

//---------------------------------------------------------------------------











