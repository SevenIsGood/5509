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
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU_current-Linux-x86
CND_CONF=LinuxDebugGnu
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/TestTIesrEngine.o

# C Compiler Flags
CFLAGS=-v

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../Dist/${CND_CONF}/lib -Wl,-rpath ${CND_BASEDIR}/../../Dist/${CND_CONF}/lib -lTIesrEngineCore -lTIesrEngineSI

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-LinuxDebugGnu.mk ../../Dist/LinuxDebugGnu/bin/testtiesrengine

../../Dist/LinuxDebugGnu/bin/testtiesrengine: ${OBJECTFILES}
	${MKDIR} -p ../../Dist/LinuxDebugGnu/bin
	${LINK.c} -o ../../Dist/${CND_CONF}/bin/testtiesrengine ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/TestTIesrEngine.o: nbproject/Makefile-${CND_CONF}.mk TestTIesrEngine.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DLINUX -I../src -MMD -MP -MF $@.d -o ${OBJECTDIR}/TestTIesrEngine.o TestTIesrEngine.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/LinuxDebugGnu
	${RM} ../../Dist/LinuxDebugGnu/bin/testtiesrengine

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
