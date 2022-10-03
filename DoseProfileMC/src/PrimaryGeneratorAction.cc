#include "G4GeneralParticleSource.hh"
#include "G4Event.hh"
#include "G4String.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

// #include "TObjArray.h"
// #include "TBranch.h"
// #include "TString.h"

#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"

using namespace CLHEP;

//---------------------------------------------------------------------------

PrimaryGeneratorAction::PrimaryGeneratorAction()

{
  fGunMessenger       = new PrimaryGeneratorMessenger(this);


  fNevent             = 1;
  fDump               = 0;

  fParticleTable      = G4ParticleTable::GetParticleTable();
  fIonTable           = G4IonTable::GetIonTable(); 
  fPDefinition        = NULL;

  fParticleSource     = new G4GeneralParticleSource();
  
  fParticleGun        = new G4ParticleGun(1);
}

//---------------------------------------------------------------------------

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fParticleSource;
  delete fGunMessenger;
}

//---------------------------------------------------------------------------

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
//   switch(fMode) {

//   case EPGA_GPS:

    fParticleSource->GeneratePrimaryVertex(anEvent);

    fVx          = fParticleSource->GetParticlePosition().getX();
    fVy          = fParticleSource->GetParticlePosition().getY();
    fVz          = fParticleSource->GetParticlePosition().getZ();
    fPxp         = fParticleSource->GetParticleMomentumDirection().getX();
    fPyp         = fParticleSource->GetParticleMomentumDirection().getY();
    fPzp         = fParticleSource->GetParticleMomentumDirection().getZ();
    fEp          = fParticleSource->GetParticleEnergy()/GeV;
    fTp          = fParticleSource->GetParticleTime();
    fPDefinition = fParticleSource->GetParticleDefinition();

//     G4cout << fVx << " " << fVy << " " << fVz << G4endl;
//     G4cout << fPxp << " " << fPyp << " " << fPzp << G4endl;

   //  break;

//   default:
//     G4cout << "Unknown mode given to PrimiaryGeneratorAction (0 for gps or 1 for root)" << G4endl;
//   }			       
}


// //---------------------------------------------------------------------------
