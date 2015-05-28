#include "windowEngine.h"
#include "../../augusto/driver.h"
#include "vscp_type.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_appLayer.h"
#include "vscp_firmware.h"

uint8_t WE_readRegister(windowEngineStruct *data, uint8_t idx){
    switch (idx){
        case WE_REG_SUBZONE_UP: return (*data).subzone.up_subzone;
        case WE_REG_SUBZONE_DOWN: return (*data).subzone.down_subzone;
        case WE_REG_FLYTIME_HIGH: return (char)((*data).flyTime >> 8);
        case WE_REG_FLYTIME_LOW: return (char)((*data).flyTime & 0xFF);
        case WE_REG_CURTIME_HIGH: return (char)((*data).currentTime >> 8);
        case WE_REG_CURTIME_LOW: return (char)((*data).currentTime & 0xFF);
        case WE_REG_CMDPIN: return (char) (*data).pins.command;
        case WE_REG_DIRPIN: return (char) (*data).pins.direction;
        case WE_REG_SLOWTIME: return (char) (*data).info.slowTime;
        case WE_REG_CMDINV: return (char) (*data).info.cmdInv;
        case WE_REG_DIRINV: return (char) (*data).info.dirInv;
        case WE_REG_STATE: return (char) (*data).state.main;
        case WE_REG_SUBSTATE: return (char) (*data).state.sub;
        default: return 0;
    }
}
void WE_writeRegister(windowEngineStruct *data, uint8_t idx, uint8_t value){
    switch (idx){
        case WE_REG_SUBZONE_UP: (*data).subzone.up_subzone = value; break;
        case WE_REG_SUBZONE_DOWN: (*data).subzone.down_subzone = value; break;
        case WE_REG_FLYTIME_HIGH: (*data).flyTime = (value << 8) | ((*data).flyTime & 0xFF); break;
        case WE_REG_FLYTIME_LOW: (*data).flyTime = value | ((*data).flyTime & 0xFF00); break;
        case WE_REG_CURTIME_HIGH: (*data).currentTime = (value << 8) | ((*data).currentTime & 0xFF); break;
        case WE_REG_CURTIME_LOW: (*data).currentTime = value | ((*data).currentTime & 0xFF00); break;
        case WE_REG_CMDPIN: (*data).pins.command = value & 0x0F; break;
        case WE_REG_DIRPIN: (*data).pins.direction = value & 0x0F; break;
        case WE_REG_SLOWTIME: (*data).info.slowTime = value & 0x0F; break;
        case WE_REG_CMDINV: (*data).info.cmdInv = value & 0x01; break;
        case WE_REG_DIRINV: (*data).info.dirInv = value & 0x01; break;
    }
    return;
}

void WE_initEEPROM(windowEngineStruct *data, uint16_t eeprom_start_sector){
    (*data).flyTime = 450;
    (*data).currentTime = 275;
    (*data).pins.command = 0;
    (*data).pins.direction = 0;
    (*data).info.cmdInv = 0;
    (*data).info.dirInv = 0;
    (*data).info.slowTime = 5;
    (*data).subzone.up_subzone = 0;
    (*data).subzone.down_subzone = 1;
    (*data).EEPROM_start_sector = eeprom_start_sector;
    WE_writeEEPROM(data, eeprom_start_sector);
}
void WE_writeEEPROM(windowEngineStruct *data, uint16_t eeprom_start_sector){
    char temp;
    eeprom_write(eeprom_start_sector + 0, (char)(((*data).flyTime >> 8) & 0xFF));
    eeprom_write(eeprom_start_sector + 1, (char)((*data).flyTime & 0xFF));
    eeprom_write(eeprom_start_sector + 2, (char)(((*data).currentTime >> 8) & 0xFF));
    eeprom_write(eeprom_start_sector + 3, (char)((*data).currentTime & 0xFF));
    temp = ((*data).pins.direction << 4) + (*data).pins.command;
    eeprom_write(eeprom_start_sector + 4, temp);
    temp = (0x80 * (*data).info.cmdInv) + (0x40 * (*data).info.dirInv) + (*data).info.slowTime;
    eeprom_write(eeprom_start_sector + 5, temp);
    eeprom_write(eeprom_start_sector + 6, (*data).subzone.up_subzone);
    eeprom_write(eeprom_start_sector + 7, (*data).subzone.down_subzone);
}
void WE_readEEPROM(windowEngineStruct *data, uint16_t eeprom_start_sector){
    char temp;
    (*data).flyTime = eeprom_read(eeprom_start_sector + 0);
    (*data).flyTime = (*data).flyTime<<8 + eeprom_read(eeprom_start_sector + 1);
    (*data).currentTime = eeprom_read(eeprom_start_sector + 2);
    (*data).currentTime = (*data).currentTime<<8 + eeprom_read(eeprom_start_sector + 3);
    temp = eeprom_read(eeprom_start_sector + 4);
    (*data).pins.command = temp & 0x0F;
    (*data).pins.direction = (temp >> 4) & 0x0F;
    temp = eeprom_read(eeprom_start_sector + 5);
    (*data).info.cmdInv = ((temp & 0x80)>0);
    (*data).info.dirInv = ((temp & 0x40)>0);
    (*data).info.slowTime = temp & 0x0F;
    (*data).subzone.up_subzone = eeprom_read(eeprom_start_sector + 6);
    (*data).subzone.down_subzone = eeprom_read(eeprom_start_sector + 7);
    (*data).inputCode = WE_ACTION_KEYDOWN_PRESS;
    (*data).EEPROM_start_sector = eeprom_start_sector;
    WE_stateMachine_running(data);
}

