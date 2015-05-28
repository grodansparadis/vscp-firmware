#include "driver.h"
#include <vscp_firmware.h>
#include <vscp_class.h>
#include <vscp_type.h>
#include <xc.h>
#include <can18f.h>

timeBasedEventStruct timeEvent, timeOverride;

//Definition of I/O
volatile unsigned char *IN_PIN_PORT[PIN_IN_SIZE] = {&PORTD, &PORTD, &PORTD, &PORTD, &PORTB, &PORTB, &PORTB, &PORTA};
char IN_PIN_NUM[PIN_IN_SIZE] = {0x10, 0x20, 0x40, 0x80, 0x01, 0x02, 0x10, 0x10};
volatile unsigned char *OUT_PIN_PORT[PIN_OUT_SIZE] = {&LATC, &LATD, &LATD, &LATD, &LATD, &LATC, &LATC, &LATC};
char OUT_PIN_NUM[PIN_OUT_SIZE] = {0x20, 0x08, 0x04, 0x02, 0x01, 0x04, 0x02, 0x01};


/* DEFINITION OF STATUS / CONFIGURATION BYTE OF INPUT PIN
 * bit 0: currentStatus
 * bit 1: Reversed logic
 * bit 2: Reserved for future use ****
 * bit 3: Reserved for future use ****
 * bit 4: 0=Button/switch, 1=door/window switch
 * bit 5: OFF/ Closed event is required
 * bit 6: ON / Opened event is required
 * bit 7: Button event is required
 */
struct vscpBoard_inputVar{
    unsigned currentStatus: 1;
    unsigned reversedLogic: 1;
    unsigned doorLogic: 1;
    unsigned offEvent: 1;
    unsigned onEvent: 1;
    unsigned buttonEvent: 1;
}input[PIN_IN_SIZE];

/* DEFINITION OF STATUS / CONFIGURATION BYTE OF OUTPUT PIN
 * bit 0: currentStatus
 * bit 1: Reversed logic
 * bit 2: Reserved for future use ****
 * bit 3: Reserved for future use ****
 * bit 4: Reserved for future use ****
 * bit 5: OFF is required
 * bit 6: ON is required
 * bit 7: Reserved for future use ****
 */
struct vscpBoard_outputVar{
    unsigned currentStatus: 1;
    unsigned reversedLogic: 1;
    unsigned offEvent: 1;
    unsigned onEvent: 1;
}output[PIN_OUT_SIZE];

uint8_t zoneForInput[PIN_IN_SIZE];
uint8_t subzoneForInput[PIN_IN_SIZE];
uint8_t subzoneForOutput[PIN_OUT_SIZE];

void hardware_reinit(); //Internal usage

void IO_setup();  //Internal usage
void TMR0_setup();//Internal usage
uint8_t getInputByte(struct vscpBoard_inputVar in);
uint8_t getOutputByte(struct vscpBoard_outputVar out);
void setOutputStruct(struct vscpBoard_outputVar *out, uint8_t value);
void setInputStruct(struct vscpBoard_inputVar *in, uint8_t value);

void setOutput (unsigned char pin, unsigned char state){
    if (pin>PIN_OUT_SIZE) return;
    if ((state & 0x01) ^ output[pin].reversedLogic)
        *(OUT_PIN_PORT[pin]) |= OUT_PIN_NUM[pin];
    else{
        *(OUT_PIN_PORT[pin]) &= (0xFF ^ OUT_PIN_NUM[pin]);
    }
    if ((state&0x01) != output[pin].currentStatus){
        vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
        vscp_omsg.priority = 3;
        vscp_omsg.flags = 3;
        vscp_omsg.data[0]= 0;
        vscp_omsg.data[1] = vscp_zone;
        vscp_omsg.data[2] = subzoneForOutput[pin];
        if (output[pin].onEvent & (state&0x01)){
            vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_ON;
            vscp_sendEvent();
        }else if (output[pin].offEvent & (state&0x01)==0){
            vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_OFF;
            vscp_sendEvent();
        }
    }
    output[pin].currentStatus = state & 0x01;

}

