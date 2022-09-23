#!/bin/bash

check=`echo "$2" | grep -E ^\-?[0-9]*\.?[0-9]+$`
if [ "$check" != '' ]; then    
    if [[ $1 == gamma || $1 == proton || $1 == ion ]]; 
    then
	sed -i '14,15s/BEAMX/'$1'/g' macros/batch.mac 
	sed -i '14,15s/ENERGYY/'$2'/g' macros/batch.mac 
	sed -i '14,15s/BEAMX/'$1'/g' macros/vis.mac 
	sed -i '14,15s/ENERGYY/'$2'/g' macros/vis.mac 

	root -b -q GenerateBeam.C++\(\"$1\",$2,10000,\"data/Gen_$1_$2.root\"\)
	./DoseProfileMC macros/batch.mac
	root -b -q PlotDoseL.C++\(\"data/G4_$1_$2.root\",\"all\"\)
	root -b -q PlotDoseT.C++\(\"data/G4_$1_$2.root\",\"all\"\)
	root -b -q PlotDoseP.C++\(\"data/G4_$1_$2.root\",$2/5\)
    
	rm ~/data/*.root

	sed -i '14,15s/'$1'/BEAMX/g' macros/batch.mac 
	sed -i '14,15s/'$2'/ENERGYY/g' macros/batch.mac 
	sed -i '14,15s/'$1'/BEAMX/g' macros/vis.mac 
	sed -i '14,15s/'$2'/ENERGYY/g' macros/vis.mac 
    else
	echo 'WARNING, wrong input: please re-run as eg ./runsim.sh [beam type] [energy]'
	echo 'where [beam type] is either gamma, electron, proton or ion and [energy] is in MeV'
    fi
else
    echo 'WARNING, wrong input: please re-run as eg ./runsim.sh [beam type] [energy]'
    echo 'where [beam type] is either gamma, electron, proton or ion and [energy] is in MeV'

fi


