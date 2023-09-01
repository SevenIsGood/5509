################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
D:/CCStudio_v3.3/MyProjects/5509/GPIO/Debug/gpio.obj: D:/allen/workSvn/5509A/C5509A/Program/GPIO/gpio.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5509A --memory_model=large -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="D:/BaiduNetdiskDownload/5509example/test1/C5500include" --include_path="C:/ti/xdais_7_24_00_04/packages/ti/xdais" --include_path="/include" --define="_DEBUG" --define="CHIP_5509" --define=c5509a --quiet --display_error_number --ptrdiff_size=32 --obj_directory="D:/CCStudio_v3.3/MyProjects/5509/GPIO/Debug" --preproc_with_compile --preproc_dependency="gpio.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


