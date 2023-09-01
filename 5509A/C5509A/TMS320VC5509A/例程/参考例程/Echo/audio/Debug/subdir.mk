################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/aic23.c \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/audio.c \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/i2c.c \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/sdram_init.c \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/util.c 

CMD_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/audio.cmd 

OBJS += \
./aic23.obj \
./audio.obj \
./i2c.obj \
./sdram_init.obj \
./util.obj 

C_DEPS += \
./aic23.pp \
./audio.pp \
./i2c.pp \
./sdram_init.pp \
./util.pp 

OBJS__QTD += \
".\aic23.obj" \
".\audio.obj" \
".\i2c.obj" \
".\sdram_init.obj" \
".\util.obj" 

C_DEPS__QTD += \
".\aic23.pp" \
".\audio.pp" \
".\i2c.pp" \
".\sdram_init.pp" \
".\util.pp" 

C_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/aic23.c" \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/audio.c" \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/i2c.c" \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/sdram_init.c" \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/util.c" 


# Each subdirectory must supply rules for building sources it contributes
./aic23.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/aic23.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --optimize_with_debug --large_memory_model --preproc_with_compile --preproc_dependency="aic23.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

./audio.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/audio.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --optimize_with_debug --large_memory_model --preproc_with_compile --preproc_dependency="audio.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

./i2c.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/i2c.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --optimize_with_debug --large_memory_model --preproc_with_compile --preproc_dependency="i2c.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

./sdram_init.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/sdram_init.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --optimize_with_debug --large_memory_model --preproc_with_compile --preproc_dependency="sdram_init.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

./util.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/Echo/util.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --optimize_with_debug --large_memory_model --preproc_with_compile --preproc_dependency="util.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


