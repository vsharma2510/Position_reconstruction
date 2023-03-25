/*
********************************************************************************************
//
// Macro to get counts and info of "near" and "far" events for a dataset. Uses text file containing channel "near" and "far" pairs and
CUORE production files.
// Command line inputs: dataset
// author: Vivek Sharma
// date: 2022-04-14
//
*******************************************************************************************
*/

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
#include "QPulseInfo.hh"
#include "QCuoreDb.hh"
#include "QChain.hh"
#include "QPulse.hh"
#include "QVector.hh"
#include "QHeader.hh"
#include "QBaselineData.hh"
//#include "QCoincidence.hh"
#include "QCoincidenceData.hh"
#include "QPulseParameters.hh"
#include "QCountPulsesData.hh"
#include "QObject.hh"
#include "QBool.hh"
#include "QOFData.hh"
#include "QBaseType.hh"

using namespace std;
using namespace Cuore;

std::vector<int> GetRuns(int dataset)
  {
    // Accessing database
    QDb::QDbTable table;
    QCuoreDb *db = QCuoreDb::Get();
    TString tQuery = Form("SELECT r.run_number FROM runs AS r, data_sets_runs AS d, runs as s WHERE r.run_number=d.run_number AND r.run_type='Reprocess' AND s.run_number=r.source_run AND s.run_type='Background' AND d.data_set=%d ORDER by r.source_run;", dataset);
    string query(tQuery.Data());
    // Getting list of background runs for dataset
    db->DoQuery(query, table);
    //TChain* qC = new TChain("qtree");
    // Storing run numbers in vector
    std::vector<int> runVector;
    if(!table["run_number"].empty())
      {
      for (unsigned int i = 0; i < table["run_number"].size(); ++i)
        {
          runVector.push_back(table["run_number"][i].GetInt());
          cout<<"Adding "<<table["run_number"][i].GetInt()<<endl;
        }
      }
  }

