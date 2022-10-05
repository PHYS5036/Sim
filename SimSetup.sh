#!/bin/bash

mkdir -p ~/data

cp -n ~/.bashrc ~/bashrc.orig
cp /local/cern/software/Sim/bashrc ~/.bashrc

bash
