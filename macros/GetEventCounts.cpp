//********************************************************************************************
//
// Macro to get counts and info of "near" and "far" events for a dataset. Uses text file containing channel "near" and "far" pairs and super reduced CUORE production files. 
// author: Vivek Sharma
// date: 2022-04-14
//
// *******************************************************************************************

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

int main(int argc, char* argv[]){

  int dataset = stoi(argv[1]);

  TString dir = "/project/projectdirs/cuore/syncData/CUORE/OfficialProcessed/ReproSpring20/super_reduced/";
  TString filename_bkg;
  //int runs [58];
  QChain ch("qredtree");
  /*for(int i=0;i<58;i++)
    {
      cout<<"Working on run "<<351032+i<<endl;
      for(int j=1;j<20;j++)
	{
	  listname.Form("%s/ds%d/FinalizedReduced_%d_%.3d_R.list", dir.Data(), dataset, 351032+i, j);
	}
      cout<<"Working on file "<<listname.Data()<<endl;
      int nAdded = ch.Add(listname.Data());
      if(nAdded==0){cout<<"nAdded did not work"<<endl;exit(1);}
      
      }*/

  //TODO: Figure out how to combine cal and bkg files

  filename_bkg.Form("%s/SuperReduced_Background_NoPCACut_ds%d.root",dir.Data(),dataset);
  //filename_cal.Form("%s/SuperReduced_Calibration_NoPCACut_ds%d.root",dir.Data(),dataset);
  int nAdded1 = ch.Add(filename_bkg.Data());
  //int nAdded2 = ch.Add(filename_cal.Data());

  if(nAdded1==0){cout<<"Could not add files"<<endl;}
  //if(nAdded1==0){cout<<"Could not add files"<<endl;}

  int tree_channel, multiplicity, nearEvents, farEvents;
  double energy, baselineSlope, riseTime, decayTime, delay, TVL, TVR;
  Bool_t IsNear;
  int nearEventArr[988] = {0};
  int farEventArr[988] = {0};

  //int channelV [2];
  int *channelV = new int [988]; 

  TString outString = Form("ds%d_M2_3_6_MeV.root", dataset);
  TFile* outFile = TFile::Open(outString, "RECREATE");

  //Accessing tree branches
  ch.SetBranchAddress("Channel", &tree_channel); // Main channel of event
  ch.SetBranchAddress("TotalEnergy", &energy); // Total energy of the whole multiplet
  ch.SetBranchAddress("Multiplicity", &multiplicity); // Multiplicity of event
  //ch.SetBranchAddress("ChannelV", &channelV);
  ch.SetBranchAddress("ChannelV", channelV); // Vector of channels in the multiplet
  ch.SetBranchAddress("fNormBaselineSlope", &baselineSlope);
  ch.SetBranchAddress("fNormRiseTime", &riseTime);
  ch.SetBranchAddress("fNormDecayTime", &decayTime);
  ch.SetBranchAddress("fNormDelay", &delay);
  ch.SetBranchAddress("fNormTVL", &TVL);
  ch.SetBranchAddress("fNormTVR", &TVR);
  ch.SetBranchAddress("IsNear", &IsNear);

  int numEntries = ch.GetEntries();
  cout<<"Number of entries are "<<numEntries<<endl;
  string fname;
  fname = "channel_pairs.txt";
  
  vector<vector<string> > content;
  vector<string> row;
  string line, word;
  
  fstream file (fname, ios::in);
  if(file.is_open())
    {
      while(getline(file, line))
	    {
	      row.clear();	  
	      stringstream str(line);
	      while(getline(str, word, ','))
          {
	        row.push_back(word);
          }
	      content.push_back(row);
	    }
    }
  else
    cout<<"Could not open the file\n";
  
  int channel=0, near_channel=0, far_channel=0;
  //int near_event [988];
  //int far_event [988];
  int near_events=0;
  int far_events=0;

  TTree* outTree = new TTree("outputTree", "outputTree");
  outTree->Branch("channel", &channel);
  //outTree->Branch("nearChannel", &near_channel);
  //outTree->Branch("farChannel", &far_channel);
  outTree->Branch("riseTime", &riseTime);
  outTree->Branch("decayTime", &decayTime);
  outTree->Branch("delay", &delay);
  outTree->Branch("TVL", &TVL);
  outTree->Branch("TVR", &TVR);
  
  TTree* countsTree = new TTree("countsTree", "countsTree");
  countsTree->Branch("channel", &channel);
  countsTree->Branch("nearEvents", &nearEvents);
  countsTree->Branch("farEvents", &farEvents);

  /* for(int i=0;i<989;i++)
    {
      channel = stoi(content[i][0]);
      near_channel = stoi(content[i][1]);
      far_channel = stoi(content[i][2]);
      cout<<"Working on set "<<channel<<" "<<near_channel<<" "<<far_channel<<endl; */

  for(int e=0;e<numEntries;e++)
	    {
	      ch.GetEntry(e);
	      //cout<<"Working on tree_channel "<<tree_channel<<" multiplicity "<<multiplicity<<" energy "<<energy<<endl;
	      if(multiplicity==2)
	        {
	          if(energy<6000 || energy>3000) //TODO: Better way to check for alpha events
		          {
                //outTree->Fill();
                for(int i=0;i<988;i++)
                {
                  // Checking if channel pair is a near channel pair and counting it if so
                  if(tree_channel == stoi(content[i][0]) && channelV[1] == stoi(content[i][1]))
                  {
                    IsNear = 1;
                    nearEventArr[tree_channel-1]++;
                  }

                  // Checking if channel pair is a far channel pair and counting it if so
                  if(tree_channel == stoi(content[i][0]) && channelV[1] == stoi(content[i][2]))
                  {
                    IsNear = 0;
                    farEventArr[tree_channel-1]++;
                  }
                }

                // Filling tree with relevant variables
                outTree->Fill();
                
		            /*if(channel==tree_channel && channelV[1]==near_channel)
		              {
		                near_events++;
		              }
		            if(channel==tree_channel && channelV[1]==far_channel)
		              {
		                far_events++;
		              }*/
		          }
	        }
	    }
      //cout<<"Channel "<<i+1<<" near events are "<<near_event[i]<<" and far events are "<<far_event[i]<<endl;
      //countsTree->Fill();
      //cout<<"Channel "<<i+1<<" near events are "<<near_events<<" and far events are "<<far_events<<endl;
      //near_events=0;
      //far_events=0;

  //TString outString = Form("ds%d_M2_3_6_MeV.root", dataset);
  //TFile* outFile = TFile::Open(outString);

  for(int i=0;i<988;i++)
  {
    channel = stoi(content[i][0]);
    nearEvents = nearEventArr[channel-1];
    farEvents = farEventArr[channel-1];
    countsTree->Fill();
  }

  outFile->cd();
  outFile->Write("outTree");
  outFile->Write("countsTree");
  outFile->Close();

  return 0;
}

