################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
FIRLMS/FIRLMS.obj: ../FIRLMS/FIRLMS.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"D:/ti/ccs5.5/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5509A --memory_model=large -g --include_path="D:/ti/ccs5.5/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="../Include" --include_path="../../Include" --define=c5509a --define=CHIP_5509A --display_error_number --diag_warning=225 --ptrdiff_size=32 --preproc_with_compile --preproc_dependency="FIRLMS/FIRLMS.pp" --obj_directory="FIRLMS" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

