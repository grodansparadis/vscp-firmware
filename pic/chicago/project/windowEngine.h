/* 
 * File:   windowEngine.h
 * Author: Musumeci Salvatore
 *
 * Instruction for module implementation
 * 1) Reserve windowEngineStruct data array in main program.
 *          windowEngineStruct WE_data[size] //where size should be also 1.
 *          // More than one in case of multiple controls
 * 2) In setup procedure of your main, call WE_readEEPROM(&WE_data[0], EEPROM_START_SECTOR) to init structure
 * 3) In initEEPROM add WE_initEEPROM(&WE_data[0], EEPROM_START_SECTOR) to init structure to standard value
 * 4) In 100mS scheduler, put WE_stateMachine_running(&WE_data[0]);
 *
 * Created on 10 agosto 2014, 2.07
 */


/*  EEPROM Local Layout (Total space = xx bytes)
 *  flyTime     0x00
 *  curTime     0x02
 *  pins        0x03
 *  info        0x04
 *  slowTime    0x05
 *  upSubzone   0x06
 *  downSubzone 0x07
 */

#ifndef WINDOWENGINE_H
#define	WINDOWENGINE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include "inttypes.h"


typedef struct{
        unsigned main :5;
        unsigned sub  :3;
    } WE_stateStruct;
typedef struct{
    unsigned command   :4;
    unsigned direction :4;
} WE_pinsStruct;
typedef struct{
    unsigned cmdInv   :1;
    unsigned dirInv   :1;
    unsigned slowTime :4; // Time-step 100mS
} WE_infoStruct;
typedef struct{
    uint8_t up_subzone;
    uint8_t down_subzone;
} WE_subzoneStruct;

typedef struct{
    WE_stateStruct state;
    WE_pinsStruct pins;
    WE_infoStruct info;
    WE_subzoneStruct subzone;
    unsigned int flyTime;
    unsigned int currentTime;
    char inputCode;
    unsigned int setTime;
    unsigned buttonPressed :2;
    uint8_t buttonPressedTime;
    uint16_t EEPROM_start_sector;
} windowEngineStruct;

#define WE_EEPROM_SPACE 8
#define WE_REG_SPACE    0x0F

//status for windowEngineStruct::buttonPressed
#define WE_SUBZONE_NONE  0
#define WE_SUBZONE_UP    1
#define WE_SUBZONE_DOWN  2

//Definition of all states
#define WE_STATE_STOPPED    0
#define WE_STATE_MAN_UP     1
#define WE_STATE_MAN_DOWN   2
#define WE_STATE_OPEN_DOWN  3
#define WE_STATE_OPEN_STOP  4
#define WE_STATE_OPEN_UP    5
#define WE_STATE_CLOSE_UP   6
#define WE_STATE_CLOSE_STOP 7
#define WE_STATE_CLOSE_DOWN 8

//Definition of all possible actions
#define WE_ACTION_NO_INPUT          0
#define WE_ACTION_KEYUP_PRESS       1
#define WE_ACTION_KEYUP_FAST_RLS    2
#define WE_ACTION_KEYUP_SLOW_RLS    3
#define WE_ACTION_KEYDOWN_PRESS     4
#define WE_ACTION_KEYDOWN_FAST_RLS  5
#define WE_ACTION_KEYDOWN_SLOW_RLS  6
#define WE_ACTION_MOVE_TO           7
#define WE_ACTION_OPEN_TO           8
#define WE_ACTION_CLOSE_TO          9
#define WE_ACTION_FULL_OPEN        10
#define WE_ACTION_FULL_CLOSE       11

//Definition of register
#define WE_REG_SUBZONE_UP   0x00
#define WE_REG_SUBZONE_DOWN 0x01
#define WE_REG_FLYTIME_HIGH 0x02
#define WE_REG_FLYTIME_LOW  0x03
#define WE_REG_CURTIME_HIGH 0x04
#define WE_REG_CURTIME_LOW  0x05
#define WE_REG_CMDPIN       0x06
#define WE_REG_DIRPIN       0x07
#define WE_REG_SLOWTIME     0x08
#define WE_REG_STATE        0x09
#define WE_REG_SUBSTATE     0x0A
#define WE_REG_CMDINV       0x0B //move to driver
#define WE_REG_DIRINV       0x0C


void WE_setUp(windowEngineStruct *data,
                int flyTime,
                char commandPin,
                char directionPin,
                char commandInv,
                char directionInv,
                char slowTime,
                char up_subzone,
                char down_subzone);

void WE_getInputCode(struct _imsg *inMsg, windowEngineStruct *data);
uint8_t WE_checkSubZone(uint8_t subzone, windowEngineStruct *data);

void WE_stateMachine_100mS_thread(windowEngineStruct *data);
void WE_stateMachine_running(windowEngineStruct *data);

void WE_auto_stopped_enter(windowEngineStruct *data);
void WE_stopped_enter(windowEngineStruct *data);
void WE_auto_moving_enter(windowEngineStruct *data, unsigned char upDirection, unsigned char OpenAction);
void WE_man_moving_enter(windowEngineStruct *data, unsigned char upDirection);
char WE_moving_running(windowEngineStruct *data);

void WE_readEEPROM(windowEngineStruct *data, uint16_t eeprom_start_sector);
void WE_writeEEPROM(windowEngineStruct *data, uint16_t eeprom_start_sector);
void WE_initEEPROM(windowEngineStruct *data, uint16_t eeprom_start_sector);

void WE_debug(windowEngineStruct *data);

#ifdef	__cplusplus
}
#endif

#endif	/* WINDOWENGINE_H */

