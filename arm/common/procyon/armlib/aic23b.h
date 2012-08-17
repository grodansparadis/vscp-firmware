/*! \file aic23b.h \brief TI TLV320AIC23B Audio Codec Driver. */
//*****************************************************************************
//
// File Name	: 'aic23b.h'
// Title		: TI TLV320AIC23B Audio Codec Driver
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.05.05
// Revised		: 2004.07.12
// Version		: 0.1
// Target MCU	: ARM processors
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#ifndef AIC23B_H
#define AIC23B_H

#define AIC_I2C_ADDR				0x34

#define AIC_REG_VOL_LINEIN_LEFT		0x0000
#define AIC_REG_VOL_LINEIN_RIGHT	0x0200
#define AIC_REG_VOL_HP_LEFT			0x0400
#define AIC_REG_VOL_HP_RIGHT		0x0600
#define AIC_REG_ANALOG_PATH			0x0800
#define AIC_REG_DIGITAL_PATH		0x0A00
#define AIC_REG_POWERDOWN			0x0C00
#define AIC_REG_INTERFACE_FORMAT	0x0E00
#define AIC_REG_SAMPLE_RATE			0x1000
#define AIC_REG_DIGITAL_IF_ACT		0x1200
#define AIC_REG_RESET				0x1E00

// AIC_REG_VOL_LINEIN_LEFT bits
#define AIC_VLIL_LRS	8	// 0=disabled 1=left/right simultaneous update
#define AIC_VLIL_LIM	7	// 0=Normal 1=Muted
#define AIC_VLIL_LIV4	4
#define AIC_VLIL_LIV3	3
#define AIC_VLIL_LIV2	2
#define AIC_VLIL_LIV1	1
#define AIC_VLIL_LIV0	0

// AIC_REG_VOL_LINEIN_RIGHT bits
#define AIC_VLIR_RLS	8	// 0=disabled 1=left/right simultaneous update
#define AIC_VLIR_RIM	7	// Mute: 0=Normal 1=Muted
#define AIC_VLIR_RIV4	4
#define AIC_VLIR_RIV3	3
#define AIC_VLIR_RIV2	2
#define AIC_VLIR_RIV1	1
#define AIC_VLIR_RIV0	0

// AIC_REG_VOL_HP_LEFT bits
#define AIC_VHPL_LRS	8	// 0=disabled 1=left/right simultaneous update
#define AIC_VHPL_LZC	7

// AIC_REG_VOL_HP_RIGHT bits
#define AIC_VHPR_RLS	8	// 0=disabled 1=left/right simultaneous update
#define AIC_VHPR_RZC	7

// AIC_REG_ANALOG_PATH bits
#define AIC_AP_STA2		8	// sidetone attenuation
#define AIC_AP_STA1		7
#define AIC_AP_STA0		6
#define AIC_AP_STE		5	// sidetone enable
#define AIC_AP_DAC		4	// DAC enable: 0=off 1=on
#define AIC_AP_BYP		3	// analog bypass linein->lineout: 0=disabled
#define AIC_AP_INSEL	2	// ADC input select: 0=line 1=mic
#define AIC_AP_MICM		1	// MIC mute:  0=normal 1=mute
#define AIC_AP_MICB		0	// MIC boost: 0=0dB    1=+20dB

// AIC_REG_DIGITAL_PATH bits
#define AIC_DP_DACM		3	// DAC mute: 0=disabled
#define AIC_DP_DEEMP1	2	// De-emphasis: 00=off 01=32KHz 10=44.1KHz 11=48KHz
#define AIC_DP_DEEMP0	1	
#define AIC_DP_ADCHP	0	// ADC high-pass: 0=disabled

// AIC_REG_POWERDOWN bits
#define AIC_PD_OFF		7	// 0=power on 1=power off
#define AIC_PD_CLK		6
#define AIC_PD_OSC		5
#define AIC_PD_OUT		4
#define AIC_PD_DAC		3
#define AIC_PD_ADC		2
#define AIC_PD_MIC		1
#define AIC_PD_LINE		0

// AIC_REG_INTERFACE_FORMAT bits
#define AIC_IF_MS		6	// Master/Slave: 0=slave 1=master
#define AIC_IF_LRSWAP	5	// DAC LR swap: 0=disabled
#define AIC_IF_LRP		4	// left/right phase
#define AIC_IF_IWL1		3	// Input Length:
#define AIC_IF_IWL0		2	//	00=16bit 01=20bit 10=24bit 11=32bit
#define AIC_IF_FOR1		1	// Data Format:
#define AIC_IF_FOR0		0	//	11=DSP 10=I2S MSB 1st left 01=MSB 1st left 00=MSB 1st right

#define AIC_IF_16BIT	0x00
#define AIC_IF_20BIT	0x04
#define AIC_IF_24BIT	0x08
#define AIC_IF_32BIT	0x0C

#define AIC_IF_DSP		0x03
#define AIC_IF_I2S		0x02
#define AIC_IF_MSBL		0x01
#define AIC_IF_MSBR		0x00

// AIC_REG_SAMPLE_RATE bits
#define AIC_SR_CLKOUT	7	// Clock output divider: 0=MCLK 1=MCLK/2
#define AIC_SR_CLKIN	6	// Clock input divider:  0=MCLK 1=MCLK/2
#define AIC_SR_SR3		5	// Sample Rate Control:
#define AIC_SR_SR2		4
#define AIC_SR_SR1		3
#define AIC_SR_SR0		2
#define AIC_SR_BOSR		1	// Base Oversampl. Rate: Nor:0=256fs 1=384fs USB: 0=250fs 1=272fs 
#define AIC_SR_USB		0	// Clock mode select: 0=Normal 1=USB (12MHz)

#define AIC_SR_USB_96KHZ	(BIT(AIC_SR_USB) | BIT(AIC_SR_SR0) | BIT(AIC_SR_SR1) | BIT(AIC_SR_SR2))
#define AIC_SR_USB_48KHZ	(BIT(AIC_SR_USB))
#define AIC_SR_USB_44KHZ	(BIT(AIC_SR_USB) | BIT(AIC_SR_BOSR) | BIT(AIC_SR_SR3))
#define AIC_SR_USB_32KHZ	(BIT(AIC_SR_USB) | BIT(AIC_SR_SR1) | BIT(AIC_SR_SR2))
#define AIC_SR_USB_8KHZ		(BIT(AIC_SR_USB) | BIT(AIC_SR_SR0) | BIT(AIC_SR_SR1))

#define AIC_SR_18432_96KHZ	(BIT(AIC_SR_BOSR) | BIT(AIC_SR_SR0) | BIT(AIC_SR_SR1) | BIT(AIC_SR_SR2))
#define AIC_SR_18432_48KHZ	(BIT(AIC_SR_BOSR))
#define AIC_SR_18432_32KHZ	(BIT(AIC_SR_BOSR) | BIT(AIC_SR_SR1) | BIT(AIC_SR_SR2))
#define AIC_SR_18432_8KHZ	(BIT(AIC_SR_BOSR) | BIT(AIC_SR_SR0) | BIT(AIC_SR_SR1))


// AIC_REG_DIGITAL_IF_ACT bits

void aicInit(void);
int  aicWriteReg(u16 regdata);

void aicSetVolumeHp(u08 volume);
void aicSetVolumeLineIn(u08 volume);
void aicSetVolumeLineOut(u08 volume);

void aicDigitalEnable(void);



#endif
