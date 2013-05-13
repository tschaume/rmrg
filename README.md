HOWTO for StRootMerger and rmrg program
=======================================

AUTHORS: Patrick Huck (CCNU/LBNL)  
EMAIL: phuck@lbl.gov  
DATE: February 2013
(c) 2012 - 2013  
released under MIT License

based on hadd.C of http://www.root.cern.ch  

- provides additional fine-grained control currently missing in hadd.
- automatic file-list generation based on an input directory
- step-by-step verbose TTree and Histogram merging.
- rmrg: options for number of files to merge at once and minimum file size.
- ...

### rmrg

if you want to use the `rmrg` program to control StRootMerger, do:

- don't load StRoot!  (STAR software)
- install ```ckon```: http://tschaume.github.io/ckon/
- modify ckon.cfg to

        suffix=0
        yaml=0
        [ckon]
        src_dir=src
        prog_subdir=programs
        build_dir=build
        install_dir=build
        exclSuffix="Gnuplot Options"
        NoRootCint=YamlCpp
        cppflags="-Wall"
        boost=""

- run ```ckon``` to install ```rmrg```
- ```./build/bin/rmrg testdir/ 7```

alternatively, you can always control StRootMerger from a ROOT macro. 

#### arguments:  
1 = input dir   
2 = #files to merge at once  
optional:  
3 = NrFilesStop: stop at given number of merged-files (set 0 to disable)  
4 = FileSizeStop: stop at given file size of merged-files (in MB), stops when all files > FileSizeStop