uint8_t getInput (unsigned char pin){
    return input[pin].currentStatus;
}
void hardware_setup(){
    //hardware_reinit(); //TODO: to be removed
    IO_setup();
    vscp18f_init( TRUE );
    TMR0_setup();
    //INTCONbits.PEIE = 1;
    ei(); // Enable global interrupts
}
void hardware_reinit(){
    for (uint8_t i = 0; i<PIN_IN_SIZE; i++){
        input[i].buttonEvent = 1;
        input[i].doorLogic = 0;
        input[i].offEvent = 1;
        input[i].onEvent = 1;
        input[i].reversedLogic = 1;
        subzoneForInput[i] = i+1;
        zoneForInput[i] = vscp_zone;
    }
    for (uint8_t i=0; i<PIN_OUT_SIZE; i++){
        output[i].currentStatus = 0;
        output[i].offEvent = 1;
        output[i].onEvent = 1;
        output[i].reversedLogic = 0;
        subzoneForOutput[i] = i+1;
    }
}

// TODO: REFACT TO 10mS + debounce counter
void hardware_freeRunning(){
    uint8_t newState;
    for (uint8_t i = 0; i < PIN_IN_SIZE; i++){
        newState = (((*(IN_PIN_PORT[i]) & IN_PIN_NUM[i]) > 0)^ input[i].reversedLogic );
        if (newState != input[i].currentStatus){
            vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
            vscp_omsg.flags = VSCP_VALID_MSG + 3;
            vscp_omsg.priority = 3;
            vscp_omsg.data[1] = zoneForInput[i];
            vscp_omsg.data[2] = subzoneForInput[i];
            if (input[i].buttonEvent){
                vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_BUTTON;
                vscp_omsg.data[0]= newState;
                vscp_sendEvent();
            }
            if (input[i].onEvent & (newState == 1)){
                vscp_omsg.data[0]= 0;
                if (input[i].doorLogic == 1) vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_OPENED;
                else vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_ON;
                vscp_sendEvent();
            }
            if (input[i].offEvent & (newState == 0)){
                vscp_omsg.data[0]= 0;
                if (input[i].doorLogic == 1) vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_CLOSED;
                else vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_OFF;
                vscp_sendEvent();
            }
            input[i].currentStatus = newState;
        }
    }
}

void IO_setup(){
/*  PORTA
 * RA0 --> TEMP_INT
 * RA1 --> TEMP_EXT
 * RA2 --> VSCP LED
 * RA3 --> Vref
 * RA4 --> INPUT6
 * RA5 --> ANALOG3
 *
 *   PORTB
 * RB0 --> INPUT5
 * RB1 --> INPUT4
 * RB2 --> CAN_TXD
 * RB3 --> CAN_RXD
 * RB4 --> INPUT7
 * RB5 --> VSSCP BUTTON
 * RB6 --> ISCP PGC
 * RB7 --> ISCP PGD
 *
 *   PORTC
 * RC0 --> OUTPUT4
 * RC1 --> OUTPUT5
 * RC2 --> OUTPUT6
 * RC3 --> I2C SCL
 * RC4 --> I2C SDA
 * RC5 --> OUTPUT0
 * RC6 --> LED RED
 * RC7 --> LED GREEN
 *
 *   PORTD
 * RD0 --> OUTPUT7
 * RD1 --> OUTPUT3
 * RD2 --> OUTPUT2
 * RD3 --> OUTPUT1
 * RD4 --> INPUT0
 * RD5 --> INPUT1
 * RD6 --> INPUT2
 * RD7 --> INPUT3
 *
 *   PORTE
 * RE0 --> ANALOG2
 * RE1 --> ANALOG1
 * RE2 --> ANALOG0
 */   
    ADCON1 = 0x17; //ADC Configuration
    ADCON0 = 0x01;
    CMCON = 0x07;//Disable comparator
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    TRISA = 0xFF;
    TRISB = 0x3F;
    TRISC = 0xFF;
    TRISD = 0xFF;
    TRISE = 0x0F;
    
    //Leds and button configuration
    vscp_ledTris = 0;
    vscp_btnTris = 1;
    redLed_tris = 0;
    greenLed_tris = 0;

    vscp_ledPin = 0;
    greenLed_pin = 1;
    redLed_pin = 1;

    // I/O configuration
    for (char i=0; i<PIN_IN_SIZE; i++){
        *(IN_PIN_PORT[i] + OFFSET_PORT_TO_TRIS) |= IN_PIN_NUM[i]; //offset is used to access to TRIS register

    }
    for (char i=0; i<PIN_OUT_SIZE; i++){
        *(OUT_PIN_PORT[i] + OFFSET_LAT_TO_TRIS) &= (0xFF ^ OUT_PIN_NUM[i]); //offset is used to access to TRIS register
        setOutput(i, 1);
    }
    
}


