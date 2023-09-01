#!/bin/sh 

cd ../../TIesr_model_build/Data/GramDir

../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII dim.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII config.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII hmm2phone.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII o2amidx.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII pdf.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII net.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII hmm.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII gconst.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII tran.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII mixture.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII vqcentr.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII var.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII mu.bin
../../../TIesr_C55_demo/TIesrBintoASCII/TIesrBintoASCII word.lis

cp *.c ../../../TIesr_C55_demo/C55xTIesrData/GramKWS
cp *.h ../../../TIesr_C55_demo/C55xTIesrData/GramKWS
