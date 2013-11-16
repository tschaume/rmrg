/*!
 * \class StRootMerger
 *
 * \author Patrick Huck
 *
 */

#ifndef StRootMerger_h
#define StRootMerger_h

#include <cstdlib>
#include <sys/stat.h>
#include <TList.h>
#include <TString.h>
#include <TDirectory.h>
#include <TKey.h>
#include <TClass.h>

class StRootMerger {

 private: 
   Bool_t mHistosOnly;

 public:

   StRootMerger(const Bool_t&);
   virtual ~StRootMerger() {};

   Bool_t callSpecificMerger(TObject* obj, const char* nextname) ;
   Bool_t ObjInheritsFromTClass(TObject* obj, TClass* tcl) ;
   Bool_t TypeIsImplemented(TObject* obj, const char* firstname) ;
   TList* GenerateFileList(TString dir, Int_t& nfiles, Int_t start = -1, Int_t end = -1) ;
   void MergeObjects(TDirectory *target, TList *filelist) ;

  ClassDef(StRootMerger,0)
};
#endif

