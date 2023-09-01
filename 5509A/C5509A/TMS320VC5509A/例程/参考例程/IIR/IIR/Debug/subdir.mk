################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/IIR/iir.c 

CMD_SRCS += \
../IIR.cmd 

OBJS += \
./iir.obj 

C_DEPS += \
./iir.pp 

OBJS__QTD += \
".\iir.obj" 

C_DEPS__QTD += \
".\iir.pp" 

C_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/IIR/iir.c" 


# Each subdirectory must supply rules for building sources it contributes
./iir.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/IIR/iir.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="iir.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


