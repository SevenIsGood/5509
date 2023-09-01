################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
src/grlib/fonts/fontfixed6x8.obj: ../src/grlib/fonts/fontfixed6x8.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/a0232058/workspace_v7_2/IBM_Voice_UI_Demo_CC3220SF_LAUNCHXL/lib" --include_path="C:/Users/a0232058/workspace_v7_2/IBM_Voice_UI_Demo_CC3220SF_LAUNCHXL/src" --include_path="C:/Users/a0232058/workspace_v7_2/IBM_Voice_UI_Demo_CC3220SF_LAUNCHXL/src/grlib" --include_path="C:/Users/a0232058/workspace_v7_2/IBM_Voice_UI_Demo_CC3220SF_LAUNCHXL/src/LcdDriver" --include_path="C:/Users/a0232058/workspace_v7_2/IBM_Voice_UI_Demo_CC3220SF_LAUNCHXL/src/httpclient" --include_path="C:/Users/a0232058/workspace_v7_2/IBM_Voice_UI_Demo_CC3220SF_LAUNCHXL" --include_path="C:/ti/simplelink_cc32xx_sdk_1_40_00_03/kernel/tirtos/packages/ti/sysbios/posix" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --define=ti_net_http_HTTPCli__deprecated --define=CC32XX --define=__SL__ --define=NET_SL --define=HTTPCli_LIBTYPE_MIN -g --printf_support=full --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="src/grlib/fonts/fontfixed6x8.d" --obj_directory="src/grlib/fonts" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


