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

DetectorConstruction::DetectorConstruction( PrimaryGeneratorAction* pga,int commandLength ) {

  fPGA = pga;
  fDetMessenger = new DetectorMessenger(this);
  fNistManager  = G4NistManager::Instance();

  // fSourceHolderType = 0; // New 3D printed source holder, default
  // fDetectorType   = 0;   // Sodium Iodide Detector, default
  // fDetectorDistance = 100. *mm; // Distance from source to detector front face

  // For attentuation, this is fixed at roughly 55mm, with absorber thickness centered in this.
  if (fAbsorberOn==1) {

    // Ensure detector distance is not too close (based on 3D printed source holder length)
    if (fDetectorDistance < 55. *mm) {
      G4cout << "### WARNING: Detector distance too close for absorber thickness - setting to 55mm" << G4endl;
      fDetectorDistance = 55. *mm;
    }

    // Check absorber thickness is not too large for detector distance
    if (fAbsorberThick > (fDetectorDistance - 10.*mm) ) {
      G4cout << "### WARNING: Absorber thickness too large for detector distance - setting to " << (fDetectorDistance - 10.*mm)/mm << " mm" << G4endl;
      fAbsorberThick = fDetectorDistance - 10.*mm;
    }
  }

  if(commandLength==1){
    G4UImanager* UI = G4UImanager::GetUIpointer();
    G4String command = "/control/execute macros/DetectorSetup.mac";
    UI->ApplyCommand(command);
  }

}

//---------------------------------------------------------------------------

DetectorConstruction::~DetectorConstruction() {
}

//---------------------------------------------------------------------------

