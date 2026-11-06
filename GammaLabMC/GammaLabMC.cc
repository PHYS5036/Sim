#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>
#include "globals.hh"
#include "G4SystemOfUnits.hh"

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

int main(int argc, char** argv) {
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
  std::cout << "Interactive mode" << std::endl;
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

    // Generate an isotropic event list with the same pre-transport geometric
    // cut used by the batch pipeline, then transport only the retained gammas.
    // Override defaults with GLMC_VIS_SOURCE / GLMC_VIS_NDECAYS; set
    // GLMC_VIS_CUT=0 to skip this and use whatever vis.mac configured.
    const char* cutEnv = std::getenv("GLMC_VIS_CUT");
    if (!cutEnv || std::string(cutEnv) != "0") {
      const std::string sourceDir = GAMMALABMC_SOURCE_DIR;
      const char* srcEnv = std::getenv("GLMC_VIS_SOURCE");
      const char* nEnv   = std::getenv("GLMC_VIS_NDECAYS");
      const std::string specFile = srcEnv ? srcEnv : sourceDir + "/macros/Cs137.txt";
      const std::string nDecays  = nEnv ? nEnv : "50000";

      std::filesystem::create_directories("output");
      const std::string genFile =
          (std::filesystem::absolute("output") / "Gen_vis_cut.root").string();

      std::ostringstream rootCmd;
      rootCmd << "root -l -b -q '" << sourceDir << "/GenerateSpectrum.C(\""
              << specFile << "\",\"" << genFile << "\"," << nDecays << ",0,"
              << detCon->GetDetectorType() << ","
              << detCon->GetSourceHolderType() << ","
              << detCon->GetDetectorDistance()/CLHEP::mm << ")'";
      std::cout << "Generating cut vis events: " << rootCmd.str() << std::endl;

      if (std::system(rootCmd.str().c_str()) == 0) {
        UImanager->ApplyCommand("/GammaLabMC/generator/Mode 1");
        UImanager->ApplyCommand("/GammaLabMC/generator/InputFile " + genFile);
        if (pga->GetNEvents() > 0) {
          UImanager->ApplyCommand("/run/beamOn " + std::to_string(pga->GetNEvents()));
        } else {
          std::cout << "No events survived the cut; nothing to draw." << std::endl;
        }
      } else {
        std::cout << "Cut event generation failed; vis.mac settings are unchanged." << std::endl;
      }
    }

    // Start the interactive session
    ui->SessionStart();

     // Clean up
     if (ui) delete ui;
     if (visManager) delete visManager;
     delete anaManager;
     delete runManager;

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
    delete anaManager;
    delete runManager;
  }
  
  return 0;
}

//---------------------------------------------------------------------------
