#ifndef DetectorHit_h
#define DetectorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"

//---------------------------------------------------------------------------

class DetectorHit : public G4VHit
{
public:
  
  DetectorHit();
  ~DetectorHit();
  DetectorHit(const DetectorHit&);
  const DetectorHit& operator=(const
						DetectorHit&);
  int operator==(const DetectorHit&) const;
  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  
  virtual void Draw();
  void Print();

protected:

  G4int                  fID; 
  G4ParticleDefinition*  fPDef; 
  G4double               fTime; 
  G4double               fEdep;
  G4ThreeVector          fMom;   
  G4ThreeVector          fPosPre; 
  G4ThreeVector          fPosPost; 

public:
  
  inline void SetID       (G4int i)                  { fID  = i;   };
  inline void SetPDef     (G4ParticleDefinition* pd) { fPDef = pd;  };
  inline void SetTime     (G4double t)               { fTime = t;   };
  inline void SetMomentum (G4ThreeVector m)          { fMom = m;    };
  inline void SetPrePosition (G4ThreeVector pos)     { fPosPre=pos;    };
  inline void SetPostPosition (G4ThreeVector pos)    { fPosPost=pos;    };
  inline void SetEnergy   (G4double de)              { fEdep = de; };
  
  inline G4int                 GetID()               { return fID;  };
  inline G4ParticleDefinition* GetPDef()             { return fPDef; };
  inline G4double              GetTime()             { return fTime; };
  inline G4ThreeVector         GetMom()              { return fMom; };
  inline G4ThreeVector         GetPosPre()           { return fPosPre; };
  inline G4ThreeVector         GetPosPost()          { return fPosPost; };
  inline G4double              GetEdep()             { return fEdep; };  
};

//---------------------------------------------------------------------------

typedef G4THitsCollection<DetectorHit> DetectorHitsCollection;

extern G4Allocator<DetectorHit> DetectorHitAllocator;


inline void* DetectorHit::operator new(size_t)
{
  void* aHit;
  aHit = (void*) DetectorHitAllocator.MallocSingle();
  return aHit;
}


inline void DetectorHit::operator delete(void* aHit)
{
  DetectorHitAllocator.FreeSingle((DetectorHit*) aHit);
}

#endif

//---------------------------------------------------------------------------










