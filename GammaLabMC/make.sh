#!/bin/bash

cmake -B build -S .
cmake --build build -j8 
cmake --install build --prefix ../bin/
