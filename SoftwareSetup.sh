#!/bin/bash

cp -n ~/.bashrc ~/bashrc.orig
cp /local/cern/Sim/bashrc ~/.bash_profile
source ~/.bash_profile

cd ~

# Include or exclude??
# TODO: in future, get students to fork so we can track their progress.
# Clone the labs repo if it doesn't exist or is empty
if [ ! -d ~/PHYS_5036-labs/.git ] && [ ! "$(ls -A ~/PHYS_5036-labs 2>/dev/null)" ]; then
    git clone https://github.com/PHYS5036/Notebooks.git ~/PHYS_5036-labs
fi
