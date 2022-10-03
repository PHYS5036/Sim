#include <stdexcept>
#include <iostream>
#include "globals.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "AnalysisManager.hh"

#include "G4VisExecutive.hh"

#include "G4UIExecutive.hh"

//---------------------------------------------------------------------------

int main(int argc, char** argv)
{

  G4RunManager* runManager = new G4RunManager;
  PhysicsList*  phys       = new PhysicsList();
  runManager->SetUserInitialization(phys);

  PrimaryGeneratorAction* pga        = new PrimaryGeneratorAction();
  AnalysisManager*        anaManager = new AnalysisManager();
  EventAction*            event      = new EventAction( anaManager, pga );
  DetectorConstruction*   detCon     = new DetectorConstruction();
  runManager->SetUserInitialization(detCon);

  runManager->SetUserAction(pga);
  runManager->SetUserAction(event);

  G4UImanager * UI         = G4UImanager::GetUIpointer();
  G4String session;

  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive("quite");
  visManager->Initialize();

  if (argc==1)   // Define UI session for interactive mode.
    {
      G4UIExecutive * ui = new G4UIExecutive(argc,argv,session);
      UI->ApplyCommand("/control/execute macros/vis.mac");
      ui->SessionStart();
      delete ui;
    }
  else           // Batch mode
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UI->ApplyCommand(command+fileName);
    }

  anaManager->WriteOutput();

  delete visManager;
  delete runManager;

  return 0;
}

//---------------------------------------------------------------------------
