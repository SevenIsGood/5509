################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LIB_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/USB/DSK5509_USBLIB.lib \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/USB/csl5509x.lib \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/USB/rts55x.lib 

C_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/USB/DSK5509_USB.c 

S55_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/USB/vectors.s55 

CMD_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/USB/DSK5509_USB.cmd 

OBJS += \
./DSK5509_USB.obj \
./vectors.obj 

S55_DEPS += \
./vectors.pp 

C_DEPS += \
./DSK5509_USB.pp 

OBJS__QTD += \
".\DSK5509_USB.obj" \
".\vectors.obj" 

S55_DEPS__QTD += \
".\vectors.pp" 

C_DEPS__QTD += \
".\DSK5509_USB.pp" 

C_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/USB/DSK5509_USB.c" 

S55_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/USB/vectors.s55" 


# Each subdirectory must supply rules for building sources it contributes
./DSK5509_USB.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/USB/DSK5509_USB.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --define="CHIP_5509" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --verbose_diagnostics --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="DSK5509_USB.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

./vectors.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/USB/vectors.s55 $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --define="CHIP_5509" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --verbose_diagnostics --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="vectors.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


