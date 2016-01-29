/*---------------------------------------------------------------------------
 * version 0.0.1
 * 29/08/2010
 *
 * Sven Zwiers
 *
 * more information about VSCP: http://www.vscp.org
 *---------------------------------------------------------------------------
*/

// Register offset for local storage
// VSCP_EEPROM_REG_ZONE marks the blockstart

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


// system registers
#define RegisterPagesUsed				1  //future expansion of DM would be page 2
