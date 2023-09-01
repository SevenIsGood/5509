################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/CCS5.1/ccsv5/tools/compiler/c5500

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./vectors_mcbsp.obj" \
"./burncsi.obj" \
"E:/online store/网盘资料/TMS320VC5509A/例程/测试例程/SPI BOOTLOAD/burncsi.cmd" \
"E:/online store/网盘资料/TMS320VC5509A/例程/测试例程/SPI BOOTLOAD/csl5509ax.lib" \
"E:/online store/网盘资料/TMS320VC5509A/例程/测试例程/SPI BOOTLOAD/rts55x.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: E:/online\ store/网盘资料/TMS320VC5509A/例程/测试例程/SPI\ BOOTLOAD/SPI_BOOTLOAD/../Debug/burncsi.out

# Tool invocations
E:/online\ store/网盘资料/TMS320VC5509A/例程/测试例程/SPI\ BOOTLOAD/SPI_BOOTLOAD/../Debug/burncsi.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C5500 Linker'
	"D:/CCS5.1/ccsv5/tools/compiler/c5500/bin/cl55" -v5510 -g --define="_DEBUG" --define="CHIP_5509A" --quiet --verbose_diagnostics --diag_warning=225 --large_memory_model -z -m"E:/online store/网盘资料/TMS320VC5509A/例程/测试例程/SPI BOOTLOAD/SPI_BOOTLOAD/../Debug/burncsi.map" --stack_size=0x400 --heap_size=0x400 --warn_sections -i"D:/CCS5.1/ccsv5/tools/compiler/c5500/lib" -i"D:/CCS5.1/ccsv5/tools/compiler/c5500/include" -i"E:/online store/网盘资料/TMS320VC5509A/例程/测试例程/SPI BOOTLOAD/SPI_BOOTLOAD" -i"E:/online store/网盘资料/TMS320VC5509A/例程/测试例程/SPI BOOTLOAD/SPI_BOOTLOAD/.." -i"/lib" --reread_libs --rom_model --sys_stacksize=0x400 -o "E:/online store/网盘资料/TMS320VC5509A/例程/测试例程/SPI BOOTLOAD/SPI_BOOTLOAD/../Debug/burncsi.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C5500_EXECUTABLE_OUTPUTS__QUOTED) "E:\online store\网盘资料\TMS320VC5509A\例程\测试例程\SPI BOOTLOAD\Debug\burncsi.out"
	-$(RM) "burncsi.pp" 
	-$(RM) "burncsi.obj" "vectors_mcbsp.obj" 
	-$(RM) "vectors_mcbsp.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
