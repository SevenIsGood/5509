################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/240128/LED.c \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/240128/clk_init.c \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/240128/sdram_init.c 

CMD_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/240128/led.cmd 

OBJS += \
./LED.obj \
./clk_init.obj \
./sdram_init.obj 

C_DEPS += \
./LED.pp \
./clk_init.pp \
./sdram_init.pp 

OBJS__QTD += \
".\LED.obj" \
".\clk_init.obj" \
".\sdram_init.obj" 

C_DEPS__QTD += \
".\LED.pp" \
".\clk_init.pp" \
".\sdram_init.pp" 

C_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/240128/LED.c" \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/240128/clk_init.c" \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/240128/sdram_init.c" 


# Each subdirectory must supply rules for building sources it contributes
./LED.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/240128/LED.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="LED.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

./clk_init.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/240128/clk_init.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="clk_init.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

./sdram_init.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/240128/sdram_init.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="sdram_init.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


