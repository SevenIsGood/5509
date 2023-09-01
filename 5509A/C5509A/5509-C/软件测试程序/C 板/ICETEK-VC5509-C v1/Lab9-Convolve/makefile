#
#  Makefile for creation of the program named by the PROG variable
#
#  The following naming conventions are used by this makefile:
#	<prog>.asm	- C55 assembly language source file
#	<prog>.obj	- C55 object file (compiled/assembled source)
#	<prog>.out	- C55 executable (fully linked program)
#	<prog>cfg.s55	- configuration assembly source file generated
#			  by Configuration Tool
#	<prog>cfg.h55	- configuration assembly header file generated
#			  by Configuration Tool
#	<prog>cfg.cmd	- configuration linker command file generated
#			  by Configuration Tool
#

TI_DIR  := $(subst \,/,$(TI_DIR))
include $(TI_DIR)/c5500/bios/include/c55rules.mak

#
#  Compiler, assembler, and linker options.
#
# -g enable symbolic debugging
# -i Need to specify include dir since A_DIR is broken
CC55OPTS = -g
AS55OPTS = -i%TI_DIR%\c5500\bios\include
# -c            ROM autoinotoalization model
# -stack 0x400  set C stack syze to 0x400 words
# -heap 0x400   set heap size to 0x400 words
# -q            quiet run
LD55OPTS = -c -stack 0x400 -heap 0x400 -q

PROG	= volume
OBJS	= vectors.obj load.obj
LIBS	= -lrts55.lib
CMDS	= $(PROG).cmd

#
#  Targets:
#
all:: $(PROG).out

$(PROG).out: $(OBJS) $(CMDS)
$(PROG).obj:

.clean clean::
	@ echo removing object files and binaries ...
	@$(REMOVE) -f *.obj *.out *.lst *.map
