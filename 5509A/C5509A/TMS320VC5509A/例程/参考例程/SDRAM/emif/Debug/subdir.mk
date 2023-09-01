################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LIB_SRCS += \
../csl5509x.lib \
../rts55x.lib 

C_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SDRAM/emif.c 

CMD_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SDRAM/emif.cmd 

OBJS += \
./emif.obj 

C_DEPS += \
./emif.pp 

OBJS__QTD += \
".\emif.obj" 

C_DEPS__QTD += \
".\emif.pp" 

C_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SDRAM/emif.c" 


# Each subdirectory must supply rules for building sources it contributes
./emif.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/SDRAM/emif.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --define="CHIP_5509" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="emif.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


