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
CC=arm-none-linux-gnueabi-gcc
CCC=arm-none-linux-gnueabi-g++
CXX=arm-none-linux-gnueabi-g++
FC=
AS=arm-none-linux-gnueabi-as

# Macros
CND_PLATFORM=arm-none-linux-gnueabi-Linux-x86
CND_CONF=ArmLinuxDebugGnueabi
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/src/TestTIesrDict.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-v
CXXFLAGS=-v

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../Dist/${CND_CONF}/lib -Wl,-rpath ${CND_BASEDIR}/../../Dist/${CND_CONF}/lib -lTIesrDict

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-ArmLinuxDebugGnueabi.mk ../../Dist/ArmLinuxDebugGnueabi/bin/testtiesrdict

../../Dist/ArmLinuxDebugGnueabi/bin/testtiesrdict: ${OBJECTFILES}
	${MKDIR} -p ../../Dist/ArmLinuxDebugGnueabi/bin
	${LINK.cc} -v -o ../../Dist/${CND_CONF}/bin/testtiesrdict ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/src/TestTIesrDict.o: nbproject/Makefile-${CND_CONF}.mk ../src/TestTIesrDict.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../src -I../../TIesrDT/src -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/src/TestTIesrDict.o ../src/TestTIesrDict.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/ArmLinuxDebugGnueabi
	${RM} ../../Dist/ArmLinuxDebugGnueabi/bin/testtiesrdict

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
