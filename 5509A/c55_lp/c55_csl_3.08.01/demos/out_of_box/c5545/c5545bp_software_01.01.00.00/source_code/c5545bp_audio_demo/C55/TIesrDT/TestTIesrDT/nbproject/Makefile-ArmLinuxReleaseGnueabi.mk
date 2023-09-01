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
CND_CONF=ArmLinuxReleaseGnueabi
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/src/TestTIesrDT.o

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
LDLIBSOPTIONS=-L../../Dist/${CND_CONF}/lib -Wl,-rpath ${CND_BASEDIR}/../../Dist/${CND_CONF}/lib -lTIesrDT

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-ArmLinuxReleaseGnueabi.mk ../../Dist/ArmLinuxReleaseGnueabi/bin/testtiesrdt

../../Dist/ArmLinuxReleaseGnueabi/bin/testtiesrdt: ${OBJECTFILES}
	${MKDIR} -p ../../Dist/ArmLinuxReleaseGnueabi/bin
	${LINK.c} -o ../../Dist/${CND_CONF}/bin/testtiesrdt ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/src/TestTIesrDT.o: nbproject/Makefile-${CND_CONF}.mk ../src/TestTIesrDT.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/src
	${RM} $@.d
	$(COMPILE.c) -O2 -Wall -I../src -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/src/TestTIesrDT.o ../src/TestTIesrDT.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/ArmLinuxReleaseGnueabi
	${RM} ../../Dist/ArmLinuxReleaseGnueabi/bin/testtiesrdt

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
