################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs612/ccsv6/tools/compiler/ti-cgt-arm_5.2.5

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt"
GEN_CMDS__FLAG := -l"configPkg/linker.cmd"

ORDERED_OBJS += \
"C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/CCS/Config/ccsLinkerDefines.cmd" \
"./Application/c5545BoostEqualizer.obj" \
"./Application/util.obj" \
"./Board/Interfaces/bsp_spi.obj" \
"./Drivers/I2C/I2C.obj" \
"./Drivers/I2C/I2CCC26XX.obj" \
"./Drivers/PIN/PINCC26XX.obj" \
"./Drivers/SPI/SPI.obj" \
"./Drivers/SPI/SPICC26XXDMA.obj" \
"./Drivers/UDMA/UDMACC26XX.obj" \
"./ICall/ICall.obj" \
"./ICall/ICallCC2650.obj" \
"./ICallBLE/ICallBleAPI.obj" \
"./ICallBLE/bleUserConfig.obj" \
"./PROFILES/c5545BoostEqualizerService.obj" \
"./PROFILES/devinfoservice.obj" \
"./PROFILES/gatt_uuid.obj" \
"./PROFILES/gattservapp_util.obj" \
"./PROFILES/peripheral.obj" \
"./Startup/Board.obj" \
"./Startup/ccfg.obj" \
"./Startup/main.obj" \
"C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/common/cc26xx/CCS/cc26xx_ble_app_oad.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \
-l"C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600/driverlib/bin/ccs/driverlib.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include Application/subdir_vars.mk
-include Board/Interfaces/subdir_vars.mk
-include Drivers/I2C/subdir_vars.mk
-include Drivers/PIN/subdir_vars.mk
-include Drivers/SPI/subdir_vars.mk
-include Drivers/UDMA/subdir_vars.mk
-include ICall/subdir_vars.mk
-include ICallBLE/subdir_vars.mk
-include PROFILES/subdir_vars.mk
-include Startup/subdir_vars.mk
-include TOOLS/subdir_vars.mk
-include Application/subdir_rules.mk
-include Board/Interfaces/subdir_rules.mk
-include Drivers/I2C/subdir_rules.mk
-include Drivers/PIN/subdir_rules.mk
-include Drivers/SPI/subdir_rules.mk
-include Drivers/UDMA/subdir_rules.mk
-include ICall/subdir_rules.mk
-include ICallBLE/subdir_rules.mk
-include PROFILES/subdir_rules.mk
-include Startup/subdir_rules.mk
-include TOOLS/subdir_rules.mk
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
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
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
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
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
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
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
EXE_OUTPUTS += \
c5545BoostEqualizer.out \

EXE_OUTPUTS__QUOTED += \
"c5545BoostEqualizer.out" \

BIN_OUTPUTS += \
c5545BoostEqualizer.hex \

BIN_OUTPUTS__QUOTED += \
"c5545BoostEqualizer.hex" \


# All Target
all: c5545BoostEqualizer.out

# Tool invocations
c5545BoostEqualizer.out: $(OBJS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccs612/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" --cmd_file="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/CCS/c5545BoostEqualizer/../../CCS/Application/CC2650/../../../CCS/Config/ccsCompilerDefines.bcfg"  -mv7M3 --code_state=16 --abi=eabi -me -O3 --define=POWER_SAVING --define=GATT_TI_UUID_128_BIT --define=FEATURE_REGISTER_SERVICE --define=FACTORY_IMAGE --define=USE_ICALL --define=TI_DRIVERS_PIN_INCLUDED --define=TI_DRIVERS_I2C_INCLUDED --define=TI_DRIVERS_SPI_INCLUDED --define=GAPROLE_TASK_STACK_SIZE=550 --define=HEAPMGR_SIZE=2872 --define=ICALL_MAX_NUM_TASKS=8 --define=ICALL_MAX_NUM_ENTITIES=11 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=CC26XXWARE --define=CC26XX --define=CC26XXC55BP_4XS --diag_wrap=off --diag_suppress=48 --diag_warning=225 --display_error_number --gen_func_subsections=on -z -m"c5545BoostEqualizer.map" --heap_size=0 --stack_size=256 -i"C:/ti/ccs612/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/lib" -i"C:/ti/ccs612/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --reread_libs --define=APP_BASE=0x00000000 --display_error_number --diag_suppress=10247-D --diag_wrap=off --warn_sections --xml_link_info="c5545BoostEqualizer_linkInfo.xml" --rom_model --unused_section_elimination=on --compress_dwarf=on -o "c5545BoostEqualizer.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

c5545BoostEqualizer.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccs612/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armhex"  -o "c5545BoostEqualizer.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_OPTS__QUOTED)$(GEN_CMDS__QUOTED)
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) "Application\c5545BoostEqualizer.d" "Application\util.d" "Board\Interfaces\bsp_spi.d" "Drivers\I2C\I2C.d" "Drivers\I2C\I2CCC26XX.d" "Drivers\PIN\PINCC26XX.d" "Drivers\SPI\SPI.d" "Drivers\SPI\SPICC26XXDMA.d" "Drivers\UDMA\UDMACC26XX.d" "ICall\ICall.d" "ICall\ICallCC2650.d" "ICallBLE\ICallBleAPI.d" "ICallBLE\bleUserConfig.d" "PROFILES\c5545BoostEqualizerService.d" "PROFILES\devinfoservice.d" "PROFILES\gatt_uuid.d" "PROFILES\gattservapp_util.d" "PROFILES\peripheral.d" "Startup\Board.d" "Startup\ccfg.d" "Startup\main.d" 
	-$(RM) "Application\c5545BoostEqualizer.obj" "Application\util.obj" "Board\Interfaces\bsp_spi.obj" "Drivers\I2C\I2C.obj" "Drivers\I2C\I2CCC26XX.obj" "Drivers\PIN\PINCC26XX.obj" "Drivers\SPI\SPI.obj" "Drivers\SPI\SPICC26XXDMA.obj" "Drivers\UDMA\UDMACC26XX.obj" "ICall\ICall.obj" "ICall\ICallCC2650.obj" "ICallBLE\ICallBleAPI.obj" "ICallBLE\bleUserConfig.obj" "PROFILES\c5545BoostEqualizerService.obj" "PROFILES\devinfoservice.obj" "PROFILES\gatt_uuid.obj" "PROFILES\gattservapp_util.obj" "PROFILES\peripheral.obj" "Startup\Board.obj" "Startup\ccfg.obj" "Startup\main.obj" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"C:/ti/ccs612/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armhex" -order MS --memwidth=8 --romwidth=8 --intel -o "c5545BoostEqualizer.hex" "c5545BoostEqualizer.out"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

