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
	${OBJECTDIR}/_ext/2105459482/ClientState.o \
	${OBJECTDIR}/_ext/461788971/Thread.o \
	${OBJECTDIR}/_ext/297160205/TCPSocket.o \
	${OBJECTDIR}/_ext/2105459482/ClientGetRequest.o \
	${OBJECTDIR}/_ext/1159880425/SignalPipe.o \
	${OBJECTDIR}/_ext/1472/config.o \
	${OBJECTDIR}/_ext/413176034/HTTPURIParser.o \
	${OBJECTDIR}/_ext/2113231069/TaskQueue.o \
	${OBJECTDIR}/_ext/238894769/Logger.o \
	${OBJECTDIR}/_ext/1327237076/AsyncDispatcher.o \
	${OBJECTDIR}/_ext/1005991299/Semaphore.o \
	${OBJECTDIR}/_ext/2136648385/Proxy.o \
	${OBJECTDIR}/_ext/2113231069/SelectTask.o \
	${OBJECTDIR}/_ext/2105459482/Client.o \
	${OBJECTDIR}/_ext/1472/main.o \
	${OBJECTDIR}/_ext/2083495164/Downloader.o \
	${OBJECTDIR}/_ext/2083495164/DownloaderState.o \
	${OBJECTDIR}/_ext/1948483646/Mutex.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pedantic -Wall -g -lrt
CXXFLAGS=-pedantic -Wall -g -lrt

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/proxy_prj

dist/Debug/GNU-Linux-x86/proxy_prj: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/proxy_prj ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/2105459482/ClientState.o: ../Client/ClientState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2105459482
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2105459482/ClientState.o ../Client/ClientState.cpp

${OBJECTDIR}/_ext/461788971/Thread.o: ../../libs/Thread/Thread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/461788971
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/461788971/Thread.o ../../libs/Thread/Thread.cpp

${OBJECTDIR}/_ext/297160205/TCPSocket.o: ../../libs/TCPSocket/TCPSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/297160205
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/297160205/TCPSocket.o ../../libs/TCPSocket/TCPSocket.cpp

${OBJECTDIR}/_ext/2105459482/ClientGetRequest.o: ../Client/ClientGetRequest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2105459482
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2105459482/ClientGetRequest.o ../Client/ClientGetRequest.cpp

${OBJECTDIR}/_ext/1159880425/SignalPipe.o: ../../libs/SignalPipe/SignalPipe.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1159880425
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1159880425/SignalPipe.o ../../libs/SignalPipe/SignalPipe.cpp

${OBJECTDIR}/_ext/1472/config.o: ../config.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/config.o ../config.cpp

${OBJECTDIR}/_ext/413176034/HTTPURIParser.o: ../../libs/HTTPURIParser/HTTPURIParser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/413176034
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/413176034/HTTPURIParser.o ../../libs/HTTPURIParser/HTTPURIParser.cpp

${OBJECTDIR}/_ext/2113231069/TaskQueue.o: ../TaskQueue/TaskQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2113231069
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2113231069/TaskQueue.o ../TaskQueue/TaskQueue.cpp

${OBJECTDIR}/_ext/238894769/Logger.o: ../../libs/Logger/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/238894769
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/238894769/Logger.o ../../libs/Logger/Logger.cpp

${OBJECTDIR}/_ext/1327237076/AsyncDispatcher.o: ../AsyncDispatcher/AsyncDispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1327237076
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1327237076/AsyncDispatcher.o ../AsyncDispatcher/AsyncDispatcher.cpp

${OBJECTDIR}/_ext/1005991299/Semaphore.o: ../../libs/Semaphore/Semaphore.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1005991299
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1005991299/Semaphore.o ../../libs/Semaphore/Semaphore.cpp

${OBJECTDIR}/_ext/2136648385/Proxy.o: ../Proxy/Proxy.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2136648385
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2136648385/Proxy.o ../Proxy/Proxy.cpp

${OBJECTDIR}/_ext/2113231069/SelectTask.o: ../TaskQueue/SelectTask.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2113231069
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2113231069/SelectTask.o ../TaskQueue/SelectTask.cpp

${OBJECTDIR}/_ext/2105459482/Client.o: ../Client/Client.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2105459482
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2105459482/Client.o ../Client/Client.cpp

${OBJECTDIR}/_ext/1472/main.o: ../main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/main.o ../main.cpp

${OBJECTDIR}/_ext/2083495164/Downloader.o: ../Downloader/Downloader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2083495164
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2083495164/Downloader.o ../Downloader/Downloader.cpp

${OBJECTDIR}/_ext/2083495164/DownloaderState.o: ../Downloader/DownloaderState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2083495164
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2083495164/DownloaderState.o ../Downloader/DownloaderState.cpp

${OBJECTDIR}/_ext/1948483646/Mutex.o: ../../libs/Mutex/Mutex.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1948483646
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1948483646/Mutex.o ../../libs/Mutex/Mutex.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/proxy_prj

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
