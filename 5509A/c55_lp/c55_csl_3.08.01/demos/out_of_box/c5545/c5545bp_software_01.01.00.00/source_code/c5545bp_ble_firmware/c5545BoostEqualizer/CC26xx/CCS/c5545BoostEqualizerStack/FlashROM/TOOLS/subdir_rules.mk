################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
TOOLS/OnBoard.obj: C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/common/cc26xx/OnBoard.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs612/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" --cmd_file="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/CCS/c5545BoostEqualizerStack/../../CCS/Stack/CC2650/../../../../../config/buildComponents.opt" --cmd_file="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/CCS/c5545BoostEqualizerStack/../../CCS/Stack/CC2650/buildConfig.opt"  -mv7M3 --code_state=16 --abi=eabi -me -O4 --opt_for_speed=0 --include_path="C:/ti/ccs612/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/Source/Stack" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/common/cc26xx" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/hal/target/CC2650TIRTOS" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/hal/target/_common/cc26xx" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/hal/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/osal/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/services/saddr" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/icall/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/ble/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/ble/controller/CC26xx/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/ble/ROM" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/ble/hci" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/ble/host" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/services/aes/CC26xx" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/npi" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/common/npi/npi_np/CC26xx/Stack" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/Include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/ICall/Include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/Profiles/Roles" --include_path="C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/common/cc26xx/boards/sensortag_cc26xx/resources" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/bsp/sensortag_cc26xx/drivers/source" --define=CC26XX --define=USE_ICALL --define=FLASH_ROM_BUILD --define=POWER_SAVING --define=GATT_NO_CLIENT --define=NO_OSAL_SNV --define=INCLUDE_AES_DECRYPT --define=xPM_DISABLE_PWRDOWN --define=xTESTMODES --define=xTEST_BLEBOARD --define=OSAL_CBTIMER_NUM_TASKS=1 --define=xDEBUG --define=HALNODEBUG --define=xDEBUG_GPIO --define=xDEBUG_ENC --define=xDEBUG_SW_TRACE --define=NEAR_FUNC= --define=DATA= --define=OSAL_MAX_NUM_PROXY_TASKS=8 --define=CC26XXWARE --define=ccs --define=DEBUG --diag_wrap=off --diag_suppress=48 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="TOOLS/OnBoard.d" --obj_directory="TOOLS" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


