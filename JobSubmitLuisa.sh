#!/bin/bash
export LC_NUMERIC="en_US.UTF-8"
############ Define folders
hepmcFolder="./hepmc_files"
simFolder="./sim_files"
recFolder="./rec_files"
anaFolder="./ana_files"
############ Define Compact file and Folder(Don't change unless needed)
compactFolder=$(pwd)
compactFile="epic/epic_arches.xml"
############ Define Entries
nEntries=10
############ Check and create folders
mkdir -p $hepmcFolder
mkdir -p $simFolder
mkdir -p $recFolder
mkdir -p $anaFolder
edm4hep="edm4hep.root" 
PDG=(211 321)
########################Looping
for m in ${PDG[@]}; do ### PDG loop
  pdg=$m
  for i in $(seq 1 1 1);do  ### EtaBin Loop( For full eta bin scan should be 0 1 2)
    for j in $(seq 58 5 60); do  ### Momentm Loop (For full gas analysis scan should be 10 2.5 60)
      hepmcfile="$hepmcFolder/out.$i.$j.$pdg.hepmc" 
      simfile="$simFolder/sim.$nEntries.$i.$j.$pdg.$edm4hep"
      recfile="$recFolder/rec.$nEntries.$i.$j.$pdg.$edm4hep" 
      anafile="$anaFolder/ana.$nEntries.$i.$j.$pdg.$edm4hep" 

      root -l "drich-hepmc-writer_sp.C($nEntries,$i,$j,$pdg)"
      cp out.hepmc $hepmcfile

      echo "/////////////////////////////////////////////////////////////////////////////////////////"
      echo "                             Now Running npsm Simulation                                 "    
      echo "/////////////////////////////////////////////////////////////////////////////////////////"
      npsim --compactFile=$compactFolder/$compactFile --runType=run -G -N=$nEntries --inputFiles out.hepmc --outputFile=sim.$edm4hep --part.userParticleHandler='' --random.seed 0x12345678 --random.enableEventSeed
      cp sim.$edm4hep $simfile
      echo "/////////////////////////////////////////////////////////////////////////////////////////"
      echo "                             Now Running Reconstruction                                  "    
      echo "/////////////////////////////////////////////////////////////////////////////////////////"
      recon.rb -s sim.$edm4hep -r rec.$edm4hep
      cp rec.$edm4hep $recfile
      echo "/////////////////////////////////////////////////////////////////////////////////////////"
      echo "                             Now Running RICH Benchmark-plots                            "    
      echo "/////////////////////////////////////////////////////////////////////////////////////////"
      benchmark.rb -b --rec-file rec.$edm4hep --ana-file ana.$edm4hep 
      cp ana.$edm4hep $anafile
       
      ls -ltrh $hepmcfile;
      ls -ltrh $simfile;
      ls -ltrh $recfile;
      ls -ltrh $anafile;
    done
  done
done
