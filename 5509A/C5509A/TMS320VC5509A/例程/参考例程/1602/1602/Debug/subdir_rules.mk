################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
E:/Program/DSP/VC5509A/1602/Debug/lcd.obj: E:/Program/DSP/VC5509A/1602/lcd.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"D:/CCS5.1/install file/ccsv5/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --define="CHIP_5509" --include_path="D:/CCS5.1/install file/ccsv5/tools/compiler/c5500/include" --include_path="/packages/ti/xdais" --include_path="/include" --quiet --diag_warning=225 --large_memory_model --obj_directory="E:/Program/DSP/VC5509A/1602/1602/../Debug" --preproc_with_compile --preproc_dependency="lcd.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


