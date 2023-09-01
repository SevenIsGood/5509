/*   Do *not* directly modify this file.  It was    */
/*   generated by the Configuration Tool; any  */
/*   changes risk being overwritten.                */

/* INPUT TIesrDemoC55.cdb */

/* MODULE PARAMETERS */
-u _FXN_F_nop
GBL_USERINITFXN = _FXN_F_nop;
-u _FXN_F_nop
GBL_BIOS_BOOTFXN = _FXN_F_nop;

-u SARAM
MEM_SEGZERO = SARAM;
-u SARAM
MEM_MALLOCSEG = SARAM;

-u FXN_F_nop
CLK_TIMER0FUNC = FXN_F_nop;
-u _gpt1Isr
CLK_TIMER1FUNC = _gpt1Isr;
-u FXN_F_nop
CLK_TIMER2FUNC = FXN_F_nop;
-u FXN_F_zero
CLK_TIMEFXN = FXN_F_zero;
-u FXN_F_nop
CLK_HOOKFXN = FXN_F_nop;

-u KNL_tick_stub
PRD_THOOKFXN = KNL_tick_stub;

-u DARAM
RTDX_DATAMEMSEG = DARAM;

-u DARAM
HST_DSMBUFSEG = DARAM;

-u GBL_NULL
SWI_EHOOKFXN = GBL_NULL;
-u GBL_NULL
SWI_IHOOKFXN = GBL_NULL;
-u SWI_F_exec
SWI_EXECFXN = SWI_F_exec;
-u SWI_F_run
SWI_RUNFXN = SWI_F_run;

-u MEM_NULL
TSK_STACKSEG = MEM_NULL;
-u _FXN_F_nop
TSK_VCREATEFXN = _FXN_F_nop;
-u _FXN_F_nop
TSK_VDELETEFXN = _FXN_F_nop;
-u _FXN_F_nop
TSK_VEXITFXN = _FXN_F_nop;

-u GBL_NULL
IDL_CALIBRFXN = GBL_NULL;

-u _UTL_doAbort
SYS_ABORTFXN = _UTL_doAbort;
-u _UTL_doError
SYS_ERRORFXN = _UTL_doError;
-u _UTL_halt
SYS_EXITFXN = _UTL_halt;
-u _UTL_doPutc
SYS_PUTCFXN = _UTL_doPutc;

-u _FXN_F_nop
GIO_CREATEFXN = _FXN_F_nop;
-u _FXN_F_nop
GIO_DELETEFXN = _FXN_F_nop;
-u _FXN_F_nop
GIO_PENDFXN = _FXN_F_nop;
-u _FXN_F_nop
GIO_POSTFXN = _FXN_F_nop;

-u _FXN_F_nop
PWRM_PWRM_BOOTHOOKFXN = _FXN_F_nop;

/* OBJECT ALIASES */
_VECT = VECT;
_NOR = NOR;
_NAND = NAND;
_SRAM = SRAM;
_DARAM = DARAM;
_SARAM = SARAM;
_SARAM31 = SARAM31;
_Char_ROM = Char_ROM;
_HWI_RESET = HWI_RESET;
_HWI_NMI = HWI_NMI;
_HWI_INT2 = HWI_INT2;
_HWI_INT3 = HWI_INT3;
_HWI_INT4 = HWI_INT4;
_HWI_INT5 = HWI_INT5;
_HWI_INT6 = HWI_INT6;
_HWI_INT7 = HWI_INT7;
_HWI_INT8 = HWI_INT8;
_HWI_INT9 = HWI_INT9;
_HWI_INT10 = HWI_INT10;
_HWI_INT11 = HWI_INT11;
_HWI_INT12 = HWI_INT12;
_HWI_INT13 = HWI_INT13;
_HWI_INT14 = HWI_INT14;
_HWI_INT15 = HWI_INT15;
_HWI_INT16 = HWI_INT16;
_HWI_INT17 = HWI_INT17;
_HWI_INT18 = HWI_INT18;
_HWI_INT19 = HWI_INT19;
_HWI_INT20 = HWI_INT20;
_HWI_INT21 = HWI_INT21;
_HWI_INT22 = HWI_INT22;
_HWI_INT23 = HWI_INT23;
_HWI_BERR = HWI_BERR;
_HWI_DLOG = HWI_DLOG;
_HWI_RTOS = HWI_RTOS;
_HWI_SINT27 = HWI_SINT27;
_HWI_SINT28 = HWI_SINT28;
_HWI_SINT29 = HWI_SINT29;
_HWI_SINT30 = HWI_SINT30;
_HWI_SINT31 = HWI_SINT31;
_KNL_swi = KNL_swi;
_SWI_AudioDataCollection = SWI_AudioDataCollection;
_SWI_Recognizer = SWI_Recognizer;
_SWI_SysInit = SWI_SysInit;
_SWI_UserInput = SWI_UserInput;
_TSK_idle = TSK_idle;
_IDL_userIdle = IDL_userIdle;
_LOG_system = LOG_system;

