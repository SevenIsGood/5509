################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Drivers/PIN/PINCC26XX.obj: C:/ti/tirtos_simplelink_2_13_00_06/packages/ti/drivers/pin/PINCC26XX.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs612/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" --cmd_file="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/CCS/c5545BoostEqualizer/../../CCS/Application/CC2650/../../../CCS/Config/ccsCompilerDefines.bcfg"  -mv7M3 --code_state=16 --abi=eabi -me -O3 --include_path="C:/ti/ccs612/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/Source/Application" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/ICall/Include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/Profiles/Roles/CC26xx" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/Profiles/c5545BoostEqualizerProfile/CC26xx" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/Profiles/Roles" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/Profiles/Keys" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/Profiles/DevInfo" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/Profiles/SensorProfile/cc26xx" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/Profiles/OAD/cc26xxST" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Projects/ble/common/cc26xx" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/applib/heap" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/ble/hci" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/ble/controller/CC26xx/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/ble/host" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/hal/target/CC2650TIRTOS" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/hal/target/_common/cc26xx" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/hal/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/osal/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/services/sdata" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/services/saddr" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/icall/include" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/icall/ports/tirtos" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/Components/ble/include" --include_path="C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/Source/Application/Board_patch/interfaces" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/Source/Application/Board_patch/devices" --include_path="C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/Source/Application/Board_patch/CC26XXC55BP_4XS" --include_path="C:/ti/tirtos_simplelink_2_13_00_06/packages" --define=POWER_SAVING --define=GATT_TI_UUID_128_BIT --define=FEATURE_REGISTER_SERVICE --define=FACTORY_IMAGE --define=USE_ICALL --define=TI_DRIVERS_PIN_INCLUDED --define=TI_DRIVERS_I2C_INCLUDED --define=TI_DRIVERS_SPI_INCLUDED --define=GAPROLE_TASK_STACK_SIZE=550 --define=HEAPMGR_SIZE=2872 --define=ICALL_MAX_NUM_TASKS=8 --define=ICALL_MAX_NUM_ENTITIES=11 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=CC26XXWARE --define=CC26XX --define=CC26XXC55BP_4XS --diag_wrap=off --diag_suppress=48 --diag_warning=225 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="Drivers/PIN/PINCC26XX.d" --obj_directory="Drivers/PIN" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


