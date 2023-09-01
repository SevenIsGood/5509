################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv7/tools/compiler/c5500_4.4.1

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := -l"./BF_rt_bios_cfg.cmd"

ORDERED_OBJS += \
"./AudioCodec_DMA.obj" \
"./BF_rt_bios_cfg.obj" \
"./BF_rt_bios_cfg_c.obj" \
"./appAudio.obj" \
"./audio_data_collection.obj" \
"./bf_asnr_mss_vau.obj" \
"./codec_aic3254.obj" \
"./codec_pcm186x.obj" \
"./csl_i2c_ioExpander.obj" \
"./led_blink_XF.obj" \
"./mem.obj" \
"./mhm.obj" \
"./mss.obj" \
"./mssproc.obj" \
"./oled.obj" \
"./pll_control.obj" \
"./sysbfflt.obj" \
"../VC5505_I2S.cmd" \
$(GEN_CMDS__FLAG) \
-lDebug/C55XXCSL_LP.lib \
-lCloakedSensoryLib.lib \
-lrts55x.lib \
-lbios.a55 \
-llibc.a \
-laer_a.a55L \
-laer_c.a55L \
-lvpe_a.a55L \
-lvpe_c.a55L \
-lutil_c.a55L \
-lutil_a.a55L \

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
c5545_voiceuicloud.out \

EXE_OUTPUTS__QUOTED += \
"c5545_voiceuicloud.out" \

BIN_OUTPUTS += \
c5545_voiceuicloud.hex \

BIN_OUTPUTS__QUOTED += \
"c5545_voiceuicloud.hex" \


# All Target
all: c5545_voiceuicloud.out

dependents:
	-cd C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/ccs_v6.x_examples/C55XXCSL_LP/Debug && $(MAKE) all

# Tool invocations
c5545_voiceuicloud.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS) C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/ccs_v6.x_examples/C55XXCSL_LP/Debug/C55XXCSL_LP.lib
	@echo 'Building target: $@'
	@echo 'Invoking: C5500 Linker'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5505 --memory_model=large -g --define=_DEBUG --define=c5505 --define=ti_targets_C55_large --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=16 --algebraic --asm_source=algebraic -z -m"c5545_voiceuicloud.map" --stack_size=0x200 --heap_size=0x400 -i"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/lib" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/build" -i"C:/ti/bios_5_42_02_10/packages/ti/rtdx/lib/c5500" -i"C:/ti/bios_5_42_02_10/packages/ti/bios/lib" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/ccs_v6.x_examples/C55XXCSL_LP" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/voice_ui_cloud/c5545" -i"/packages/ti/rtdx/lib/c5000" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/aer_c55l_cpuv3.3_obj_17_0_0_0/packages/ti/mas/aer/lib" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/volib_C55_CPU3_2_1_0_1/packages/ti/mas/vpe/lib" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/volib_C55_CPU3_2_1_0_1/packages/ti/mas/util/lib" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/voice_ui_cloud/third_party/sensory/inc" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/voice_ui_cloud/third_party/sensory/model_data/english" -i"C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/voice_ui_cloud/third_party/sensory/lib" --reread_libs --warn_sections --xml_link_info="CSL_I2S_IdleLoop_Out_linkInfo.xml" --rom_model --sys_stacksize=0x200 -o "c5545_voiceuicloud.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

c5545_voiceuicloud.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: C5500 Hex Utility'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/hex55"  -o "c5545_voiceuicloud.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(GEN_CMDS__QUOTED)$(GEN_MISC_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_HDRS__QUOTED)
	-$(RM) "BF_rt_bios_cfg.d" 
	-$(RM) "AudioCodec_DMA.obj" "BF_rt_bios_cfg.obj" "BF_rt_bios_cfg_c.obj" "appAudio.obj" "audio_data_collection.obj" "bf_asnr_mss_vau.obj" "codec_aic3254.obj" "codec_pcm186x.obj" "csl_i2c_ioExpander.obj" "led_blink_XF.obj" "mem.obj" "mhm.obj" "mss.obj" "mssproc.obj" "oled.obj" "pll_control.obj" "sysbfflt.obj" 
	-$(RM) "AudioCodec_DMA.d" "BF_rt_bios_cfg_c.d" "appAudio.d" "audio_data_collection.d" "bf_asnr_mss_vau.d" "codec_aic3254.d" "codec_pcm186x.d" "csl_i2c_ioExpander.d" "led_blink_XF.d" "mem.d" "mhm.d" "mss.d" "mssproc.d" "oled.d" "pll_control.d" "sysbfflt.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/hex55 -q -i "C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/voice_ui_cloud/c5545/Debug/c5545_voiceuicloud.out" -o "C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/voice_ui_cloud/c5545/Debug/bootimg.bin" -boot -v5505 -b -serial8
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/ccs_v6.x_examples/C55XXCSL_LP/Debug/C55XXCSL_LP.lib:

-include ../makefile.targets