/* MODULE GBL */

SECTIONS {
   .vers block(0x20000) (COPY): {} /* version information */
}

--"diag_suppress=10286" /* new linker needs new BIOS (SDSCM00026767) */

--"diag_suppress=16002"
-priority
-llnknone.a55H
-ldrivers.a55H         /* device drivers support */
-lsioboth.a55H         /* supports both SIO models */
-lbios5505.a55H        /* 5505 and compatible chips */
-lbios_NONINST.a55H    /* DSP/BIOS support */
-lrts55h.lib           /* C and C++ run- time library support */




/* MODULE MEM */
-stack 0x2000
-sysstack 0x800

MEMORY {
   PAGE 0:     VECT:     origin = 0xff00, len = 0x100
   PAGE 0:     NOR:      origin = 0x800000, len = 0x400000
   PAGE 0:     NAND:     origin = 0xe00000, len = 0x100000
   PAGE 0:     SRAM:     origin = 0x50000, len = 0x7b0000
   PAGE 0:     DARAM:    origin = 0xc0, len = 0xfe40
   PAGE 0:     SARAM:    origin = 0x10000, len = 0x3e000
   PAGE 0:     SARAM31:  origin = 0x4e000, len = 0x2000
   PAGE 0:     Char_ROM: origin = 0xfe0000, len = 0x600
}
/* MODULE CLK */
SECTIONS {
   .clk: block(0x20000), RUN_START(CLK_A_TABBEGb) {
       CLK_F_gethtime = FXN_F_zero;
       CLK_A_TABBEG = CLK_A_TABBEGb / 2;
        /* no CLK objs */ 
   } > DARAM PAGE 0
}
_CLK_PRD = CLK_PRD;
_CLK_COUNTSPMS = CLK_COUNTSPMS;
_CLK_REGS = CLK_REGS;
_CLK_USETIMER = CLK_USETIMER;
_CLK_TIMERNUM = CLK_TIMERNUM;
_CLK_TCR = CLK_TCR;
_CLK_TDDR = CLK_TDDR;

/* MODULE PRD */
SECTIONS {
   .prd: block(0x20000) RUN_START(PRD_A_TABBEGb), RUN_END(PRD_A_TABENDb) {
       PRD_A_TABBEG = PRD_A_TABBEGb / 2;
       PRD_A_TABEND = PRD_A_TABENDb / 2;
   } > DARAM PAGE 0
}
PRD_A_TABLEN = 0;

/* MODULE HWI */
HWI_TINT = HWI_INT4;
_HWI_TINT = HWI_TINT;
/* MODULE SWI */
SECTIONS {
   .swi: block(0x20000) RUN_START(SWI_A_TABBEGb), RUN_END(SWI_A_TABENDb) {
       SWI_A_TABBEG = SWI_A_TABBEGb / 2;
       SWI_A_TABEND = SWI_A_TABENDb / 2;
   } > DARAM PAGE 0
}
SWI_A_TABLEN = 5;

/* MODULE TSK */
SECTIONS {
   .tsk: block(0x20000) {
        *(.tsk) 
   } > DARAM PAGE 0
}

/* MODULE IDL */
SECTIONS {
   .idl: block(0x20000), RUN_START(IDL_A_TABBEGb) {
       IDL_A_TABBEG = IDL_A_TABBEGb / 2;
   } > DARAM PAGE 0
   
   .idlcal: block(0x20000), RUN_START(IDL_A_CALBEGb) {
       IDL_A_CALBEG = IDL_A_CALBEGb / 2;
   } > DARAM PAGE 0 
}


LOG_A_TABLEN = 1; _LOG_A_TABLEN = 1;

PIP_A_TABLEN = 0;


