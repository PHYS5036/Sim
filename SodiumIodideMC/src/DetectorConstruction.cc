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

DetectorConstruction::DetectorConstruction( PrimaryGeneratorAction* pga,int commandLength )
{
  fPGA = pga;
  fDetMessenger = new DetectorMessenger(this);

  fNistManager  = G4NistManager::Instance();

  if(commandLength==1){
    G4UImanager* UI = G4UImanager::GetUIpointer();
    G4String command = "/control/execute macros/DetectorSetup.mac";
    UI->ApplyCommand(command);
  }
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

  //---------------------------------------------------------------------------
  // Define Materials
  //---------------------------------------------------------------------------

  // Wood
  G4Element* H = new G4Element("Hydrogen", "H", 1., 1.00794*g/mole );
  G4Element* O  = new G4Element("Oxygen", "O" , 8., 16.00*g/mole );
  G4Element* C  = new G4Element("Carbon", "C", 6. , 12.011*g/mole);

  G4Material* wood = new G4Material("wood", 0.9*g/cm3, 3);
  wood->AddElement(H , 4);
  wood->AddElement(O , 1);
  wood->AddElement(C , 2);


  // GAGG(Ce) - Gadolinium Aluminium Gallium Garnet (Gd₃Al₂Ga₃O₁₂)
  G4Element* Gd = new G4Element("Gadolinium", "Gd", 64., 157.25*g/mole);
  G4Element* Al = new G4Element("Aluminium", "Al", 13., 26.982*g/mole);
  G4Element* Ga = new G4Element("Gallium", "Ga", 31., 69.723*g/mole);
  G4Element* Ce = new G4Element("Cerium", "Ce", 58., 140.116*g/mole);

  G4Material* GAGG = new G4Material("GAGG", 6.63*g/cm3, 5);
  GAGG->AddElement(Ce, 1*perCent);
  GAGG->AddElement(Gd, 50.*perCent);
  GAGG->AddElement(Al, 6.*perCent);
  GAGG->AddElement(Ga, 23.*perCent);
  GAGG->AddElement(O, 20.*perCent);

  // PLA+ (Polylactic Acid) - 3D printing material
  //  (elemental breakdown is an approximation - can't find manufacturer source but used ratios from: https://doi.org/10.1016/j.apradiso.2023.110908 )
  G4Material* PLA = new G4Material("PLA", 1.30*g/cm3, 3);
  PLA->AddElement(C, 50.*perCent);
  PLA->AddElement(O, 44.46*perCent);
  PLA->AddElement(H, 5.54*perCent);


  //---------------------------------------------------------------------------
  // 1. Create Experimental Hall
  //---------------------------------------------------------------------------

  G4Box* expHall_box           = new G4Box("expHall_box",
					   1. *m, 1. *m, 1. *m );
  
  G4LogicalVolume* expHall_log = new G4LogicalVolume(expHall_box,
						     fNistManager->FindOrBuildMaterial("G4_AIR"),
						     "expHall_log", 0, 0, 0);

  fExpHall                     = new G4PVPlacement(0, G4ThreeVector(),
						   expHall_log, "expHall", 0, false, 0);

  //---------------------------------------------------------------------------
  // 2.1 Create GAGG Detector Case -- Front part is solid with crystal, back part hollow with air void (where electronics go) 
  //---------------------------------------------------------------------------
  
  // Define case dimensions
  G4Tubs* detcase_tubs = new G4Tubs("detcase_tubs",
                                    0 *mm, 14*mm, 43.75 *mm, // inner radius (0 for solid), outer radius, half-length of cylinder
                                    0. *deg, 360. *deg );    // starting phi, segment angle
  
  // Set logical volume for case (the material)
  G4LogicalVolume* detcase_log = new G4LogicalVolume(detcase_tubs,
						     fNistManager->FindOrBuildMaterial("PLA"),
						     "detcase_log", 0, 0, 0);
  
  // Set physical volume for case (the placement in the world: rotation, position)
  fDetCase = new G4PVPlacement(0, G4ThreeVector(0., 0. *mm, 15.00 *mm),
						                  detcase_log, "detcase", expHall_log, false, 0);

    //---------------------------------------------------------------------------
    // 2.2 Create and place air gap (where electronics sit) at back of detector
    //---------------------------------------------------------------------------

    G4Tubs* airgap_tubs = new G4Tubs("airgap_tubs",
            0. *mm, 12.0 *mm, 26.75 *mm,
            0. *deg, 360. *deg);

    G4LogicalVolume* airgap_log = new G4LogicalVolume(airgap_tubs,
                  fNistManager->FindOrBuildMaterial("G4_AIR"),
                  "airgap_log", 0, 0, 0);

    fGapVol = new G4PVPlacement(0, G4ThreeVector(0., 0., 15. *mm),
              airgap_log, "airgap", detcase_log, false, 0);


    //---------------------------------------------------------------------------
    // 2.3 Create and place GAGG Crystal
    //---------------------------------------------------------------------------
    G4Box* det_box = new G4Box("det_box",
                6.75 *mm, 6.75 *mm, 10.10 *mm);
    
    G4LogicalVolume* det_log = new G4LogicalVolume(det_box,
              fNistManager->FindOrBuildMaterial("GAGG"),
              "det_log", 0, 0, 0);
    
    fDetVol = new G4PVPlacement(0, G4ThreeVector(0., 0., -31.65*mm),
                  det_log, "det", detcase_log, false, 0);


  //---------------------------------------------------------------------------
  // Create Absorber Geometry - LEGACY
  //---------------------------------------------------------------------------

  G4Tubs* hole_tubs;
  G4Box* abs_box, *lead_box, *source_box, *desk_box;
  G4LogicalVolume* abs_log, *lead_log, *source_log, *hole_log, *desk_log;

  // Legacy code from OLD NAI setup - version for attenuation experiment tbc.
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
				   .3 *m, 10. *mm, .3 *m );
    
    desk_log = new G4LogicalVolume(desk_box,
				   wood,
				   "desk_log", 0, 0, 0);
    
    fDesk                     = new G4PVPlacement(0, G4ThreeVector(0., -55. *mm, 0.),
						  desk_log, "desk", expHall_log, false, 0);

    //---------------------------------------------------------------------------
    // Create Lead Shields
    //---------------------------------------------------------------------------
    
    lead_box           = new G4Box("lead_box",
				   12.5 *mm, 50. *mm, 102.5 *mm );
    
    lead_log = new G4LogicalVolume(lead_box,
				   fNistManager->FindOrBuildMaterial("G4_Pb"),
				   "lead_log", 0, 0, 0);
    
    fLead1                     = new G4PVPlacement(0, G4ThreeVector(62. *mm, 0., -30. *mm),
						   lead_log, "lead1", expHall_log, false, 0);
    
    fLead2                     = new G4PVPlacement(0, G4ThreeVector(-62. *mm, 0., -30. *mm),
						   lead_log, "lead2", expHall_log, false, 0);

    //---------------------------------------------------------------------------
    // Create Source Holder - Lead box with air hole for stick source
    //---------------------------------------------------------------------------
    
    source_box          = new G4Box("source_box",
				     47.5 *mm, 50. *mm, 25. *mm );
    
    source_log          = new G4LogicalVolume(source_box,
				     fNistManager->FindOrBuildMaterial("G4_Pb"),
				     "source_log", 0, 0, 0);
    
    fSource             = new G4PVPlacement(0, G4ThreeVector(0., 0., -60. *mm),
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

  expHall_log->SetVisAttributes(G4VisAttributes::GetInvisible());
  // hole_log->SetVisAttributes(G4VisAttributes::GetInvisible());
  airgap_log->SetVisAttributes(G4VisAttributes::GetInvisible());

  G4VisAttributes* blue    = new G4VisAttributes( G4Colour::Blue() );
  blue->SetForceSolid(true);
  hole_log->SetVisAttributes(blue);
  
  G4VisAttributes* yellow     = new G4VisAttributes( G4Colour::Yellow()   );
  yellow->SetForceSolid(true);
  det_log->SetVisAttributes(yellow);

  G4VisAttributes* red    = new G4VisAttributes( G4Colour(1.,0.,0.,0.55)   );
  red->SetForceSolid(true);
  // cyan->SetForceWireframe(true);
  // cyan->SetLineWidth(5.);
  detcase_log->SetVisAttributes(red);

  G4VisAttributes* white   = new G4VisAttributes( G4Colour::White() );
  white->SetForceSolid(true);
  source_log->SetVisAttributes(white);
  desk_log->SetVisAttributes(white);

  G4VisAttributes* grey    = new G4VisAttributes( G4Colour(0.5, 0.5, 0.5,0.35) );
  grey->SetForceSolid(true);
  lead_log->SetVisAttributes(grey);
  source_log->SetVisAttributes(grey);

  // G4VisAttributes* brown   = new G4VisAttributes( G4Colour(0.45,0.45,0.));
  // brown->SetForceSolid(true);
  // desk_log->SetVisAttributes(brown);
    
  return fExpHall;
}

//---------------------------------------------------------------------------

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

//---------------------------------------------------------------------------