void TMR0_setup(){
/* Timer0 Configuration
 *
 * Target = interrupt each 10mS
 * Fosc = 32MHz --> Counter 80'000 ticks (input Fosc/4)
 * Prescaler 1:2, TMR0 = 0xFFFF - 80000d = 0x63C0
 *
 * If the TMR0L register is written, the increment is inhibited for the
 * following two instruction cycles. The user can work
 * around this by writing an adjusted value to the TMR0L register.
 */
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS2 = 0;
    T0CONbits.T0PS1 = 0;
    T0CONbits.T0PS0 = 0;
    TMR0H = TMR0H_INIT;
    TMR0L = TMR0L_INIT;
    T0CONbits.TMR0ON = 1;
    INTCONbits.TMR0IF = 0;
    INTCON2bits.TMR0IP = 1;
    INTCONbits.TMR0IE = 1;
}
void TMR0_interrupt(){
    static char time[2] = {0, 0};
    if (INTCONbits.TMR0IF){ //10mS Event
        INTCONbits.TMR0IF = 0;
        char temp = TMR0L;
        TMR0H = TMR0H + TMR0H_INIT;
        TMR0L += temp + TMR0L_INIT+1;
        vscp_timer+=10; //Timer of vscp firmware
        if (timeEvent._10mS == 1) timeOverride._10mS = 1; else timeEvent._10mS = 1;
        if((++time[0])>=10){ //100mS Event
            time[0] = 0; 
            if (timeEvent._100mS == 1) timeOverride._100mS = 1; else timeEvent._100mS = 1;
            if((++time[1])>=10){ //1s Event
                time[1] = 0;
                if (timeEvent._1s == 1) timeOverride._1s = 1; else timeEvent._1s = 1;
            }
        }
        
    }
}


/* REGISTER MAP
 * This part of code uses all first page of VSCP register dedicated to application 0x00 to 0x7F
 * Allocation is dynamic and it depends on PIN_OUT_SIZE and PIN_IN_SIZE
 *
 * Rules used to maps the registers are:
 * R- FROM 0 TO PIN_IN_SIZE --> Status of input (according to reversedLogic)
 * RW FROM PIN_IN_SIZE TO 2*PIN_IN_SIZE --> Input Status byte
 * RW FROM 2*PIN_IN_SIZE TO 3*PIN_IN_SIZE --> Zone for input
 * RW FROM 3*PIN_IN_SIZE TO 4*PIN_IN_SIZE --> Subzone for input
 * RW FROM 4*PIN_IN_SIZE TO 4*PIN_IN_SIZE+PIN_OUT_SIZE --> Status of output (according to reversedLogic)
 * RW FROM 4*PIN_IN_SIZE+PIN_OUT_SIZE TO 4*PIN_IN_SIZE+2*PIN_OUT_SIZE --> Output Status byte
 * RW FROM 4*PIN_IN_SIZE+2*PIN_OUT_SIZE TO 4*PIN_IN_SIZE+3*PIN_OUT_SIZE --> Subzone for Output
 *
 * 0x7F save RAM in EEPROM (is 1 is written) --> should used for all modules
 */
