#include <iostream>
#include <fstream>
#include <getopt.h>
#include <vector>
#include <string>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <cstdlib>

#include <TH2.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TString.h>
#include <TFile.h>
#include <TLegend.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TPad.h>


#include "QCuore.hh"
#include "QError.hh"
#include "QObject.hh"
#include "QGlobalHandle.hh"
#include "QMessage.hh"
#include "QGlobalDataManager.hh"
#include "QDetChannelCollectionHandle.hh"
#include "QDetChannelCollection.hh"
#include "QHeader.hh"
#include "QPulseInfo.hh"
#include "QChain.hh"
#include "QTObject.hh"
#include "QTObjectHandle.hh"
#include "QCalibrationParametersHandle.hh"
#include "QCalibrationResiduals.hh"

using namespace std;
using namespace Cuore;

int main(int argc, char* argv[]){

  int dataset=3615;

  TString dir = "/project/projectdirs/cuore/syncData/CUORE/OfficialProcessed/ReproSpring20/super_reduced/";
  TString filename_bkg,filename_cal;
  
  QChain ch("qredtree");

  filename_bkg.Form("%s/SuperReduced_Background_ds%d.root",dir.Data(),dataset);
  filename_cal.Form("%s/SuperReduced_Calibration_ds%d.root",dir.Data(),dataset);
  int nAdded1 = ch.Add(filename_bkg.Data());
  int nAdded2 = ch.Add(filename_cal.Data());

  if(nAdded1==0 || nAdded2==0){cout<<"Could not add files"<<endl;}
  //if(nAdded1==0){cout<<"Could not add files"<<endl;}

  int tree_channel, multiplicity;
  double energy;
  //int channelV [2];
  int *channelV = new int [988];
  //double *energyV = new double [988];

  //Accessing tree branches
  ch.SetBranchAddress("Channel", &tree_channel);
  ch.SetBranchAddress("TotalEnergy", &energy);
  ch.SetBranchAddress("Multiplicity", &multiplicity);
  //ch.SetBranchAddress("ChannelV", &channelV);
  ch.SetBranchAddress("ChannelV", channelV);
  //ch.SetBranchAddress("EnergyV", energyV);

  int numEntries = ch.GetEntries();
  cout<<"Number of entries are: "<<numEntries<<endl;

  //TH2D* e1e2 = new TH2D("e1e2","E1 vs E2",0,6000,0,6000);
  TH1D* m2_histo = new TH1D("m2_histo","M2 events", 988,1,988);
  TH1D* m2_energy = new TH1D("m2_energy","M2_energy",500,5000,5500);
  
  for(int e=0;e<numEntries;e++)
    {
      ch.GetEntry(e);
      if(multiplicity==2)
	{
	  if(energy>5000)
	    {
	      m2_histo->Fill(tree_channel);
	      m2_energy->Fill(energy);
	    }
	}
    }

  TFile* output_file = TFile::Open("m2_events.root","RECREATE");
  m2_histo->Write("m2_histo");
  m2_energy->Write("m2_energy");
  output_file->Close();
  return 0;
}
