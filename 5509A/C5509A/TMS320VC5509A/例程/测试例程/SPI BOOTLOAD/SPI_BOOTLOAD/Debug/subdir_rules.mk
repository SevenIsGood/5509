################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
burncsi.obj: E:/online\ store/网盘资料/TMS320VC5509A/例程/测试例程/SPI\ BOOTLOAD/burncsi.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"D:/CCS5.1/ccsv5/tools/compiler/c5500/bin/cl55" -v5510 -g --define="_DEBUG" --define="CHIP_5509A" --include_path="D:/CCS5.1/ccsv5/tools/compiler/c5500/include" --include_path="/packages/ti/xdais" --include_path="/include" --quiet --verbose_diagnostics --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="burncsi.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

vectors_mcbsp.obj: E:/online\ store/网盘资料/TMS320VC5509A/例程/测试例程/SPI\ BOOTLOAD/vectors_mcbsp.s55 $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"D:/CCS5.1/ccsv5/tools/compiler/c5500/bin/cl55" -v5510 -g --define="_DEBUG" --define="CHIP_5509A" --include_path="D:/CCS5.1/ccsv5/tools/compiler/c5500/include" --include_path="/packages/ti/xdais" --include_path="/include" --quiet --verbose_diagnostics --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="vectors_mcbsp.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


