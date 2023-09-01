################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1483366785:
	@$(MAKE) -Onone -f subdir_rules.mk build-1483366785-inproc

build-1483366785-inproc: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/TIesrDemoC55.tcf
	@echo 'Building file: $<'
	@echo 'Invoking: TConf'
	"C:/ti/bios_5_42_01_09/xdctools/tconf" -b -Dconfig.importPath="C:/ti/bios_5_42_01_09/packages;" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TIesrDemoC55cfg.cmd: build-1483366785 C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/TIesrDemoC55.tcf
TIesrDemoC55cfg.s??: build-1483366785
TIesrDemoC55cfg_c.c: build-1483366785
TIesrDemoC55cfg.h: build-1483366785
TIesrDemoC55cfg.h??: build-1483366785
TIesrDemoC55.cdb: build-1483366785

TIesrDemoC55cfg.obj: ./TIesrDemoC55cfg.s?? $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="TIesrDemoC55cfg.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

TIesrDemoC55cfg_c.obj: ./TIesrDemoC55cfg_c.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="TIesrDemoC55cfg_c.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

audio_data_collection.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/src/audio_data_collection.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="audio_data_collection.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

codec_aic3254.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/src/codec_aic3254.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="codec_aic3254.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_config.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_config.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_config.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_dim.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_dim.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_dim.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_gconst.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_gconst.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_gconst.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_hmm.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_hmm.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_hmm.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_mixture.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_mixture.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_mixture.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_mu.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_mu.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_mu.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_net.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_net.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_net.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_pdf.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_pdf.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_pdf.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_tran.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_tran.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_tran.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_var.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_var.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_var.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gd_word.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55xTIesrData/GramKWS/gd_word.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gd_word.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gpio_control.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/src/gpio_control.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="gpio_control.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

lcd_osd.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/src/lcd_osd.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="lcd_osd.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

pll_control.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/src/pll_control.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="pll_control.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

recognizer.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/src/recognizer.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="recognizer.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sys_init.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/src/sys_init.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="sys_init.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

user_interface.obj: C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/src/user_interface.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv7/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="C:/ti/ccsv7/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/build/ccsv5/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/inc" --include_path="C:/Users/a0221074/Desktop/C5000/IBM_Watson_TIdesign_workarea/CSL_bitbucket/c55_csl/c55xx_csl/demos/TIesr/c5545/TIesr_C55_demo/C55/TIesrEngine/src" --define=c5515 --define=C55x --define=PF_C5535_EZDSP --undefine=PF_C5515_EVM --display_error_number --diag_warning=225 --gen_func_subsections=on --ptrdiff_size=32 --algebraic --preproc_with_compile --preproc_dependency="user_interface.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


