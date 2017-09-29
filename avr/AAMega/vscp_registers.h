/*---------------------------------------------------------------------------
 * more information about VSCP: http://www.vscp.org
 *---------------------------------------------------------------------------
*/

// Register offset for local storage
// VSCP_EEPROM_REG_ZONE marks the blockstart

//page 0

#define REG_DM_START                   0
#define REG_END							(REG_DM_START + (DESCION_MATRIX_ELEMENTS)*16)
#define PAGE0_END						(REG_DM_START + (DESCION_MATRIX_ELEMENTS)*16)

//page 1
// USer registers
#define PAGE1_START						(PAGE0_END+1)
#define REG_ZONE						(PAGE1_START+0)//only to retain compatible with common routines
#define REG_OUTPUT1_ZONE             	(PAGE1_START+0)
#define REG_OUTPUT2_ZONE             	(PAGE1_START+1)
#define REG_OUTPUT3_ZONE             	(PAGE1_START+2)
#define REG_OUTPUT4_ZONE             	(PAGE1_START+3)
#define REG_OUTPUT5_ZONE             	(PAGE1_START+4)
#define REG_OUTPUT6_ZONE             	(PAGE1_START+5)
#define REG_OUTPUT7_ZONE             	(PAGE1_START+6)
#define REG_OUTPUT8_ZONE             	(PAGE1_START+7)
#define REG_OUTPUT9_ZONE             	(PAGE1_START+8)
#define REG_OUTPUT10_ZONE            	(PAGE1_START+9)
#define REG_OUTPUT11_ZONE            	(PAGE1_START+10)
#define REG_OUTPUT12_ZONE            	(PAGE1_START+11)
#define REG_OUTPUT13_ZONE            	(PAGE1_START+12)
#define REG_OUTPUT14_ZONE            	(PAGE1_START+13)
#define REG_OUTPUT15_ZONE           	(PAGE1_START+14)
#define REG_OUTPUT16_ZONE            	(PAGE1_START+15)
#define REG_OUTPUT17_ZONE             	(PAGE1_START+16)
#define REG_OUTPUT18_ZONE             	(PAGE1_START+17)
#define REG_OUTPUT19_ZONE             	(PAGE1_START+18)
#define REG_OUTPUT20_ZONE             	(PAGE1_START+19)
#define REG_OUTPUT21_ZONE             	(PAGE1_START+20)
#define REG_OUTPUT22_ZONE             	(PAGE1_START+21)
#define REG_OUTPUT23_ZONE             	(PAGE1_START+22)
#define REG_OUTPUT24_ZONE             	(PAGE1_START+23)
#define REG_OUTPUT25_ZONE             	(PAGE1_START+24)
#define REG_OUTPUT26_ZONE            	(PAGE1_START+25)
#define REG_OUTPUT27_ZONE            	(PAGE1_START+26)
#define REG_OUTPUT28_ZONE            	(PAGE1_START+27)
#define REG_OUTPUT29_ZONE            	(PAGE1_START+28)
#define REG_OUTPUT30_ZONE            	(PAGE1_START+29)
#define REG_OUTPUT31_ZONE           	(PAGE1_START+30)
#define REG_OUTPUT32_ZONE            	(PAGE1_START+31)

#define REG_OUTPUT1_SUBZONE             (PAGE1_START+32)
#define REG_SUBZONE						(REG_OUTPUT1_SUBZONE)
#define REG_OUTPUT2_SUBZONE             (PAGE1_START+33)
#define REG_OUTPUT3_SUBZONE             (PAGE1_START+34)
#define REG_OUTPUT4_SUBZONE             (PAGE1_START+35)
#define REG_OUTPUT5_SUBZONE             (PAGE1_START+36)
#define REG_OUTPUT6_SUBZONE             (PAGE1_START+37)
#define REG_OUTPUT7_SUBZONE             (PAGE1_START+38)
#define REG_OUTPUT8_SUBZONE             (PAGE1_START+39)
#define REG_OUTPUT9_SUBZONE             (PAGE1_START+40)
#define REG_OUTPUT10_SUBZONE            (PAGE1_START+41)
#define REG_OUTPUT11_SUBZONE            (PAGE1_START+42)
#define REG_OUTPUT12_SUBZONE            (PAGE1_START+43)
#define REG_OUTPUT13_SUBZONE            (PAGE1_START+44)
#define REG_OUTPUT14_SUBZONE            (PAGE1_START+45)
#define REG_OUTPUT15_SUBZONE            (PAGE1_START+46)
#define REG_OUTPUT16_SUBZONE            (PAGE1_START+47)
#define REG_OUTPUT17_SUBZONE            (PAGE1_START+48)
#define REG_OUTPUT18_SUBZONE			(PAGE1_START+49)
#define REG_OUTPUT19_SUBZONE            (PAGE1_START+50)
#define REG_OUTPUT20_SUBZONE            (PAGE1_START+51)
#define REG_OUTPUT21_SUBZONE            (PAGE1_START+52)
#define REG_OUTPUT22_SUBZONE            (PAGE1_START+53)
#define REG_OUTPUT23_SUBZONE            (PAGE1_START+54)
#define REG_OUTPUT24_SUBZONE            (PAGE1_START+55)
#define REG_OUTPUT25_SUBZONE            (PAGE1_START+56)
#define REG_OUTPUT26_SUBZONE            (PAGE1_START+57)
#define REG_OUTPUT27_SUBZONE            (PAGE1_START+58)
#define REG_OUTPUT28_SUBZONE            (PAGE1_START+59)
#define REG_OUTPUT29_SUBZONE            (PAGE1_START+60)
#define REG_OUTPUT30_SUBZONE            (PAGE1_START+61)
#define REG_OUTPUT31_SUBZONE            (PAGE1_START+62)
#define REG_OUTPUT32_SUBZONE            (PAGE1_START+63)

