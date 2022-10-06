#include "G4GeneralParticleSource.hh"
#include "G4Event.hh"
#include "G4String.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

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

  std::cout << "HI" << std::endl;
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

    std::cout << fVx << " " << fPxp << " "  << std::endl;

}


// //---------------------------------------------------------------------------
