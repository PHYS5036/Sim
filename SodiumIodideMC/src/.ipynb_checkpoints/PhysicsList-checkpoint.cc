#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"

// Standard EM Proceses
#include "G4PhotoElectricEffect.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

// Extra EM Processes
#include "G4RayleighScattering.hh" 
#include "G4UniversalFluctuation.hh"

// Low Energy PENELOPE EM Model
#include "G4PenelopePhotoElectricModel.hh"
#include "G4PenelopeComptonModel.hh"
#include "G4PenelopeGammaConversionModel.hh"
#include "G4PenelopeRayleighModel.hh"
#include "G4PenelopeIonisationModel.hh"
#include "G4PenelopeBremsstrahlungModel.hh"
#include "G4PenelopeAnnihilationModel.hh"

// Low Energy Livermore EM Model
#include "G4LivermorePhotoElectricModel.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4LivermoreGammaConversionModel.hh"
#include "G4LivermoreRayleighModel.hh"
#include "G4LivermoreIonisationModel.hh"
#include "G4LivermoreBremsstrahlungModel.hh"

// General Tools and Processes
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4PhysicsListHelper.hh"
#include "G4Decay.hh"
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"
#include "globals.hh"

using namespace CLHEP;

//---------------------------------------------------------------------------

PhysicsList::PhysicsList():  G4VUserPhysicsList()
{
  defaultCutValue = 0.001*mm;
  SetVerboseLevel(1);

  fPmessenger = new PhysicsListMessenger(this);
}

//---------------------------------------------------------------------------

PhysicsList::~PhysicsList()
{
}

//---------------------------------------------------------------------------

void PhysicsList::ConstructParticle()
{
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();
  G4Gamma::GammaDefinition();

  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
}

//---------------------------------------------------------------------------

void PhysicsList::ConstructProcess()
{
  AddTransportation();
  ConstructEM();
  ConstructGeneral();
  AddStepMax();
}

//---------------------------------------------------------------------------

