################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/WDT/wdt.c 

CMD_SRCS += \
E:/Dsp_Program/Porgrame_For_CCS4/5509easy/WDT/wdt.cmd 

OBJS += \
./wdt.obj 

C_DEPS += \
./wdt.pp 

OBJS__QTD += \
".\wdt.obj" 

C_DEPS__QTD += \
".\wdt.pp" 

C_SRCS_QUOTED += \
"E:/Dsp_Program/Porgrame_For_CCS4/5509easy/WDT/wdt.c" 


# Each subdirectory must supply rules for building sources it contributes
./wdt.obj: E:/Dsp_Program/Porgrame_For_CCS4/5509easy/WDT/wdt.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -g --define="_DEBUG" --define="CHIP_5509" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --include_path="C:/Program Files/Texas Instruments/xdais_6_25_01_08/packages/ti/xdais" --include_path="C:/CCSTUD~1.3/C5500/csl/include" --quiet --diag_warning=225 --preproc_with_compile --preproc_dependency="wdt.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


