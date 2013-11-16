#include "StRootMerger.h"
#include <Riostream.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>
#include <TChain.h>
using namespace std;

const Int_t NrClTypes = 2;
TString clTypes[NrClTypes] = { "TH1" , "TTree" };
TChain* globChain = NULL;

ClassImp(StRootMerger)

StRootMerger::StRootMerger(const Bool_t& b) : mHistosOnly(b) { }

TList* StRootMerger::GenerateFileList(TString dir, Int_t &nfiles, Int_t start, Int_t end) {
  // function to generate filelist of given directory
  void *pdir = gSystem->OpenDirectory(dir.Data());
  const char *file = NULL; Int_t nrf = 0; TString url = "";
  TList *filelist = new TList(); TFile* tmpfile = NULL;
  struct stat st_buf;
  while ((file = gSystem->GetDirEntry(pdir))) {
    url = dir.Data();
    url += file;
    stat(url.Data(),&st_buf);
    if ( S_ISDIR(st_buf.st_mode) ) continue;
    if ( ( start >= 0 && nrf >= start && nrf < end ) || start < 0 ) {
      tmpfile = TFile::Open(url.Data());
      filelist->Add(tmpfile);
      if ( start >= 0 ) cout << nrf << " : " << file << endl;
    }
    nrf++;
  }
  if ( start < 0 ) {
    nfiles = nrf;
    cout << "-----------------------------------------" << endl;
    cout << "total number of files to process: " << nfiles << endl;
  }
  else {
    cout << "generated filelist from file " << start << " to file " << end-1 << endl;
  }
  return filelist;
}

Bool_t StRootMerger::ObjInheritsFromTClass(TObject* obj, TClass* tcl) {
  return obj->IsA()->InheritsFrom( tcl );
}

Bool_t StRootMerger::TypeIsImplemented(TObject* obj, const char* firstname) {
  for ( Int_t nct = 0; nct < NrClTypes; ++nct ) {
    TClass* tcl = TClass::GetClass(clTypes[nct].Data());
    if ( clTypes[nct].Contains("TTree") ) {
      globChain = new TChain(obj->GetName());
      globChain->Add(firstname);
    }
    else {
      tcl->SetDirectoryAutoAdd(0);
    }
    if ( ObjInheritsFromTClass(obj,tcl) ) { return kTRUE; }
  }
  return kFALSE;
}

Bool_t StRootMerger::callSpecificMerger(TObject* obj, const char* nextname) {
  TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(obj->GetName());
  if (key2) {

    if ( ObjInheritsFromTClass(obj,TH1::Class()) ) {
      TH1* h1 = (TH1*)obj;
      TH1* h2 = (TH1*)key2->ReadObj();
      Bool_t mrgOk = h1->Add( h2 );
      delete h2;
      return mrgOk;
    }

    if ( ObjInheritsFromTClass(obj,TTree::Class()) ) {
      return globChain->Add(nextname);
    }

  }
  return kFALSE;
}

void StRootMerger::MergeObjects(TDirectory *target, TList *filelist) {

  cout << "Target path: " << target->GetPath() << endl;
  TString path( (char*)strstr( target->GetPath(), ":" ) );
  path.Remove( 0, 2 );
  TFile *first_source = (TFile*)filelist->First();
  first_source->cd(path);
  TDirectory *current_sourcedir = gDirectory;
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key, *oldkey=0;
  while ( (key = (TKey*)nextkey())) {
    if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;
    oldkey = key;
    first_source->cd(path);
    TObject *obj = key->ReadObj();
    if ( mHistosOnly && ObjInheritsFromTClass(obj,TTree::Class()) ) continue;
    if ( TypeIsImplemented(obj,first_source->GetName()) ) {
      cout << "Merging object " << obj->GetName() << " ... " << flush;
      TFile *nextsource = (TFile*)filelist->After( first_source );
      Bool_t mrgOk = kTRUE;
      while ( nextsource ) {
	nextsource->cd(path);
	mrgOk = callSpecificMerger(obj,nextsource->GetName());
	nextsource = (TFile*)filelist->After( nextsource );
      }
      if ( mrgOk ) { cout << "ok" << endl; }
      else { cout << "failed" << endl; }
      target->cd();
      if ( ObjInheritsFromTClass(obj,TTree::Class()) ) {
	globChain->Merge(target->GetFile(),0,"keepfast");
      }
      else {
	obj->Write( key->GetName() );
      }
    }
    else if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
      cout << "Found subdirectory " << obj->GetName() << endl;
      target->cd();
      TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );
      MergeObjects( newdir, filelist);
    }
    else continue;
  }
  target->SaveSelf(kTRUE);
  return;
}

