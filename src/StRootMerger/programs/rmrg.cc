/*!
 *
 * \class rmrg
 *
 * \brief program for merging objects in root files manually 
 *
 * \par Purpose:
 * program for merging ROOT objects. List of files in a 
 * directory is automatically generated. All histograms
 * found in the filelist are merged and their directory structure is preserved.
 * Based on root hadd.C example.
 *
 */

#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <string>
#include <TApplication.h>
#include <TFile.h>
#include <TList.h>
#include <TString.h>
#include <TDirectory.h>
#include <TH1.h>
#include <TKey.h>
#include <TObject.h>
#include <TSystemFile.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TClass.h>
#include "src/StRootMerger/StRootMerger.h"
using namespace std;

int main(int argc, char **argv) {

  TApplication* theApp = new TApplication("App", &argc, argv);

  if ( theApp->Argc() >= 3 && theApp->Argc() <= 5 ) {
    // 0 = program name
    // 1 = input dir
    // 2 = #files to merge at once
    // optional:
    // 3 = NrFilesStop: stop at given number of merged-files (set 0 to disable)
    // 4 = FileSizeStop: stop at given file size of merged-files

    // copy argv to allow for optional arguments
    string args[5];
    for ( Int_t i = 0; i < 5; i++ ) { args[i] = "Null"; }
    for ( Int_t i = 1; i < theApp->Argc() && i < 5; i++ ) {
      args[i] = theApp->Argv(i);
      cout << "args[" << i << "] = " << args[i] << endl;
    }

    // definitions
    Int_t nrdiv = 1000000; // initialized w/ very high number of divs
    Int_t NrFiles = 1000000; // initialized w/ very high number of files
    Int_t mfiles = atoi(args[2].c_str());
    Int_t NrFilesStop = -1; // disabled by default
    if ( args[3].compare("Null") != 0 ) NrFilesStop = atoi(args[3].c_str());
    Float_t FileSizeStop = 1000000; // MB, disabled by default
    Float_t minFileSize = -1.; // only init for first while condition
    if ( args[4].compare("Null") != 0 ) FileSizeStop = atof(args[4].c_str());
    TString indir, subdir, outdir, mstr;
    Int_t nrIter = 0;
    TList* filelist = NULL;
    TFile* hpico  = NULL;
    Int_t start, end;
    Int_t nf_tmp;

    StRootMerger* rmrg = new StRootMerger();

    cout << "NrFilesStop = " << NrFilesStop << "    FileSizeStop = " << FileSizeStop << endl;

    while ( nrdiv > 1 && nrdiv > NrFilesStop && (minFileSize < FileSizeStop) ) {

      // get input
      cout << "=========================================" << endl;
      cout << "nrIter = " << nrIter << endl;
      subdir = (nrIter>0) ? Form("merged%d/",nrIter) : "";
      cout << "subdir = " << subdir.Data() << endl;
      indir = Form("%s%s",args[1].c_str(),subdir.Data());
      cout << "indir = " << indir.Data() << endl;
      rmrg->GenerateFileList(indir,NrFiles); // only set total number of files in input directory (NrFiles)
      nrdiv = !(NrFiles%mfiles) ? NrFiles/mfiles : NrFiles/mfiles+1;
      cout << "==> nrdiv = " << nrdiv << endl;
      cout << "-----------------------------------------" << endl;

      // create output subdirectory 
      outdir = Form("%smerged%d/",args[1].c_str(),nrIter+1);
      gSystem->mkdir(outdir.Data());

      // loop divs
      minFileSize = 1000000;
      for ( Int_t f = 0; f < nrdiv; ++f ) {
	start = f*mfiles; end = (f+1)*mfiles; // current set of files being merged
	// generate filelist for current set of files to merge
	filelist = rmrg->GenerateFileList(indir,nf_tmp,start,end); 
	mstr = Form("%smerged%d-div%d.root",outdir.Data(),nrIter+1,f);
	cout << "mstr = " << mstr.Data() << endl;
	hpico = TFile::Open(mstr.Data(),"recreate");
	rmrg->MergeObjects(hpico,filelist);// merge
        Float_t hpicofilesize = (Float_t)(hpico->GetSize())/1024./1024.; // MB
        if ( hpicofilesize < minFileSize ) minFileSize = hpicofilesize; // gives the currently smallest file
	hpico->Close();
	cout << "-----------------------------------------" << endl;
      }
      gSystem->Exec(Form("ls -l %s",outdir.Data()));

      cout << "current smallest filesize = " << minFileSize << " MB" << endl;
      if ( nrIter > 0 ) { // remove temporary folders
        cout << "removing temporary folders for nrIter " << nrIter << endl;
        gSystem->Exec(Form("rm -rfv %smerged%d",args[1].c_str(),nrIter));
      }

      ++nrIter;
    } // merging done

  }
  else {
    cout << "Usage:" << endl;
    cout << "./rmrg InputDir mfiles [ NrFilesStop [FileSizeStop] ]" << endl;
  }

  cout << "==> Application finished." << endl;

  return 0;

}
