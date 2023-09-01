################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
DSK5509_USB.obj: E:/Program/DSP/VC5509A/USB/DSK5509_USB.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"D:/CCS5.1/install file/ccsv5/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --define="CHIP_5509" --include_path="D:/CCS5.1/install file/ccsv5/tools/compiler/c5500/include" --include_path="/packages/ti/xdais" --include_path="/include" --quiet --verbose_diagnostics --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="DSK5509_USB.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

vectors.obj: E:/Program/DSP/VC5509A/USB/vectors.s55 $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"D:/CCS5.1/install file/ccsv5/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --define="CHIP_5509" --include_path="D:/CCS5.1/install file/ccsv5/tools/compiler/c5500/include" --include_path="/packages/ti/xdais" --include_path="/include" --quiet --verbose_diagnostics --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="vectors.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


