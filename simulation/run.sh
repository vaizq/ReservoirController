#!/bin/bash

cmake -B build -S .
cmake --build build
./build/Simulator