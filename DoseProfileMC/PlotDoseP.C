// -----------------------------------------------------------------------------
//
// This analysis code is written in C++ to be used with CERN ROOT
// It reads in the data produced by the geant4 simulation from the 
// root file (TFile)
// Then it gets the root tree (TTree) in which the data is stored 
// and loops over each step taken by a particle in the phantom.
// It plots the energy deposited as a function of particle type.
//
// P423M Lab - David Hamilton
//
// root -b -q PlotDoseP.C++
//
// -----------------------------------------------------------------------------

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1.h"
#include "TLatex.h"

#include <iostream>
using namespace std;



void PlotDoseP( const char* finname = "~/data/G4_BEAMX_ENERGYY.root",
		float Emax = 70. )
{
  
  // -----------------------------------------------------------------------------
  // Open root file, get root tree and associate local and tree variables
  // -----------------------------------------------------------------------------
  
  TFile *fin = new TFile( finname );
  TTree* T   = (TTree*)fin->Get("T");
  
  //Declaration of leaves types
  static const Int_t MaxHits = 10000;
  Float_t         Prim_E;
  Float_t         Prim_Th;
  Float_t         Prim_Ph;
  Int_t           Prim_pdg;
  Int_t           Phantom_Nhits;
  Int_t           Phantom_pdg[MaxHits];
  Int_t           Phantom_id[MaxHits];
  Float_t         Phantom_x[MaxHits];
  Float_t         Phantom_y[MaxHits];
  Float_t         Phantom_z[MaxHits];
  Float_t         Phantom_Ed[MaxHits];

  // Set branch addresses.
  T->SetBranchAddress("Prim_E",&Prim_E);
  T->SetBranchAddress("Prim_Th",&Prim_Th);
  T->SetBranchAddress("Prim_Ph",&Prim_Ph);
  T->SetBranchAddress("Prim_pdg",&Prim_pdg);
  T->SetBranchAddress("Phantom_Nhits",&Phantom_Nhits);
  T->SetBranchAddress("Phantom_pdg",Phantom_pdg);
  T->SetBranchAddress("Phantom_id",Phantom_id);
  T->SetBranchAddress("Phantom_x",Phantom_x);
  T->SetBranchAddress("Phantom_y",Phantom_y);
  T->SetBranchAddress("Phantom_z",Phantom_z);
  T->SetBranchAddress("Phantom_Ed",Phantom_Ed);
  
  Long64_t nentries = T->GetEntries();
  
  // -----------------------------------------------------------------------------
  // Open root file, get root tree and associate local and tree variables
  // -----------------------------------------------------------------------------
 
  TH1F* hE1 = new TH1F("hE1","",100, 0.0, Emax );
  TH1F* hE2 = new TH1F("hE2","",100, 0.0, Emax );
  TH1F* hE3 = new TH1F("hE3","",100, 0.0, Emax );
  TH1F* hE4 = new TH1F("hE4","",100, 0.0, Emax );
  
  float edep1 = 0.0;
  float edep2 = 0.0;
  float edep3 = 0.0;
  float edep4 = 0.0;
  for ( Long64_t i = 0; i < nentries; i++ ) {
    
    T->GetEntry(i);                       // get the ith detector hit
    if( i % 1000 == 0 )
      cout << i << endl;
    
    for ( Int_t j = 0; j < Phantom_Nhits ; j++ ) {   // loop over each step in phantom
      
      if( Phantom_Ed[j] == 0 ) continue;
      
      if( abs( Phantom_pdg[j] ) > 0 &&  abs( Phantom_pdg[j] ) < 100 ) {
	hE1->Fill(Phantom_Ed[j]);
	edep1 += Phantom_Ed[j];
      }
      if( abs( Phantom_pdg[j] ) > 100 &&  abs( Phantom_pdg[j] ) < 1000 ) {
	hE2->Fill(Phantom_Ed[j]);
	edep2 += Phantom_Ed[j];
      }
      if( abs( Phantom_pdg[j] ) > 1000 &&  abs( Phantom_pdg[j] ) < 10000 ) {
	hE3->Fill(Phantom_Ed[j]);
	edep3 += Phantom_Ed[j];
      }
      if( abs( Phantom_pdg[j] ) > 10000 ) {
	hE4->Fill(Phantom_Ed[j]);
	edep4 += Phantom_Ed[j];
      }
    }
  }
      
  // ---------------------------------------------------------------------------
  // Create a canvas, make it look pretty and plot the graph
  // ---------------------------------------------------------------------------

  gStyle->SetCanvasColor(0);
  gStyle->SetFrameFillColor(0);

  gStyle->SetPadTopMargin(.05);
  gStyle->SetPadLeftMargin(.15);
  gStyle->SetPadRightMargin(.05);
  gStyle->SetPadBottomMargin(.15);

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetStripDecimals(kFALSE);

  TCanvas* c2 = new TCanvas( "c2", "", 800, 600 );
  c2->cd()->SetLogy(1);

  hE1->SetLineColor( 1 );
  hE1->SetLineWidth( 2 );
  hE1->GetXaxis()->SetTitleOffset(1.1);
  hE1->GetYaxis()->SetTitleOffset(1.1);
  hE1->GetXaxis()->SetTitleFont(42);
  hE1->GetYaxis()->SetTitleFont(42);
  hE1->GetXaxis()->SetTitleSize(0.045);
  hE1->GetYaxis()->SetTitleSize(0.045);
  hE1->GetXaxis()->SetLabelOffset(0.01);
  hE1->GetYaxis()->SetLabelOffset(0.01);
  hE1->GetXaxis()->SetLabelFont(42);
  hE1->GetYaxis()->SetLabelFont(42);
  hE1->GetXaxis()->SetLabelSize(0.035);
  hE1->GetYaxis()->SetLabelSize(0.035);
  hE1->GetXaxis()->SetNdivisions(505);
  hE1->GetYaxis()->SetNdivisions(505);
  hE1->Draw("");
  hE1->SetMinimum(0.00001);
  hE1->GetXaxis()->SetTitle("Energy Deposited (MeV)");

  hE2->SetLineColor( 2 );
  hE2->SetLineWidth( 2 );
  hE2->Draw("same");
  hE3->SetLineColor( 4 );
  hE3->SetLineWidth( 2 );
  hE3->Draw("same");
  hE4->SetLineColor( 6 );
  hE4->SetLineWidth( 2 );
  hE4->Draw("same");

  // ---------------------------------------------------------------------------

  TLatex*   tex0 = new TLatex(0.53,0.90,Form("Total Edep = %f MeV", (edep1+edep2+edep3+edep4)/(float)nentries ));
  tex0->SetNDC();
  tex0->SetTextColor(1);
  tex0->SetTextFont(42);
  tex0->SetTextSize(0.036);
  tex0->SetLineWidth(2);
  tex0->Draw();

  TLatex*   tex1 = new TLatex(0.53,0.85,Form("leptons (%f)", edep1/(edep1+edep2+edep3+edep4)));
  tex1->SetNDC();
  tex1->SetTextColor(1);
  tex1->SetTextFont(42);
  tex1->SetTextSize(0.036);
  tex1->SetLineWidth(2);
  tex1->Draw();

  TLatex*   tex2 = new TLatex(0.53,0.80,Form("mesons (%f)", edep2/(edep1+edep2+edep3+edep4)));
  tex2->SetNDC();
  tex2->SetTextColor(2);
  tex2->SetTextFont(42);
  tex2->SetTextSize(0.036);
  tex2->SetLineWidth(2);
  tex2->Draw();

  TLatex*   tex3 = new TLatex(0.53,0.75,Form("baryons (%f)", edep3/(edep1+edep2+edep3+edep4)));
  tex3->SetNDC();
  tex3->SetTextColor(4);
  tex3->SetTextFont(42);
  tex3->SetTextSize(0.036);
  tex3->SetLineWidth(2);
  tex3->Draw();

  TLatex*   tex4 = new TLatex(0.53,0.70,Form("ions (%f)",edep4/(edep1+edep2+edep3+edep4) ));
  tex4->SetNDC();
  tex4->SetTextColor(6);
  tex4->SetTextFont(42);
  tex4->SetTextSize(0.036);
  tex4->SetLineWidth(2);
  tex4->Draw();

  // ---------------------------------------------------------------------------

  string s1 = (string)finname;
  string outname = s1.substr(0, s1.find(".", 0));
  outname.append("P.pdf");
  c2->Print(outname.c_str());

  // cout << endl << "----------------------------------------------------------------" << endl << endl;
  // cout << "Total energy deposited          = " << (edep1+edep2+edep3+edep4)/(float)nentries << " MeV" << endl;
  // cout << "Energy deposited lepton fraction = " << edep1/(edep1+edep2+edep3+edep4) << endl;
  // cout << "Energy deposited meson fraction  = " << edep2/(edep1+edep2+edep3+edep4) << endl;
  // cout << "Energy deposited baryon fraction = " << edep3/(edep1+edep2+edep3+edep4) << endl;
  // cout << "Energy deposited ion fraction    = " << edep4/(edep1+edep2+edep3+edep4) << endl;
  // cout << endl << "----------------------------------------------------------------" << endl << endl;

}
