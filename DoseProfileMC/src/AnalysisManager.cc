#include "AnalysisManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "AnalysisMessenger.hh"

#include "G4UnitsTable.hh"
#include "G4RunManager.hh"
#include "G4Point3D.hh"
#include "G4Transform3D.hh"

#include <iostream>
#include <fstream>

//---------------------------------------------------------------------------

AnalysisManager::AnalysisManager()
{
  ZeroArray();

  fOutFileName = G4String("output/out_default.csv");

  fAnaMessenger = new AnalysisMessenger(this);
}

//---------------------------------------------------------------------------

AnalysisManager::~AnalysisManager()
{

}

//---------------------------------------------------------------------------

void AnalysisManager::InitOutput()
{ 
  
  for(int i=0; i<nPixX; i++){
    for(int j=0; j<nPixY; j++){
      inputPos[i][j] = 0;
      for(int k=0; k<nPixZ; k++){
	for(int l=0; l<nTypes; l++){
	  for(int m=0; m<2; m++){
	  
	    eDep[i][j][k][l][m] = 0;

	  }
	}     
      }
    }
  }
  primaryName = fPPDef->GetParticleName();
  
}
//---------------------------------------------------------------------------

void AnalysisManager::WriteOutput()
{ 

  std::ofstream outfile;
  outfile.open(fOutFileName);

  std::cout << fOutFileName << std::endl;

  outfile << "nevent,particle,energy,xbins,xmin,xmax,ybins,ymin,ymax,zbins,zmin,zmax\n";
  outfile << fEventN+1 << "," << primaryName << "," << fPEne*1000 << "," << nPixX << "," << minX << ", "<< maxX << ", " << nPixY << "," << minY << ", "<< maxY << ", " << nPixZ << "," << minZ << ", "<< maxZ <<  "\n";
  outfile << "x,y,z,lepton,leptonT,meson,mesonT,baryon,baryonT,ion,ionT\n";


  for(int i=0; i<nPixX; i++){

    for(int j=0; j<nPixY; j++){
      for(int k=0; k<nPixZ; k++){
	outfile << i << ", " << j << ", " << k;

	for(int l=0; l<nTypes; l++){
	  for(int m=0; m<2; m++){
	    outfile << ", " << eDep[i][j][k][l][m];
	  }
	}     
	outfile << "\n";
      }
    }
  }

  outfile.close();

}

//---------------------------------------------------------------------------

void AnalysisManager::ZeroArray()
{
  // Primary
  G4ThreeVector zero(0.,0.,0.);
  fPEne   = 9999;
  fPdir   = (zero);
  fPth    = 9999;
  fPph    = 9999;
  fPTime  = 9999;
  //fPPDef  = NULL;
  fPpdg   = 9999;

  // Raw Hits
  fRAW_Nhits  = 0;
  fSteppdef    = NULL;
  fStepp3      = (zero);
  fSteppospre  = (zero);
  fSteppospost = (zero);
  fStepid      = 0;
  fSteptime    = 0;
  fStepedep    = 0;

  for ( int i = 0; i < fMaxhits; i++ ) {
    fRAW_id[i]      = 9999;  
    fRAW_time[i]    = 9999;
    fRAW_Edep[i]    = 9999;
    fRAW_pdg[i]     = 9999;
    fRAW_mass[i]    = 9999;
    fRAW_mom[i]     = 9999;
    fRAW_px[i]      = 9999;
    fRAW_py[i]      = 9999;
    fRAW_pz[i]      = 9999;
    fRAW_xpre[i]    = 9999;
    fRAW_ypre[i]    = 9999;
    fRAW_zpre[i]    = 9999;
    fRAW_xpost[i]   = 9999;
    fRAW_ypost[i]   = 9999;
    fRAW_zpost[i]   = 9999;
    fRAW_Energy[i]  = 9999;
  }
}

//---------------------------------------------------------------------------

void AnalysisManager::FillArray( int hitn ) 
{
    fRAW_Nhits++;
    fRAW_id[hitn]     = (int)fStepid;
    fRAW_pdg[hitn]    = (int)fSteppdef->GetPDGEncoding();
    fRAW_mass[hitn]   = (float)fSteppdef->GetPDGMass();
    fRAW_time[hitn]   = (float)fSteptime;                                   
    fRAW_mom[hitn]    = (float)fStepp3.mag();                             
    fRAW_px[hitn]     = (float)fStepp3.getX();                             
    fRAW_py[hitn]     = (float)fStepp3.getY();                             
    fRAW_pz[hitn]     = (float)fStepp3.getZ();                             
    fRAW_xpre[hitn]   = (float)fSteppospre.getX();                             
    fRAW_ypre[hitn]   = (float)fSteppospre.getY();                             
    fRAW_zpre[hitn]   = (float)fSteppospre.getZ();                             
    fRAW_xpost[hitn]  = (float)fSteppospost.getX();                             
    fRAW_ypost[hitn]  = (float)fSteppospost.getY();                             
    fRAW_zpost[hitn]  = (float)fSteppospost.getZ();                             
    fRAW_Edep[hitn]   = (float)fStepedep;
    fRAW_Energy[hitn] = sqrt( fRAW_mom[hitn]*fRAW_mom[hitn] 
				     + fRAW_mass[hitn]*fRAW_mass[hitn] );
 
    fRAW_xpre[hitn]   = (fRAW_xpre[hitn] + fRAW_xpost[hitn])/2.;
    fRAW_ypre[hitn]   = (fRAW_ypre[hitn] + fRAW_ypost[hitn])/2.;
    fRAW_zpre[hitn]   = (fRAW_zpre[hitn] + fRAW_zpost[hitn])/2.;

}

//---------------------------------------------------------------------------

void AnalysisManager::FillTree()
{
  // Primary Variables
  fPTime  = (float)fPTime;
  fPth    = (float)fPdir.getTheta();                         
  fPph    = (float)fPdir.getPhi();                                                      
  fPEne   = (float)fPEne;                         
  fPpdg   = (int)  fPPDef->GetPDGEncoding();
  
  for(int i=0; i<fRAW_Nhits; i++){

    int x_i     = nPixX*(fRAW_xpre[i]-minX)/(maxX-minX);
    int y_i     = nPixY*(fRAW_ypre[i]-minY)/(maxY-minY);
    int z_i     = nPixZ*(fRAW_zpre[i]-minZ)/(maxZ-minZ);


    if(x_i<0 || y_i<0 || z_i<0 ||x_i>=nPixX || y_i>=nPixY || z_i>=nPixZ) continue;;

    int pdg_i = 0;
    for(int limit : pdg_limit){
      if(fRAW_pdg[i]<limit)break;
      pdg_i++;
    }

    int hit_id = fRAW_id[i];

    eDep[x_i][y_i][z_i][pdg_i][hit_id] += fRAW_Edep[i];

  }

}

//---------------------------------------------------------------------------
