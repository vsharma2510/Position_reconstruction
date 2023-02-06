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
  double *energyV = new double [988];

  //Accessing tree branches
  ch.SetBranchAddress("Channel", &tree_channel);
  ch.SetBranchAddress("TotalEnergy", &energy);
  ch.SetBranchAddress("Multiplicity", &multiplicity);
  //ch.SetBranchAddress("ChannelV", &channelV);
  ch.SetBranchAddress("ChannelV", channelV);
  ch.SetBranchAddress("EnergyV", energyV);

  int numEntries = ch.GetEntries();
  cout<<"Number of entries are: "<<numEntries<<endl;

  TH2D* e1e2 = new TH2D("e1e2","E1 vs E2",2000,0,6000,2000,0,6000);
  TH1D* ch_dist = new TH1D("ch_dist","M2 channel 2", 988,1,988);

  int chn = atoi(argv[1]);
  
  for(int e=0;e<numEntries;e++)
    {
      ch.GetEntry(e);
      if(tree_channel==chn)
	{
	  if(multiplicity==2)
	    {
	      if(energy>3500)
		{
		  ch_dist->Fill(channelV[1]);
		  e1e2->Fill(energyV[1],energyV[2]);
		}
	    }
	}
    }
  TFile* output_file = TFile::Open("M2Ch2_dist.root","RECREATE");
  ch_dist->Write("m2ch2_histo");
  e1e2->Write("e1e2");
  output_file->Close();
  return 0;
}
