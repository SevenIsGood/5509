################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/AD/main.c \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/AD/sdram_init.c \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/AD/util.c 

CMD_SRCS += \
../ad.cmd 

OBJS += \
D:/CeShi/yx/test/AD/Debug/main.obj \
D:/CeShi/yx/test/AD/Debug/sdram_init.obj \
D:/CeShi/yx/test/AD/Debug/util.obj 

C_DEPS += \
./main.pp \
./sdram_init.pp \
./util.pp 

OBJS__QTD += \
"D:\CeShi\yx\test\AD\Debug\main.obj" \
"D:\CeShi\yx\test\AD\Debug\sdram_init.obj" \
"D:\CeShi\yx\test\AD\Debug\util.obj" 

C_DEPS__QTD += \
".\main.pp" \
".\sdram_init.pp" \
".\util.pp" 

C_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/AD/main.c" \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/AD/sdram_init.c" \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/AD/util.c" 


# Each subdirectory must supply rules for building sources it contributes
D:/CeShi/yx/test/AD/Debug/main.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/AD/main.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --large_memory_model --obj_directory="D:/CeShi/yx/test/AD/Debug" --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

D:/CeShi/yx/test/AD/Debug/sdram_init.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/AD/sdram_init.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --large_memory_model --obj_directory="D:/CeShi/yx/test/AD/Debug" --preproc_with_compile --preproc_dependency="sdram_init.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

D:/CeShi/yx/test/AD/Debug/util.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/AD/util.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --large_memory_model --obj_directory="D:/CeShi/yx/test/AD/Debug" --preproc_with_compile --preproc_dependency="util.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


