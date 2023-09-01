################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LIB_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SPI\ BOOTLOAD/csl5509ax.lib \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SPI\ BOOTLOAD/rts55x.lib 

C_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SPI\ BOOTLOAD/burncsi.c 

S55_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SPI\ BOOTLOAD/vectors_mcbsp.s55 

CMD_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SPI\ BOOTLOAD/burncsi.cmd 

OBJS += \
./burncsi.obj \
./vectors_mcbsp.obj 

S55_DEPS += \
./vectors_mcbsp.pp 

C_DEPS += \
./burncsi.pp 

OBJS__QTD += \
".\burncsi.obj" \
".\vectors_mcbsp.obj" 

S55_DEPS__QTD += \
".\vectors_mcbsp.pp" 

C_DEPS__QTD += \
".\burncsi.pp" 

C_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SPI BOOTLOAD/burncsi.c" 

S55_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SPI BOOTLOAD/vectors_mcbsp.s55" 


# Each subdirectory must supply rules for building sources it contributes
./burncsi.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SPI\ BOOTLOAD/burncsi.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -v5510 -g --define="_DEBUG" --define="CHIP_5509A" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --verbose_diagnostics --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="burncsi.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

./vectors_mcbsp.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SPI\ BOOTLOAD/vectors_mcbsp.s55 $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -v5510 -g --define="_DEBUG" --define="CHIP_5509A" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --verbose_diagnostics --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="vectors_mcbsp.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


