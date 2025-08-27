#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "DetectorSD.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

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

DetectorConstruction::DetectorConstruction( PrimaryGeneratorAction* pga )
{
  fPGA = pga;
  fDetMessenger = new DetectorMessenger(this);

  fNistManager  = G4NistManager::Instance();

  G4UImanager* UI = G4UImanager::GetUIpointer();
  G4String command = "/control/execute macros/DetectorSetup.mac";
  UI->ApplyCommand(command);
}

//---------------------------------------------------------------------------

DetectorConstruction::~DetectorConstruction() 
{}

//---------------------------------------------------------------------------

G4VPhysicalVolume* DetectorConstruction::Construct()
{ 

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  // wood
  G4Element* H = new G4Element
    ("Hydrogen", "H", 1., 1.00794*g/mole );
  G4Element* O  = new G4Element
    ("Oxygen", "O" , 8., 16.00*g/mole );
  G4Element* C  = new G4Element
    ("Carbon", "C", 6. , 12.011*g/mole);

  G4Material* wood = new G4Material
    ("wood", 0.9*g/cm3, 3);
  wood->AddElement(H , 4);
  wood->AddElement(O , 1);
  wood->AddElement(C , 2);

  //---------------------------------------------------------------------------
  // Create Experimental Hall
  //---------------------------------------------------------------------------

  G4Box* expHall_box           = new G4Box("expHall_box",
					   1. *m, 1. *m, 1. *m );
  
  G4LogicalVolume* expHall_log = new G4LogicalVolume(expHall_box,
						     fNistManager->FindOrBuildMaterial("G4_AIR"),
						     "expHall_log", 0, 0, 0);

  fExpHall                     = new G4PVPlacement(0, G4ThreeVector(),
						   expHall_log, "expHall", 0, false, 0);

  //---------------------------------------------------------------------------
  // Create Sodium Iodide Detector Aluminium Case
  //---------------------------------------------------------------------------

  G4Tubs* detcase_tubs        = new G4Tubs("detcase_tubs",
					   0 *mm, 28.596 *mm, 28.45 *mm,
					   0. *deg, 360. *deg );
  
  G4LogicalVolume* detcase_log = new G4LogicalVolume(detcase_tubs,
						     fNistManager->FindOrBuildMaterial("G4_Al"),
						     "detcase_log", 0, 0, 0);
  
  fDetCase                  = new G4PVPlacement(0, G4ThreeVector(0.,-10. *mm, 28.45 *mm),
						detcase_log, "detcase", expHall_log, false, 0);

  //---------------------------------------------------------------------------
  //Create air gap between case and detector
  //---------------------------------------------------------------------------

  G4Tubs* airgap_tubs = new G4Tubs("airgap_tubs",
				   0. *cm, 26.4 *mm, 26.4 *mm,
				   0. *deg, 360. *deg);

  G4LogicalVolume* airgap_log = new G4LogicalVolume(airgap_tubs,
						    fNistManager->FindOrBuildMaterial("G4_AIR"),
						    "airgap_log", 0, 0, 0);

  fGapVol = new G4PVPlacement(0, G4ThreeVector(0., 0., 0. *mm),
			      airgap_log, "airgap", detcase_log, false, 0);

  //---------------------------------------------------------------------------
  // Create Sodium Iodide Detector
  //---------------------------------------------------------------------------

  G4Tubs* det_tubs        = new G4Tubs("det_tubs",
				       0. *cm, 25.4 *mm, 25.4 *mm,
				       0. *deg, 360. *deg );
  
  G4LogicalVolume* det_log = new G4LogicalVolume(det_tubs,
						 fNistManager->FindOrBuildMaterial("G4_SODIUM_IODIDE"),
						 "det_log", 0, 0, 0);
  
  fDetVol                  = new G4PVPlacement(0, G4ThreeVector(0.,0.,0. *mm),
					       det_log, "det", airgap_log, false, 0);

  //---------------------------------------------------------------------------
  // Create Dummy PMT and Base
  //---------------------------------------------------------------------------
 
  G4Tubs* pmt_tubs        = new G4Tubs("pmt_tubs",
					   0 *mm, 28.596 *mm, 70.0 *mm,
					   0. *deg, 360. *deg );
  
  G4LogicalVolume* pmt_log = new G4LogicalVolume(pmt_tubs,
						 fNistManager->FindOrBuildMaterial("G4_AIR"),
						 "pmt_log", 0, 0, 0);
  
  fPmt                  = new G4PVPlacement(0, G4ThreeVector(0.,-10. *mm, (28.45+28.45+70) *mm),
					    pmt_log, "pmt", expHall_log, false, 0);

  //---------------------------------------------------------------------------
  // Create Absorber Geometry
  //---------------------------------------------------------------------------

  G4Tubs* hole_tubs;
  G4Box* abs_box, *lead_box, *source_box, *desk_box;
  G4LogicalVolume* abs_log, *lead_log, *source_log, *hole_log, *desk_log;

  if( fAbsorberOn == 1 ) {

    abs_box           = new G4Box("abs_box",
				  6.0 *cm, 6.0 *cm, (fAbsorberThick/2) *mm );
    
    abs_log = new G4LogicalVolume(abs_box,
				  fNistManager->FindOrBuildMaterial(fAbsorberMaterial),
				  "abs_log", 0, 0, 0);
    
    fAbsorber                = new G4PVPlacement(0, G4ThreeVector(0., -10. *mm, -(50 + fAbsorberThick/2) *mm ),
						 abs_log, "abs", expHall_log, false, 0);
  }
  else {

    //---------------------------------------------------------------------------
    // Create Desk Surface
    //---------------------------------------------------------------------------
    
    desk_box           = new G4Box("desk_box",
				   0.3 *m, 10. *mm, 0.3 *m );
    
    desk_log = new G4LogicalVolume(desk_box,
				   wood,
				   "desk_log", 0, 0, 0);
    
    fDesk                     = new G4PVPlacement(0, G4ThreeVector(0., -60. *mm, 0.),
						  desk_log, "desk", expHall_log, false, 0);

    //---------------------------------------------------------------------------
    // Create Lead Shields
    //---------------------------------------------------------------------------
    
    lead_box           = new G4Box("lead_box",
				   12 *mm, 40. *mm, 100 *mm );
    
    lead_log = new G4LogicalVolume(lead_box,
				   fNistManager->FindOrBuildMaterial("G4_Pb"),
				   "lead_log", 0, 0, 0);
    
    fLead1                     = new G4PVPlacement(0, G4ThreeVector(70. *mm, -10. *mm, 30. *mm),
						   lead_log, "lead1", expHall_log, false, 0);
    
    fLead2                     = new G4PVPlacement(0, G4ThreeVector(-70. *mm, -10. *mm, 30. *mm),
						   lead_log, "lead2", expHall_log, false, 0);

    //---------------------------------------------------------------------------
    // Create Source Holder
    //---------------------------------------------------------------------------

    source_box          = new G4Box("source_box",
				     50. *mm, 40. *mm, 25. *mm );
    
    source_log          = new G4LogicalVolume(source_box,
				     fNistManager->FindOrBuildMaterial("G4_Pb"),
				     "source_log", 0, 0, 0);
    
    fSource             = new G4PVPlacement(0, G4ThreeVector(0. *mm, -10. *mm, -60. *mm),
					    source_log, "source", expHall_log, false, 0);


    hole_tubs         = new G4Tubs("hole_tubs",
				     0. *mm, 8. *mm, 25. *mm, 0. *deg, 360. *deg);
    
    hole_log          = new G4LogicalVolume(hole_tubs,
					      fNistManager->FindOrBuildMaterial("G4_AIR"),
					      "hole_log", 0, 0, 0);
    
    fHole             = new G4PVPlacement(0, G4ThreeVector(0. *mm, 0. *mm, 0. *mm),
					    hole_log, "hole", source_log, false, 0);

  }  

  //---------------------------------------------------------------------------
  // Set Sensitive Detectors
  //---------------------------------------------------------------------------
  
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  
  fDetSD = new DetectorSD("DetSD", 1);
  SDman->AddNewDetector( fDetSD );
  det_log->SetSensitiveDetector( fDetSD );
    
  //---------------------------------------------------------------------------
  // Set Visulisation Attributes
  //---------------------------------------------------------------------------
  
  G4VisAttributes* red     = new G4VisAttributes( G4Colour(1.0,0.0,0.0)   );
  G4VisAttributes* cyan    = new G4VisAttributes( G4Colour(0.0,1.0,1.0)   );

  expHall_log->SetVisAttributes(G4VisAttributes::GetInvisible());
  det_log->SetVisAttributes(red);
  pmt_log->SetVisAttributes(cyan);
    
  return fExpHall;
}

//---------------------------------------------------------------------------

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

//---------------------------------------------------------------------------
