#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "DetectorSD.hh"

#include "G4Material.hh"
#include "G4BooleanSolid.hh"
#include "G4CSGSolid.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4RotationMatrix.hh"
#include "G4UserLimits.hh"

#include "G4TransportationManager.hh"
#include "G4SDManager.hh"

#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4VisAttributes.hh"
#include "G4String.hh"
#include "globals.hh"

using namespace CLHEP;

//---------------------------------------------------------------------------

DetectorConstruction::DetectorConstruction(int commandLength)
{
  fDetMessenger = new DetectorMessenger(this);

  fNistManager  = G4NistManager::Instance();

  fTumourRadius = 0.5;
  fTumourHeight = 0.0;
  
  if(commandLength==1){
    G4UImanager* UI = G4UImanager::GetUIpointer();
    G4String command = "/control/execute macros/DetectorSetup.mac";
    UI->ApplyCommand(command);
  }

}

//---------------------------------------------------------------------------

DetectorConstruction::~DetectorConstruction() 
{
}

//---------------------------------------------------------------------------

G4VPhysicalVolume* DetectorConstruction::Construct()
{ 
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  //vacuum
  G4double vacuumDensity  = 1.e-25 *g/cm3;
  G4double pressure       = 3.e-18*pascal;
  G4double temperature    = 2.73*kelvin;
  G4Material* vacuum      = new G4Material("Galactic", 1., 1.01*g/mole,
					   vacuumDensity,kStateGas,temperature,pressure);

  //---------------------------------------------------------------------------
  // Create Experimental Hall
  //---------------------------------------------------------------------------

  G4Box* expHall_box           = new G4Box("expHall_box",
					   1.5 *m, 1.5 *m, 1.5 *m );
  
  G4LogicalVolume* expHall_log = new G4LogicalVolume(expHall_box,
						     vacuum,
						     "expHall_log", 0, 0, 0);
  
  fExpHall                     = new G4PVPlacement(0, G4ThreeVector(),
						   expHall_log, "expHall", 0, false, 2);

  //---------------------------------------------------------------------------
  // Create Water Phantom for Radiotherapy
  //---------------------------------------------------------------------------

  G4RotationMatrix* phant_rm  = new G4RotationMatrix();
  phant_rm->rotateY(90. *deg);
  
  G4Tubs* det_tubs            = new G4Tubs("det_tubs",
					   0. *mm, 13.0 *cm, 29.34 *cm,
					   0. *deg, 360. *deg );
  
  G4LogicalVolume* det1_log = new G4LogicalVolume(det_tubs,
						  fNistManager->FindOrBuildMaterial("G4_WATER"),
						  "det1_log", 0, 0, 0);
  
  fDet1Vol                  = new G4PVPlacement(phant_rm, G4ThreeVector(0.,0.,0.),
					       det1_log, "det1", expHall_log, false, 0);

  //---------------------------------------------------------------------------
  // Set Step Limits, Sensitive Detector and Visualisation
  //---------------------------------------------------------------------------

  G4double maxStep = 0.5 *mm;;
  G4UserLimits* stepLimit = new G4UserLimits(maxStep);
  //  det1_log->SetUserLimits(stepLimit);

  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  fDetSD = new DetectorSD("DetSD", 2);
  SDman->AddNewDetector( fDetSD );
  det1_log->SetSensitiveDetector( fDetSD );
  
  G4VisAttributes* blue    = new G4VisAttributes( G4Colour(0.0,0.0,1.0)   );
  G4VisAttributes* red     = new G4VisAttributes( G4Colour(1.0,0.0,0.0)   );

  expHall_log->SetVisAttributes(G4VisAttributes::GetInvisible());
  det1_log->SetVisAttributes(blue);

  //---------------------------------------------------------------------------

  G4Sphere* det_sph;
  G4LogicalVolume* det2_log;
 
  if( fTumourOn == 1 ) {
    
    det_sph            = new G4Sphere("det_sph",
				      0. *mm, fTumourRadius *cm,
				      0. *deg, 360. *deg,
				      0. *deg, 180. *deg );
    
    det2_log = new G4LogicalVolume(det_sph,
				   fNistManager->FindOrBuildMaterial("G4_WATER"),
				   "det2_log", 0, 0, 0);
    
    fDet2Vol                  = new G4PVPlacement(0, G4ThreeVector(0., fTumourHeight *cm, 0.),
						  det2_log, "det2", det1_log, false, 1);

    //    det2_log->SetUserLimits(stepLimit);
    det2_log->SetSensitiveDetector( fDetSD );
    det2_log->SetVisAttributes(red);

  }

  return fExpHall;
}

//---------------------------------------------------------------------------

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

//---------------------------------------------------------------------------