int main(int argc, char* argv[]){

  int dataset = stoi(argv[1]);
  //TString dir = Form("/project/projectdirs/cuore/syncData/CUORE/OfficialProcessed/ReproSpring20/output/ds%d/", dataset);
  //TString filename_bkg;

  // Getting list of run numbers and storing them in vector
  std::vector<int> runVector;
  runVector = GetRuns(dataset);
  QChain *ch = new QChain();

  // Adding list files to QChain
  for(std::vector<int>::iterator it = runVector.begin(); it != runVector.end(); ++it)
    {
      TString dir = Form("/nfs/cuore7/data/CUORE/OfficialProcessed/ReproSpring20/output/ds%d/Production_%d_R.list", dataset, (*it));
      ch->Add(dir.Data());
    }

  //filename_bkg.Form("%s/SuperReduced_Background_NoPCACut_ds%d.root",dir.Data(),dataset);
  //filename_cal.Form("%s/SuperReduced_Calibration_NoPCACut_ds%d.root",dir.Data(),dataset);
  //int nAdded1 = ch->Add(filename_bkg.Data());
  //int nAdded2 = ch->Add(filename_cal.Data());

  //if(nAdded1==0){cout<<"Could not add files"<<endl;}
  //if(nAdded1==0){cout<<"Could not add files"<<endl;}

  //int nearEvents, farEvents;
  Bool_t isNear = false;
  //int nearEventArr[988] = {0};
  //int farEventArr[988] = {0};

  //int channelV [2];
  int *channelV = new int [988];

  TString outString = Form("../output/ds%d_M2_3_6_MeV.root", dataset);
  TFile* outFile = TFile::Open(outString, "RECREATE");

  ch->SetBranchStatus("*", 0);
  ch->SetBranchStatus("DAQ@PulseInfo.*", 1); // Pulse Info
  //ch->SetBranchStatus("DAQ@Pulse.*", 1);
  ch->SetBranchStatus("DAQ@Header*", 1); // Header
  //ch->SetBranchStatus("FilteredPulseAmplitude_wOF@OFData.*", 1);
  ch->SetBranchStatus("ApplySelectedEnergy@SelectedEnergy.*", 1); //Energy
  ch->SetBranchStatus("RejectBadIntervals@Passed.*", 1);// Bad Intervals
  ch->SetBranchStatus("SampleInfoFilter@Passed.*", 1); // Single Trigger
  ch->SetBranchStatus("BadForAnalysis_GoodForAnalysis@Passed.*", 1); // Bad for analysis
  ch->SetBranchStatus("BCountPulses@CountPulsesData.*", 1); // Number of pulses
  ch->SetBranchStatus("FastCoincidence_70keV_150ms_BM@CoincidenceData.*", 1); // Multiplicity, Multiplet vector
  ch->SetBranchStatus("BaselineModule@BaselineData.*", 1); // Baseline slope
  ch->SetBranchStatus("PulseBasicParameters@Parameters.*", 1); // Basic  - Rise time, Decay time
  ch->SetBranchStatus("FilteredPulseAmplitude@OFData.*"); // OF - Rise time, decay time, tvl, tvr, delay

  //Accessing tree branches
  QPulse* pulse = new QPulse;
  ch->SetBranchAddress("DAQ@Pulse.", &pulse);

  QPulseInfo* pulseInfo = 0;
  int treeChannel = 0;
  bool isSignal = false;
  ch->SetBranchAddress("DAQ@PulseInfo.", &pulseInfo);

  QHeader* header = 0;
  ch->SetBranchAddress("DAQ@Header.", &header);

  QOFData* of = 0;
  double riseTime, decayTime, ofDelay, tvl, tvr;
  ch->SetBranchAddress("FilteredPulseAmplitude@OFData.", &of);

  QBaseType<double>* energy = 0;
  double Energy = 0;
  ch->SetBranchAddress("ApplySelectedEnergy@SelectedEnergy.", &energy);

  QBool* rbi = 0;
  bool rejectBadIntervals = false;
  ch->SetBranchAddress("RejectBadIntervals@Passed.", &rbi);

  QBool* st = 0;
  bool singleTrigger = false;
  ch->SetBranchAddress("SampleInfoFilter@Passed.", &st);

  QBool* bfa = 0;
  bool badForAnalysis = false;
  ch->SetBranchAddress("BadForAnalysis_GoodForAnalysis@Passed.", &bfa);

  QCountPulsesData* count = 0;
  int numberOfPulses = 0;
  ch->SetBranchAddress("BCountPulses@CountPulsesData.", &count);

  QCoincidenceData* coinc = 0;
  int multiplicity = 0;
  std::vector<QCoincidentChannel> coincVector;
  ch->SetBranchAddress("FastCoincidence_70keV_150ms_BM@CoincidenceData.", &coinc);

  QBaselineData* bsl = 0;
  double baselineSlope = 0;
  ch->SetBranchAddress("BaselineModule@BaselineData.", &bsl);

  QPulseParameters* pbp = 0;
  double basicRiseTime, basicDecayTime = 0;
  ch->SetBranchAddress("PulseBasicParameters@Parameters.", &pbp);

  int numEntries = ch->GetEntries();
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
  int outChannel=0;

  TTree* outTree = new TTree("outputTree", "outputTree");
  outTree->Branch("channel", &outChannel);
  //outTree->Branch("nearChannel", &near_channel);
  //outTree->Branch("farChannel", &far_channel);
  outTree->Branch("ofRiseTime", &riseTime);
  outTree->Branch("ofDecayTime", &decayTime);
  outTree->Branch("ofDelay", &ofDelay);
  outTree->Branch("TVL", &tvl);
  outTree->Branch("TVR", &tvr);
  outTree->Branch("isNear", &isNear);
  outTree->Branch("basicRiseTime", &basicRiseTime);
  outTree->Branch("basicDecayTime", &basicDecayTime);
  outTree->Branch("baselineSlope", &baselineSlope);

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
	    ch->GetEntry(e);
      ofDelay = (of->GetDelay())/1e3;
      isSignal =pulseInfo->GetMasterSample().GetIsSignal();
      Energy = (double)(*energy);
      rejectBadIntervals = (bool)(*bfa);
      badForAnalysis = (bool)(*bfa);
      singleTrigger = (bool)(*st);
      numberOfPulses = count->GetNumberOfPulses();
      treeChannel = pulseInfo->GetChannelId();
      multiplicity = coinc->fMultiplicity;
      coincVector = coinc->fCoincidentChannels;

	      //cout<<"Working on tree_channel "<<tree_channel<<" multiplicity "<<multiplicity<<" energy "<<energy<<endl;
	    if(multiplicity==2 && isSignal && rejectBadIntervals && badForAnalysis && singleTrigger && numberOfPulses==1 && Energy<6000 && Energy>3000)
	      {
          channelV[0] = coincVector[0].fChannelId;
          channelV[1] = coincVector[1].fChannelId;
          //outTree->Fill();
          for(int i=0;i<988;i++)
            {
              // Checking if channel pair is a near channel pair and counting it if so
              if(treeChannel == stoi(content[i][0]) && channelV[1] == stoi(content[i][1]))
                {
                  isNear = 1;
                  //nearEventArr[tree_channel-1]++;
		              outTree->Fill();
		              continue;
                }

              // Checking if channel pair is a far channel pair and counting it if so
              if(treeChannel == stoi(content[i][0]) && channelV[1] == stoi(content[i][2]))
                {
                  isNear = 0;
                  //farEventArr[tree_channel-1]++;
		              outTree->Fill();
		              continue;
                }
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
  /*
  for(int i=0;i<988;i++)
  {
    channel = stoi(content[i][0]);
    nearEvents = nearEventArr[channel-1];
    farEvents = farEventArr[channel-1];
    countsTree->Fill();
  }
  */

  outFile->cd();
  outFile->Write("outTree");
  //outFile->Write("countsTree");
  outFile->Close();

  return 0;
}

