#include <stdexcept>
#include <iostream>
#include "globals.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4UIExecutive.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "AnalysisManager.hh"

#include "G4VisExecutive.hh"

//---------------------------------------------------------------------------

int main(int argc, char** argv)
{
  G4RunManager*           runManager = new G4RunManager();
  PhysicsList*            phys       = new PhysicsList();
  runManager->SetUserInitialization(phys);

  PrimaryGeneratorAction* pga        = new PrimaryGeneratorAction();
  AnalysisManager*        anaManager = new AnalysisManager();
  EventAction*            event      = new EventAction( anaManager, pga );
  DetectorConstruction*   detCon     = new DetectorConstruction( pga, argc );

  runManager->SetUserInitialization(detCon);

  runManager->SetUserAction(pga);
  runManager->SetUserAction(event);

  G4UImanager * UI         = G4UImanager::GetUIpointer();
  G4VisManager* visManager = 0;

  if (argc==1) {

  // ----------------------------------------
  // LEGACY CODE - W.O couldnt get it working, replaced with new code. Also updated vis.mac settings.

    // std::cout << "Interactive mode" << std::endl;
    // #ifdef G4VIS_USE
    //     std::cout << "Initialize visualization" << std::endl;
    //     visManager = new G4VisExecutive;
    //     visManager->Initialize();
    // #endif
    //     G4UIsession * session = 0;
    // #ifdef G4UI_USE_TCSH
    //     std::cout << "Using tcsh" << std::endl;
    //     session = new G4UIterminal(new G4UItcsh);
    // #else
    //     session = new G4UIterminal();
    // #endif
    //     session->SessionStart();
    //     delete session;
  // ----------------------------------------


    // Initialize the visualization
    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    std::cout << "Visualization manager initialized." << std::endl;

    // Start UI session
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    std::cout << "UI session started." << std::endl;

    UImanager->ApplyCommand("/control/execute ./macros/vis.mac");

    std::cout << "Visualization macro executed." << std::endl;

    // runManager->BeamOn(1);

    // std::cout << "Beam on complete." << std::endl;

    // Start the interactive session
    ui->SessionStart();

    // Clean up
    if (ui) delete ui; // Delete the UI manager first if it exists
    if (visManager) delete visManager; // Then delete the Visualization Manager
    if (runManager) delete runManager; // Finally, delete the Run Manager

  } else {
    std::cout << "Batch mode" << std::endl;
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UI->ApplyCommand(command+fileName);

    if (pga->GetMode()==EPGA_ROOT ) {
        G4String commandr = "/run/beamOn ";
        G4int nev = pga->GetNEvents();
        char snev[50];
        sprintf( snev, "%d",nev );
        UI->ApplyCommand(commandr+snev);
    }
  }

  if(visManager) delete visManager;
  delete anaManager;
  delete runManager;
  
  return 0;
}

//---------------------------------------------------------------------------
