################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/FIRLMS/FIRLMS.c 

CMD_SRCS += \
../test.cmd 

OBJS += \
./FIRLMS.obj 

C_DEPS += \
./FIRLMS.pp 

OBJS__QTD += \
".\FIRLMS.obj" 

C_DEPS__QTD += \
".\FIRLMS.pp" 

C_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/FIRLMS/FIRLMS.c" 


# Each subdirectory must supply rules for building sources it contributes
./FIRLMS.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/FIRLMS/FIRLMS.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="FIRLMS.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


