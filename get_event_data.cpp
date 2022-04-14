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

  TString dir = "/project/projectdirs/cuore/syncData/CUORE/OfficialProcessed/ReproSpring20/output/";
  TString listname;
  //int runs [58];
  QChain ch("qtree");
  for(int i=0;i<58;i++)
    {
      cout<<"Working on run "<<351032+i<<endl;
      for(int j=1;j<20;j++)
	{
	  listname.Form("%s/ds%d/FinalizedReduced_%d_%.3d_R.list", dir.Data(), dataset, 351032+i, j);
	}
      cout<<"Working on file "<<listname.Data()<<endl;
      int nAdded = ch.Add(listname.Data());
      if(nAdded==0){cout<<"nAdded did not work"<<endl;exit(1);}
      
    }
  return 0;
}

