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

//---------------------------------------------------------------------------

int main(int argc, char** argv)
{

  std::cout << "### SodiumIodideMC: Application Start ###" << std::endl;
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

  std::cout << "### SodiumIodideMC: UI Initialization Start ###" << std::endl;
  G4UImanager * UI         = G4UImanager::GetUIpointer();
  G4VisManager* visManager = 0;
  std::cout << "### SodiumIodideMC: UI Initialization End ###" << std::endl;
  if (argc==1) {
#ifdef G4VIS_USE
    visManager = new G4VisExecutive;
    visManager->Initialize();
#endif
    G4UIsession * session = 0;
#ifdef G4UI_USE_TCSH
    session = new G4UIterminal(new G4UItcsh);
#else
    session = new G4UIterminal();
#endif
    session->SessionStart();
    delete session;
  }
  else {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UI->ApplyCommand(command+fileName);
    if( pga->GetMode()==EPGA_ROOT ) {
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
