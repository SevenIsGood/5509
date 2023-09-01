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
	${OBJECTDIR}/_ext/1360937237/TestTIesrFlex.o


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
LDLIBSOPTIONS=-L../../Dist/${CND_CONF}/lib -Wl,-rpath,${CND_BASEDIR}/../../Dist/${CND_CONF}/lib -lTIesrFlex

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../Dist/${CND_CONF}/bin/testtiesrflex

../../Dist/${CND_CONF}/bin/testtiesrflex: ${OBJECTFILES}
	${MKDIR} -p ../../Dist/${CND_CONF}/bin
	${LINK.cc} -o ../../Dist/${CND_CONF}/bin/testtiesrflex ${OBJECTFILES} ${LDLIBSOPTIONS} -v

${OBJECTDIR}/_ext/1360937237/TestTIesrFlex.o: ../src/TestTIesrFlex.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../src -I../../TIesrDict/src -I../../TIesrDT/src -v -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/TestTIesrFlex.o ../src/TestTIesrFlex.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../../Dist/${CND_CONF}/bin/testtiesrflex

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
