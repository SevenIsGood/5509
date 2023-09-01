################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv7/tools/compiler/c5500_4.4.1

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := -l"./TIesrDemoC55cfg.cmd"

ORDERED_OBJS += \
"./TIesrDemoC55cfg.obj" \
"./TIesrDemoC55cfg_c.obj" \
"./audio_data_collection.obj" \
"./codec_aic3254.obj" \
"./gd_config.obj" \
"./gd_dim.obj" \
"./gd_gconst.obj" \
"./gd_hmm.obj" \
"./gd_mixture.obj" \
"./gd_mu.obj" \
"./gd_net.obj" \
"./gd_pdf.obj" \
"./gd_tran.obj" \
"./gd_var.obj" \
"./gd_word.obj" \
"./gpio_control.obj" \
"./lcd_osd.obj" \
"./pll_control.obj" \
"./recognizer.obj" \
"./sys_init.obj" \
"./user_interface.obj" \
"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/C5515.cmd" \
$(GEN_CMDS__FLAG) \
-lC55XXCSL_LP.lib \
-lTIesrEngineC55.lib \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
TIesrDemoC55.out \

EXE_OUTPUTS__QUOTED += \
"TIesrDemoC55.out" \

BIN_OUTPUTS += \
TIesrDemoC55.hex \

BIN_OUTPUTS__QUOTED += \
"TIesrDemoC55.hex" \


# All Target
all: TIesrDemoC55.out

dependents:
	-cd C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/TIesrEngineC55/ccsv5/Debug && $(MAKE) clean all
	-cd C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/ccs_v6.x_examples/C55XXCSL_LP/Debug && $(MAKE) clean all

# Tool invocations
TIesrDemoC55.out: $(OBJS) $(CMD_SRCS) $(CMD_UPPER_SRCS) $(GEN_CMDS) C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/TIesrEngineC55/ccsv5/Debug/TIesrEngineC55.lib C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/ccs_v6.x_examples/C55XXCSL_LP/Debug/C55XXCSL_LP.lib
	@echo 'Building target: $@'
	@echo 'Invoking: C5500 Linker'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic -z -m"TIesrDemoC55.map" --stack_size=0x200 --heap_size=0 -i"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/lib" -i"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" -i"C:/ti/bios_5_42_01_09/packages/ti/rtdx/lib/c5500" -i"C:/ti/bios_5_42_01_09/packages/ti/bios/lib" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/ccs_v6.x_examples/C55XXCSL_LP/Debug" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/TIesrEngineC55/ccsv5/Debug" --reread_libs --display_error_number --warn_sections --xml_link_info="TIesrDemoC55_linkInfo.xml" --rom_model --sys_stacksize=0x200 -o "TIesrDemoC55.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

TIesrDemoC55.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: C5500 Hex Utility'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/hex55"  -o "TIesrDemoC55.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(GEN_CMDS__QUOTED)$(GEN_MISC_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_HDRS__QUOTED)
	-$(RM) "TIesrDemoC55cfg.d" 
	-$(RM) "TIesrDemoC55cfg.obj" "TIesrDemoC55cfg_c.obj" "audio_data_collection.obj" "codec_aic3254.obj" "gd_config.obj" "gd_dim.obj" "gd_gconst.obj" "gd_hmm.obj" "gd_mixture.obj" "gd_mu.obj" "gd_net.obj" "gd_pdf.obj" "gd_tran.obj" "gd_var.obj" "gd_word.obj" "gpio_control.obj" "lcd_osd.obj" "pll_control.obj" "recognizer.obj" "sys_init.obj" "user_interface.obj" 
	-$(RM) "TIesrDemoC55cfg_c.d" "audio_data_collection.d" "codec_aic3254.d" "gd_config.d" "gd_dim.d" "gd_gconst.d" "gd_hmm.d" "gd_mixture.d" "gd_mu.d" "gd_net.d" "gd_pdf.d" "gd_tran.d" "gd_var.d" "gd_word.d" "gpio_control.d" "lcd_osd.d" "pll_control.d" "recognizer.d" "sys_init.d" "user_interface.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/hex55 C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/hexutil.cmd -o bootimg.bin TIesrDemoC55.out & C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/ofd55 -x -o="TIesrDemoC55.xml" "TIesrDemoC55.out" & "/bin/sectti.exe" "TIesrDemoC55.xml" > "TIesrDemoC55.txt"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/TIesrEngineC55/ccsv5/Debug/TIesrEngineC55.lib:
C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/ccs_v6.x_examples/C55XXCSL_LP/Debug/C55XXCSL_LP.lib:

-include ../makefile.targets