void WE_setUp(windowEngineStruct *data, int flyTime, char commandPin, char directionPin, char commandInv, char directionInv, char slowTime, char up_subzone, char down_subzone){
    (*data).state.main = WE_STATE_STOPPED;
    (*data).state.sub = 0;
    (*data).currentTime = flyTime;
    (*data).flyTime = flyTime;
    (*data).pins.command = commandPin;
    (*data).pins.direction = directionPin;
    (*data).info.cmdInv = commandInv;
    (*data).info.dirInv = directionInv;
    (*data).info.slowTime = slowTime;
    (*data).inputCode = WE_ACTION_KEYDOWN_PRESS;
    (*data).subzone.down_subzone = down_subzone;
    (*data).subzone.up_subzone = up_subzone;
    WE_stateMachine_running(data);
}

uint8_t WE_checkSubZone(uint8_t subzone, windowEngineStruct *data){
    if(subzone == (*data).subzone.up_subzone) return WE_SUBZONE_UP;
    if(subzone == (*data).subzone.down_subzone) return WE_SUBZONE_DOWN;
    return WE_SUBZONE_NONE;
}
void WE_getInputCode(struct _imsg *inMsg, windowEngineStruct *data){
    uint8_t subzone = WE_checkSubZone((*inMsg).data[2], data);
    if (subzone == WE_SUBZONE_NONE){
        (*data).inputCode = WE_ACTION_NO_INPUT;
        return;
    }
    if ((*inMsg).vscp_class == VSCP_CLASS1_CONTROL){ //Control Class
        switch ((*inMsg).vscp_type){
            case VSCP_TYPE_CONTROL_OPEN: 
                (*data).inputCode = WE_ACTION_OPEN_TO;
                (*data).setTime = (unsigned int)((*inMsg).data[0] * (*data).flyTime / 100);
                break;
            case VSCP_TYPE_CONTROL_CLOSE:
                (*data).inputCode = WE_ACTION_CLOSE_TO;
                (*data).setTime = (unsigned int)((*inMsg).data[0] * (*data).flyTime / 100);
                break;
            case VSCP_TYPE_CONTROL_CHANGE_LEVEL:
                (*data).setTime = (unsigned int)((*inMsg).data[0] * (*data).flyTime / 100);
                (*data).inputCode = WE_ACTION_MOVE_TO;
                break;
            case VSCP_TYPE_CONTROL_STOP: //TODO: action is currently not available
            default: (*data).inputCode = WE_ACTION_NO_INPUT; break;
        }
    }else if((*inMsg).vscp_class == VSCP_CLASS1_INFORMATION){
        switch((*inMsg).vscp_type){
            case VSCP_TYPE_INFORMATION_BUTTON:
                switch ((*inMsg).data[0]) {
                    case 0://Button Release
                         if ((*data).buttonPressed == subzone){ // Release of button previosly pressed
                            if ((*data).buttonPressedTime < (*data).info.slowTime){
                                if(subzone == WE_SUBZONE_UP) (*data).inputCode = WE_ACTION_KEYUP_FAST_RLS;
                                else (*data).inputCode = WE_ACTION_KEYDOWN_FAST_RLS;
                            }else{
                                if(subzone == WE_SUBZONE_UP) (*data).inputCode = WE_ACTION_KEYUP_SLOW_RLS;
                                else (*data).inputCode = WE_ACTION_KEYDOWN_SLOW_RLS;
                            }
                         } else (*data).inputCode = WE_ACTION_NO_INPUT;
                         (*data).buttonPressedTime = 0;
                         (*data).buttonPressed = WE_SUBZONE_NONE;
                         break;
                    case 1: //Button is pressed
                        if (subzone == WE_SUBZONE_UP){
                            (*data).inputCode = WE_ACTION_KEYUP_PRESS;
                            (*data).buttonPressed = WE_SUBZONE_UP;
                        }else{
                            (*data).inputCode = WE_ACTION_KEYDOWN_PRESS;
                            (*data).buttonPressed = WE_SUBZONE_DOWN;
                        }
                        (*data).buttonPressedTime = 1;
                        break;
                }
                break;
            default: (*data).inputCode = WE_ACTION_NO_INPUT;
        }
    }
}