G4VPhysicalVolume* DetectorConstruction::Construct() { 

  std::cout << "### DetectorConstruction::Construct()" << std::endl;

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  // --------------------------------------------------------------------------
  // Set Visulaisation Colours
  // --------------------------------------------------------------------------

  G4VisAttributes* red    = new G4VisAttributes( G4Colour(1.,0.,0.,0.55)   );
  red->SetForceSolid(true);

  G4VisAttributes* blue    = new G4VisAttributes( G4Colour(0.,0.,1.,0.5) );
  blue->SetForceSolid(true);

  G4VisAttributes* cyan = new G4VisAttributes( G4Colour(0., 1., 1., 0.5) );
  cyan->SetForceSolid(true);

  G4VisAttributes* grey    = new G4VisAttributes( G4Colour(0.5, 0.5, 0.5,0.35) );
  grey->SetForceSolid(true);

  G4VisAttributes* brown   = new G4VisAttributes( G4Colour(0.45,0.45,0.));
  brown->SetForceSolid(true);

  G4VisAttributes* green    = new G4VisAttributes( G4Colour::Green() );
  green->SetForceSolid(true);

  G4VisAttributes* yellow     = new G4VisAttributes(G4Colour::Yellow());
  yellow->SetForceSolid(true);

  G4VisAttributes* white   = new G4VisAttributes( G4Colour::White() );
  white->SetForceSolid(true);

  G4VisAttributes* magenta = new G4VisAttributes( G4Colour::Magenta() );
  magenta->SetForceSolid(true);


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


  // GAGG(:Ce) - Gadolinium Aluminium Gallium Garnet (Gd₃Al₂Ga₃O₁₂)
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

  std::cout << "Created Materials" << G4endl;
  //---------------------------------------------------------------------------
  // 1. Create Experimental Hall
  //---------------------------------------------------------------------------

  G4Box* expHall_box = new G4Box("expHall_box", 1. *m, 1. *m, 1. *m );
  G4LogicalVolume* expHall_log = new G4LogicalVolume(expHall_box, fNistManager->FindOrBuildMaterial("G4_AIR"), "expHall_log", 0, 0, 0);
  fExpHall = new G4PVPlacement(0, G4ThreeVector(), expHall_log, "expHall", 0, false, 0);
  expHall_log->SetVisAttributes(G4VisAttributes::GetInvisible());
  

  //---------------------------------------------------------------------------
  // 2. Create Detector
  //---------------------------------------------------------------------------
  
  G4LogicalVolume* det_log;
  G4LogicalVolume* detcase_log;
  G4LogicalVolume* pmt_log;

  G4double holder_offset;
  if (fSourceHolderType == 0) {
    holder_offset = 12. *mm; // New 3D printed source holder
  } else if (fSourceHolderType == 1) {
    holder_offset = 2 *mm; // Old pencil source holder
  }
  G4double det_length;
  G4double det_offset;

  if (fDetectorType == 0) {
    //---------------------------------------------------------------------------
    // 2.1.0 Create GAGG Detector Case -- Front part is solid with crystal, back part hollow with air void (where electronics go) 
    //---------------------------------------------------------------------------

    // Define case dimensions
    det_length = 87.5 *mm; // total length of case
    det_offset = holder_offset + fDetectorDistance + det_length/2; // distance from source origin to centre of detector case

    G4Tubs* detcase_tubs = new G4Tubs("detcase_tubs",
                                      0 *mm, 14*mm, det_length/2, // inner radius (0 for solid), outer radius, half-length of cylinder
                                      0. *deg, 360. *deg );    // starting phi, segment angle
    // Set logical volume for case (the material)
    detcase_log = new G4LogicalVolume(detcase_tubs, fNistManager->FindOrBuildMaterial("PLA"), "detcase_log", 0, 0, 0);
    // Set physical volume for case (the placement in the world: rotation, position)
    fDetCase = new G4PVPlacement(0, G4ThreeVector(0., 0. *mm, det_offset), detcase_log, "detcase", expHall_log, false, 0);
    detcase_log->SetVisAttributes(red);

    //---------------------------------------------------------------------------
    // 2.1.1 Create and place air gap (where electronics sit) at back of detector
    //---------------------------------------------------------------------------

    G4Tubs* airgap_tubs = new G4Tubs("airgap_tubs", 0. *mm, 12.0 *mm, 26.75 *mm, 0. *deg, 360. *deg);
    G4LogicalVolume* airgap_log = new G4LogicalVolume(airgap_tubs, fNistManager->FindOrBuildMaterial("G4_AIR"), "airgap_log", 0, 0, 0);
    fGapVol = new G4PVPlacement(0, G4ThreeVector(0., 0., 15. *mm), airgap_log, "airgap", detcase_log, false, 0);


    //---------------------------------------------------------------------------
    // 2.1.2 Create and place GAGG Crystal
    //---------------------------------------------------------------------------
    G4Box* det_box = new G4Box("det_box", 6.75 *mm, 6.75 *mm, 10.10 *mm);
    det_log = new G4LogicalVolume(det_box, fNistManager->FindOrBuildMaterial("GAGG"), "det_log", 0, 0, 0);
    fDetVol = new G4PVPlacement(0, G4ThreeVector(0., 0., -31.65*mm), det_log, "det", detcase_log, false, 0);
    det_log->SetVisAttributes(yellow);

  } else if (fDetectorType == 1) {
    //---------------------------------------------------------------------------
    // 2.2.0 Create Sodium Iodide Detector Aluminium Case
    //---------------------------------------------------------------------------

    det_length = 56.9 *mm; // total length of case
    det_offset = holder_offset + fDetectorDistance + det_length/2; // distance from source origin to centre of detector case

    G4Tubs* detcase_tubs = new G4Tubs("detcase_tubs", 0 *mm, 28.596 *mm, det_length/2, 0. *deg, 360. *deg );
    detcase_log          = new G4LogicalVolume(detcase_tubs, fNistManager->FindOrBuildMaterial("G4_Al"), "detcase_log", 0, 0, 0);
    fDetCase             = new G4PVPlacement(0, G4ThreeVector(0.,0., det_offset), detcase_log, "detcase", expHall_log, false, 0);
    detcase_log->SetVisAttributes(red);

    //---------------------------------------------------------------------------
    // 2.2.1 Create air gap between detector case and detector
    //---------------------------------------------------------------------------
    G4Tubs* airgap_tubs = new G4Tubs("airgap_tubs", 0. *cm, 26.4 *mm, 26.4 *mm, 0. *deg, 360. *deg);
    G4LogicalVolume* airgap_log = new G4LogicalVolume(airgap_tubs, fNistManager->FindOrBuildMaterial("G4_AIR"), "airgap_log", 0, 0, 0);
    fGapVol = new G4PVPlacement(0, G4ThreeVector(0., 0., 0. *mm), airgap_log, "airgap", detcase_log, false, 0);

    //---------------------------------------------------------------------------
    // 2.2.2 Create Sodium Iodide Detector
    //---------------------------------------------------------------------------

    G4Tubs* det_tubs = new G4Tubs("det_tubs", 0. *cm, 25.4 *mm, 25.4 *mm, 0. *deg, 360. *deg );
    det_log = new G4LogicalVolume(det_tubs, fNistManager->FindOrBuildMaterial("G4_SODIUM_IODIDE"), "det_log", 0, 0, 0);
    fDetVol = new G4PVPlacement(0, G4ThreeVector(0.,0.,0. *mm), det_log, "det", airgap_log, false, 0);
    det_log->SetVisAttributes(yellow);
    
    //---------------------------------------------------------------------------
    // 2.2.3 Create Dummy PMT and Base
    //---------------------------------------------------------------------------
  
    G4Tubs* pmt_tubs = new G4Tubs("pmt_tubs", 0 *mm, 28.596 *mm, 70.0 *mm, 0. *deg, 360. *deg );
    pmt_log = new G4LogicalVolume(pmt_tubs, fNistManager->FindOrBuildMaterial("G4_AIR"), "pmt_log", 0, 0, 0);
    fPmt = new G4PVPlacement(0, G4ThreeVector(0.,0., (det_offset+det_length/2+70) *mm), pmt_log, "pmt", expHall_log, false, 0);
    pmt_log->SetVisAttributes(cyan);

  } else {
    G4cout << "Detector type not recognised - no detector constructed" << G4endl
            << "  (0 = GAGG, 1 = Sodium Iodide)" << G4endl;
  }
  //---------------------------------------------------------------------------
  // 3. Create Source Holder 
  //---------------------------------------------------------------------------
  
  G4LogicalVolume* LeadShield_log;
  G4LogicalVolume* source_log;

  if (fSourceHolderType == 0) {

    //---------------------------------------------------------------------------
    // 3.1 Create New Source Holder (3D printed plastic with lead lining)
    //---------------------------------------------------------------------------
    
    // Plastic outer holder
    G4Tubs* plastic_holder_tubs = new G4Tubs("plastic_holder_tubs", 17.915 *mm, 18.915 *mm, 55. *mm, 0. *deg, 360. *deg);
    source_log = new G4LogicalVolume(plastic_holder_tubs, fNistManager->FindOrBuildMaterial("PLA"), "source_log", 0, 0, 0);
    fSource = new G4PVPlacement(0, G4ThreeVector(0., 0., -43. *mm), source_log, "plastic_holder", expHall_log, false, 0);
    source_log->SetVisAttributes(blue);
    
    // Add lead lining
    G4Tubs* lead_lining_tubs = new G4Tubs("lead_lining_tubs", 15.915 *mm, 17.914 *mm, 50. *mm, 0. *deg, 360. *deg);
    LeadShield_log = new G4LogicalVolume(lead_lining_tubs, fNistManager->FindOrBuildMaterial("G4_Pb"), "LeadShield_log", 0, 0, 0);
    fShield = new G4PVPlacement(0, G4ThreeVector(0., 0., -5. *mm), LeadShield_log, "lead_lining", source_log, false, 0);
    LeadShield_log->SetVisAttributes(green);


  } else if (fSourceHolderType == 1) {

    //---------------------------------------------------------------------------
    // 3.2 Create Old Pencil Source Holder (lead block with hole) -- LEGACY
    //---------------------------------------------------------------------------

    // Lead Block
    G4Box* LeadShield_box = new G4Box("LeadShield_box", 47.5 *mm, 50. *mm, 25. *mm );
    LeadShield_log = new G4LogicalVolume(LeadShield_box, fNistManager->FindOrBuildMaterial("G4_Pb"), "LeadShield_log", 0, 0, 0);
    fShield = new G4PVPlacement(0, G4ThreeVector(0., 0., 0. *mm), LeadShield_log, "source", expHall_log, false, 0);
    LeadShield_log->SetVisAttributes(green);

    // Add hole for source
    G4Tubs* source_tubs    = new G4Tubs("source_tubs", 0. *mm, 8. *mm, 25. *mm, 0. *deg, 360. *deg);
    source_log     = new G4LogicalVolume(source_tubs, fNistManager->FindOrBuildMaterial("G4_AIR"), "source_log", 0, 0, 0);
    fSource        = new G4PVPlacement(0, G4ThreeVector(0. *mm, 0. *mm, 0. *mm), source_log, "source", LeadShield_log, false, 0);
  
  }
  //---------------------------------------------------------------------------
  // 4. Create Absorber Geometry
  //---------------------------------------------------------------------------

  G4Tubs* hole_tubs;
  G4Box* abs_box, *lead_box, *desk_box;
  G4LogicalVolume* abs_log, *lead_log, *desk_log;

  if (fAbsorberOn == 1) {
    std::cout << "Creating Absorber" << std::endl;
    G4double absorber_offset = holder_offset + (fDetectorDistance/2);

    abs_box = new G4Box("abs_box", 7.0/2 *cm, 7.0/2 *cm, (fAbsorberThick/2)*mm);
    abs_log = new G4LogicalVolume(abs_box, fNistManager->FindOrBuildMaterial(fAbsorberMaterial), "abs_log", 0, 0, 0);
    fAbsorber = new G4PVPlacement(0, G4ThreeVector(0., 0., absorber_offset), abs_log, "abs", expHall_log, false, 0);
    abs_log->SetVisAttributes(magenta);
  }

  //---------------------------------------------------------------------------
  // 5. Create Desk Surface
  //---------------------------------------------------------------------------
  
  desk_box = new G4Box("desk_box",.2 *m, 10. *mm, .4 *m );
  desk_log = new G4LogicalVolume(desk_box, wood, "desk_log", 0, 0, 0);
  fDesk = new G4PVPlacement(0, G4ThreeVector(0., -55. *mm, 100. *mm), desk_log, "desk", expHall_log, false, 0);
  desk_log->SetVisAttributes(grey);

  //---------------------------------------------------------------------------
  // 6. Create Lead Shields - 1 behind source holder, and to 4 sides of detector 
  //---------------------------------------------------------------------------
  
  lead_box = new G4Box("lead_box", 12.5 *mm, 50. *mm, 102.5 *mm );
  lead_log = new G4LogicalVolume(lead_box, fNistManager->FindOrBuildMaterial("G4_Pb"), "lead_log", 0, 0, 0);
  
  fLead1 = new G4PVPlacement(0, G4ThreeVector(90. *mm, 0., -25. *mm), lead_log, "lead1", expHall_log, false, 0);
  fLead2 = new G4PVPlacement(0, G4ThreeVector(-90. *mm, 0., -25. *mm), lead_log, "lead2", expHall_log, false, 0);
  fLead3 = new G4PVPlacement(0, G4ThreeVector(90. *mm, 0., 185. *mm), lead_log, "lead1", expHall_log, false, 0);
  fLead4 = new G4PVPlacement(0, G4ThreeVector(-90. *mm, 0., 185. *mm), lead_log, "lead2", expHall_log, false, 0);

  G4RotationMatrix* rotm = new G4RotationMatrix();
  rotm->rotateY(90.*deg);
  fLead5 = new G4PVPlacement(rotm, G4ThreeVector(0., 0., -145.0 *mm), lead_log, "lead5", expHall_log, false, 0);

  //---------------------------------------------------------------------------
  // Set Sensitive Detectors
  //---------------------------------------------------------------------------
  
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  fDetSD = new DetectorSD("DetSD", 1);
  SDman->AddNewDetector( fDetSD );
  det_log->SetSensitiveDetector( fDetSD );

  // ----------------
    
  std::cout <<"### Geometry Created" << std::endl;
  return fExpHall;
}

//---------------------------------------------------------------------------

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

//---------------------------------------------------------------------------
