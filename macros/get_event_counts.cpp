//********************************************************************************************
//
// Macro to get counts of "near" and "far" events for a dataset. Uses text file containing chan// nel "near" and "far" pairs and super reduced CUORE production files. 
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
using namespace Cuore;

int main(int argc, char* argv[]){

  int dataset = 3615;

  TString dir = "/project/projectdirs/cuore/syncData/CUORE/OfficialProcessed/ReproSpring20/super_reduced/";
  TString filename_bkg,filename_cal;
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
  filename_cal.Form("%s/SuperReduced_Calibration_NoPCACut_ds%d.root",dir.Data(),dataset);
  int nAdded1 = ch.Add(filename_bkg.Data());
  int nAdded2 = ch.Add(filename_cal.Data());

  if(nAdded1==0 || nAdded2==0){cout<<"Could not add files"<<endl;}
  //if(nAdded1==0){cout<<"Could not add files"<<endl;}

  int tree_channel, multiplicity;
  double energy;
  //int channelV [2];
  int *channelV = new int [988]; 

  //Accessing tree branches
  ch.SetBranchAddress("Channel", &tree_channel);
  ch.SetBranchAddress("TotalEnergy", &energy);
  ch.SetBranchAddress("Multiplicity", &multiplicity);
  //ch.SetBranchAddress("ChannelV", &channelV);
  ch.SetBranchAddress("ChannelV", channelV);

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
	    row.push_back(word);
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

  for(int i=0;i<989;i++)
    {
      channel = stoi(content[i][0]);
      near_channel = stoi(content[i][1]);
      far_channel = stoi(content[i][2]);

      cout<<"Working on set "<<channel<<" "<<near_channel<<" "<<far_channel<<endl;
      for(int e=0;e<numEntries;e++)
	{
	  ch.GetEntry(e);
	  //cout<<"Working on tree_channel "<<tree_channel<<" multiplicity "<<multiplicity<<" energy "<<energy<<endl;
	  if(multiplicity==2)
	    {
	      if(energy>5000 || energy<200) //TODO: Better way to check for alpha events
		{
		  if(channel==tree_channel && channelV[1]==near_channel)
		    {
		      near_events++;
		    }
		  if(channel==tree_channel && channelV[1]==far_channel)
		    {
		      far_events++;
		    }
		  }
	    }
	}
      //cout<<"Channel "<<i+1<<" near events are "<<near_event[i]<<" and far events are "<<far_event[i]<<endl;
      cout<<"Channel "<<i+1<<" near events are "<<near_events<<" and far events are "<<far_events<<endl;
      near_events=0;
      far_events=0;
    }

  return 0;
}
