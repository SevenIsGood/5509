## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em3 linker.cmd package/cfg/appBLE_pem3.oem3

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/appBLE_pem3.xdl
	$(SED) 's"^\"\(package/cfg/appBLE_pem3cfg.cmd\)\"$""\"C:/ti/c55_lp/c55_csl_3.07/demos/out_of_box/c5545/c5545bp_software_01.01.00.00/source_code/c5545bp_ble_firmware/c5545BoostEqualizer/CC26xx/CCS/c5545BoostEqualizer/FlashOnlyOAD/configPkg/\1\""' package/cfg/appBLE_pem3.xdl > $@
	-$(SETDATE) -r:max package/cfg/appBLE_pem3.h compiler.opt compiler.opt.defs
