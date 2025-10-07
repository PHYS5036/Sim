#!/bin/bash

if [[ $1 == Cs137 || $1 == Na22 || $1 == Co60 || $1 == Am241 ]]; 
then
    sed -i '14s/SOURCE1/'$1'/g' macros/batch.mac 
    sed -i '20s/SOURCE1/'$1'/g' macros/batch.mac 
    sed -i '14s/SOURCE1/'$1'/g' macros/vis.mac 
    sed -i '20s/SOURCE1/'$1'/g' macros/vis.mac 
    sed -i '25s/SOURCE1/'$1'/g' GenerateSpectrum.C
    sed -i '28s/SOURCE1/'$1'/g' GenerateSpectrum.C
    sed -i '33s/SOURCE1/'$1'/g' SmearSpectrum.C

    root -b -q GenerateSpectrum.C++
    GammaLabMC macros/batch.mac
    root -b -q SmearSpectrum.C++

    rm ~/data/*.root

    sed -i '14s/'$1'/SOURCE1/g' macros/batch.mac 
    sed -i '20s/'$1'/SOURCE1/g' macros/batch.mac 
    sed -i '14s/'$1'/SOURCE1/g' macros/vis.mac 
    sed -i '20s/'$1'/SOURCE1/g' macros/vis.mac 
    sed -i '25s/'$1'/SOURCE1/g' GenerateSpectrum.C
    sed -i '28s/'$1'/SOURCE1/g' GenerateSpectrum.C
    sed -i '33s/'$1'/SOURCE1/g' SmearSpectrum.C

else
    echo ' ' 
    echo 'WARNING, wrong input: please re-run as./runsim.sh [source name]'
    echo 'where [source name] can be Na22, Cs137, Co60 or Am241'
fi
