//********************************************************************************************
//
// Macro to get counts and info of "near" and "far" events for a dataset. Uses text file containing channel "near" and "far" pairs and CUORE production files.
// Command line inputs: dataset
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
#include <chrono>

#include "QChain.hh"

using namespace std;

std::vector<int> GetRuns(dataset)
  {
    QDb::QDbTable table; 
    QCuoreDb *db = QCuoreDb::Get();
    TString tQuery = Form("SELECT r.run_number FROM runs AS r, data_sets_runs AS d, runs as s WHERE r.run_number=d.run_number AND r.run_type='Reprocess' AND s.run_number=r.source_run AND s.run_type='Background' AND d.data_set=%d ORDER by r.source_run;", dataset);
    string query(tQuery.Data());
    db->DoQuery(query, table);
    //TChain* qC = new TChain("qtree");
    std::vector<int> runVector;
    if(!table["run_number"].empty()) 
      {
      for (unsigned int i = 0; i < table["run_number"].size(); ++i) 
        {
          runVector.push_back(table["run_number"][i].GetInt());
          cout<<"Adding "<<run_number<<endl;
        }
      }
  }

int main(int argc, char* argv[]){

  int dataset = stoi(argv[1]);
  //TString dir = Form("/project/projectdirs/cuore/syncData/CUORE/OfficialProcessed/ReproSpring20/output/ds%d/", dataset);
  //TString filename_bkg;
  std::vector<int> runVector;
  runVector = GetRuns(dataset);
  QChain ch("qredtree");

  for(std::vector<int>::iterator it = runVector.begin(); it != runVector.end(); ++it)
    {
      TString dir = Form("/project/projectdirs/cuore/syncData/CUORE/OfficialProcessed/ReproSpring20/output/ds%d/Production_%d_R.list", dataset, (*it));
      ch->Add(dir.Data());    
    }
      
  //filename_bkg.Form("%s/SuperReduced_Background_NoPCACut_ds%d.root",dir.Data(),dataset);
  //filename_cal.Form("%s/SuperReduced_Calibration_NoPCACut_ds%d.root",dir.Data(),dataset);
  //int nAdded1 = ch.Add(filename_bkg.Data());
  //int nAdded2 = ch.Add(filename_cal.Data());

  //if(nAdded1==0){cout<<"Could not add files"<<endl;}
  //if(nAdded1==0){cout<<"Could not add files"<<endl;}

  int tree_channel, multiplicity, nearEvents, farEvents;
  double energy, baselineSlope, riseTime, decayTime, delay, TVL, TVR;
  Bool_t IsNear;
  int nearEventArr[988] = {0};
  int farEventArr[988] = {0};

  //int channelV [2];
  int *channelV = new int [988]; 

  TString outString = Form("../output/ds%d_M2_3_6_MeV.root", dataset);
  TFile* outFile = TFile::Open(outString, "RECREATE");

  ch->SetbranchStatus("*", 0);
  ch->SetBranchStatus("DAQ@Pulse.fFiller.fChannel", 1);

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
  //ch.SetBranchAddress("IsNear", &IsNear);

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
  outTree->Branch("channel", &tree_channel);
  //outTree->Branch("nearChannel", &near_channel);
  //outTree->Branch("farChannel", &far_channel);
  outTree->Branch("riseTime", &riseTime);
  outTree->Branch("decayTime", &decayTime);
  outTree->Branch("delay", &delay);
  outTree->Branch("TVL", &TVL);
  outTree->Branch("TVR", &TVR);
  outTree->Branch("IsNear", &IsNear);
  
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

  cout<<"Reading data tree"<<endl;
  auto start = chrono::high_resolution_clock::now();
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
		    outTree->Fill();
		    continue;
                  }

                  // Checking if channel pair is a far channel pair and counting it if so
                  if(tree_channel == stoi(content[i][0]) && channelV[1] == stoi(content[i][2]))
                  {
                    IsNear = 0;
                    farEventArr[tree_channel-1]++;
		    outTree->Fill();
		    continue;
                  }
                }

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
  auto stop = chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> runtime = stop - start;
  
  cout<<"Reading time was "<< runtime.count()/1000.0<<" seconds"<<endl;
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

