#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = %C%\ti\xdctools_3_25_06_96\gmake.exe
override XDCROOT = C:/ti/xdctools_3_31_01_33_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/tirtos_simplelink_2_13_00_06;C:/ti/ccs613/ccsv6/ccs_base;C:/ti/bios_6_42_02_29/packages;C:/ti/xdctools_3_31_01_33_core/packages;..
HOSTOS = Windows
endif
