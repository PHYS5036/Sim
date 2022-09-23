#!/bin/bash

mkdir -p ~/data

mv bashrc ~/.bashrc
source ~/.bashrc

mv ~/Sim/PHYS5036_setup/SimTools2021/SodiumIodideMC ~/
mv ~/Sim/PHYS5036_setup/SimTools2021/DoseProfileMC ~/

cd ~/DoseProfileMC/ ; gmake clean ; gmake ; cd ~
cd ~/SodiumIodideMC/ ; gmake clean ; gmake ; cd ~
bash