void PhysicsList::ConstructEM()
{
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper(); 

  //theParticleIterator->reset();
    auto theParticleIterator = GetParticleIterator();
    while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    //-------------------------------------------------------------------------------------
     
    if( fProcessName != "all" || fProcessName != "All" )
      fModelName = "standard";

    if( fModelName == "standard" || fModelName == "Standard" ) {

      if (particleName == "gamma") {
	if( fProcessName == "all" || fProcessName == "All" ) {
	  pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
	  pmanager->AddDiscreteProcess(new G4ComptonScattering);
	  pmanager->AddDiscreteProcess(new G4GammaConversion);
	}
	else if( fProcessName == "photoelectric" || fProcessName == "Photoelectric" ) {
	  pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
	}
	else if( fProcessName == "compton" || fProcessName == "Compton" ) {
	  pmanager->AddDiscreteProcess(new G4ComptonScattering);
	}
	else if( fProcessName == "pair" || fProcessName == "Pair" ) {
	  pmanager->AddDiscreteProcess(new G4GammaConversion);
	}
      }
      else if (particleName == "e-") {
	pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4eIonisation,         -1, 2, 2);
	pmanager->AddProcess(new G4eBremsstrahlung,     -1, 3, 3); //why not -1 -1 3 ?
      }
      else if (particleName == "e+") {
	pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4eIonisation,         -1, 2, 2);
	pmanager->AddProcess(new G4eBremsstrahlung,     -1, 3, 3); //why not -1 -1 3 ? 
	pmanager->AddProcess(new G4eplusAnnihilation,    0,-1, 4);
      }
    }
    else if( fModelName == "penelope" || fModelName == "Penelope" ) {

      if (particleName == "gamma") {
    	G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
    	thePhotoElectricEffect->SetEmModel(new G4PenelopePhotoElectricModel());
    	ph->RegisterProcess(thePhotoElectricEffect, particle);
    	G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
    	theComptonScattering->SetEmModel(new G4PenelopeComptonModel());
    	ph->RegisterProcess(theComptonScattering, particle);
    	G4GammaConversion* theGammaConversion = new G4GammaConversion();
    	theGammaConversion->SetEmModel(new G4PenelopeGammaConversionModel());
    	ph->RegisterProcess(theGammaConversion, particle);
    	G4RayleighScattering* theRayleigh = new G4RayleighScattering();
    	theRayleigh->SetEmModel(new G4PenelopeRayleighModel());
    	ph->RegisterProcess(theRayleigh, particle);
      } 
      else if (particleName == "e-") {
    	G4eMultipleScattering* msc = new G4eMultipleScattering();
    	ph->RegisterProcess(msc, particle);
    	G4eIonisation* eIoni = new G4eIonisation();
    	eIoni->SetEmModel(new G4PenelopeIonisationModel());
    	eIoni->SetFluctModel(new G4UniversalFluctuation() );
    	ph->RegisterProcess(eIoni, particle);
    	G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
    	eBrem->SetEmModel(new G4PenelopeBremsstrahlungModel());
    	ph->RegisterProcess(eBrem, particle);
      } 
      else if (particleName == "e+") {
    	G4eMultipleScattering* msc = new G4eMultipleScattering();
    	ph->RegisterProcess(msc, particle);
    	G4eIonisation* eIoni = new G4eIonisation();
    	eIoni->SetEmModel(new G4PenelopeIonisationModel());
    	eIoni->SetFluctModel(new G4UniversalFluctuation() );
    	ph->RegisterProcess(eIoni, particle);
    	G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
    	eBrem->SetEmModel(new G4PenelopeBremsstrahlungModel());
    	ph->RegisterProcess(eBrem, particle);
    	G4eplusAnnihilation* eAnni = new G4eplusAnnihilation();
    	eAnni->SetEmModel(new G4PenelopeAnnihilationModel());
    	ph->RegisterProcess(eAnni, particle);
      }
    }
    else if( fModelName == "livermore" || fModelName == "Livermore" ) {

      if (particleName == "gamma") {
    	G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
    	thePhotoElectricEffect->SetEmModel(new G4LivermorePhotoElectricModel());
    	ph->RegisterProcess(thePhotoElectricEffect, particle);
    	G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
    	theComptonScattering->SetEmModel(new G4LivermoreComptonModel());
    	ph->RegisterProcess(theComptonScattering, particle);
    	G4GammaConversion* theGammaConversion = new G4GammaConversion();
    	theGammaConversion->SetEmModel(new G4LivermoreGammaConversionModel());
    	ph->RegisterProcess(theGammaConversion, particle);
    	G4RayleighScattering* theRayleigh = new G4RayleighScattering();
    	theRayleigh->SetEmModel(new G4LivermoreRayleighModel());
    	ph->RegisterProcess(theRayleigh, particle);
      } 
      else if (particleName == "e-") {
    	G4eMultipleScattering* msc = new G4eMultipleScattering();
    	ph->RegisterProcess(msc, particle);
    	G4eIonisation* eIoni = new G4eIonisation();
    	eIoni->SetEmModel(new G4LivermoreIonisationModel());
    	eIoni->SetFluctModel(new G4UniversalFluctuation() );
    	ph->RegisterProcess(eIoni, particle);
    	G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
    	eBrem->SetEmModel(new G4LivermoreBremsstrahlungModel());
    	ph->RegisterProcess(eBrem, particle);
      } 
      else if (particleName == "e+") {
    	G4eMultipleScattering* msc = new G4eMultipleScattering();
    	ph->RegisterProcess(msc, particle);
    	G4eIonisation* eIoni = new G4eIonisation();
    	ph->RegisterProcess(eIoni, particle);
    	G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
    	ph->RegisterProcess(eBrem, particle);
    	G4eplusAnnihilation* eAnni = new G4eplusAnnihilation();
    	ph->RegisterProcess(eAnni, particle);
      }
    }

  }
}

//---------------------------------------------------------------------------

void PhysicsList::ConstructGeneral()
{
  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();
//  theParticleIterator->reset();
  auto theParticleIterator = GetParticleIterator();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager ->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}

//---------------------------------------------------------------------------

void PhysicsList::AddStepMax()
{
  // Step limitation seen as a process
  G4StepLimiter* stepLimiter = new G4StepLimiter();
  ////G4UserSpecialCuts* userCuts = new G4UserSpecialCuts();
  
 // theParticleIterator->reset();
  auto theParticleIterator = GetParticleIterator();
  while ((*theParticleIterator)()){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      if (particle->GetPDGCharge() != 0.0)
        {
	  pmanager ->AddDiscreteProcess(stepLimiter);
	  ////pmanager ->AddDiscreteProcess(userCuts);
        }
  }
}

//---------------------------------------------------------------------------

void PhysicsList::SetCuts()
{
  //G4VUserPhysicsList::SetCutsWithDefault method sets 
  //the default cut value for all particle types 
  //
  SetCutsWithDefault();
     
  if (verboseLevel>0) DumpCutValuesTable();
}

//---------------------------------------------------------------------------
