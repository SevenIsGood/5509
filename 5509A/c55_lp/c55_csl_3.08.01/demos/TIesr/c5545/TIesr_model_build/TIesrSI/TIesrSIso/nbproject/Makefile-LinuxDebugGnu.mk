#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=LinuxDebugGnu
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1360937237/TIesrSI.o


# C Compiler Flags
CFLAGS=-v -O0

# CC Compiler Flags
CCFLAGS=-v
CXXFLAGS=-v

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../Dist/${CND_CONF}/lib -Wl,-rpath,${CND_BASEDIR}/../../Dist/${CND_CONF}/lib -lpthread ../../TIesrEngine/TIesrEngineSIso/../../Dist/LinuxDebugGnu/lib/libTIesrEngineSI.so.1 ../../TIesrFA/TIesrFA_ALSAso/../../Dist/LinuxDebugGnu/lib/libTIesrFA_ALSA.so.1

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../Dist/${CND_CONF}/lib/libTIesrSI.so.1

../../Dist/${CND_CONF}/lib/libTIesrSI.so.1: ../../TIesrEngine/TIesrEngineSIso/../../Dist/LinuxDebugGnu/lib/libTIesrEngineSI.so.1

../../Dist/${CND_CONF}/lib/libTIesrSI.so.1: ../../TIesrFA/TIesrFA_ALSAso/../../Dist/LinuxDebugGnu/lib/libTIesrFA_ALSA.so.1

../../Dist/${CND_CONF}/lib/libTIesrSI.so.1: ${OBJECTFILES}
	${MKDIR} -p ../../Dist/${CND_CONF}/lib
	${LINK.c} -o ../../Dist/${CND_CONF}/lib/libTIesrSI.so.1 ${OBJECTFILES} ${LDLIBSOPTIONS} -Wl,-znow,-zdefs -Wl,-h,libTIesrSI.so.1 -Wl,--version-script=../resource/TIesrSIso.ver -shared -fPIC

${OBJECTDIR}/_ext/1360937237/TIesrSI.o: ../src/TIesrSI.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DLINUX -I../../TIesrEngine/src -I../../TIesrFA/src -v -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/TIesrSI.o ../src/TIesrSI.c

# Subprojects
.build-subprojects:
	cd ../../TIesrEngine/TIesrEngineSIso && ${MAKE}  -f Makefile CONF=LinuxDebugGnu
	cd ../../TIesrFA/TIesrFA_ALSAso && ${MAKE}  -f Makefile CONF=LinuxDebugGnu

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../../Dist/${CND_CONF}/lib/libTIesrSI.so.1

# Subprojects
.clean-subprojects:
	cd ../../TIesrEngine/TIesrEngineSIso && ${MAKE}  -f Makefile CONF=LinuxDebugGnu clean
	cd ../../TIesrFA/TIesrFA_ALSAso && ${MAKE}  -f Makefile CONF=LinuxDebugGnu clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
