#!/bin/bash

mkdir results

cd ./WBSLT-optimized
mkdir build
cd build
cmake ..
make
./WBAES >> ../../results/data.txt 2>&1
cd ../..

cd ./WBSLT-general
mkdir build
cd build
cmake ..
make 
./WBAES >> ../../results/data.txt 2>&1
cd ../..

cd ./CEJO-whitebox-AES-master
mkdir build
cd build
cmake ..
make 
./WBAES >> ../../results/data.txt 2>&1
cd ../..

cd ./Xiao-Lai-White-box-AES-master
mkdir build
cd build
cmake ..
make
./WBAES >> ../../results/data.txt 2>&1
cd ../.. 

cd ./Table-Redundancy-Method-for-White-box-AES-master
mkdir build 
cd build
cmake ..
make 
./WBAES >> ../../results/data.txt 2>&1
cd ../.. 

cd ./Improved-Masking-for-White-box-AES-main 
mkdir build 
cd build 
cmake .. 
make 
./WBAES >> ../../results/data.txt 2>&1
cd ../.. 
