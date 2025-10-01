#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"

//---------------------------------------------------------------------------

class G4VPhysicalVolume;
class DetectorSD;
class DetectorMessenger;
class PrimaryGeneratorAction;

//---------------------------------------------------------------------------

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  DetectorConstruction( PrimaryGeneratorAction*, int commands=1 );
  ~DetectorConstruction();

  G4VPhysicalVolume* Construct();

  void UpdateGeometry();

  inline G4VPhysicalVolume* GetExpHall()    { return fExpHall;  };
  inline G4VPhysicalVolume* GetDetVol()     { return fDetVol;   };
  inline DetectorSD*        GetDetSD()      { return fDetSD;    };

  inline void SetDetectorType     ( G4int type )     { fDetectorType     = type; }
  inline void SetDetectorDistance ( G4double dist )  { fDetectorDistance = dist; }

  inline void SetAbsorberOn        ( G4int    abson )  { fAbsorberOn       = abson; }
  inline void SetAbsorberMaterial  ( G4String mname )  { fAbsorberMaterial = mname; }
  inline void SetAbsorberThickness ( G4double thick )  { fAbsorberThick    = thick; }

  private:

  // Init members
  G4NistManager*     fNistManager;
  DetectorMessenger* fDetMessenger;
  PrimaryGeneratorAction* fPGA;

  // Physical Volumes
  G4VPhysicalVolume* fExpHall;
  G4VPhysicalVolume* fDesk;
  G4VPhysicalVolume* fDetVol;
  G4VPhysicalVolume* fAbsorber;
  G4VPhysicalVolume* fDetCase;
  G4VPhysicalVolume* fGapVol;
  G4VPhysicalVolume* fPmt;
  G4VPhysicalVolume* fSource;
  G4VPhysicalVolume* fShield;

  G4VPhysicalVolume* fLead1;
  G4VPhysicalVolume* fLead2;
  G4VPhysicalVolume* fLead3;
  G4VPhysicalVolume* fLead4;
  G4VPhysicalVolume* fLead5;

  DetectorSD*        fDetSD;

  G4int              fDetectorType;
  G4double           fDetectorDistance;

  G4int              fAbsorberOn;
  G4String           fAbsorberMaterial;
  G4double           fAbsorberThick;

};
#endif

//---------------------------------------------------------------------------

