#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"

#include "EventAction.hh"
#include "DetectorHit.hh"
#include "AnalysisManager.hh"
#include "PrimaryGeneratorAction.hh"

//---------------------------------------------------------------------------

EventAction::EventAction( AnalysisManager* ana, PrimaryGeneratorAction* pga )
  :fAnaManager(ana), fPGA(pga)
{;}

//---------------------------------------------------------------------------

EventAction::~EventAction()
{;}

//---------------------------------------------------------------------------

void EventAction::BeginOfEventAction(const G4Event* evt)
{ 
  if( evt->GetEventID() == 0 ){
    fAnaManager->SetPrimaryPDef      ( (G4ParticleDefinition*)fPGA->GetPrimPDef() );
    fAnaManager->InitOutput();
  }
}

//---------------------------------------------------------------------------

void EventAction::EndOfEventAction(const G4Event* evt)
{

  G4int event_id   = evt->GetEventID();  
  if ( event_id%1000 == 0 ) 
    G4cout <<"Event " << event_id << G4endl;

  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  DetectorHit* hit;
  G4int nfpd_phys = 0;

  if(HCE) {
    G4int CollSize = HCE->GetNumberOfCollections();
    G4int hci      = 0;
    
    fAnaManager->ZeroArray(); 

    for(G4int i = 0; i < CollSize; i++) {
      DetectorHitsCollection* hc;
      while(!(hc = static_cast<DetectorHitsCollection*>(HCE->GetHC(hci++))));
      G4int hc_nhits = hc->entries();
      
      if( hc_nhits == 0 ) continue;
      
      // Fill output hit arrays for Phantom
      else if( hc->GetName().contains("Det")  ) 
	{
	  for(G4int j = 0; j < hc_nhits; j++) {
	    hit = static_cast<DetectorHit*>( hc->GetHit(j) );
	    fAnaManager->SetStepPDef( (G4ParticleDefinition*) hit->GetPDef() );
	    fAnaManager->SetStepP3( (G4ThreeVector) hit->GetMom() );
	    fAnaManager->SetStepPosPre( (G4ThreeVector) hit->GetPosPre() );
	    fAnaManager->SetStepPosPost( (G4ThreeVector) hit->GetPosPost() );
	    fAnaManager->SetStepTime( (G4double) hit->GetTime() );
	    fAnaManager->SetStepEdep( (G4double) hit->GetEdep() );
	    fAnaManager->SetStepID( (G4int) hit->GetID() );
	    fAnaManager->FillArray( nfpd_phys ); 
	    nfpd_phys++;
	  }
	}
    }
    
    if( nfpd_phys != 0 ) {
      // Fill output data from PrimaryGeneratorAction
      fAnaManager->SetPrimaryDirection ( (G4ThreeVector)fPGA->GetDirection() );
      fAnaManager->SetPrimaryEnergy    ( (G4double)fPGA->GetEnergy() );
      fAnaManager->SetPrimaryTime      ( (G4double)fPGA->GetTime() );
      fAnaManager->SetPrimaryPDef      ( (G4ParticleDefinition*)fPGA->GetPrimPDef() );
      
      fAnaManager->FillTree(); 
    }
  }

  fAnaManager->SetEventN(event_id);

}

//---------------------------------------------------------------------------