void WE_debug(windowEngineStruct *data){
    char _data[4];
    _data[0] = (char)(100 * (*data).currentTime / (*data).flyTime);
    _data[1] = ((*data).state.main << 4) + (*data).state.sub;
    _data[2] = ((*data).buttonPressed * 0x80) + ((*data).buttonPressedTime & 0x0F);
    _data[3] = (*data).inputCode;
    vscp_debugMsg((*data).subzone.up_subzone, _data, 4, 1);
    _data[0] = (char)((((*data).currentTime)>>8) & 0xFF);
    _data[1] = (char)((((*data).currentTime)) & 0xFF);
    _data[2] = (char)((((*data).flyTime)>>8) & 0xFF);
    _data[3] = (char)((((*data).flyTime)) & 0xFF);
    vscp_debugMsg((*data).subzone.up_subzone, _data, 4, 2);
}

void WE_stateMachine_running(windowEngineStruct *data){
    WE_debug(data);
    if ((*data).currentTime > (*data).flyTime) (*data).currentTime = (uint16_t) ((*data).flyTime/2);
    switch ((*data).state.main){
        case WE_STATE_STOPPED:
            setOutput((*data).pins.direction, 0);
            if (((*data).inputCode == WE_ACTION_KEYUP_PRESS) | ((*data).inputCode == WE_ACTION_KEYDOWN_PRESS))
                WE_man_moving_enter(data, ((*data).inputCode == WE_ACTION_KEYUP_PRESS));
            if (((*data).inputCode == WE_ACTION_OPEN_TO) | ((*data).inputCode == WE_ACTION_CLOSE_TO))
                WE_auto_moving_enter(data, ((*data).inputCode != WE_ACTION_OPEN_TO), ((*data).inputCode == WE_ACTION_OPEN_TO));
            if((*data).inputCode == WE_ACTION_MOVE_TO)
                if ((*data).setTime >= (*data).currentTime)
                    WE_auto_moving_enter(data, 1,1);
                else
                    WE_auto_moving_enter(data, 0,0);
            break;
        //TODO: insert eeprom copy routine
        case WE_STATE_MAN_UP:
            if (WE_moving_running(data)) (*data).inputCode = WE_ACTION_FULL_OPEN;
            if ((*data).inputCode == WE_ACTION_FULL_OPEN | (*data).inputCode == WE_ACTION_KEYUP_SLOW_RLS | (*data).inputCode == WE_ACTION_KEYDOWN_PRESS)
                WE_stopped_enter(data);
            break;
        case WE_STATE_MAN_DOWN:
            if (WE_moving_running(data)) (*data).inputCode = WE_ACTION_FULL_CLOSE;
            if ((*data).inputCode == WE_ACTION_FULL_CLOSE | (*data).inputCode == WE_ACTION_KEYDOWN_SLOW_RLS | (*data).inputCode == WE_ACTION_KEYUP_PRESS)
                WE_stopped_enter(data);
            break;
        case WE_STATE_OPEN_DOWN:
            if(WE_moving_running(data)) WE_auto_stopped_enter(data);
            break;
        case WE_STATE_OPEN_STOP:
            if ((*data).state.sub < 5) (*data).state.sub++; else WE_auto_moving_enter(data, 1, 1);
            break;
        case WE_STATE_OPEN_UP:
            if(WE_moving_running(data) | ((*data).currentTime>=(*data).setTime))
                    WE_stopped_enter(data);
            break;
        case WE_STATE_CLOSE_UP:
            if(WE_moving_running(data)) WE_auto_stopped_enter(data);
            break;
        case WE_STATE_CLOSE_STOP:
            if ((*data).state.sub < 5) (*data).state.sub++; else WE_auto_moving_enter(data, 0, 0);
            break;
        case WE_STATE_CLOSE_DOWN:
            if(WE_moving_running(data) | ((*data).currentTime<=(*data).setTime))
                    WE_stopped_enter(data);
            break;
    }
    if ((*data).buttonPressedTime>0 & (*data).buttonPressedTime<=(*data).info.slowTime) (*data).buttonPressedTime++;
    (*data).inputCode = WE_ACTION_NO_INPUT;
}

