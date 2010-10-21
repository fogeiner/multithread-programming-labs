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
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/Semaphore.o \
	${OBJECTDIR}/_ext/_DOTDOT/Thread.o \
	${OBJECTDIR}/_ext/_DOTDOT/MsgQueue.o \
	${OBJECTDIR}/_ext/_DOTDOT/source.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-lrt -pthread -lpthread
CXXFLAGS=-lrt -pthread -lpthread

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/msg_queue

dist/Debug/GNU-Linux-x86/msg_queue: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/msg_queue ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/Semaphore.o: nbproject/Makefile-${CND_CONF}.mk ../Semaphore.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/Semaphore.o ../Semaphore.cpp

${OBJECTDIR}/_ext/_DOTDOT/Thread.o: nbproject/Makefile-${CND_CONF}.mk ../Thread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/Thread.o ../Thread.cpp

${OBJECTDIR}/_ext/_DOTDOT/MsgQueue.o: nbproject/Makefile-${CND_CONF}.mk ../MsgQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/MsgQueue.o ../MsgQueue.cpp

${OBJECTDIR}/_ext/_DOTDOT/source.o: nbproject/Makefile-${CND_CONF}.mk ../source.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/source.o ../source.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/msg_queue

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