#define REG_OUTPUT_DEFAULT_STATE1       (PAGE1_START+64)
#define REG_OUTPUT_DEFAULT_STATE2       (PAGE1_START+65)
#define REG_OUTPUT_DEFAULT_STATE3       (PAGE1_START+66)
#define REG_OUTPUT_DEFAULT_STATE4       (PAGE1_START+67)

#define REG_OUTPUT_STATE1	            (PAGE1_START+68) //never written to, handled in readAppreg routine
#define REG_OUTPUT_STATE2	            (PAGE1_START+69) //...
#define REG_OUTPUT_STATE3	            (PAGE1_START+70) //never written to, handled in readAppreg routine
#define REG_OUTPUT_STATE4	            (PAGE1_START+71) //...

#define PAGE1_END						(REG_OUTPUT_STATE4)

//page 2
#define NRofTimers						8
#define PAGE2_START						(PAGE1_END+1)
#define REG_TIMER_CONFIG				PAGE2_START //Reg 0
#define REG_TIMER1_VALUE				(1) //RAM only
#define REG_TIMER2_VALUE				(2) //RAM
#define REG_TIMER3_VALUE				(3) //RAM
#define REG_TIMER4_VALUE				(4) //RAM
#define REG_TIMER5_VALUE				(5) //RAM
#define REG_TIMER6_VALUE				(6) //RAM
#define REG_TIMER7_VALUE				(7) //RAM
#define REG_TIMER8_VALUE				(8) //RAM
#define REG_TIMER1_PRE					(PAGE2_START+1)
#define REG_TIMER2_PRE					(PAGE2_START+2)
#define REG_TIMER3_PRE					(PAGE2_START+3)
#define REG_TIMER4_PRE					(PAGE2_START+4)
#define REG_TIMER5_PRE					(PAGE2_START+5)
#define REG_TIMER6_PRE					(PAGE2_START+6)
#define REG_TIMER7_PRE					(PAGE2_START+7)
#define REG_TIMER8_PRE					(PAGE2_START+8)
//T1
#define REG_TIMER1_ZONE					(PAGE2_START+9)
#define REG_TIMER1_SUBZONE				(PAGE2_START+10)
#define REG_TIMER1_CLASS				(PAGE2_START+11)
#define REG_TIMER1_TYPE					(PAGE2_START+12)
#define REG_TIMER1_TOGGLE1				(PAGE2_START+13)
#define REG_TIMER1_TOGGLE2				(PAGE2_START+14)
#define REG_TIMER1_ON1					(PAGE2_START+15)
#define REG_TIMER1_ON2					(PAGE2_START+16)
#define REG_TIMER1_OFF1					(PAGE2_START+17)
#define REG_TIMER1_OFF2					(PAGE2_START+18)
#define REG_TIMER1_SETTIMER				(PAGE2_START+19)
#define REG_TIMER1_SETTIMER_VALUE		(PAGE2_START+20)
//T2
#define REG_TIMER2_ZONE					(REG_TIMER1_SETTIMER_VALUE+1)
#define REG_TIMER2_SUBZONE				(REG_TIMER2_ZONE+1)
#define REG_TIMER2_CLASS				(REG_TIMER2_ZONE+2)
#define REG_TIMER2_TYPE					(REG_TIMER2_ZONE+3)
#define REG_TIMER2_TOGGLE1				(REG_TIMER2_ZONE+4)
#define REG_TIMER2_TOGGLE2				(REG_TIMER2_ZONE+5)
#define REG_TIMER2_ON1					(REG_TIMER2_ZONE+6)
#define REG_TIMER2_ON2					(REG_TIMER2_ZONE+7)
#define REG_TIMER2_OFF1					(REG_TIMER2_ZONE+8)
#define REG_TIMER2_OFF2					(REG_TIMER2_ZONE+9)
#define REG_TIMER2_SETTIMER				(REG_TIMER2_ZONE+10)
#define REG_TIMER2_SETTIMER_VALUE		(REG_TIMER2_ZONE+11)
//T3
#define REG_TIMER3_ZONE					(REG_TIMER2_SETTIMER_VALUE+1)
#define REG_TIMER3_SUBZONE				(REG_TIMER3_ZONE+1)
#define REG_TIMER3_CLASS				(REG_TIMER3_ZONE+2)
#define REG_TIMER3_TYPE					(REG_TIMER3_ZONE+3)
#define REG_TIMER3_TOGGLE1				(REG_TIMER3_ZONE+4)
#define REG_TIMER3_TOGGLE2				(REG_TIMER3_ZONE+5)
#define REG_TIMER3_ON1					(REG_TIMER3_ZONE+6)
#define REG_TIMER3_ON2					(REG_TIMER3_ZONE+7)
#define REG_TIMER3_OFF1					(REG_TIMER3_ZONE+8)
#define REG_TIMER3_OFF2					(REG_TIMER3_ZONE+9)
#define REG_TIMER3_SETTIMER				(REG_TIMER3_ZONE+10)
#define REG_TIMER3_SETTIMER_VALUE		(REG_TIMER3_ZONE+11)
//T4
#define REG_TIMER4_ZONE					(REG_TIMER3_SETTIMER_VALUE+1)
#define REG_TIMER4_SUBZONE				(REG_TIMER4_ZONE+1)
#define REG_TIMER4_CLASS				(REG_TIMER4_ZONE+2)
#define REG_TIMER4_TYPE					(REG_TIMER4_ZONE+3)
#define REG_TIMER4_TOGGLE1				(REG_TIMER4_ZONE+4)
#define REG_TIMER4_TOGGLE2				(REG_TIMER4_ZONE+5)
#define REG_TIMER4_ON1					(REG_TIMER4_ZONE+6)
#define REG_TIMER4_ON2					(REG_TIMER4_ZONE+7)
#define REG_TIMER4_OFF1					(REG_TIMER4_ZONE+8)
#define REG_TIMER4_OFF2					(REG_TIMER4_ZONE+9)
#define REG_TIMER4_SETTIMER				(REG_TIMER4_ZONE+10)
#define REG_TIMER4_SETTIMER_VALUE		(REG_TIMER4_ZONE+11)
//T5
#define REG_TIMER5_ZONE					(REG_TIMER4_SETTIMER_VALUE+1)
#define REG_TIMER5_SUBZONE				(REG_TIMER5_ZONE+1)
#define REG_TIMER5_CLASS				(REG_TIMER5_ZONE+2)
#define REG_TIMER5_TYPE					(REG_TIMER5_ZONE+3)
#define REG_TIMER5_TOGGLE1				(REG_TIMER5_ZONE+4)
#define REG_TIMER5_TOGGLE2				(REG_TIMER5_ZONE+5)
#define REG_TIMER5_ON1					(REG_TIMER5_ZONE+6)
#define REG_TIMER5_ON2					(REG_TIMER5_ZONE+7)
#define REG_TIMER5_OFF1					(REG_TIMER5_ZONE+8)
#define REG_TIMER5_OFF2					(REG_TIMER5_ZONE+9)
#define REG_TIMER5_SETTIMER				(REG_TIMER5_ZONE+10)
#define REG_TIMER5_SETTIMER_VALUE		(REG_TIMER5_ZONE+11)
//T6
#define REG_TIMER6_ZONE					(REG_TIMER5_SETTIMER_VALUE+1)
#define REG_TIMER6_SUBZONE				(REG_TIMER6_ZONE+1)
#define REG_TIMER6_CLASS				(REG_TIMER6_ZONE+2)
#define REG_TIMER6_TYPE					(REG_TIMER6_ZONE+3)
#define REG_TIMER6_TOGGLE1				(REG_TIMER6_ZONE+4)
#define REG_TIMER6_TOGGLE2				(REG_TIMER6_ZONE+5)
#define REG_TIMER6_ON1					(REG_TIMER6_ZONE+6)
#define REG_TIMER6_ON2					(REG_TIMER6_ZONE+7)
#define REG_TIMER6_OFF1					(REG_TIMER6_ZONE+8)
#define REG_TIMER6_OFF2					(REG_TIMER6_ZONE+9)
#define REG_TIMER6_SETTIMER				(REG_TIMER6_ZONE+10)
#define REG_TIMER6_SETTIMER_VALUE		(REG_TIMER6_ZONE+11)
//T7
#define REG_TIMER7_ZONE					(REG_TIMER6_SETTIMER_VALUE+1)
#define REG_TIMER7_SUBZONE				(REG_TIMER7_ZONE+1)
#define REG_TIMER7_CLASS				(REG_TIMER7_ZONE+2)
#define REG_TIMER7_TYPE					(REG_TIMER7_ZONE+3)
#define REG_TIMER7_TOGGLE1				(REG_TIMER7_ZONE+4)
#define REG_TIMER7_TOGGLE2				(REG_TIMER7_ZONE+5)
#define REG_TIMER7_ON1					(REG_TIMER7_ZONE+6)
#define REG_TIMER7_ON2					(REG_TIMER7_ZONE+7)
#define REG_TIMER7_OFF1					(REG_TIMER7_ZONE+8)
#define REG_TIMER7_OFF2					(REG_TIMER7_ZONE+9)
#define REG_TIMER7_SETTIMER				(REG_TIMER7_ZONE+10)
#define REG_TIMER7_SETTIMER_VALUE		(REG_TIMER7_ZONE+11)
//T8
#define REG_TIMER8_ZONE					(REG_TIMER7_SETTIMER_VALUE+1)
#define REG_TIMER8_SUBZONE				(REG_TIMER8_ZONE+1)
#define REG_TIMER8_CLASS				(REG_TIMER8_ZONE+2)
#define REG_TIMER8_TYPE					(REG_TIMER8_ZONE+3)
#define REG_TIMER8_TOGGLE1				(REG_TIMER8_ZONE+4)
#define REG_TIMER8_TOGGLE2				(REG_TIMER8_ZONE+5)
#define REG_TIMER8_ON1					(REG_TIMER8_ZONE+6)
#define REG_TIMER8_ON2					(REG_TIMER8_ZONE+7)
#define REG_TIMER8_OFF1					(REG_TIMER8_ZONE+8)
#define REG_TIMER8_OFF2					(REG_TIMER8_ZONE+9)
#define REG_TIMER8_SETTIMER				(REG_TIMER8_ZONE+10)
#define REG_TIMER8_SETTIMER_VALUE		(REG_TIMER8_ZONE+11)