SECTIONS {
        .hwi_disp_sec:  block(0x20000) {} > DARAM PAGE 0

        .sysdata: block(0x20000) {} > DARAM PAGE 0

        .mem: 	  {} > DARAM PAGE 0

        .sysregs: {} > DARAM PAGE 0

        .trcdata:    {} > DARAM PAGE 0

        .dsm: block(0x20000) {} > DARAM PAGE 0

        .gio: block(0x20000)    {} > DARAM PAGE 0

        .sys: block(0x20000)    {} > DARAM PAGE 0

        .pwrmdb: block(0x20000)   {} > DARAM PAGE 0

        .gblinit: block(0x20000)   {} > DARAM PAGE 0

        .TSK_idle$stk: block(0x20000){
            *(.TSK_idle$stk)
        } > DARAM PAGE 0

        /* LOG_system buffer */
        .LOG_system$buf: block(0x20000) align = 0x400 {} > DARAM PAGE 0

        .args: align = 0x4  fill=0 block(0x20000) {
            *(.args)
            . += 0x8;
        } > DARAM PAGE 0

        .trace: block(0x20000) fill = 0x0 {
           _SYS_PUTCBEG = .;
           . += 0x400;
           _SYS_PUTCEND = . - 1;
        } > DARAM PAGE 0

        .hst: block(0x20000), RUN_START(HST_A_TABBEGb), RUN_END(HST_A_TABENDb) {
           HST_A_TABBEG = HST_A_TABBEGb / 2; _HST_A_TABBEG = HST_A_TABBEGb / 2;
           HST_A_TABEND = HST_A_TABENDb / 2; _HST_A_TABEND = HST_A_TABENDb / 2;
        } > DARAM PAGE 0

        .log: block(0x20000), RUN_START(LOG_A_TABBEGb), RUN_END(LOG_A_TABENDb) {
           LOG_A_TABBEG = LOG_A_TABBEGb / 2; _LOG_A_TABBEG = LOG_A_TABBEGb / 2;
           LOG_A_TABEND = LOG_A_TABENDb / 2; _LOG_A_TABEND = LOG_A_TABENDb / 2;
        } > DARAM PAGE 0

        .pip: block(0x20000), RUN_START(PIP_A_TABBEGb), RUN_END(PIP_A_TABENDb) {
           PIP_A_TABBEG = PIP_A_TABBEGb / 2; _PIP_A_TABBEG = PIP_A_TABBEGb / 2;
           PIP_A_TABEND = PIP_A_TABENDb / 2; _PIP_A_TABEND = PIP_A_TABENDb / 2;
        } > DARAM PAGE 0

        .sts: block(0x20000), RUN_START(STS_A_TABBEGb), RUN_END(STS_A_TABENDb) {
           STS_A_TABBEG = STS_A_TABBEGb / 2; _STS_A_TABBEG = STS_A_TABBEGb / 2;
           STS_A_TABEND = STS_A_TABENDb / 2; _STS_A_TABEND = STS_A_TABENDb / 2;
        } > DARAM PAGE 0

        GROUP { 
                .sysstack :  align = 0x4 {
                GBL_sysstackbeg = .;
                 *(.sysstack)
                 GBL_sysstackend = GBL_sysstackbeg + 0x800 -1 ;
                _HWI_SYSSTKTOP = GBL_sysstackbeg; 
               _HWI_SYSSTKBOTTOM = (GBL_sysstackend+1);
               } 
                .stack: align = 0x4 {
                GBL_stackbeg = .;
                 *(.stack)
                GBL_stackend = (GBL_stackbeg + 0x2000 - 1) ;
                _HWI_STKBOTTOM = (GBL_stackend+1);
                _HWI_STKTOP = (GBL_stackbeg);
                }
        } BLOCK(0x20000), run  > DARAM PAGE 0

        .bios:    {} > SARAM PAGE 0

        .sysinit:    {} > SARAM PAGE 0

        .hwi:  {
       /* no HWI stubs are necessary */
       }  > SARAM PAGE 0

        .SARAM$heap: RUN_START(SARAM$Bb), RUN_START(_SARAM_baseb), RUN_SIZE(SARAM$L), RUN_SIZE(_SARAM_length) {
            . += 0x14000;
            SARAM$B = SARAM$Bb / 2;
            _SARAM_base = _SARAM_baseb / 2;
        }  > SARAM PAGE 0

        .hwi_vec:  align = 0x100 {
            *(.hwi_vec)
        } RUN_START(HWI_A_VECS),  > VECT PAGE 0

}

