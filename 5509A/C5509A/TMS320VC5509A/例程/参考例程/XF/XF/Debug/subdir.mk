################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LIB_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/XF/csl5509.lib \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/XF/rts55.lib 

C_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/XF/timer.c 

S55_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/XF/vectors.s55 

CMD_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/XF/timer.cmd 

OBJS += \
./timer.obj \
./vectors.obj 

S55_DEPS += \
./vectors.pp 

C_DEPS += \
./timer.pp 

OBJS__QTD += \
".\timer.obj" \
".\vectors.obj" 

S55_DEPS__QTD += \
".\vectors.pp" 

C_DEPS__QTD += \
".\timer.pp" 

C_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/XF/timer.c" 

S55_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/XF/vectors.s55" 


# Each subdirectory must supply rules for building sources it contributes
./timer.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/XF/timer.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --define="CHIP_5509" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --preproc_with_compile --preproc_dependency="timer.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

./vectors.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/XF/vectors.s55 $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --define="CHIP_5509" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --preproc_with_compile --preproc_dependency="vectors.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


