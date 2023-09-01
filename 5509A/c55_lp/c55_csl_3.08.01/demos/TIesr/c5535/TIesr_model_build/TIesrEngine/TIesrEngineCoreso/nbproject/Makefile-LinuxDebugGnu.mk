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
	${OBJECTDIR}/_ext/1360937237/dist.o \
	${OBJECTDIR}/_ext/1360937237/gmhmm_type_common.o \
	${OBJECTDIR}/_ext/1360937237/load.o \
	${OBJECTDIR}/_ext/1360937237/mfcc_f.o \
	${OBJECTDIR}/_ext/1360937237/my_fileio.o \
	${OBJECTDIR}/_ext/1360937237/noise_sub.o \
	${OBJECTDIR}/_ext/1360937237/obsprob.o \
	${OBJECTDIR}/_ext/1360937237/pack.o \
	${OBJECTDIR}/_ext/1360937237/pmc_f.o \
	${OBJECTDIR}/_ext/1360937237/rapidsearch.o \
	${OBJECTDIR}/_ext/1360937237/sbc.o \
	${OBJECTDIR}/_ext/1360937237/search.o \
	${OBJECTDIR}/_ext/1360937237/uttdet.o \
	${OBJECTDIR}/_ext/1360937237/volume.o


# C Compiler Flags
CFLAGS=-O0

# CC Compiler Flags
CCFLAGS=-v
CXXFLAGS=-v

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../Dist/${CND_CONF}/lib/libTIesrEngineCore.so.1

../../Dist/${CND_CONF}/lib/libTIesrEngineCore.so.1: ${OBJECTFILES}
	${MKDIR} -p ../../Dist/${CND_CONF}/lib
	${LINK.c} -o ../../Dist/${CND_CONF}/lib/libTIesrEngineCore.so.1 ${OBJECTFILES} ${LDLIBSOPTIONS} -Wl,-znow,-zdefs -Wl,-h,libTIesrEngineCore.so.1 -Wl,--version-script=../resource/TIesrEngineCore.ver -shared -fPIC

${OBJECTDIR}/_ext/1360937237/dist.o: ../src/dist.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/dist.o ../src/dist.c

${OBJECTDIR}/_ext/1360937237/gmhmm_type_common.o: ../src/gmhmm_type_common.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/gmhmm_type_common.o ../src/gmhmm_type_common.c

${OBJECTDIR}/_ext/1360937237/load.o: ../src/load.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/load.o ../src/load.c

${OBJECTDIR}/_ext/1360937237/mfcc_f.o: ../src/mfcc_f.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/mfcc_f.o ../src/mfcc_f.c

${OBJECTDIR}/_ext/1360937237/my_fileio.o: ../src/my_fileio.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/my_fileio.o ../src/my_fileio.c

${OBJECTDIR}/_ext/1360937237/noise_sub.o: ../src/noise_sub.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/noise_sub.o ../src/noise_sub.c

${OBJECTDIR}/_ext/1360937237/obsprob.o: ../src/obsprob.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/obsprob.o ../src/obsprob.c

${OBJECTDIR}/_ext/1360937237/pack.o: ../src/pack.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/pack.o ../src/pack.c

${OBJECTDIR}/_ext/1360937237/pmc_f.o: ../src/pmc_f.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/pmc_f.o ../src/pmc_f.c

${OBJECTDIR}/_ext/1360937237/rapidsearch.o: ../src/rapidsearch.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/rapidsearch.o ../src/rapidsearch.c

${OBJECTDIR}/_ext/1360937237/sbc.o: ../src/sbc.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/sbc.o ../src/sbc.c

${OBJECTDIR}/_ext/1360937237/search.o: ../src/search.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/search.o ../src/search.c

${OBJECTDIR}/_ext/1360937237/uttdet.o: ../src/uttdet.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/uttdet.o ../src/uttdet.c

${OBJECTDIR}/_ext/1360937237/volume.o: ../src/volume.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -Wall -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/volume.o ../src/volume.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../../Dist/${CND_CONF}/lib/libTIesrEngineCore.so.1

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
