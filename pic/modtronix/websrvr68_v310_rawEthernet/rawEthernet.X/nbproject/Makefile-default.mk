#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/rawEthernet.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/rawEthernet.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/appcfg.o ${OBJECTDIR}/_ext/1472/delay.o ${OBJECTDIR}/_ext/1472/helpers.o ${OBJECTDIR}/_ext/1472/mac.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/tick.o ${OBJECTDIR}/_ext/1472/serint.o ${OBJECTDIR}/_ext/1472/i2c.o ${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o ${OBJECTDIR}/_ext/1472/vscpmain.o ${OBJECTDIR}/_ext/1472/xeeprom.o ${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o ${OBJECTDIR}/_ext/907750266/eeprom.o ${OBJECTDIR}/_ext/1472/stacktsk.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/appcfg.o.d ${OBJECTDIR}/_ext/1472/delay.o.d ${OBJECTDIR}/_ext/1472/helpers.o.d ${OBJECTDIR}/_ext/1472/mac.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/tick.o.d ${OBJECTDIR}/_ext/1472/serint.o.d ${OBJECTDIR}/_ext/1472/i2c.o.d ${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o.d ${OBJECTDIR}/_ext/1472/vscpmain.o.d ${OBJECTDIR}/_ext/1472/xeeprom.o.d ${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o.d ${OBJECTDIR}/_ext/907750266/eeprom.o.d ${OBJECTDIR}/_ext/1472/stacktsk.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/appcfg.o ${OBJECTDIR}/_ext/1472/delay.o ${OBJECTDIR}/_ext/1472/helpers.o ${OBJECTDIR}/_ext/1472/mac.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/tick.o ${OBJECTDIR}/_ext/1472/serint.o ${OBJECTDIR}/_ext/1472/i2c.o ${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o ${OBJECTDIR}/_ext/1472/vscpmain.o ${OBJECTDIR}/_ext/1472/xeeprom.o ${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o ${OBJECTDIR}/_ext/907750266/eeprom.o ${OBJECTDIR}/_ext/1472/stacktsk.o


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/rawEthernet.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F6680
MP_PROCESSOR_OPTION_LD=18f6680
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0xfd30 -u_DEBUGCODELEN=0x2d0 -u_DEBUGDATASTART=0xcf4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/appcfg.o: ../appcfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/appcfg.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/appcfg.o   ../appcfg.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/appcfg.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/appcfg.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/delay.o: ../delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/delay.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/delay.o   ../delay.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/delay.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/delay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/helpers.o: ../helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/helpers.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/helpers.o   ../helpers.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/mac.o: ../mac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mac.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/mac.o   ../mac.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/mac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/main.o   ../main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/tick.o: ../tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/tick.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/tick.o   ../tick.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/tick.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/tick.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/serint.o: ../serint.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/serint.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/serint.o   ../serint.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/serint.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/serint.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/i2c.o: ../i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/i2c.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/i2c.o   ../i2c.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/i2c.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/i2c.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o: ../vscp_raw_ethernet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o   ../vscp_raw_ethernet.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/vscpmain.o: ../vscpmain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/vscpmain.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/vscpmain.o   ../vscpmain.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/vscpmain.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/vscpmain.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/xeeprom.o: ../xeeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/xeeprom.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/xeeprom.o   ../xeeprom.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/xeeprom.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/xeeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o: ../../../../common/vscp_firmware_level2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1889496363 
	@${RM} ${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o   ../../../../common/vscp_firmware_level2.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/907750266/eeprom.o: ../../../common/eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/907750266 
	@${RM} ${OBJECTDIR}/_ext/907750266/eeprom.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/907750266/eeprom.o   ../../../common/eeprom.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/907750266/eeprom.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/907750266/eeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/stacktsk.o: ../stacktsk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/stacktsk.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/stacktsk.o   ../stacktsk.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/stacktsk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/stacktsk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/_ext/1472/appcfg.o: ../appcfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/appcfg.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/appcfg.o   ../appcfg.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/appcfg.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/appcfg.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/delay.o: ../delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/delay.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/delay.o   ../delay.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/delay.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/delay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/helpers.o: ../helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/helpers.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/helpers.o   ../helpers.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/mac.o: ../mac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mac.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/mac.o   ../mac.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/mac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/main.o   ../main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/tick.o: ../tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/tick.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/tick.o   ../tick.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/tick.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/tick.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/serint.o: ../serint.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/serint.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/serint.o   ../serint.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/serint.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/serint.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/i2c.o: ../i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/i2c.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/i2c.o   ../i2c.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/i2c.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/i2c.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o: ../vscp_raw_ethernet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o   ../vscp_raw_ethernet.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/vscp_raw_ethernet.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/vscpmain.o: ../vscpmain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/vscpmain.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/vscpmain.o   ../vscpmain.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/vscpmain.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/vscpmain.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/xeeprom.o: ../xeeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/xeeprom.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/xeeprom.o   ../xeeprom.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/xeeprom.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/xeeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o: ../../../../common/vscp_firmware_level2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1889496363 
	@${RM} ${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o   ../../../../common/vscp_firmware_level2.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1889496363/vscp_firmware_level2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/907750266/eeprom.o: ../../../common/eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/907750266 
	@${RM} ${OBJECTDIR}/_ext/907750266/eeprom.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/907750266/eeprom.o   ../../../common/eeprom.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/907750266/eeprom.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/907750266/eeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/stacktsk.o: ../stacktsk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/stacktsk.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -DBRD_SBC68EC -I"c:/users/akhe/documents/development/vscp_firmware/pic/common" -I".." -I"c:/users/akhe/documents/development/vscp_software/src/vscp/common" -I"c:/users/akhe/documents/development/vscp_firmware/common" -ml -oa- -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/stacktsk.o   ../stacktsk.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/stacktsk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/stacktsk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/rawEthernet.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "..\18f6680_v302_nobl.lkr"  -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG -m"$(BINDIR_)$(TARGETBASE).map" -w -l"D:/Program Files/Microchip/mplabc18/v3.37.01/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_REAL_ICE=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/rawEthernet.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/rawEthernet.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "..\18f6680_v302_nobl.lkr"  -p$(MP_PROCESSOR_OPTION_LD)  -w  -m"$(BINDIR_)$(TARGETBASE).map" -w -l"D:/Program Files/Microchip/mplabc18/v3.37.01/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/rawEthernet.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