#define PAGE2_END						(REG_TIMER8_SETTIMER_VALUE)
#define REG_ZONE_END					(PAGE2_END)

/*
// USer registers
#define REG_ZONE						0 //only to retain compatible with common routines
#define REG_SUBZONE						1 //only to retain compatible with common routines
#define REG_OUTPUT1_ZONE             	0
#define REG_OUTPUT2_ZONE             	1
#define REG_OUTPUT3_ZONE             	2
#define REG_OUTPUT4_ZONE             	3
#define REG_OUTPUT5_ZONE             	4
#define REG_OUTPUT6_ZONE             	5
#define REG_OUTPUT7_ZONE             	6
#define REG_OUTPUT8_ZONE             	7
#define REG_OUTPUT9_ZONE             	8
#define REG_OUTPUT10_ZONE            	9
#define REG_OUTPUT11_ZONE            	10
#define REG_OUTPUT12_ZONE            	11
#define REG_OUTPUT13_ZONE            	12
#define REG_OUTPUT14_ZONE            	13
#define REG_OUTPUT15_ZONE           	14
#define REG_OUTPUT16_ZONE            	15
#define REG_OUTPUT1_SUBZONE             16
#define REG_OUTPUT2_SUBZONE             17
#define REG_OUTPUT3_SUBZONE             18
#define REG_OUTPUT4_SUBZONE             19
#define REG_OUTPUT5_SUBZONE             20
#define REG_OUTPUT6_SUBZONE             21
#define REG_OUTPUT7_SUBZONE             22
#define REG_OUTPUT8_SUBZONE             23
#define REG_OUTPUT9_SUBZONE             24
#define REG_OUTPUT10_SUBZONE            25
#define REG_OUTPUT11_SUBZONE            26
#define REG_OUTPUT12_SUBZONE            27
#define REG_OUTPUT13_SUBZONE            28
#define REG_OUTPUT14_SUBZONE            29
#define REG_OUTPUT15_SUBZONE            30
#define REG_OUTPUT16_SUBZONE            31
#define REG_OUTPUT_STATE1	            32 //never written to, handled in readAppreg routine
#define REG_OUTPUT_STATE2	            33 //...
#define REG_OUTPUT_DEFAULT_STATE1       34
#define REG_OUTPUT_DEFAULT_STATE2       35
#define REG_ZONE_END					36
// This marks the space for the decision matrix start.
#define REG_DM_START                    37
#define REG_END							(REG_DM_START + (DESCION_MATRIX_ELEMENTS)*16)

*/
// system registers
#define RegisterPagesUsed				2  //future expansion of DM would be page 2
