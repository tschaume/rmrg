HOWTO for StRootMerger and rmrg program
=======================================

AUTHORS: Patrick Huck (CCNU/LBNL)  
EMAIL: phuck@lbl.gov  
DATE: February 2013
(c) 2012 - 2013

don't load StRoot!  

- install ```ckon```: tschaume.github.com/ckon
- modify ckon.cfg to

        boost=0
        suffix=0
        [ckon]
        src_dir=StRoot
        prog_subdir=programs
        build_dir=build
        install_dir=build
        exclSuffix="Gnuplot Options"
        NoRootCint=YamlCpp
        cppflags="-Wall"

- run ```ckon``` to install ```rmrg```
- ```./build/bin/rmrg testdir/ 7```

#### arguments:  
1 = input dir   
2 = #files to merge at once  
optional:  
3 = NrFilesStop: stop at given number of merged-files (set 0 to disable)
4 = FileSizeStop: stop at given file size of merged-files (in MB), stops when all files > FileSizeStop
