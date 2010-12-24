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
	${OBJECTDIR}/_ext/461788971/Thread.o \
	${OBJECTDIR}/_ext/1472/Client.o \
	${OBJECTDIR}/_ext/1472/Cache.o \
	${OBJECTDIR}/_ext/1472/Downloader.o \
	${OBJECTDIR}/_ext/297160205/TCPSocket.o \
	${OBJECTDIR}/_ext/575434844/CondVar.o \
	${OBJECTDIR}/_ext/1472/config.o \
	${OBJECTDIR}/_ext/413176034/HTTPURIParser.o \
	${OBJECTDIR}/_ext/238894769/Logger.o \
	${OBJECTDIR}/_ext/1472/CacheEntry.o \
	${OBJECTDIR}/_ext/1472/main.o \
	${OBJECTDIR}/_ext/1948483646/Mutex.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/mt_proxy

dist/Debug/GNU-Linux-x86/mt_proxy: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mt_proxy ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/461788971/Thread.o: ../../libs/Thread/Thread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/461788971
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/461788971/Thread.o ../../libs/Thread/Thread.cpp

${OBJECTDIR}/_ext/1472/Client.o: ../Client.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/Client.o ../Client.cpp

${OBJECTDIR}/_ext/1472/Cache.o: ../Cache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/Cache.o ../Cache.cpp

${OBJECTDIR}/_ext/1472/Downloader.o: ../Downloader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/Downloader.o ../Downloader.cpp

${OBJECTDIR}/_ext/297160205/TCPSocket.o: ../../libs/TCPSocket/TCPSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/297160205
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/297160205/TCPSocket.o ../../libs/TCPSocket/TCPSocket.cpp

${OBJECTDIR}/_ext/575434844/CondVar.o: ../../libs/CondVar/CondVar.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/575434844
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/575434844/CondVar.o ../../libs/CondVar/CondVar.cpp

${OBJECTDIR}/_ext/1472/config.o: ../config.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/config.o ../config.cpp

${OBJECTDIR}/_ext/413176034/HTTPURIParser.o: ../../libs/HTTPURIParser/HTTPURIParser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/413176034
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/413176034/HTTPURIParser.o ../../libs/HTTPURIParser/HTTPURIParser.cpp

${OBJECTDIR}/_ext/238894769/Logger.o: ../../libs/Logger/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/238894769
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/238894769/Logger.o ../../libs/Logger/Logger.cpp

${OBJECTDIR}/_ext/1472/CacheEntry.o: ../CacheEntry.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CacheEntry.o ../CacheEntry.cpp

${OBJECTDIR}/_ext/1472/main.o: ../main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/main.o ../main.cpp

${OBJECTDIR}/_ext/1948483646/Mutex.o: ../../libs/Mutex/Mutex.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1948483646
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1948483646/Mutex.o ../../libs/Mutex/Mutex.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/mt_proxy

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
