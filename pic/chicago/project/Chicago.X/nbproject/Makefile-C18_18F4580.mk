#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-C18_18F4580.mk)" "nbproject/Makefile-local-C18_18F4580.mk"
include nbproject/Makefile-local-C18_18F4580.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=C18_18F4580
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Chicago.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Chicago.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../main.c ../../../common/can18f.c ../../../common/eeprom.c ../../../../common/vscp_firmware.c ../../../augusto/driver.c ../windowEngine.c ../../../augusto/vscp_appLayer.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/main.p1 ${OBJECTDIR}/_ext/907750266/can18f.p1 ${OBJECTDIR}/_ext/907750266/eeprom.p1 ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1 ${OBJECTDIR}/_ext/761923357/driver.p1 ${OBJECTDIR}/_ext/1472/windowEngine.p1 ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/main.p1.d ${OBJECTDIR}/_ext/907750266/can18f.p1.d ${OBJECTDIR}/_ext/907750266/eeprom.p1.d ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1.d ${OBJECTDIR}/_ext/761923357/driver.p1.d ${OBJECTDIR}/_ext/1472/windowEngine.p1.d ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/main.p1 ${OBJECTDIR}/_ext/907750266/can18f.p1 ${OBJECTDIR}/_ext/907750266/eeprom.p1 ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1 ${OBJECTDIR}/_ext/761923357/driver.p1 ${OBJECTDIR}/_ext/1472/windowEngine.p1 ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1

# Source Files
SOURCEFILES=../main.c ../../../common/can18f.c ../../../common/eeprom.c ../../../../common/vscp_firmware.c ../../../augusto/driver.c ../windowEngine.c ../../../augusto/vscp_appLayer.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-C18_18F4580.mk dist/${CND_CONF}/${IMAGE_TYPE}/Chicago.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F4580
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/main.p1: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1472/main.p1  ../main.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/main.d ${OBJECTDIR}/_ext/1472/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/907750266/can18f.p1: ../../../common/can18f.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/907750266" 
	@${RM} ${OBJECTDIR}/_ext/907750266/can18f.p1.d 
	@${RM} ${OBJECTDIR}/_ext/907750266/can18f.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/907750266/can18f.p1  ../../../common/can18f.c 
	@-${MV} ${OBJECTDIR}/_ext/907750266/can18f.d ${OBJECTDIR}/_ext/907750266/can18f.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/907750266/can18f.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/907750266/eeprom.p1: ../../../common/eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/907750266" 
	@${RM} ${OBJECTDIR}/_ext/907750266/eeprom.p1.d 
	@${RM} ${OBJECTDIR}/_ext/907750266/eeprom.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/907750266/eeprom.p1  ../../../common/eeprom.c 
	@-${MV} ${OBJECTDIR}/_ext/907750266/eeprom.d ${OBJECTDIR}/_ext/907750266/eeprom.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/907750266/eeprom.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1: ../../../../common/vscp_firmware.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1889496363" 
	@${RM} ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1  ../../../../common/vscp_firmware.c 
	@-${MV} ${OBJECTDIR}/_ext/1889496363/vscp_firmware.d ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/761923357/driver.p1: ../../../augusto/driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/761923357" 
	@${RM} ${OBJECTDIR}/_ext/761923357/driver.p1.d 
	@${RM} ${OBJECTDIR}/_ext/761923357/driver.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/761923357/driver.p1  ../../../augusto/driver.c 
	@-${MV} ${OBJECTDIR}/_ext/761923357/driver.d ${OBJECTDIR}/_ext/761923357/driver.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/761923357/driver.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/windowEngine.p1: ../windowEngine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/windowEngine.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/windowEngine.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1472/windowEngine.p1  ../windowEngine.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/windowEngine.d ${OBJECTDIR}/_ext/1472/windowEngine.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/windowEngine.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1: ../../../augusto/vscp_appLayer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/761923357" 
	@${RM} ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1.d 
	@${RM} ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1  ../../../augusto/vscp_appLayer.c 
	@-${MV} ${OBJECTDIR}/_ext/761923357/vscp_appLayer.d ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1472/main.p1: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1472/main.p1  ../main.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/main.d ${OBJECTDIR}/_ext/1472/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/907750266/can18f.p1: ../../../common/can18f.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/907750266" 
	@${RM} ${OBJECTDIR}/_ext/907750266/can18f.p1.d 
	@${RM} ${OBJECTDIR}/_ext/907750266/can18f.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/907750266/can18f.p1  ../../../common/can18f.c 
	@-${MV} ${OBJECTDIR}/_ext/907750266/can18f.d ${OBJECTDIR}/_ext/907750266/can18f.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/907750266/can18f.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/907750266/eeprom.p1: ../../../common/eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/907750266" 
	@${RM} ${OBJECTDIR}/_ext/907750266/eeprom.p1.d 
	@${RM} ${OBJECTDIR}/_ext/907750266/eeprom.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/907750266/eeprom.p1  ../../../common/eeprom.c 
	@-${MV} ${OBJECTDIR}/_ext/907750266/eeprom.d ${OBJECTDIR}/_ext/907750266/eeprom.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/907750266/eeprom.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1: ../../../../common/vscp_firmware.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1889496363" 
	@${RM} ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1  ../../../../common/vscp_firmware.c 
	@-${MV} ${OBJECTDIR}/_ext/1889496363/vscp_firmware.d ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1889496363/vscp_firmware.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/761923357/driver.p1: ../../../augusto/driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/761923357" 
	@${RM} ${OBJECTDIR}/_ext/761923357/driver.p1.d 
	@${RM} ${OBJECTDIR}/_ext/761923357/driver.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/761923357/driver.p1  ../../../augusto/driver.c 
	@-${MV} ${OBJECTDIR}/_ext/761923357/driver.d ${OBJECTDIR}/_ext/761923357/driver.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/761923357/driver.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/windowEngine.p1: ../windowEngine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/windowEngine.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/windowEngine.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1472/windowEngine.p1  ../windowEngine.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/windowEngine.d ${OBJECTDIR}/_ext/1472/windowEngine.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/windowEngine.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1: ../../../augusto/vscp_appLayer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/761923357" 
	@${RM} ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1.d 
	@${RM} ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1  ../../../augusto/vscp_appLayer.c 
	@-${MV} ${OBJECTDIR}/_ext/761923357/vscp_appLayer.d ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/761923357/vscp_appLayer.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Chicago.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -mdist/${CND_CONF}/${IMAGE_TYPE}/Chicago.X.${IMAGE_TYPE}.map  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     --rom=default,-7dc0-7fff --ram=default,-5f4-5ff,-f9c-f9c,-fd4-fd4,-fdb-fdf,-fe3-fe7,-feb-fef,-ffd-fff  -odist/${CND_CONF}/${IMAGE_TYPE}/Chicago.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/Chicago.X.${IMAGE_TYPE}.hex 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Chicago.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -mdist/${CND_CONF}/${IMAGE_TYPE}/Chicago.X.${IMAGE_TYPE}.map  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"../" -I"../../../common" -I"../../../../common" -I"../../../augusto" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -odist/${CND_CONF}/${IMAGE_TYPE}/Chicago.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/C18_18F4580
	${RM} -r dist/C18_18F4580

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
