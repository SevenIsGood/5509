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
	${OBJECTDIR}/_ext/1360937237/engine_sireco_init.o \
	${OBJECTDIR}/_ext/1360937237/jac-estm.o \
	${OBJECTDIR}/_ext/1360937237/jac_one.o \
	${OBJECTDIR}/_ext/1360937237/liveproc.o \
	${OBJECTDIR}/_ext/1360937237/siinit.o \
	${OBJECTDIR}/_ext/1360937237/siproc.o


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
LDLIBSOPTIONS=-Wl,-rpath,${CND_BASEDIR}/../../Dist/${CND_CONF}/lib ../TIesrEngineCoreso/../../Dist/LinuxDebugGnu/lib/libTIesrEngineCore.so.1

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../Dist/${CND_CONF}/lib/libTIesrEngineSI.so.1

../../Dist/${CND_CONF}/lib/libTIesrEngineSI.so.1: ../TIesrEngineCoreso/../../Dist/LinuxDebugGnu/lib/libTIesrEngineCore.so.1

../../Dist/${CND_CONF}/lib/libTIesrEngineSI.so.1: ${OBJECTFILES}
	${MKDIR} -p ../../Dist/${CND_CONF}/lib
	${LINK.c} -o ../../Dist/${CND_CONF}/lib/libTIesrEngineSI.so.1 ${OBJECTFILES} ${LDLIBSOPTIONS} -Wl,-znow,-zdefs -Wl,-h,libTIesrEngineSI.so.1 -Wl,--version-script=../resource/TIesrEngineSI.ver -shared -fPIC

${OBJECTDIR}/_ext/1360937237/engine_sireco_init.o: ../src/engine_sireco_init.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/engine_sireco_init.o ../src/engine_sireco_init.c

${OBJECTDIR}/_ext/1360937237/jac-estm.o: ../src/jac-estm.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/jac-estm.o ../src/jac-estm.c

${OBJECTDIR}/_ext/1360937237/jac_one.o: ../src/jac_one.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/jac_one.o ../src/jac_one.c

${OBJECTDIR}/_ext/1360937237/liveproc.o: ../src/liveproc.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/liveproc.o ../src/liveproc.c

${OBJECTDIR}/_ext/1360937237/siinit.o: ../src/siinit.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/siinit.o ../src/siinit.c

${OBJECTDIR}/_ext/1360937237/siproc.o: ../src/siproc.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.c) -g -DDEBUG_STATES -O0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/siproc.o ../src/siproc.c

# Subprojects
.build-subprojects:
	cd ../TIesrEngineCoreso && ${MAKE}  -f Makefile CONF=LinuxDebugGnu

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../../Dist/${CND_CONF}/lib/libTIesrEngineSI.so.1

# Subprojects
.clean-subprojects:
	cd ../TIesrEngineCoreso && ${MAKE}  -f Makefile CONF=LinuxDebugGnu clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