void WE_auto_stopped_enter(windowEngineStruct *data){
    setOutput((*data).pins.command, (*data).info.cmdInv); //Release the engine (if it was working)
    if ((*data).state.main == WE_STATE_OPEN_DOWN)
        (*data).state.main = WE_STATE_OPEN_STOP;
    else
        (*data).state.main = WE_STATE_CLOSE_STOP;
    (*data).state.sub = 0;
}
void WE_auto_moving_enter(windowEngineStruct *data, unsigned char upDirection, unsigned char OpenAction){
    if (OpenAction) (*data).state.main = WE_STATE_OPEN_DOWN; else (*data).state.main = WE_STATE_CLOSE_UP;
    if(upDirection & ((*data).currentTime == (*data).flyTime)){
        if (OpenAction) WE_stopped_enter(data); else WE_auto_stopped_enter(data);
        return;
    }
    if(!upDirection & ((*data).currentTime == 0)){
        if (!OpenAction) WE_stopped_enter(data); else WE_auto_stopped_enter(data);
        return;
    }
    (*data).state.sub = 0; //Initialing sub-flow
    if (OpenAction)
        if (upDirection) (*data).state.main = WE_STATE_OPEN_UP; else (*data).state.main = WE_STATE_OPEN_DOWN;
    else
        if (upDirection) (*data).state.main = WE_STATE_CLOSE_UP; else (*data).state.main = WE_STATE_CLOSE_DOWN;
    setOutput((*data).pins.command, (*data).info.cmdInv); //Release the engine (if it was working)
}
void WE_man_moving_enter(windowEngineStruct *data, unsigned char upDirection){
    if(upDirection & ((*data).currentTime == (*data).flyTime)) return;
    if(!upDirection & ((*data).currentTime == 0)) return;
    (*data).state.sub = 0; //Initialing sub-flow
    if (upDirection) (*data).state.main = WE_STATE_MAN_UP;
    else (*data).state.main = WE_STATE_MAN_DOWN;
    setOutput((*data).pins.command, (*data).info.cmdInv); //Release the engine (if it was working)
}
char WE_moving_running(windowEngineStruct *data){ //Return full closed/opened event
    char status = (((*data).state.main == WE_STATE_MAN_UP) | ((*data).state.main == WE_STATE_OPEN_UP) | ((*data).state.main ==  WE_STATE_CLOSE_UP));
    if ((*data).state.sub == 0){
        (*data).state.sub++;
        setOutput((*data).pins.direction, status ^ (*data).info.dirInv);
        return 0;
    }else if((*data).state.sub == 1){
        (*data).state.sub++;
        setOutput((*data).pins.command, ~(*data).info.cmdInv);
        return 0;
    }else{
       if (status) return (++(*data).currentTime >= (*data).flyTime);
       else return (--(*data).currentTime == 0);
    }
}
void WE_stopped_enter(windowEngineStruct *data){
    setOutput((*data).pins.command, (*data).info.cmdInv); //Release the engine (if it was working)
    (*data).state.main = WE_STATE_STOPPED;
    WE_writeEEPROM(data, (*data).EEPROM_start_sector);
}