uint8_t hardware_readRegister(uint8_t address){
    if (address<PIN_IN_SIZE) return input[address].currentStatus;
    else if (address<2*PIN_IN_SIZE) return getInputByte(input[address-PIN_IN_SIZE]);
    else if (address<3*PIN_IN_SIZE) return zoneForInput[address-2*PIN_IN_SIZE];
    else if (address<4*PIN_IN_SIZE) return subzoneForInput[address-3*PIN_IN_SIZE];
    else if (address<(4*PIN_IN_SIZE+PIN_OUT_SIZE)) return output[address-4*PIN_IN_SIZE].currentStatus;
    else if (address<(4*PIN_IN_SIZE+2*PIN_OUT_SIZE)) return getOutputByte(output[address-4*PIN_IN_SIZE-PIN_OUT_SIZE]);
    else if (address<(4*PIN_IN_SIZE+3*PIN_OUT_SIZE)) return subzoneForOutput[address-4*PIN_IN_SIZE-2*PIN_OUT_SIZE];
    else return 0;
}
void hardware_writeRegister(uint8_t address, uint8_t value){
    if (address<PIN_IN_SIZE); //Ignored because it's not possible to override an input
    else if (address<2*PIN_IN_SIZE) setInputStruct(&(input[address-PIN_IN_SIZE]), value);
    else if (address<3*PIN_IN_SIZE) zoneForInput[address-2*PIN_IN_SIZE] = value;
    else if (address<4*PIN_IN_SIZE) subzoneForInput[address-3*PIN_IN_SIZE] = value;
    else if (address<(4*PIN_IN_SIZE+PIN_OUT_SIZE)) setOutput(address-4*PIN_IN_SIZE, value & 0x01);
    else if (address<(4*PIN_IN_SIZE+2*PIN_OUT_SIZE)){
        setOutputStruct(&(output[address-4*PIN_IN_SIZE-PIN_OUT_SIZE]), value & 0xFE);
        setOutput(address-4*PIN_IN_SIZE, value & 0x01);
    }
    else if (address<(4*PIN_IN_SIZE+3*PIN_OUT_SIZE)) subzoneForOutput[address-4*PIN_IN_SIZE-2*PIN_OUT_SIZE] = value;
}


void hardware_saveEEPROM(uint16_t eeprom_start_sector){
    for (uint8_t i=0; i<PIN_IN_SIZE; i++){
        eeprom_write(eeprom_start_sector+i, getInputByte(input[i]));
        eeprom_write(eeprom_start_sector+i+PIN_IN_SIZE, zoneForInput[i]);
        eeprom_write(eeprom_start_sector+i+2*PIN_IN_SIZE, subzoneForInput[i]);
    }
    for (uint8_t i=0; i<PIN_OUT_SIZE; i++){
        eeprom_write(eeprom_start_sector+i+3*PIN_IN_SIZE, getOutputByte(output[i]));
        eeprom_write(eeprom_start_sector+i+3*PIN_IN_SIZE+PIN_OUT_SIZE, subzoneForOutput[i]);
    }
}
void hardware_loadEEPROM(uint16_t eeprom_start_sector){
    for (uint8_t i=0; i<PIN_IN_SIZE; i++){
        setInputStruct(&(input[i]),eeprom_read(eeprom_start_sector+i));
        zoneForInput[i] = eeprom_read(eeprom_start_sector+i+PIN_IN_SIZE);
        subzoneForInput[i] = eeprom_read(eeprom_start_sector+i+2*PIN_IN_SIZE);
    }
    for (uint8_t i=0; i<PIN_OUT_SIZE; i++){
        setOutputStruct(&(output[i]),eeprom_read(eeprom_start_sector+i+3*PIN_IN_SIZE));
        subzoneForOutput[i] = eeprom_read(eeprom_start_sector+i+3*PIN_IN_SIZE+PIN_OUT_SIZE);
    }
}

uint8_t getOutputByte(struct vscpBoard_outputVar out){
    uint8_t temp; temp=0;
    if (out.currentStatus) temp |=0x01;
    if (out.offEvent) temp |= 0x20;
    if (out.onEvent) temp |= 0x40;
    return temp;
}
void setOutputStruct(struct vscpBoard_outputVar *out, uint8_t value){
    (*out).reversedLogic = ((value & 0x02)>0);
    (*out).offEvent = ((value & 0x20)>0);
    (*out).onEvent = ((value & 0x40)>0);
}
uint8_t getInputByte(struct vscpBoard_inputVar in){
    uint8_t temp; temp=0;
    if (in.reversedLogic) temp |= 0x02;
    if (in.doorLogic) temp |= 0x10;
    if (in.offEvent) temp |= 0x20;
    if (in.onEvent) temp |= 0x40;
    if (in.buttonEvent) temp |= 0x80;
    return temp;
}
void setInputStruct(struct vscpBoard_inputVar *in, uint8_t value){
    (*in).reversedLogic = ((value & 0x02)>0);
    (*in).doorLogic = ((value & 0x10)>0);
    (*in).offEvent = ((value & 0x20)>0);
    (*in).onEvent = ((value & 0x40)>0);
    (*in).buttonEvent = ((value & 0x80)>0);
}


