///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:23 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\Program\IAR Systems\Embedded Workbench               /
//                    5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75 /
//                    x_mrcc.c                                                /
//    Command line =  "D:\Program\IAR Systems\Embedded Workbench              /
//                    5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75 /
//                    x_mrcc.c" -lC D:\development\gediminas\CAN4USB\project\ /
//                    RELEASE\List\ -lA D:\development\gediminas\CAN4USB\proj /
//                    ect\RELEASE\List\ -o D:\development\gediminas\CAN4USB\p /
//                    roject\RELEASE\Obj\ --no_cse --no_unroll --no_inline    /
//                    --no_code_motion --no_tbaa --no_clustering              /
//                    --no_scheduling --debug --endian little --cpu           /
//                    ARM7TDMI-S -e --fpu None --dlib_config "D:\Program\IAR  /
//                    Systems\Embedded Workbench                              /
//                    5.0\ARM\INC\DLib_Config_Normal.h" -I                    /
//                    D:\development\gediminas\CAN4USB\project\ -I            /
//                    D:\development\gediminas\CAN4USB\project\app\ -I        /
//                    D:\development\gediminas\CAN4USB\project\board\ -I      /
//                    D:\development\gediminas\CAN4USB\project\module\ -I     /
//                    D:\development\gediminas\CAN4USB\project\STR75xLibrary\ /
//                    library\inc\ -I j:\common\ -I "D:\Program\IAR           /
//                    Systems\Embedded Workbench 5.0\ARM\INC\" --interwork    /
//                    --cpu_mode thumb -On                                    /
//    List file    =  D:\development\gediminas\CAN4USB\project\RELEASE\List\7 /
//                    5x_mrcc.s                                               /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME `75x_mrcc`

        PUBLIC MRCC_CKRTCConfig
        PUBLIC MRCC_CKSYSConfig
        PUBLIC MRCC_CKTIMConfig
        PUBLIC MRCC_CKUSBConfig
        PUBLIC MRCC_ClearFlag
        PUBLIC MRCC_ClearITPendingBit
        PUBLIC MRCC_DeInit
        PUBLIC MRCC_EnterSTANDBYMode
        PUBLIC MRCC_EnterSTOPMode
        PUBLIC MRCC_EnterWFIMode
        PUBLIC MRCC_GenerateSWReset
        PUBLIC MRCC_GetCKSYSCounter
        PUBLIC MRCC_GetClocksStatus
        PUBLIC MRCC_GetFlagStatus
        PUBLIC MRCC_GetITStatus
        PUBLIC MRCC_HCLKConfig
        PUBLIC MRCC_IOVoltageRangeConfig
        PUBLIC MRCC_ITConfig
        PUBLIC MRCC_LPMC_DBGConfig
        PUBLIC MRCC_LPOSCConfig
        PUBLIC MRCC_MCOConfig
        PUBLIC MRCC_OSC32KConfig
        PUBLIC MRCC_OSC4MConfig
        PUBLIC MRCC_PCLKConfig
        PUBLIC MRCC_PeripheralClockConfig
        PUBLIC MRCC_PeripheralSWResetConfig
        PUBLIC MRCC_RTCMConfig
        PUBLIC MRCC_ReadBackupRegister
        PUBLIC MRCC_SetBuilderCounter
        PUBLIC MRCC_WaitForOSC4MStartUp
        PUBLIC MRCC_WriteBackupRegister
        PUBLIC MRCC_XTDIV2Config
        
        CFI Names cfiNames0
        CFI StackFrame CFA R13 DATA
        CFI Resource R0:32, R1:32, R2:32, R3:32, R4:32, R5:32, R6:32, R7:32
        CFI Resource R8:32, R9:32, R10:32, R11:32, R12:32, R13:32, R14:32
        CFI EndNames cfiNames0
        
        CFI Common cfiCommon0 Using cfiNames0
        CFI CodeAlign 2
        CFI DataAlign 4
        CFI ReturnAddress R14 CODE
        CFI CFA R13+0
        CFI R0 Undefined
        CFI R1 Undefined
        CFI R2 Undefined
        CFI R3 Undefined
        CFI R4 SameValue
        CFI R5 SameValue
        CFI R6 SameValue
        CFI R7 SameValue
        CFI R8 SameValue
        CFI R9 SameValue
        CFI R10 SameValue
        CFI R11 SameValue
        CFI R12 Undefined
        CFI R14 SameValue
        CFI EndCommon cfiCommon0
        
// D:\Program\IAR Systems\Embedded Workbench 5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75x_mrcc.c
//    1 /******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
//    2 * File Name          : 75x_mrcc.c
//    3 * Author             : MCD Application Team
//    4 * Date First Issued  : 03/10/2006
//    5 * Description        : This file provides all the MRCC software functions.
//    6 ********************************************************************************
//    7 * History:
//    8 * 07/17/2006 : V1.0
//    9 * 03/10/2006 : V0.1
//   10 ********************************************************************************
//   11 * THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
//   12 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
//   13 * AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
//   14 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
//   15 * CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
//   16 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
//   17 *******************************************************************************/
//   18 
//   19 /* Includes ------------------------------------------------------------------*/
//   20 #include "75x_mrcc.h"
//   21 
//   22 /* Private typedef -----------------------------------------------------------*/
//   23 /* Private define ------------------------------------------------------------*/
//   24 #define MRCC_FLAG_Mask    0x1F    /* MRCC Flag Mask */
//   25 
//   26 /* MRCC_PWRCTRL mask bits */
//   27 #define MRCC_LP_Set_Mask             0x00000001
//   28 #define MRCC_LP_Reset_Mask           0xFFFFFFFE
//   29 #define MRCC_SWRESET_Mask            0x00000002
//   30 #define MRCC_WFI_Mask                0x00000004
//   31 #define MRCC_STANDBY_Mask            0x00000006
//   32 #define MRCC_LPMC_Reset_Mask         0xFFFFFFF9
//   33 #define MRCC_LPDONE_Reset_Mask       0xFFFFFF7F
//   34 #define MRCC_LPPARAM_Reset_Mask      0xFFFF1FFF
//   35 #define MRCC_WFIParam_Reset_Mask     0xFFFF1FEF
//   36 #define MRCC_CKRTCSEL_Set_Mask       0x03000000
//   37 #define MRCC_CKRTCSEL_Reset_Mask     0xFCFFFFFF
//   38 #define MRCC_CKRTCOK_Mask            0x08000000
//   39 #define MRCC_LPOSCEN_Mask            0x10000000
//   40 #define MRCC_OSC32KEN_Mask           0x20000000
//   41             
//   42 /* MRCC_CLKCTL mask bits */
//   43 #define MRCC_PPRESC_Set_Mask        0x00000003
//   44 #define MRCC_PPRESC_Reset_Mask      0xFFFFFFFC
//   45 #define MRCC_PPRESC2_Mask           0x00000004
//   46 #define MRCC_HPRESC_Set_Mask        0x00000018
//   47 #define MRCC_HPRESC_Reset_Mask      0xFFFFFFE7
//   48 #define MRCC_MCOS_Reset_Mask        0xFFFFFF3F
//   49 #define MRCC_XTDIV2_Set_Mask        0x00008000
//   50 #define MRCC_XTDIV2_Reset_Mask      0xFFFF7FFF
//   51 #define MRCC_OSC4MBYP_Set_Mask      0x00010000
//   52 #define MRCC_OSC4MBYP_Reset_Mask    0xFFFEFFFF
//   53 #define MRCC_OSC4MOFF_Set_Mask      0x00020000  
//   54 #define MRCC_OSC4MOFF_Reset_Mask    0xFFFDFFFF
//   55 #define MRCC_NCKDF_Set_Mask         0x00040000
//   56 #define MRCC_NCKDF_Reset_Mask       0xFFFBFFFF
//   57 #define MRCC_CKOSCSEL_Set_Mask      0x00200000
//   58 #define MRCC_CKOSCSEL_Reset_Mask    0xFFDFFFFF
//   59 #define MRCC_CKUSBSEL_Mask          0x00400000
//   60 #define MRCC_CKSEL_Set_Mask         0x00800000
//   61 #define MRCC_CKSEL_Reset_Mask       0xFF7FFFFF
//   62 #define MRCC_CKSEL_CKOSCSEL_Mask    0x00A00000
//   63 #define MRCC_PLLEN_Set_Mask         0x01000000
//   64 #define MRCC_PLLEN_Reset_Mask       0xFEFFFFFF
//   65 #define MRCC_PLL2EN_Set_Mask        0x02000000
//   66 #define MRCC_PLL2EN_Reset_Mask      0xFDFFFFFF
//   67 #define MRCC_MX_Set_Mask            0x18000000
//   68 #define MRCC_MX_Reset_Mask          0xE7FFFFFF
//   69 #define MRCC_LOCK_Mask              0x80000000
//   70 #define MRCC_PLLEN_LOCK_Mask        0x81000000
//   71 
//   72 /* Typical Value of the OSC4M in Hz */
//   73 #define OSC4M_Value    4000000   
//   74 
//   75 /* Typical Value of the OSC4M divided by 128 (used to clock the RTC) in Hz */
//   76 #define OSC4M_Div128_Value    31250
//   77    
//   78 /* Typical Value of the OS32K Oscillator Frequency in Hz */
//   79 #define OSC32K_Value    32768     
//   80 
//   81 /* Typical Reset Value of the Internal LPOSC Oscillator Frequency in Hz */
//   82 #define LPOSC_Value    245000   
//   83 
//   84 /* Typical Reset Value of the Internal FREEOSC Oscillator Frequency in Hz */
//   85 #define FREEOSC_Value    5000000 
//   86 
//   87 /* Time out for OSC4M start up */
//   88 #define OSC4MStartUp_TimeOut   0xFE
//   89 
//   90 /* Private macro -------------------------------------------------------------*/
//   91 /* Private variables ---------------------------------------------------------*/
//   92 /* Private function prototypes -----------------------------------------------*/
//   93 static ErrorStatus SetCKSYS_FREEOSC(void);
//   94 static ErrorStatus SetCKSYS_OSC4M(u32 PLL_State);
//   95 static ErrorStatus SetCKSYS_OSC4MPLL(u32 PLL_Mul);
//   96 static ErrorStatus SetCKSYS_RTC(u32 PLL_State);
//   97 static void WriteLPBit(void);
//   98 static void WriteCKOSCSELBit(void);
//   99 
//  100 /* Private functions ---------------------------------------------------------*/
//  101 
//  102 /*******************************************************************************
//  103 * Function Name  : MRCC_DeInit
//  104 * Description    : Deinitializes the MRCC peripheral registers to their default
//  105 *                  reset values. 
//  106 *                   - Depending on the system clock state, some bits in MRCC_CLKCTL
//  107 *                     register can’t be reset.
//  108 *                   - The OSC32K, LPOSC and RTC clock selection configuration 
//  109 *                     bits in MRCC_PWRCTRL register are not cleared by this  
//  110 *                     function. To reset those bits, use the dedicated functions 
//  111 *                     available within this driver.
//  112 *                   - The MRCC_RFSR, MRCC_BKP0 and MRCC_BKP1 registers are not
//  113 *                     reset by this function.
//  114 * Input          : None
//  115 * Output         : None
//  116 * Return         : None
//  117 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function MRCC_DeInit
        THUMB
//  118 void MRCC_DeInit(void)
//  119 {
MRCC_DeInit:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  120   /* Try to clear NCKDF bit */
//  121   MRCC->CLKCTL &= MRCC_NCKDF_Reset_Mask;
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??MRCC_DeInit_0  ;; 0xfffbffff
        ANDS     R1,R1,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R1,[R0, #+0]
//  122 
//  123   if((MRCC->CLKCTL & MRCC_NCKDF_Set_Mask) != RESET)
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+13
        BPL      ??MRCC_DeInit_1
//  124   {/* No clock detected on OSC4M */
//  125 
//  126     /* Reset LOCKIE, LOCKIF, CKUSBSEL, NCKDIE, OSC4MOFF, OSC4MBYP, MCOS[1:0], 
//  127        MCOP, HPRESC[1:0], PPRES[2:0] bits */
//  128     MRCC->CLKCTL &= 0x9FB40000;
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??MRCC_DeInit_0+0x4  ;; 0x9fb40000
        ANDS     R1,R1,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R1,[R0, #+0]
//  129      
//  130     if((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET)
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BPL      ??MRCC_DeInit_2
//  131     { 
//  132       /* Clear CKOSCSEL bit --------------------------------------------------*/         
//  133       /* Execute CKOSCSEL bit writing sequence */
//  134       WriteCKOSCSELBit();
        BL       WriteCKOSCSELBit
        B        ??MRCC_DeInit_2
//  135     }
//  136   }
//  137   else
//  138   {/* Clock present on OSC4M */
//  139 
//  140     if((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET)
??MRCC_DeInit_1:
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BPL      ??MRCC_DeInit_3
//  141     { 
//  142       /* Reset CKSEL bit */
//  143       MRCC->CLKCTL &= MRCC_CKSEL_Reset_Mask;
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??DataTable19  ;; 0xff7fffff
        ANDS     R1,R1,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R1,[R0, #+0]
//  144 
//  145       /* Clear CKOSCSEL bit --------------------------------------------------*/
//  146       /* Execute CKOSCSEL bit writing sequence */
//  147       WriteCKOSCSELBit();
        BL       WriteCKOSCSELBit
//  148     }
//  149 
//  150     if((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) == RESET)
??MRCC_DeInit_3:
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+8
        BMI      ??MRCC_DeInit_4
//  151     {
//  152       /* Set CKSEL bit */
//  153       MRCC->CLKCTL |= MRCC_CKSEL_Set_Mask;  
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+16       ;; #+8388608
        ORRS     R1,R1,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R1,[R0, #+0]
//  154     }
//  155 
//  156     /* Disable PLL */
//  157     MRCC->CLKCTL &= MRCC_PLLEN_Reset_Mask;
??MRCC_DeInit_4:
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??DataTable36  ;; 0xfeffffff
        ANDS     R1,R1,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R1,[R0, #+0]
//  158 
//  159     /* Reset LOCKIE, LOCKIF, MX[1:0], CKUSBSEL, NCKDIE, MCOS[1:0], MCOP,
//  160        HPRESC[1:0], PPRES[2:0] bits */
//  161     MRCC->CLKCTL &= 0x87B70000;
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??MRCC_DeInit_0+0x8  ;; 0x87b70000
        ANDS     R1,R1,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R1,[R0, #+0]
//  162 
//  163     /* Reset CKSEL bit */
//  164     MRCC->CLKCTL &= MRCC_CKSEL_Reset_Mask;
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??DataTable19  ;; 0xff7fffff
        ANDS     R1,R1,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R1,[R0, #+0]
//  165 
//  166     /* Reset OSC4MOFF and OSC4MBYP bits */
//  167     MRCC->CLKCTL &= 0xFFFCFFFF;   
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??DataTable22  ;; 0xfffcffff
        ANDS     R1,R1,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R1,[R0, #+0]
//  168   }
//  169 
//  170   /* Reset RTCM, EN33V, LP_PARAM[15:13], WFI_FLASH_EN, LPMC_DBG and LPMC[1:0] bits */
//  171   MRCC->PWRCTRL &= 0xFBFE1FE1;
??MRCC_DeInit_2:
        LDR      R0,??DataTable49  ;; 0x60000028
        LDR      R0,[R0, #+0]
        LDR      R1,??MRCC_DeInit_0+0xC  ;; 0xfbfe1fe1
        ANDS     R1,R1,R0
        LDR      R0,??DataTable49  ;; 0x60000028
        STR      R1,[R0, #+0]
//  172   
//  173   /* Reset PCLKEN register bits */
//  174   MRCC->PCLKEN = 0x00;
        LDR      R0,??DataTable26  ;; 0x60000030
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  175   
//  176   /* Reset PSWRES register bits */
//  177   MRCC->PSWRES = 0x00;  
        LDR      R0,??DataTable27  ;; 0x60000034
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  178 
//  179   /* Clear NCKDF bit */
//  180   MRCC->CLKCTL &= MRCC_NCKDF_Reset_Mask; 
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??MRCC_DeInit_0  ;; 0xfffbffff
        ANDS     R1,R1,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R1,[R0, #+0]
//  181 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??MRCC_DeInit_0:
        DC32     0xfffbffff
        DC32     0x9fb40000
        DC32     0x87b70000
        DC32     0xfbfe1fe1
        CFI EndBlock cfiBlock0

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable19:
        DC32     0xff7fffff

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable22:
        DC32     0xfffcffff

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable26:
        DC32     0x60000030

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable27:
        DC32     0x60000034
//  182 
//  183 /*******************************************************************************
//  184 * Function Name  : MRCC_XTDIV2Config
//  185 * Description    : Enables or disables the oscillator divider by 2. This function
//  186 *                  must not be used when the PLL is enabled.
//  187 * Input          : - MRCC_XTDIV2: specifies the new state of the oscillator 
//  188 *                    divider by 2.
//  189 *                    This parameter can be one of the following values:
//  190 *                          - MRCC_XTDIV2_Disable: oscillator divider by 2 disbaled
//  191 *                          - MRCC_XTDIV2_Enable: oscillator divider by 2 enbaled
//  192 * Output         : None
//  193 * Return         : None
//  194 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function MRCC_XTDIV2Config
        THUMB
//  195 void MRCC_XTDIV2Config(u32 MRCC_XTDIV2)
//  196 {
MRCC_XTDIV2Config:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  197   if(MRCC_XTDIV2 == MRCC_XTDIV2_Enable)
        MOVS     R1,#+128
        LSLS     R1,R1,#+8        ;; #+32768
        CMP      R0,R1
        BNE      ??MRCC_XTDIV2Config_0
//  198   {
//  199     MRCC->CLKCTL |= MRCC_XTDIV2_Enable;
        LDR      R1,??DataTable58  ;; 0x60000020
        LDR      R1,[R1, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+8        ;; #+32768
        ORRS     R2,R2,R1
        LDR      R1,??DataTable58  ;; 0x60000020
        STR      R2,[R1, #+0]
        B        ??MRCC_XTDIV2Config_1
//  200   }
//  201   else
//  202   {
//  203     MRCC->CLKCTL &= MRCC_XTDIV2_Disable;
??MRCC_XTDIV2Config_0:
        LDR      R1,??DataTable58  ;; 0x60000020
        LDR      R1,[R1, #+0]
        LDR      R2,??MRCC_XTDIV2Config_2  ;; 0xffff7fff
        ANDS     R2,R2,R1
        LDR      R1,??DataTable58  ;; 0x60000020
        STR      R2,[R1, #+0]
//  204   }  
//  205 }
??MRCC_XTDIV2Config_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??MRCC_XTDIV2Config_2:
        DC32     0xffff7fff
        CFI EndBlock cfiBlock1
//  206 
//  207 /*******************************************************************************
//  208 * Function Name  : MRCC_CKSYSConfig
//  209 * Description    : Configures the system clock (CK_SYS).
//  210 * Input          : - MRCC_CKSYS: specifies the clock source used as system clock.
//  211 *                    This parameter can be one of the following values:
//  212 *                          - MRCC_CKSYS_FREEOSC
//  213 *                          - MRCC_CKSYS_OSC4M
//  214 *                          - MRCC_CKSYS_OSC4MPLL
//  215 *                          - MRCC_CKSYS_RTC (RTC clock source must be previously
//  216 *                            configured using MRCC_CKRTCConfig() function)
//  217 *                : - MRCC_PLL: specifies the PLL configuration.
//  218 *                    This parameter can be one of the following values:
//  219 *                          - MRCC_PLL_Disabled: PLL disabled
//  220 *                          - MRCC_PLL_NoChange: No change on PLL configuration
//  221 *                          - MRCC_PLL_Mul_12: Multiplication by 12
//  222 *                          - MRCC_PLL_Mul_14: Multiplication by 14
//  223 *                          - MRCC_PLL_Mul_15: Multiplication by 15
//  224 *                          - MRCC_PLL_Mul_16: Multiplication by 16
//  225 * Output         : None
//  226 * Return         : An ErrorStatus enumuration value:
//  227 *                         - SUCCESS: Clock configuration succeeded
//  228 *                         - ERROR: Clock configuration failed
//  229 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function MRCC_CKSYSConfig
        THUMB
//  230 ErrorStatus MRCC_CKSYSConfig(u32 MRCC_CKSYS, u32 MRCC_PLL)
//  231 {
MRCC_CKSYSConfig:
        PUSH     {R4-R6,LR}
        CFI R14 Frame(CFA, -4)
        CFI R6 Frame(CFA, -8)
        CFI R5 Frame(CFA, -12)
        CFI R4 Frame(CFA, -16)
        CFI CFA R13+16
        MOVS     R6,R0
        MOVS     R4,R1
//  232   ErrorStatus Status = ERROR;
        MOVS     R0,#+0
        MOVS     R5,R0
//  233 
//  234   switch(MRCC_CKSYS)
        MOVS     R0,R6
        SUBS     R0,R0,#+1
        BEQ      ??MRCC_CKSYSConfig_0
        SUBS     R0,R0,#+1
        BEQ      ??MRCC_CKSYSConfig_1
        SUBS     R0,R0,#+1
        BEQ      ??MRCC_CKSYSConfig_2
        SUBS     R0,R0,#+1
        BEQ      ??MRCC_CKSYSConfig_3
        B        ??MRCC_CKSYSConfig_4
//  235   {
//  236     case MRCC_CKSYS_FREEOSC:
//  237       if((MRCC_PLL == MRCC_PLL_Disabled) || (MRCC_PLL == MRCC_PLL_NoChange))
??MRCC_CKSYSConfig_0:
        LDR      R0,??DataTable36  ;; 0xfeffffff
        CMP      R4,R0
        BEQ      ??MRCC_CKSYSConfig_5
        CMP      R4,#+1
        BNE      ??MRCC_CKSYSConfig_6
//  238       {
//  239         Status = SetCKSYS_FREEOSC();
??MRCC_CKSYSConfig_5:
        BL       SetCKSYS_FREEOSC
        MOVS     R5,R0
        B        ??MRCC_CKSYSConfig_6
//  240       }
//  241       break;
//  242 
//  243     case MRCC_CKSYS_OSC4M:
//  244       if((MRCC_PLL == MRCC_PLL_Disabled) || (MRCC_PLL == MRCC_PLL_NoChange))
??MRCC_CKSYSConfig_1:
        LDR      R0,??DataTable36  ;; 0xfeffffff
        CMP      R4,R0
        BEQ      ??MRCC_CKSYSConfig_7
        CMP      R4,#+1
        BNE      ??MRCC_CKSYSConfig_6
//  245       {
//  246         Status = SetCKSYS_OSC4M(MRCC_PLL);
??MRCC_CKSYSConfig_7:
        MOVS     R0,R4
        BL       SetCKSYS_OSC4M
        MOVS     R5,R0
        B        ??MRCC_CKSYSConfig_6
//  247       }
//  248       break;
//  249 
//  250     case MRCC_CKSYS_OSC4MPLL:
//  251       if((MRCC_PLL == MRCC_PLL_Mul_12) || (MRCC_PLL == MRCC_PLL_Mul_14) ||
//  252          (MRCC_PLL == MRCC_PLL_Mul_15) || (MRCC_PLL == MRCC_PLL_Mul_16))
??MRCC_CKSYSConfig_2:
        MOVS     R0,#+192
        LSLS     R0,R0,#+21       ;; #+402653184
        CMP      R4,R0
        BEQ      ??MRCC_CKSYSConfig_8
        MOVS     R0,#+128
        LSLS     R0,R0,#+21       ;; #+268435456
        CMP      R4,R0
        BEQ      ??MRCC_CKSYSConfig_8
        MOVS     R0,#+128
        LSLS     R0,R0,#+20       ;; #+134217728
        CMP      R4,R0
        BEQ      ??MRCC_CKSYSConfig_8
        CMP      R4,#+0
        BNE      ??MRCC_CKSYSConfig_6
//  253       {
//  254         Status = SetCKSYS_OSC4MPLL(MRCC_PLL);
??MRCC_CKSYSConfig_8:
        MOVS     R0,R4
        BL       SetCKSYS_OSC4MPLL
        MOVS     R5,R0
        B        ??MRCC_CKSYSConfig_6
//  255       }
//  256       break;
//  257 
//  258     case MRCC_CKSYS_RTC:
//  259       if((MRCC_PLL == MRCC_PLL_Disabled) || (MRCC_PLL == MRCC_PLL_NoChange))
??MRCC_CKSYSConfig_3:
        LDR      R0,??DataTable36  ;; 0xfeffffff
        CMP      R4,R0
        BEQ      ??MRCC_CKSYSConfig_9
        CMP      R4,#+1
        BNE      ??MRCC_CKSYSConfig_6
//  260       {    
//  261         Status = SetCKSYS_RTC(MRCC_PLL);
??MRCC_CKSYSConfig_9:
        MOVS     R0,R4
        BL       SetCKSYS_RTC
        MOVS     R5,R0
        B        ??MRCC_CKSYSConfig_6
//  262       }
//  263       break;
//  264 
//  265     default:
//  266       Status = ERROR;
??MRCC_CKSYSConfig_4:
        MOVS     R0,#+0
        MOVS     R5,R0
//  267       break;
//  268   }
//  269   return Status;
??MRCC_CKSYSConfig_6:
        MOVS     R0,R5
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R4-R6}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock2
//  270 }

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable36:
        DC32     0xfeffffff
//  271 
//  272 /*******************************************************************************
//  273 * Function Name  : MRCC_HCLKConfig
//  274 * Description    : Configures the AHB clock (HCLK).
//  275 * Input          : - MRCC_HCLK: defines the AHB clock. This clock is derived
//  276 *                    from the system clock(CK_SYS).
//  277 *                    This parameter can be one of the following values:
//  278 *                          - MRCC_CKSYS_Div1: AHB clock = CK_SYS
//  279 *                          - MRCC_CKSYS_Div2: AHB clock = CK_SYS/2
//  280 *                          - MRCC_CKSYS_Div4: AHB clock = CK_SYS/4
//  281 *                          - MRCC_CKSYS_Div8: AHB clock = CK_SYS/8
//  282 * Output         : None
//  283 * Return         : None
//  284 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function MRCC_HCLKConfig
        THUMB
//  285 void MRCC_HCLKConfig(u32 MRCC_HCLK)
//  286 {
//  287   u32 Temp = 0;
MRCC_HCLKConfig:
        MOVS     R2,#+0
        MOVS     R1,R2
//  288   
//  289   /* Clear HPRESC[1:0] bits */
//  290   Temp = MRCC->CLKCTL & MRCC_HPRESC_Reset_Mask;
        LDR      R2,??DataTable58  ;; 0x60000020
        LDR      R2,[R2, #+0]
        MOVS     R3,#+24
        BICS     R2,R2,R3
        MOVS     R1,R2
//  291   
//  292   /* Set HPRESC[1:0] bits according to MRCC_HCLK value */
//  293   Temp |= MRCC_HCLK;
        ORRS     R1,R1,R0
//  294   
//  295   /* Store the new value */
//  296   MRCC->CLKCTL = Temp;  
        LDR      R2,??DataTable58  ;; 0x60000020
        STR      R1,[R2, #+0]
//  297 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock3
//  298 
//  299 /*******************************************************************************
//  300 * Function Name  : MRCC_CKTIMConfig
//  301 * Description    : Configures the TIM clock (CK_TIM).
//  302 * Input          : - MRCC_CKTIM: defines the TIM clock. This clock is derived
//  303 *                    from the AHB clock(HCLK).
//  304 *                    This parameter can be one of the following values:
//  305 *                          - MRCC_HCLK_Div1: TIM clock = HCLK
//  306 *                          - MRCC_HCLK_Div2: TIM clock = HCLK/2
//  307 *                          - MRCC_HCLK_Div4: TIM clock = HCLK/4
//  308 *                          - MRCC_HCLK_Div8: TIM clock = HCLK/8
//  309 * Output         : None
//  310 * Return         : None
//  311 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function MRCC_CKTIMConfig
        THUMB
//  312 void MRCC_CKTIMConfig(u32 MRCC_CKTIM)
//  313 {
//  314   u32 Temp = 0;
MRCC_CKTIMConfig:
        MOVS     R2,#+0
        MOVS     R1,R2
//  315   
//  316   /* Clear PPRESC[1:0] bits */
//  317   Temp = MRCC->CLKCTL & MRCC_PPRESC_Reset_Mask;
        LDR      R2,??DataTable58  ;; 0x60000020
        LDR      R2,[R2, #+0]
        MOVS     R3,#+3
        BICS     R2,R2,R3
        MOVS     R1,R2
//  318   
//  319   /* Set PPRESC[1:0] bits according to MRCC_CKTIM value */
//  320   Temp |= MRCC_CKTIM;
        ORRS     R1,R1,R0
//  321   
//  322   /* Store the new value */
//  323   MRCC->CLKCTL = Temp;
        LDR      R2,??DataTable58  ;; 0x60000020
        STR      R1,[R2, #+0]
//  324 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock4
//  325 
//  326 /*******************************************************************************
//  327 * Function Name  : MRCC_PCLKConfig
//  328 * Description    : Configures the APB clock (PCLK).
//  329 * Input          : - MRCC_PCLK: defines the APB clock. This clock is derived 
//  330 *                    from the TIM clock(CK_TIM).
//  331 *                    This parameter can be one of the following values:
//  332 *                          - MRCC_CKTIM_Div1: APB clock = CKTIM
//  333 *                          - MRCC_CKTIM_Div2: APB clock = CKTIM/2
//  334 * Output         : None
//  335 * Return         : None
//  336 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function MRCC_PCLKConfig
        THUMB
//  337 void MRCC_PCLKConfig(u32 MRCC_PCLK)
//  338 {
MRCC_PCLKConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  339   if(MRCC_PCLK == MRCC_CKTIM_Div2)
        CMP      R0,#+4
        BNE      ??MRCC_PCLKConfig_0
//  340   {
//  341     MRCC->CLKCTL |= MRCC_CKTIM_Div2;
        LDR      R1,??DataTable58  ;; 0x60000020
        LDR      R1,[R1, #+0]
        MOVS     R2,#+4
        ORRS     R2,R2,R1
        LDR      R1,??DataTable58  ;; 0x60000020
        STR      R2,[R1, #+0]
        B        ??MRCC_PCLKConfig_1
//  342   }
//  343   else
//  344   {
//  345     MRCC->CLKCTL &= MRCC_CKTIM_Div1;
??MRCC_PCLKConfig_0:
        LDR      R1,??DataTable58  ;; 0x60000020
        LDR      R1,[R1, #+0]
        MOVS     R2,#+4
        BICS     R1,R1,R2
        LDR      R2,??DataTable58  ;; 0x60000020
        STR      R1,[R2, #+0]
//  346   }
//  347 }
??MRCC_PCLKConfig_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock5
//  348 
//  349 /*******************************************************************************
//  350 * Function Name  : MRCC_CKRTCConfig
//  351 * Description    : Configures the RTC clock (CK_RTC).
//  352 * Input          : - MRCC_CKRTC: specifies the clock source to be used as RTC
//  353 *                    clock.
//  354 *                    This parameter can be one of the following values:
//  355 *                          - MRCC_CKRTC_OSC4M_Div128
//  356 *                          - MRCC_CKRTC_OSC32K (OSC32K must be previously enabled
//  357 *                            using MRCC_OSC32KConfig() function)
//  358 *                          - MRCC_CKRTC_LPOSC (LPOSC must be previously enabled
//  359 *                            using MRCC_LPOSCConfig() function)
//  360 * Output         : None
//  361 * Return         : An ErrorStatus enumuration value:
//  362 *                         - SUCCESS: Clock configuration succeeded
//  363 *                         - ERROR: Clock configuration failed
//  364 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function MRCC_CKRTCConfig
        THUMB
//  365 ErrorStatus MRCC_CKRTCConfig(u32 MRCC_CKRTC)
//  366 {
MRCC_CKRTCConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
//  367   u32 Tmp = 0;
        MOVS     R0,#+0
        MOVS     R2,R0
//  368 
//  369   if(((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET) &&
//  370      ((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) != RESET))
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BPL      ??MRCC_CKRTCConfig_0
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+8
        BPL      ??MRCC_CKRTCConfig_0
//  371   { 
//  372     /* CK_RTC used as CK_SYS clock source */
//  373     return ERROR;
        MOVS     R0,#+0
        B        ??MRCC_CKRTCConfig_1
//  374   }
//  375   else
//  376   {    
//  377     /* Clear CKRTCSEL[1:0] bits */
//  378     Tmp = MRCC->PWRCTRL & MRCC_CKRTCSEL_Reset_Mask;
??MRCC_CKRTCConfig_0:
        LDR      R0,??DataTable49  ;; 0x60000028
        LDR      R0,[R0, #+0]
        LDR      R3,??DataTable48  ;; 0xfcffffff
        ANDS     R3,R3,R0
        MOVS     R2,R3
//  379 
//  380     /* Set CKRTCSEL[1:0] bits according to MRCC_CKRTC value */
//  381     Tmp |= MRCC_CKRTC;
        ORRS     R2,R2,R1
//  382 
//  383     /* Store the new value */
//  384     MRCC->PWRCTRL = Tmp;       
        LDR      R0,??DataTable49  ;; 0x60000028
        STR      R2,[R0, #+0]
//  385   }
//  386 
//  387   return SUCCESS;
        MOVS     R0,#+1
??MRCC_CKRTCConfig_1:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock6
//  388 }

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable48:
        DC32     0xfcffffff

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable49:
        DC32     0x60000028
//  389 
//  390 /*******************************************************************************
//  391 * Function Name  : MRCC_CKUSBConfig
//  392 * Description    : Configures the USB clock(CK_USB).
//  393 * Input          : - MRCC_CKUSB: specifies the clock source to be used as USB
//  394 *                    clock.
//  395 *                    This parameter can be one of the following values:
//  396 *                          - MRCC_CKUSB_Internal(CK_PLL2 enabled)
//  397 *                          - MRCC_CKUSB_External(CK_PLL2 disabled)
//  398 * Output         : None
//  399 * Return         : An ErrorStatus enumuration value:
//  400 *                         - SUCCESS: Clock configuration succeeded
//  401 *                         - ERROR: Clock configuration failed
//  402 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function MRCC_CKUSBConfig
        THUMB
//  403 ErrorStatus MRCC_CKUSBConfig(u32 MRCC_CKUSB)
//  404 {
MRCC_CKUSBConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
//  405   if(MRCC_CKUSB == MRCC_CKUSB_External)
        MOVS     R0,#+128
        LSLS     R0,R0,#+15       ;; #+4194304
        CMP      R1,R0
        BNE      ??MRCC_CKUSBConfig_0
//  406   {
//  407     /* Disable CK_PLL2 */
//  408     MRCC->CLKCTL &= MRCC_PLL2EN_Reset_Mask;
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R2,??MRCC_CKUSBConfig_1  ;; 0xfdffffff
        ANDS     R2,R2,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R2,[R0, #+0]
//  409 
//  410     /* External USB clock selected */
//  411     MRCC->CLKCTL |= MRCC_CKUSB_External;
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+15       ;; #+4194304
        ORRS     R2,R2,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R2,[R0, #+0]
        B        ??MRCC_CKUSBConfig_2
//  412   }
//  413   else
//  414   {
//  415     if((MRCC->CLKCTL & MRCC_PLLEN_LOCK_Mask) != RESET)
??MRCC_CKUSBConfig_0:
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R2,#+129
        LSLS     R2,R2,#+24       ;; #-2130706432
        ANDS     R2,R2,R0
        CMP      R2,#+0
        BEQ      ??MRCC_CKUSBConfig_3
//  416     { /* PLL enabled and locked */
//  417       
//  418       /* Enable CK_PLL2 */
//  419       MRCC->CLKCTL |= MRCC_PLL2EN_Set_Mask;
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+18       ;; #+33554432
        ORRS     R2,R2,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R2,[R0, #+0]
//  420 
//  421       /* Internal USB clock selected */
//  422       MRCC->CLKCTL &= MRCC_CKUSB_Internal;
        LDR      R0,??DataTable58  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R2,??MRCC_CKUSBConfig_1+0x4  ;; 0xffbfffff
        ANDS     R2,R2,R0
        LDR      R0,??DataTable58  ;; 0x60000020
        STR      R2,[R0, #+0]
        B        ??MRCC_CKUSBConfig_2
//  423     }
//  424     else
//  425     {
//  426       /* PLL not enabled */
//  427       return ERROR;
??MRCC_CKUSBConfig_3:
        MOVS     R0,#+0
        B        ??MRCC_CKUSBConfig_4
//  428     }
//  429   }
//  430 
//  431   return SUCCESS;  
??MRCC_CKUSBConfig_2:
        MOVS     R0,#+1
??MRCC_CKUSBConfig_4:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??MRCC_CKUSBConfig_1:
        DC32     0xfdffffff
        DC32     0xffbfffff
        CFI EndBlock cfiBlock7
//  432 }

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable58:
        DC32     0x60000020
//  433 
//  434 /*******************************************************************************
//  435 * Function Name  : MRCC_ITConfig
//  436 * Description    : Enables or disables the specified MRCC interrupts.
//  437 * Input          : - MRCC_IT: specifies the MRCC interrupts sources to be
//  438 *                    enabled or disabled. This parameter can be any combination
//  439 *                    of the following values:
//  440 *                          - MRCC_IT_LOCK: PLL lock interrupt
//  441 *                          - MRCC_IT_NCKD: No Clock detected interrupt
//  442 *                  - NewState: new state of the MRCC interrupts.
//  443 *                    This parameter can be: ENABLE or DISABLE.
//  444 * Output         : None
//  445 * Return         : None
//  446 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function MRCC_ITConfig
        THUMB
//  447 void MRCC_ITConfig(u32 MRCC_IT, FunctionalState NewState)
//  448 {
MRCC_ITConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  449   if(NewState == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??MRCC_ITConfig_0
//  450   {
//  451     MRCC->CLKCTL |= MRCC_IT;
        LDR      R2,??MRCC_ITConfig_1  ;; 0x60000020
        LDR      R2,[R2, #+0]
        ORRS     R2,R2,R0
        LDR      R3,??MRCC_ITConfig_1  ;; 0x60000020
        STR      R2,[R3, #+0]
        B        ??MRCC_ITConfig_2
//  452   }
//  453   else
//  454   {
//  455     MRCC->CLKCTL &= ~MRCC_IT;
??MRCC_ITConfig_0:
        LDR      R2,??MRCC_ITConfig_1  ;; 0x60000020
        LDR      R2,[R2, #+0]
        BICS     R2,R2,R0
        LDR      R3,??MRCC_ITConfig_1  ;; 0x60000020
        STR      R2,[R3, #+0]
//  456   }
//  457 }
??MRCC_ITConfig_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??MRCC_ITConfig_1:
        DC32     0x60000020
        CFI EndBlock cfiBlock8
//  458 
//  459 /*******************************************************************************
//  460 * Function Name  : MRCC_PeripheralClockConfig
//  461 * Description    : Enables or disables the specified peripheral clock.
//  462 * Input          : - MRCC_Peripheral: specifies the peripheral to gates its
//  463 *                    clock. More than one peripheral can be selected using
//  464 *                    the “|” operator.
//  465 *                  - NewState: new state of the specified peripheral clock.
//  466 *                    This parameter can be one of the following values:
//  467 *                          - ENABLE: the selected peripheral clock is enabled
//  468 *                          - DISABLE: the selected peripheral clock is disabled
//  469 * Output         : None
//  470 * Return         : None
//  471 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function MRCC_PeripheralClockConfig
        THUMB
//  472 void MRCC_PeripheralClockConfig(u32 MRCC_Peripheral, FunctionalState NewState)
//  473 {
MRCC_PeripheralClockConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  474   if(NewState == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??MRCC_PeripheralClockConfig_0
//  475   {
//  476     MRCC->PCLKEN |= MRCC_Peripheral;
        LDR      R2,??MRCC_PeripheralClockConfig_1  ;; 0x60000030
        LDR      R2,[R2, #+0]
        ORRS     R2,R2,R0
        LDR      R3,??MRCC_PeripheralClockConfig_1  ;; 0x60000030
        STR      R2,[R3, #+0]
        B        ??MRCC_PeripheralClockConfig_2
//  477   }
//  478   else
//  479   {
//  480     MRCC->PCLKEN &= ~MRCC_Peripheral;
??MRCC_PeripheralClockConfig_0:
        LDR      R2,??MRCC_PeripheralClockConfig_1  ;; 0x60000030
        LDR      R2,[R2, #+0]
        BICS     R2,R2,R0
        LDR      R3,??MRCC_PeripheralClockConfig_1  ;; 0x60000030
        STR      R2,[R3, #+0]
//  481   }
//  482 }
??MRCC_PeripheralClockConfig_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??MRCC_PeripheralClockConfig_1:
        DC32     0x60000030
        CFI EndBlock cfiBlock9
//  483 
//  484 /*******************************************************************************
//  485 * Function Name  : MRCC_PeripheralSWResetConfig
//  486 * Description    : Forces or releases peripheral software reset.
//  487 * Input          : - MRCC_Peripheral: specifies the peripheral to reset. More
//  488 *                    than one peripheral can be selected using the “|” operator.
//  489 *                  - NewState: new state of the specified peripheral software
//  490 *                    reset. This parameter can be one of the following values:
//  491 *                          - ENABLE: the selected peripheral is kept under reset
//  492 *                          - DISABLE: the selected peripheral exits from reset
//  493 * Output         : None
//  494 * Return         : None
//  495 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon0
        CFI Function MRCC_PeripheralSWResetConfig
        THUMB
//  496 void MRCC_PeripheralSWResetConfig(u32 MRCC_Peripheral, FunctionalState NewState)
//  497 {
MRCC_PeripheralSWResetConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  498   if(NewState == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??MRCC_PeripheralSWResetConfig_0
//  499   {
//  500     MRCC->PSWRES |= MRCC_Peripheral;
        LDR      R2,??MRCC_PeripheralSWResetConfig_1  ;; 0x60000034
        LDR      R2,[R2, #+0]
        ORRS     R2,R2,R0
        LDR      R3,??MRCC_PeripheralSWResetConfig_1  ;; 0x60000034
        STR      R2,[R3, #+0]
        B        ??MRCC_PeripheralSWResetConfig_2
//  501   }
//  502   else
//  503   {
//  504     MRCC->PSWRES &= ~MRCC_Peripheral;
??MRCC_PeripheralSWResetConfig_0:
        LDR      R2,??MRCC_PeripheralSWResetConfig_1  ;; 0x60000034
        LDR      R2,[R2, #+0]
        BICS     R2,R2,R0
        LDR      R3,??MRCC_PeripheralSWResetConfig_1  ;; 0x60000034
        STR      R2,[R3, #+0]
//  505   }
//  506 }
??MRCC_PeripheralSWResetConfig_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??MRCC_PeripheralSWResetConfig_1:
        DC32     0x60000034
        CFI EndBlock cfiBlock10
//  507 
//  508 /*******************************************************************************
//  509 * Function Name  : MRCC_GetClocksStatus
//  510 * Description    : Returns the status and frequencies of different on chip clocks.
//  511 *                  Don’t use this function when CK_SYS is clocked by an external
//  512 *                  clock source (OSC4M bypassed).
//  513 * Input          : - MRCC_ClocksStatus: pointer to a MRCC_ClocksTypeDef structure
//  514 *                    which will hold the clocks information.
//  515 * Output         : None
//  516 * Return         : None
//  517 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon0
        CFI Function MRCC_GetClocksStatus
        THUMB
//  518 void MRCC_GetClocksStatus(MRCC_ClocksTypeDef*  MRCC_ClocksStatus)
//  519 {
MRCC_GetClocksStatus:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
//  520   u32 PLLMul = 0;
        MOVS     R4,#+0
        MOVS     R1,R4
//  521   u32 Temp = 0;
        MOVS     R4,#+0
        MOVS     R2,R4
//  522   u32 Presc = 0;
        MOVS     R4,#+0
        MOVS     R3,R4
//  523 
//  524   /* Get the Status of PLL */
//  525   if((MRCC->CLKCTL & MRCC_PLLEN_Set_Mask) == RESET)  
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        LSLS     R4,R4,#+7
        BMI      ??MRCC_GetClocksStatus_1
//  526   {
//  527     MRCC_ClocksStatus->PLL_Status = OFF;
        MOVS     R4,#+11
        STRB     R4,[R0, #+3]
        B        ??MRCC_GetClocksStatus_2
//  528   }
//  529   else
//  530   {
//  531     MRCC_ClocksStatus->PLL_Status = ON;
??MRCC_GetClocksStatus_1:
        MOVS     R4,#+10
        STRB     R4,[R0, #+3]
//  532   }
//  533   
//  534   /* Get the Status of OSC4M */
//  535   if((MRCC->CLKCTL & MRCC_OSC4MOFF_Set_Mask) == RESET)  
??MRCC_GetClocksStatus_2:
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        LSLS     R4,R4,#+14
        BMI      ??MRCC_GetClocksStatus_3
//  536   {
//  537     MRCC_ClocksStatus->OSC4M_Status = ON;
        MOVS     R4,#+10
        STRB     R4,[R0, #+4]
        B        ??MRCC_GetClocksStatus_4
//  538   }
//  539   else
//  540   {
//  541     MRCC_ClocksStatus->OSC4M_Status = OFF;
??MRCC_GetClocksStatus_3:
        MOVS     R4,#+11
        STRB     R4,[R0, #+4]
//  542   }  
//  543   
//  544   /* Get the Status of LPOSC */
//  545   if((MRCC->PWRCTRL & MRCC_LPOSCEN_Mask) == RESET)  
??MRCC_GetClocksStatus_4:
        LDR      R4,??MRCC_GetClocksStatus_0+0x4  ;; 0x60000028
        LDR      R4,[R4, #+0]
        LSLS     R4,R4,#+3
        BMI      ??MRCC_GetClocksStatus_5
//  546   {
//  547     MRCC_ClocksStatus->LPOSC_Status = OFF;
        MOVS     R4,#+11
        STRB     R4,[R0, #+5]
        B        ??MRCC_GetClocksStatus_6
//  548   }
//  549   else
//  550   {
//  551     MRCC_ClocksStatus->LPOSC_Status = ON;
??MRCC_GetClocksStatus_5:
        MOVS     R4,#+10
        STRB     R4,[R0, #+5]
//  552   }  
//  553   
//  554   /* Get the Status of OSC32K */
//  555   if((MRCC->PWRCTRL & MRCC_OSC32KEN_Mask) == RESET)  
??MRCC_GetClocksStatus_6:
        LDR      R4,??MRCC_GetClocksStatus_0+0x4  ;; 0x60000028
        LDR      R4,[R4, #+0]
        LSLS     R4,R4,#+2
        BMI      ??MRCC_GetClocksStatus_7
//  556   {
//  557     MRCC_ClocksStatus->OSC32K_Status = OFF;
        MOVS     R4,#+11
        STRB     R4,[R0, #+6]
        B        ??MRCC_GetClocksStatus_8
//  558   }
//  559   else
//  560   {
//  561     MRCC_ClocksStatus->OSC32K_Status = ON;
??MRCC_GetClocksStatus_7:
        MOVS     R4,#+10
        STRB     R4,[R0, #+6]
//  562   } 
//  563     
//  564 /* Get CKU_SB source ---------------------------------------------------------*/  
//  565   if((MRCC->CLKCTL & MRCC_CKUSBSEL_Mask) != RESET)
??MRCC_GetClocksStatus_8:
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        LSLS     R4,R4,#+9
        BPL      ??MRCC_GetClocksStatus_9
//  566   {
//  567     MRCC_ClocksStatus->CKUSB_Source = External;    
        MOVS     R4,#+9
        STRB     R4,[R0, #+2]
        B        ??MRCC_GetClocksStatus_10
//  568   }
//  569   else
//  570   {  
//  571     if((MRCC->CLKCTL & MRCC_PLL2EN_Set_Mask) != RESET)
??MRCC_GetClocksStatus_9:
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        LSLS     R4,R4,#+6
        BPL      ??MRCC_GetClocksStatus_11
//  572     {
//  573       MRCC_ClocksStatus->CKUSB_Source = Internal;
        MOVS     R4,#+8
        STRB     R4,[R0, #+2]
        B        ??MRCC_GetClocksStatus_10
//  574     
//  575     }
//  576     else 
//  577     {
//  578       MRCC_ClocksStatus->CKUSB_Source = Disabled;    
??MRCC_GetClocksStatus_11:
        MOVS     R4,#+4
        STRB     R4,[R0, #+2]
//  579     }
//  580   }
//  581 
//  582 /* Get CK_RTC source ---------------------------------------------------------*/ 
//  583   Temp = MRCC->PWRCTRL & MRCC_CKRTCSEL_Set_Mask;
??MRCC_GetClocksStatus_10:
        LDR      R4,??MRCC_GetClocksStatus_0+0x4  ;; 0x60000028
        LDR      R4,[R4, #+0]
        MOVS     R5,#+192
        LSLS     R5,R5,#+18       ;; #+50331648
        ANDS     R5,R5,R4
        MOVS     R2,R5
//  584   Temp = Temp >> 24;
        LSRS     R2,R2,#+24
//  585     
//  586   switch(Temp)
        MOVS     R4,R2
        CMP      R4,#+0
        BEQ      ??MRCC_GetClocksStatus_12
        SUBS     R4,R4,#+1
        BEQ      ??MRCC_GetClocksStatus_13
        SUBS     R4,R4,#+1
        BEQ      ??MRCC_GetClocksStatus_14
        SUBS     R4,R4,#+1
        BEQ      ??MRCC_GetClocksStatus_15
        B        ??MRCC_GetClocksStatus_16
//  587   {
//  588     case 0x00:
//  589       MRCC_ClocksStatus->CKRTC_Source = Disabled;
??MRCC_GetClocksStatus_12:
        MOVS     R4,#+4
        STRB     R4,[R0, #+1]
        B        ??MRCC_GetClocksStatus_17
//  590       break;
//  591 
//  592     case 0x01:
//  593       MRCC_ClocksStatus->CKRTC_Source = OSC4M_Div128;
??MRCC_GetClocksStatus_13:
        MOVS     R4,#+5
        STRB     R4,[R0, #+1]
        B        ??MRCC_GetClocksStatus_17
//  594       break;
//  595 
//  596     case 0x02:
//  597       MRCC_ClocksStatus->CKRTC_Source = OSC32K;
??MRCC_GetClocksStatus_14:
        MOVS     R4,#+7
        STRB     R4,[R0, #+1]
        B        ??MRCC_GetClocksStatus_17
//  598       break;
//  599         
//  600     case 0x03:
//  601       MRCC_ClocksStatus->CKRTC_Source = LPOSC;              
??MRCC_GetClocksStatus_15:
        MOVS     R4,#+6
        STRB     R4,[R0, #+1]
        B        ??MRCC_GetClocksStatus_17
//  602       break;
//  603         
//  604     default:
//  605       MRCC_ClocksStatus->CKRTC_Source = Disabled;
??MRCC_GetClocksStatus_16:
        MOVS     R4,#+4
        STRB     R4,[R0, #+1]
//  606       break;          
//  607   }
//  608       
//  609 /* Get CK_SYS source ---------------------------------------------------------*/   
//  610   if((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) != RESET)
??MRCC_GetClocksStatus_17:
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        LSLS     R4,R4,#+8
        BPL      ??MRCC_GetClocksStatus_18
//  611   {/* CK_OSC used as CK_SYS clock source */
//  612     
//  613     if((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET)
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        LSLS     R4,R4,#+10
        BPL      ??MRCC_GetClocksStatus_19
//  614     { /* CK_RTC used as CK_OSC clock source */
//  615       MRCC_ClocksStatus->CKSYS_Source = CKRTC;
        MOVS     R4,#+3
        STRB     R4,[R0, #+0]
//  616       
//  617       if(MRCC_ClocksStatus->CKRTC_Source == OSC32K)
        LDRB     R4,[R0, #+1]
        CMP      R4,#+7
        BNE      ??MRCC_GetClocksStatus_20
//  618       {
//  619         /* CK_SYS clock frequency */
//  620         MRCC_ClocksStatus->CKSYS_Frequency = OSC32K_Value;         
        MOVS     R4,#+128
        LSLS     R4,R4,#+8        ;; #+32768
        STR      R4,[R0, #+8]
        B        ??MRCC_GetClocksStatus_21
//  621       }         
//  622       else if(MRCC_ClocksStatus->CKRTC_Source == LPOSC)
??MRCC_GetClocksStatus_20:
        LDRB     R4,[R0, #+1]
        CMP      R4,#+6
        BNE      ??MRCC_GetClocksStatus_22
//  623 
//  624       {
//  625         /* CK_SYS clock frequency */
//  626         MRCC_ClocksStatus->CKSYS_Frequency = LPOSC_Value;             
        LDR      R4,??MRCC_GetClocksStatus_0+0x8  ;; 0x3bd08
        STR      R4,[R0, #+8]
        B        ??MRCC_GetClocksStatus_21
//  627       }
//  628       else if(MRCC_ClocksStatus->CKRTC_Source == OSC4M_Div128)
??MRCC_GetClocksStatus_22:
        LDRB     R4,[R0, #+1]
        CMP      R4,#+5
        BNE      ??MRCC_GetClocksStatus_21
//  629 
//  630       {
//  631         /* CK_SYS clock frequency */
//  632         MRCC_ClocksStatus->CKSYS_Frequency = OSC4M_Div128_Value;             
        LDR      R4,??MRCC_GetClocksStatus_0+0xC  ;; 0x7a12
        STR      R4,[R0, #+8]
        B        ??MRCC_GetClocksStatus_21
//  633       }
//  634     }
//  635     else
//  636     { /* OSC4M used as CK_OSC clock source */
//  637       MRCC_ClocksStatus->CKSYS_Source = OSC4M; 
??MRCC_GetClocksStatus_19:
        MOVS     R4,#+2
        STRB     R4,[R0, #+0]
//  638     
//  639       if((MRCC->CLKCTL & MRCC_XTDIV2_Set_Mask) != RESET)
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        LSLS     R4,R4,#+16
        BPL      ??MRCC_GetClocksStatus_23
//  640       {
//  641         /* CK_SYS clock frequency */
//  642         MRCC_ClocksStatus->CKSYS_Frequency = Main_Oscillator >> 1;
        LDR      R4,??MRCC_GetClocksStatus_0+0x10  ;; 0x3d0900
        STR      R4,[R0, #+8]
        B        ??MRCC_GetClocksStatus_21
//  643       }
//  644       else
//  645       {
//  646         /* CK_SYS clock frequency */
//  647         MRCC_ClocksStatus->CKSYS_Frequency = Main_Oscillator;
??MRCC_GetClocksStatus_23:
        LDR      R4,??MRCC_GetClocksStatus_0+0x14  ;; 0x7a1200
        STR      R4,[R0, #+8]
        B        ??MRCC_GetClocksStatus_21
//  648       }          
//  649     }
//  650   }     
//  651   else
//  652   {/* CK_PLL1 used as CK_SYS clock */
//  653     
//  654     if(MRCC_ClocksStatus->PLL_Status == OFF)
??MRCC_GetClocksStatus_18:
        LDRB     R4,[R0, #+3]
        CMP      R4,#+11
        BNE      ??MRCC_GetClocksStatus_24
//  655     { /* FREEOSC used as CK_PLL1 clock source */
//  656       MRCC_ClocksStatus->CKSYS_Source = FREEOSC; 
        MOVS     R4,#+0
        STRB     R4,[R0, #+0]
//  657       
//  658       /* CK_SYS clock frequency */
//  659       MRCC_ClocksStatus->CKSYS_Frequency = FREEOSC_Value;               
        LDR      R4,??MRCC_GetClocksStatus_0+0x18  ;; 0x4c4b40
        STR      R4,[R0, #+8]
        B        ??MRCC_GetClocksStatus_21
//  660     }
//  661     else
//  662     { /* OSC4M followed by PLL used as CK_PLL1 clock source */
//  663       MRCC_ClocksStatus->CKSYS_Source = OSC4MPLL;
??MRCC_GetClocksStatus_24:
        MOVS     R4,#+1
        STRB     R4,[R0, #+0]
//  664                     
//  665       /* Get PLL factor ------------------------------------------------------*/
//  666       Temp = MRCC->CLKCTL & MRCC_MX_Set_Mask;
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        MOVS     R5,#+192
        LSLS     R5,R5,#+21       ;; #+402653184
        ANDS     R5,R5,R4
        MOVS     R2,R5
//  667       Temp = Temp >> 27;
        LSRS     R2,R2,#+27
//  668     
//  669       switch(Temp)
        MOVS     R4,R2
        CMP      R4,#+0
        BEQ      ??MRCC_GetClocksStatus_25
        SUBS     R4,R4,#+1
        BEQ      ??MRCC_GetClocksStatus_26
        SUBS     R4,R4,#+1
        BEQ      ??MRCC_GetClocksStatus_27
        SUBS     R4,R4,#+1
        BEQ      ??MRCC_GetClocksStatus_28
        B        ??MRCC_GetClocksStatus_29
//  670       {
//  671         case 0x00:
//  672           PLLMul = 16;
??MRCC_GetClocksStatus_25:
        MOVS     R4,#+16
        MOVS     R1,R4
        B        ??MRCC_GetClocksStatus_30
//  673           break;
//  674 
//  675         case 0x01:
//  676           PLLMul = 15;
??MRCC_GetClocksStatus_26:
        MOVS     R4,#+15
        MOVS     R1,R4
        B        ??MRCC_GetClocksStatus_30
//  677           break;
//  678 
//  679         case 0x02:
//  680           PLLMul = 14;
??MRCC_GetClocksStatus_27:
        MOVS     R4,#+14
        MOVS     R1,R4
        B        ??MRCC_GetClocksStatus_30
//  681           break;
//  682         
//  683         case 0x03:
//  684           PLLMul = 12;
??MRCC_GetClocksStatus_28:
        MOVS     R4,#+12
        MOVS     R1,R4
        B        ??MRCC_GetClocksStatus_30
//  685           break;
//  686         
//  687         default:
//  688           PLLMul = 16;
??MRCC_GetClocksStatus_29:
        MOVS     R4,#+16
        MOVS     R1,R4
//  689           break;          
//  690       } 
//  691       
//  692       /* CK_SYS clock frequency */
//  693       MRCC_ClocksStatus->CKSYS_Frequency = OSC4M_Value * PLLMul;     
??MRCC_GetClocksStatus_30:
        LDR      R4,??MRCC_GetClocksStatus_0+0x10  ;; 0x3d0900
        MULS     R4,R1,R4
        STR      R4,[R0, #+8]
//  694     }
//  695   }
//  696 
//  697 /* Compute HCLK, CKTIM and PCLK clocks frequencies ---------------------------*/    
//  698   /* Get HCLK prescaler */
//  699   Presc = MRCC->CLKCTL & MRCC_HPRESC_Set_Mask;
??MRCC_GetClocksStatus_21:
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        MOVS     R5,#+24
        ANDS     R5,R5,R4
        MOVS     R3,R5
//  700   Presc = Presc >> 3;
        LSRS     R3,R3,#+3
//  701   /* HCLK clock frequency */
//  702   MRCC_ClocksStatus->HCLK_Frequency = MRCC_ClocksStatus->CKSYS_Frequency >> Presc;
        LDR      R4,[R0, #+8]
        LSRS     R4,R4,R3
        STR      R4,[R0, #+12]
//  703 
//  704   /* Get CK_TIM prescaler */
//  705   Presc = MRCC->CLKCTL & MRCC_PPRESC_Set_Mask;
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        LSLS     R4,R4,#+30       ;; ZeroExtS R4,R4,#+30,#+30
        LSRS     R4,R4,#+30
        MOVS     R3,R4
//  706   /* CK_TIM clock frequency */
//  707   MRCC_ClocksStatus->CKTIM_Frequency = MRCC_ClocksStatus->HCLK_Frequency >> Presc;
        LDR      R4,[R0, #+12]
        LSRS     R4,R4,R3
        STR      R4,[R0, #+16]
//  708  
//  709   /* Get PCLK prescaler */
//  710   Presc = MRCC->CLKCTL & MRCC_PPRESC2_Mask;
        LDR      R4,??MRCC_GetClocksStatus_0  ;; 0x60000020
        LDR      R4,[R4, #+0]
        MOVS     R5,#+4
        ANDS     R5,R5,R4
        MOVS     R3,R5
//  711   Presc = Presc >> 2;
        LSRS     R3,R3,#+2
//  712   /* PCLK clock frequency */
//  713   MRCC_ClocksStatus->PCLK_Frequency = MRCC_ClocksStatus->CKTIM_Frequency >> Presc;
        LDR      R4,[R0, #+16]
        LSRS     R4,R4,R3
        STR      R4,[R0, #+20]
//  714 }
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        DATA
??MRCC_GetClocksStatus_0:
        DC32     0x60000020
        DC32     0x60000028
        DC32     0x3bd08
        DC32     0x7a12
        DC32     0x3d0900
        DC32     0x7a1200
        DC32     0x4c4b40
        CFI EndBlock cfiBlock11
//  715 
//  716 /*******************************************************************************
//  717 * Function Name  : MRCC_LPMC_DBGonfig
//  718 * Description    : Enables or disables the Low Power Debug Mode.
//  719 * Input          : - MRCC_LPDM: specifies the LPDM new state value.
//  720 *                    This parameter can be one of the following values:
//  721 *                          - MRCC_LPDM_Disable
//  722 *                          - MRCC_LPDM_Enable
//  723 * Output         : None
//  724 * Return         : None
//  725 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock12 Using cfiCommon0
        CFI Function MRCC_LPMC_DBGConfig
        THUMB
//  726 void MRCC_LPMC_DBGConfig(u32 MRCC_LPDM)
//  727 {
MRCC_LPMC_DBGConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  728   if(MRCC_LPDM == MRCC_LPDM_Enable)
        CMP      R0,#+8
        BNE      ??MRCC_LPMC_DBGConfig_0
//  729   {
//  730     MRCC->PWRCTRL |= MRCC_LPDM_Enable;
        LDR      R1,??MRCC_LPMC_DBGConfig_1  ;; 0x60000028
        LDR      R1,[R1, #+0]
        MOVS     R2,#+8
        ORRS     R2,R2,R1
        LDR      R1,??MRCC_LPMC_DBGConfig_1  ;; 0x60000028
        STR      R2,[R1, #+0]
        B        ??MRCC_LPMC_DBGConfig_2
//  731   }
//  732   else
//  733   {
//  734     MRCC->PWRCTRL &= MRCC_LPDM_Disable;
??MRCC_LPMC_DBGConfig_0:
        LDR      R1,??MRCC_LPMC_DBGConfig_1  ;; 0x60000028
        LDR      R1,[R1, #+0]
        MOVS     R2,#+8
        BICS     R1,R1,R2
        LDR      R2,??MRCC_LPMC_DBGConfig_1  ;; 0x60000028
        STR      R1,[R2, #+0]
//  735   }
//  736 }
??MRCC_LPMC_DBGConfig_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??MRCC_LPMC_DBGConfig_1:
        DC32     0x60000028
        CFI EndBlock cfiBlock12
//  737 
//  738 /*******************************************************************************
//  739 * Function Name  : MRCC_EnterWFIMode
//  740 * Description    : Enters WFI mode.
//  741 *                  If the Flash is used in Burst mode, it must be kept enabled
//  742 *                  in WFI mode(use MRCC_WFIParam_FLASHOn as parameter)
//  743 * Input          : - MRCC_WFIParam: specifies the WFI mode control parameters.
//  744 *                    This parameter can be one of the following values:
//  745 *                          - MRCC_WFIParam_FLASHPowerDown(DMA not allowed during WFI)
//  746 *                          - MRCC_WFIParam_FLASHOn(DMA allowed during WFI)
//  747 *                          - MRCC_WFIParam_FLASHOff(DMA not allowed during WFI)
//  748 * Output         : None
//  749 * Return         : None
//  750 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock13 Using cfiCommon0
        CFI Function MRCC_EnterWFIMode
        THUMB
//  751 void MRCC_EnterWFIMode(u32 MRCC_WFIParam)
//  752 {
MRCC_EnterWFIMode:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//  753 /* Low Power mode configuration ----------------------------------------------*/
//  754   /* Clear LPMC[1:0] bits */
//  755    MRCC->PWRCTRL &= MRCC_LPMC_Reset_Mask;
        LDR      R0,??MRCC_EnterWFIMode_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R1,#+6
        BICS     R0,R0,R1
        LDR      R1,??MRCC_EnterWFIMode_0  ;; 0x60000028
        STR      R0,[R1, #+0]
//  756 
//  757   /* Select WFI mode */
//  758   MRCC->PWRCTRL |= MRCC_WFI_Mask;
        LDR      R0,??MRCC_EnterWFIMode_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R1,#+4
        ORRS     R1,R1,R0
        LDR      R0,??MRCC_EnterWFIMode_0  ;; 0x60000028
        STR      R1,[R0, #+0]
//  759 
//  760 /* Low Power mode control parameters configuration ---------------------------*/
//  761   /* Clear LP_PARAM[15:13] and WFI_FLASH_EN bits */
//  762   MRCC->PWRCTRL &= MRCC_WFIParam_Reset_Mask;
        LDR      R0,??MRCC_EnterWFIMode_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        LDR      R1,??MRCC_EnterWFIMode_0+0x4  ;; 0xffff1fef
        ANDS     R1,R1,R0
        LDR      R0,??MRCC_EnterWFIMode_0  ;; 0x60000028
        STR      R1,[R0, #+0]
//  763   
//  764   if(MRCC_WFIParam != MRCC_WFIParam_FLASHPowerDown)
        CMP      R4,#+0
        BEQ      ??MRCC_EnterWFIMode_1
//  765   {
//  766     /* Set LP_PARAM[15:13] and WFI_FLASH_EN bits according to MRCC_WFIParam value */
//  767     MRCC->PWRCTRL |= MRCC_WFIParam;
        LDR      R0,??MRCC_EnterWFIMode_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        ORRS     R0,R0,R4
        LDR      R1,??MRCC_EnterWFIMode_0  ;; 0x60000028
        STR      R0,[R1, #+0]
//  768   }
//  769     
//  770 /* Execute the Low Power bit writing sequence --------------------------------*/
//  771   WriteLPBit();
??MRCC_EnterWFIMode_1:
        BL       WriteLPBit
//  772 }
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        DATA
??MRCC_EnterWFIMode_0:
        DC32     0x60000028
        DC32     0xffff1fef
        CFI EndBlock cfiBlock13
//  773 
//  774 /*******************************************************************************
//  775 * Function Name  : MRCC_EnterSTOPMode
//  776 * Description    : Enters STOP mode.
//  777 * Input          : - MRCC_STOPParam: specifies the STOP mode control parameters.
//  778 *                    This parameter can be one of the following values:
//  779 *                          - MRCC_STOPParam_Default (OSC4M On, FLASH On, MVREG On)
//  780 *                          - MRCC_STOPParam_OSC4MOff
//  781 *                          - MRCC_STOPParam_FLASHOff
//  782 *                          - MRCC_STOPParam_MVREGOff
//  783 * Output         : None
//  784 * Return         : None
//  785 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock14 Using cfiCommon0
        CFI Function MRCC_EnterSTOPMode
        THUMB
//  786 void MRCC_EnterSTOPMode(u32 MRCC_STOPParam)
//  787 {
MRCC_EnterSTOPMode:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//  788 /* Low Power mode configuration ----------------------------------------------*/
//  789   /* Clear LPMC[1:0] bits (STOP mode is selected) */
//  790    MRCC->PWRCTRL &= MRCC_LPMC_Reset_Mask;
        LDR      R0,??MRCC_EnterSTOPMode_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R1,#+6
        BICS     R0,R0,R1
        LDR      R1,??MRCC_EnterSTOPMode_0  ;; 0x60000028
        STR      R0,[R1, #+0]
//  791 
//  792 /* Low Power mode control parameters configuration ---------------------------*/
//  793   /* Clear LP_PARAM[15:13] bits */
//  794   MRCC->PWRCTRL &= MRCC_LPPARAM_Reset_Mask;
        LDR      R0,??MRCC_EnterSTOPMode_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        LDR      R1,??MRCC_EnterSTOPMode_0+0x4  ;; 0xffff1fff
        ANDS     R1,R1,R0
        LDR      R0,??MRCC_EnterSTOPMode_0  ;; 0x60000028
        STR      R1,[R0, #+0]
//  795   
//  796   if(MRCC_STOPParam != MRCC_STOPParam_Default)
        CMP      R4,#+0
        BEQ      ??MRCC_EnterSTOPMode_1
//  797   {
//  798     /* Set LP_PARAM[15:13] bits according to MRCC_STOPParam value */
//  799     MRCC->PWRCTRL |= MRCC_STOPParam;
        LDR      R0,??MRCC_EnterSTOPMode_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        ORRS     R0,R0,R4
        LDR      R1,??MRCC_EnterSTOPMode_0  ;; 0x60000028
        STR      R0,[R1, #+0]
//  800   }
//  801 
//  802 /* Execute the Low Power bit writing sequence --------------------------------*/
//  803   WriteLPBit();
??MRCC_EnterSTOPMode_1:
        BL       WriteLPBit
//  804 }
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        DATA
??MRCC_EnterSTOPMode_0:
        DC32     0x60000028
        DC32     0xffff1fff
        CFI EndBlock cfiBlock14
//  805 
//  806 /*******************************************************************************
//  807 * Function Name  : MRCC_EnterSTANDBYMode
//  808 * Description    : Enters STANDBY mode.
//  809 *                  Make sure that WKPF flag is cleared before using this function.
//  810 * Input          : None
//  811 * Output         : None
//  812 * Return         : None
//  813 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock15 Using cfiCommon0
        CFI Function MRCC_EnterSTANDBYMode
        THUMB
//  814 void MRCC_EnterSTANDBYMode(void)
//  815 {
MRCC_EnterSTANDBYMode:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  816 /* Low Power mode configuration ----------------------------------------------*/
//  817   /* Clear LPMC[1:0] bits */
//  818    MRCC->PWRCTRL &= MRCC_LPMC_Reset_Mask;
        LDR      R0,??MRCC_EnterSTANDBYMode_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R1,#+6
        BICS     R0,R0,R1
        LDR      R1,??MRCC_EnterSTANDBYMode_0  ;; 0x60000028
        STR      R0,[R1, #+0]
//  819 
//  820   /* Select STANDBY mode */
//  821   MRCC->PWRCTRL |= MRCC_STANDBY_Mask;
        LDR      R0,??MRCC_EnterSTANDBYMode_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R1,#+6
        ORRS     R1,R1,R0
        LDR      R0,??MRCC_EnterSTANDBYMode_0  ;; 0x60000028
        STR      R1,[R0, #+0]
//  822 
//  823 /* Execute the Low Power bit writing sequence --------------------------------*/
//  824   WriteLPBit();
        BL       WriteLPBit
//  825 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??MRCC_EnterSTANDBYMode_0:
        DC32     0x60000028
        CFI EndBlock cfiBlock15
//  826 
//  827 /*******************************************************************************
//  828 * Function Name  : MRCC_GenerateSWReset
//  829 * Description    : Generates a system software reset.
//  830 * Input          : None
//  831 * Output         : None
//  832 * Return         : None
//  833 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock16 Using cfiCommon0
        CFI Function MRCC_GenerateSWReset
        THUMB
//  834 void MRCC_GenerateSWReset(void)
//  835 {
MRCC_GenerateSWReset:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  836 /* Low Power mode configuration ----------------------------------------------*/
//  837   /* Clear LPMC[1:0] bits */
//  838   MRCC->PWRCTRL &= MRCC_LPMC_Reset_Mask;
        LDR      R0,??MRCC_GenerateSWReset_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R1,#+6
        BICS     R0,R0,R1
        LDR      R1,??MRCC_GenerateSWReset_0  ;; 0x60000028
        STR      R0,[R1, #+0]
//  839 
//  840   /* Select software reset */
//  841   MRCC->PWRCTRL |= MRCC_SWRESET_Mask;
        LDR      R0,??MRCC_GenerateSWReset_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R1,#+2
        ORRS     R1,R1,R0
        LDR      R0,??MRCC_GenerateSWReset_0  ;; 0x60000028
        STR      R1,[R0, #+0]
//  842 
//  843 /* Execute the Low Power bit writing sequence --------------------------------*/
//  844   WriteLPBit();
        BL       WriteLPBit
//  845 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??MRCC_GenerateSWReset_0:
        DC32     0x60000028
        CFI EndBlock cfiBlock16
//  846 
//  847 /*******************************************************************************
//  848 * Function Name  : MRCC_WriteBackupRegister
//  849 * Description    : Writes user data to the specified backup register.
//  850 * Input          : - MRCC_BKP: specifies the backup register.
//  851 *                    This parameter can be one of the following values:
//  852 *                          - MRCC_BKP0
//  853 *                          - MRCC_BKP1
//  854 *                  - Data: data to write.
//  855 * Output         : None
//  856 * Return         : None
//  857 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock17 Using cfiCommon0
        CFI Function MRCC_WriteBackupRegister
        THUMB
//  858 void MRCC_WriteBackupRegister(MRCC_BKPReg MRCC_BKP, u32 Data)
//  859 {
MRCC_WriteBackupRegister:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  860   if(MRCC_BKP == MRCC_BKP0)
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+0
        BNE      ??MRCC_WriteBackupRegister_0
//  861   {
//  862     MRCC->BKP0 = Data;
        LDR      R2,??MRCC_WriteBackupRegister_1  ;; 0x60000040
        STR      R1,[R2, #+0]
        B        ??MRCC_WriteBackupRegister_2
//  863   }
//  864   else
//  865   {
//  866     MRCC->BKP1 = Data;
??MRCC_WriteBackupRegister_0:
        LDR      R2,??MRCC_WriteBackupRegister_1+0x4  ;; 0x60000044
        STR      R1,[R2, #+0]
//  867   }
//  868 }
??MRCC_WriteBackupRegister_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??MRCC_WriteBackupRegister_1:
        DC32     0x60000040
        DC32     0x60000044
        CFI EndBlock cfiBlock17
//  869 
//  870 /*******************************************************************************
//  871 * Function Name  : MRCC_ReadBackupRegister
//  872 * Description    : Reads data from the specified backup register.
//  873 * Input          : - MRCC_BKP: specifies the backup register.
//  874 *                    This parameter can be one of the following values:
//  875 *                          - MRCC_BKP0
//  876 *                          - MRCC_BKP1
//  877 * Output         : None
//  878 * Return         : The content of the specified backup register.
//  879 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock18 Using cfiCommon0
        CFI Function MRCC_ReadBackupRegister
        THUMB
//  880 u32 MRCC_ReadBackupRegister(MRCC_BKPReg MRCC_BKP)
//  881 {
MRCC_ReadBackupRegister:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
//  882   if(MRCC_BKP == MRCC_BKP0)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+0
        BNE      ??MRCC_ReadBackupRegister_0
//  883   {
//  884     return(MRCC->BKP0);
        LDR      R0,??MRCC_ReadBackupRegister_1  ;; 0x60000040
        LDR      R0,[R0, #+0]
        B        ??MRCC_ReadBackupRegister_2
//  885   }
//  886   else
//  887   {
//  888     return(MRCC->BKP1);
??MRCC_ReadBackupRegister_0:
        LDR      R0,??MRCC_ReadBackupRegister_1+0x4  ;; 0x60000044
        LDR      R0,[R0, #+0]
??MRCC_ReadBackupRegister_2:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        DATA
??MRCC_ReadBackupRegister_1:
        DC32     0x60000040
        DC32     0x60000044
        CFI EndBlock cfiBlock18
//  889   }
//  890 }
//  891 
//  892 /*******************************************************************************
//  893 * Function Name  : MRCC_IOVoltageRangeConfig
//  894 * Description    : Configures the I/O pins voltage range.
//  895 * Input          : - MRCC_IOVoltageRange: specifies the I/O pins voltage range.
//  896 *                    This parameter can be one of the following values:
//  897 *                          - MRCC_IOVoltageRange_5V
//  898 *                          - MRCC_IOVoltageRange_3V3
//  899 * Output         : None
//  900 * Return         : None
//  901 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock19 Using cfiCommon0
        CFI Function MRCC_IOVoltageRangeConfig
        THUMB
//  902 void MRCC_IOVoltageRangeConfig(u32 MRCC_IOVoltageRange)
//  903 {
MRCC_IOVoltageRangeConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  904   if(MRCC_IOVoltageRange == MRCC_IOVoltageRange_3V3)
        MOVS     R1,#+128
        LSLS     R1,R1,#+9        ;; #+65536
        CMP      R0,R1
        BNE      ??MRCC_IOVoltageRangeConfig_0
//  905   {
//  906     MRCC->PWRCTRL |= MRCC_IOVoltageRange_3V3;
        LDR      R1,??MRCC_IOVoltageRangeConfig_1  ;; 0x60000028
        LDR      R1,[R1, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+9        ;; #+65536
        ORRS     R2,R2,R1
        LDR      R1,??MRCC_IOVoltageRangeConfig_1  ;; 0x60000028
        STR      R2,[R1, #+0]
        B        ??MRCC_IOVoltageRangeConfig_2
//  907   }
//  908   else
//  909   {
//  910     MRCC->PWRCTRL &= MRCC_IOVoltageRange_5V;
??MRCC_IOVoltageRangeConfig_0:
        LDR      R1,??MRCC_IOVoltageRangeConfig_1  ;; 0x60000028
        LDR      R1,[R1, #+0]
        LDR      R2,??MRCC_IOVoltageRangeConfig_1+0x4  ;; 0xfffeffff
        ANDS     R2,R2,R1
        LDR      R1,??MRCC_IOVoltageRangeConfig_1  ;; 0x60000028
        STR      R2,[R1, #+0]
//  911   }
//  912 }
??MRCC_IOVoltageRangeConfig_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??MRCC_IOVoltageRangeConfig_1:
        DC32     0x60000028
        DC32     0xfffeffff
        CFI EndBlock cfiBlock19
//  913 
//  914 /*******************************************************************************
//  915 * Function Name  : MRCC_MCOConfig
//  916 * Description    : Selects the clock source to output on MCO pin (P0.1).
//  917 *                  To output the clock, the associated alternate function must
//  918 *                  be enabled in the I/O port controller.
//  919 * Input          : - MRCC_MCO: specifies the clock source to output.
//  920 *                    This parameter can be one of the following values:
//  921 *                          - MRCC_MCO_HCLK
//  922 *                          - MRCC_MCO_PCLK
//  923 *                          - MRCC_MCO_OSC4M
//  924 *                          - MRCC_MCO_CKPLL2
//  925 *                  - MRCC_MCOPrescaler: specifies if prescaler, divide by 1 or 2,
//  926 *                    is applied to this clock before outputting it to MCO pin.
//  927 *                    This parameter can be one of the following values:
//  928 *                          - MRCC_MCOPrescaler_1
//  929 *                          - MRCC_MCOPrescaler_2
//  930 * Output         : None
//  931 * Return         : None
//  932 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock20 Using cfiCommon0
        CFI Function MRCC_MCOConfig
        THUMB
//  933 void MRCC_MCOConfig(u32 MRCC_MCO, u32 MCO_MCOPrescaler)
//  934 {
MRCC_MCOConfig:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  935   u32 Temp = 0;
        MOVS     R3,#+0
        MOVS     R2,R3
//  936 /* MCO prescaler configuration -----------------------------------------------*/
//  937   if(MCO_MCOPrescaler == MRCC_MCOPrescaler_2)
        CMP      R1,#+32
        BNE      ??MRCC_MCOConfig_0
//  938   {
//  939     MRCC->CLKCTL |= MRCC_MCOPrescaler_2;
        LDR      R3,??MRCC_MCOConfig_1  ;; 0x60000020
        LDR      R3,[R3, #+0]
        MOVS     R4,#+32
        ORRS     R4,R4,R3
        LDR      R3,??MRCC_MCOConfig_1  ;; 0x60000020
        STR      R4,[R3, #+0]
        B        ??MRCC_MCOConfig_2
//  940   }
//  941   else
//  942   {
//  943     MRCC->CLKCTL &= MRCC_MCOPrescaler_1;
??MRCC_MCOConfig_0:
        LDR      R3,??MRCC_MCOConfig_1  ;; 0x60000020
        LDR      R3,[R3, #+0]
        MOVS     R4,#+32
        BICS     R3,R3,R4
        LDR      R4,??MRCC_MCOConfig_1  ;; 0x60000020
        STR      R3,[R4, #+0]
//  944   }
//  945 
//  946 /* MCO selection configuration -----------------------------------------------*/
//  947 
//  948   /* Clear MCOS[1:0] bits */
//  949   Temp = MRCC->CLKCTL & MRCC_MCOS_Reset_Mask;
??MRCC_MCOConfig_2:
        LDR      R3,??MRCC_MCOConfig_1  ;; 0x60000020
        LDR      R3,[R3, #+0]
        MOVS     R4,#+192
        BICS     R3,R3,R4
        MOVS     R2,R3
//  950 
//  951   /* Set MCOS[1:0] bits according to MRCC_MCO value */
//  952   Temp |= MRCC_MCO;
        ORRS     R2,R2,R0
//  953   
//  954   /* Store the new value */
//  955   MRCC->CLKCTL = Temp;
        LDR      R3,??MRCC_MCOConfig_1  ;; 0x60000020
        STR      R2,[R3, #+0]
//  956 }
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??MRCC_MCOConfig_1:
        DC32     0x60000020
        CFI EndBlock cfiBlock20
//  957 
//  958 /*******************************************************************************
//  959 * Function Name  : MRCC_OSC4MConfig
//  960 * Description    : Configures the 4MHz main oscillator (OSC4M).
//  961 *                  This function must be used when the CK_SYS is not clocked
//  962 *                  by the OSC4M and the PLL is not enabled.
//  963 * Input          : - MRCC_OSC4M: specifies the new state of the OSC4M oscillator.
//  964 *                    This parameter can be one of the following values:
//  965 *                          - MRCC_OSC4M_Default: OSC4M enabled, bypass disabled
//  966 *                          - MRCC_OSC4M_Disable: OSC4M disabled
//  967 *                          - MRCC_OSC4M_Bypass:  OSC4M bypassed
//  968 * Output         : None
//  969 * Return         : An ErrorStatus enumuration value:
//  970 *                         - SUCCESS: Clock configuration succeeded
//  971 *                         - ERROR: Clock configuration failed
//  972 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock21 Using cfiCommon0
        CFI Function MRCC_OSC4MConfig
        THUMB
//  973 ErrorStatus MRCC_OSC4MConfig(u32 MRCC_OSC4M)
//  974 {
MRCC_OSC4MConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
//  975   ErrorStatus Status = SUCCESS;
        MOVS     R2,#+1
        MOVS     R0,R2
//  976 
//  977 /* If CK_SYS is driven by OSC4M or the PLL is enabled, exit ------------------*/           
//  978   if(((MRCC->CLKCTL & MRCC_CKSEL_CKOSCSEL_Mask) == MRCC_CKSEL_Set_Mask) ||    
//  979      (((MRCC->CLKCTL & MRCC_CKSEL_CKOSCSEL_Mask) == MRCC_CKSEL_CKOSCSEL_Mask) &&
//  980      ((MRCC->PWRCTRL & MRCC_CKRTCSEL_Reset_Mask) != RESET))||
//  981      ((MRCC->CLKCTL & MRCC_PLLEN_Set_Mask) != RESET))
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        LDR      R2,[R2, #+0]
        MOVS     R3,#+160
        LSLS     R3,R3,#+16       ;; #+10485760
        ANDS     R3,R3,R2
        MOVS     R2,#+128
        LSLS     R2,R2,#+16       ;; #+8388608
        CMP      R3,R2
        BEQ      ??MRCC_OSC4MConfig_1
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        LDR      R2,[R2, #+0]
        MOVS     R3,#+160
        LSLS     R3,R3,#+16       ;; #+10485760
        ANDS     R3,R3,R2
        MOVS     R2,#+160
        LSLS     R2,R2,#+16       ;; #+10485760
        CMP      R3,R2
        BNE      ??MRCC_OSC4MConfig_2
        LDR      R2,??MRCC_OSC4MConfig_0+0x4  ;; 0x60000028
        LDR      R2,[R2, #+0]
        LDR      R3,??MRCC_OSC4MConfig_0+0x8  ;; 0xfcffffff
        ANDS     R3,R3,R2
        CMP      R3,#+0
        BNE      ??MRCC_OSC4MConfig_1
??MRCC_OSC4MConfig_2:
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        LDR      R2,[R2, #+0]
        LSLS     R2,R2,#+7
        BPL      ??MRCC_OSC4MConfig_3
//  982   {
//  983     Status = ERROR;
??MRCC_OSC4MConfig_1:
        MOVS     R2,#+0
        MOVS     R0,R2
        B        ??MRCC_OSC4MConfig_4
//  984   }
//  985 /* Else configure the OSC4MOFF and OSC4MBYP bits -----------------------------*/   
//  986   else
//  987   {  
//  988     switch(MRCC_OSC4M)
??MRCC_OSC4MConfig_3:
        MOVS     R2,R1
        MOVS     R3,#+128
        LSLS     R3,R3,#+9        ;; #+65536
        SUBS     R2,R2,R3
        BEQ      ??MRCC_OSC4MConfig_5
        MOVS     R3,#+128
        LSLS     R3,R3,#+9        ;; #+65536
        SUBS     R2,R2,R3
        BEQ      ??MRCC_OSC4MConfig_6
        LDR      R3,??MRCC_OSC4MConfig_0+0xC  ;; 0xfffaffff
        SUBS     R2,R2,R3
        BNE      ??MRCC_OSC4MConfig_7
//  989     {
//  990       case MRCC_OSC4M_Default:
//  991         MRCC->CLKCTL &= MRCC_OSC4MOFF_Reset_Mask & MRCC_OSC4MBYP_Reset_Mask;
??MRCC_OSC4MConfig_8:
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        LDR      R2,[R2, #+0]
        LDR      R3,??MRCC_OSC4MConfig_0+0x10  ;; 0xfffcffff
        ANDS     R3,R3,R2
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        STR      R3,[R2, #+0]
        B        ??MRCC_OSC4MConfig_4
//  992         break;
//  993       
//  994       case MRCC_OSC4M_Disable:
//  995         MRCC->CLKCTL &= MRCC_OSC4MBYP_Reset_Mask;
??MRCC_OSC4MConfig_6:
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        LDR      R2,[R2, #+0]
        LDR      R3,??MRCC_OSC4MConfig_0+0x14  ;; 0xfffeffff
        ANDS     R3,R3,R2
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        STR      R3,[R2, #+0]
//  996         MRCC->CLKCTL |= MRCC_OSC4MOFF_Set_Mask;
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        LDR      R2,[R2, #+0]
        MOVS     R3,#+128
        LSLS     R3,R3,#+10       ;; #+131072
        ORRS     R3,R3,R2
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        STR      R3,[R2, #+0]
        B        ??MRCC_OSC4MConfig_4
//  997         break;
//  998         
//  999       case MRCC_OSC4M_Bypass:
// 1000         MRCC->CLKCTL &= MRCC_OSC4MOFF_Reset_Mask;
??MRCC_OSC4MConfig_5:
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        LDR      R2,[R2, #+0]
        LDR      R3,??MRCC_OSC4MConfig_0+0x18  ;; 0xfffdffff
        ANDS     R3,R3,R2
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        STR      R3,[R2, #+0]
// 1001         MRCC->CLKCTL |= MRCC_OSC4MBYP_Set_Mask;
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        LDR      R2,[R2, #+0]
        MOVS     R3,#+128
        LSLS     R3,R3,#+9        ;; #+65536
        ORRS     R3,R3,R2
        LDR      R2,??MRCC_OSC4MConfig_0  ;; 0x60000020
        STR      R3,[R2, #+0]
        B        ??MRCC_OSC4MConfig_4
// 1002         break;        
// 1003       
// 1004       default:
// 1005         Status =  ERROR;
??MRCC_OSC4MConfig_7:
        MOVS     R2,#+0
        MOVS     R0,R2
// 1006         break;      
// 1007     }
// 1008   }  
// 1009   
// 1010   return Status; 
??MRCC_OSC4MConfig_4:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??MRCC_OSC4MConfig_0:
        DC32     0x60000020
        DC32     0x60000028
        DC32     0xfcffffff
        DC32     0xfffaffff
        DC32     0xfffcffff
        DC32     0xfffeffff
        DC32     0xfffdffff
        CFI EndBlock cfiBlock21
// 1011 }
// 1012 
// 1013 /*******************************************************************************
// 1014 * Function Name  : MRCC_OSC32KConfig
// 1015 * Description    : Configures the OSC32K oscillator.
// 1016 *                  This function must be used when the CK_SYS is not clocked by
// 1017 *                  the CK_RTC.
// 1018 * Input          : - MRCC_OSC32K: specifies the new state of the OSC32K oscillator.
// 1019 *                    This parameter can be one of the following values:
// 1020 *                          - MRCC_OSC32K_Disable: OSC32K disabled
// 1021 *                          - MRCC_OSC32K_Enable: OSC32K enabled
// 1022 *                  - MRCC_OSC32KBypass: specifies if the OSC32K oscillator is
// 1023 *                    bypassed or not.
// 1024 *                    This parameter can be one of the following values:
// 1025 *                          - MRCC_OSC32KBypass_Disable: OSC32K selected
// 1026 *                          - MRCC_OSC32KBypass_Enable: OSC32K bypassed                          
// 1027 * Output         : None
// 1028 * Return         : An ErrorStatus enumuration value:
// 1029 *                         - SUCCESS: Clock configuration succeeded
// 1030 *                         - ERROR: Clock configuration failed
// 1031 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock22 Using cfiCommon0
        CFI Function MRCC_OSC32KConfig
        THUMB
// 1032 ErrorStatus MRCC_OSC32KConfig(u32 MRCC_OSC32K, u32 MRCC_OSC32KBypass)
// 1033 { 
MRCC_OSC32KConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R2,R0
// 1034 /* If CK_SYS is driven by CK_RTC, exit ---------------------------------------*/     
// 1035   if(((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) != RESET) &&
// 1036       ((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET))
        LDR      R0,??MRCC_OSC32KConfig_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+8
        BPL      ??MRCC_OSC32KConfig_1
        LDR      R0,??MRCC_OSC32KConfig_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BPL      ??MRCC_OSC32KConfig_1
// 1037   {
// 1038     return ERROR;
        MOVS     R0,#+0
        B        ??MRCC_OSC32KConfig_2
// 1039   }
// 1040 /* Else configure the OSC32KEN and OSC32KBYP bits ----------------------------*/ 
// 1041   else
// 1042   { 
// 1043     /* Configure OSC32KEN bit */
// 1044     if(MRCC_OSC32K == MRCC_OSC32K_Enable)
??MRCC_OSC32KConfig_1:
        MOVS     R0,#+128
        LSLS     R0,R0,#+22       ;; #+536870912
        CMP      R2,R0
        BNE      ??MRCC_OSC32KConfig_3
// 1045     {
// 1046       MRCC->PWRCTRL |= MRCC_OSC32K_Enable;
        LDR      R0,??MRCC_OSC32KConfig_0+0x4  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R3,#+128
        LSLS     R3,R3,#+22       ;; #+536870912
        ORRS     R3,R3,R0
        LDR      R0,??MRCC_OSC32KConfig_0+0x4  ;; 0x60000028
        STR      R3,[R0, #+0]
        B        ??MRCC_OSC32KConfig_4
// 1047     }
// 1048     else
// 1049     {
// 1050       MRCC->PWRCTRL &= MRCC_OSC32K_Disable;
??MRCC_OSC32KConfig_3:
        LDR      R0,??MRCC_OSC32KConfig_0+0x4  ;; 0x60000028
        LDR      R0,[R0, #+0]
        LDR      R3,??MRCC_OSC32KConfig_0+0x8  ;; 0xdfffffff
        ANDS     R3,R3,R0
        LDR      R0,??MRCC_OSC32KConfig_0+0x4  ;; 0x60000028
        STR      R3,[R0, #+0]
// 1051     }
// 1052     
// 1053     /* Configure OSC32KBYP bit */
// 1054     if(MRCC_OSC32KBypass == MRCC_OSC32KBypass_Enable)
??MRCC_OSC32KConfig_4:
        MOVS     R0,#+128
        LSLS     R0,R0,#+23       ;; #+1073741824
        CMP      R1,R0
        BNE      ??MRCC_OSC32KConfig_5
// 1055     {
// 1056       MRCC->PWRCTRL |= MRCC_OSC32KBypass_Enable;
        LDR      R0,??MRCC_OSC32KConfig_0+0x4  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R3,#+128
        LSLS     R3,R3,#+23       ;; #+1073741824
        ORRS     R3,R3,R0
        LDR      R0,??MRCC_OSC32KConfig_0+0x4  ;; 0x60000028
        STR      R3,[R0, #+0]
        B        ??MRCC_OSC32KConfig_6
// 1057     }
// 1058     else
// 1059     {
// 1060       MRCC->PWRCTRL &= MRCC_OSC32KBypass_Disable;
??MRCC_OSC32KConfig_5:
        LDR      R0,??MRCC_OSC32KConfig_0+0x4  ;; 0x60000028
        LDR      R0,[R0, #+0]
        LDR      R3,??MRCC_OSC32KConfig_0+0xC  ;; 0xbfffffff
        ANDS     R3,R3,R0
        LDR      R0,??MRCC_OSC32KConfig_0+0x4  ;; 0x60000028
        STR      R3,[R0, #+0]
// 1061     }   
// 1062      
// 1063     return SUCCESS;   
??MRCC_OSC32KConfig_6:
        MOVS     R0,#+1
??MRCC_OSC32KConfig_2:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        DATA
??MRCC_OSC32KConfig_0:
        DC32     0x60000020
        DC32     0x60000028
        DC32     0xdfffffff
        DC32     0xbfffffff
        CFI EndBlock cfiBlock22
// 1064   }
// 1065 }
// 1066 
// 1067 /*******************************************************************************
// 1068 * Function Name  : MRCC_LPOSCConfig
// 1069 * Description    : Enables or disables the LPOSC oscillator.
// 1070 *                  This function must be used when the CK_SYS is not clocked by
// 1071 *                  the CK_RTC.
// 1072 * Input          : - MRCC_LPOSC: specifies the new state of the LPOSC oscillator.
// 1073 *                    This parameter can be one of the following values:
// 1074 *                          - MRCC_LPOSC_Disable: LPOSC disabled
// 1075 *                          - MRCC_LPOSC_Enable: LPOSC enabled
// 1076 * Output         : None
// 1077 * Return         : An ErrorStatus enumuration value:
// 1078 *                         - SUCCESS: Clock configuration succeeded
// 1079 *                         - ERROR: Clock configuration failed
// 1080 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock23 Using cfiCommon0
        CFI Function MRCC_LPOSCConfig
        THUMB
// 1081 ErrorStatus MRCC_LPOSCConfig(u32 MRCC_LPOSC)
// 1082 {
MRCC_LPOSCConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
// 1083 /* If CK_SYS is driven by CK_RTC or LPOSC is used as CK_RTC clock source, exit*/      
// 1084   if((((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) != RESET) &&
// 1085      ((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET)) ||
// 1086      ((MRCC->PWRCTRL & MRCC_CKRTCSEL_Set_Mask) == MRCC_CKRTC_LPOSC)) 
        LDR      R0,??MRCC_LPOSCConfig_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+8
        BPL      ??MRCC_LPOSCConfig_1
        LDR      R0,??MRCC_LPOSCConfig_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BMI      ??MRCC_LPOSCConfig_2
??MRCC_LPOSCConfig_1:
        LDR      R0,??MRCC_LPOSCConfig_0+0x4  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R2,#+192
        LSLS     R2,R2,#+18       ;; #+50331648
        ANDS     R2,R2,R0
        MOVS     R0,#+192
        LSLS     R0,R0,#+18       ;; #+50331648
        CMP      R2,R0
        BNE      ??MRCC_LPOSCConfig_3
// 1087   {
// 1088     return ERROR;
??MRCC_LPOSCConfig_2:
        MOVS     R0,#+0
        B        ??MRCC_LPOSCConfig_4
// 1089   }
// 1090 /* Else configure the LPOSCEN bit --------------------------------------------*/  
// 1091   else
// 1092   {   
// 1093     if(MRCC_LPOSC == MRCC_LPOSC_Enable)
??MRCC_LPOSCConfig_3:
        MOVS     R0,#+128
        LSLS     R0,R0,#+21       ;; #+268435456
        CMP      R1,R0
        BNE      ??MRCC_LPOSCConfig_5
// 1094     {
// 1095       MRCC->PWRCTRL |= MRCC_LPOSC_Enable;
        LDR      R0,??MRCC_LPOSCConfig_0+0x4  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+21       ;; #+268435456
        ORRS     R2,R2,R0
        LDR      R0,??MRCC_LPOSCConfig_0+0x4  ;; 0x60000028
        STR      R2,[R0, #+0]
        B        ??MRCC_LPOSCConfig_6
// 1096     }
// 1097     else
// 1098     {
// 1099       MRCC->PWRCTRL &= MRCC_LPOSC_Disable;
??MRCC_LPOSCConfig_5:
        LDR      R0,??MRCC_LPOSCConfig_0+0x4  ;; 0x60000028
        LDR      R0,[R0, #+0]
        LDR      R2,??MRCC_LPOSCConfig_0+0x8  ;; 0xefffffff
        ANDS     R2,R2,R0
        LDR      R0,??MRCC_LPOSCConfig_0+0x4  ;; 0x60000028
        STR      R2,[R0, #+0]
// 1100     }
// 1101 
// 1102     return SUCCESS;
??MRCC_LPOSCConfig_6:
        MOVS     R0,#+1
??MRCC_LPOSCConfig_4:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??MRCC_LPOSCConfig_0:
        DC32     0x60000020
        DC32     0x60000028
        DC32     0xefffffff
        CFI EndBlock cfiBlock23
// 1103   }     
// 1104 }
// 1105 
// 1106 /*******************************************************************************
// 1107 * Function Name  : MRCC_RTCMConfig
// 1108 * Description    : Enables or disables RTC clock measurement.
// 1109 * Input          : - MRCC_RTCM: specifies whether CK_RTC is connected to TB 
// 1110 *                    timer IC1 or not.
// 1111 *                    This parameter can be one of the following values:
// 1112 *                          - MRCC_RTCM_Disable: CK_RTC not connected to TB timer IC1
// 1113 *                          - MRCC_RTCM_Enable: CK_RTC connected to TB timer IC1
// 1114 * Output         : None
// 1115 * Return         : None
// 1116 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock24 Using cfiCommon0
        CFI Function MRCC_RTCMConfig
        THUMB
// 1117 void MRCC_RTCMConfig(u32 MRCC_RTCM)
// 1118 {
MRCC_RTCMConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
// 1119   if(MRCC_RTCM == MRCC_RTCM_Enable)
        MOVS     R1,#+128
        LSLS     R1,R1,#+19       ;; #+67108864
        CMP      R0,R1
        BNE      ??MRCC_RTCMConfig_0
// 1120   {
// 1121     MRCC->PWRCTRL |= MRCC_RTCM_Enable;
        LDR      R1,??MRCC_RTCMConfig_1  ;; 0x60000028
        LDR      R1,[R1, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+19       ;; #+67108864
        ORRS     R2,R2,R1
        LDR      R1,??MRCC_RTCMConfig_1  ;; 0x60000028
        STR      R2,[R1, #+0]
        B        ??MRCC_RTCMConfig_2
// 1122   }
// 1123   else
// 1124   {
// 1125     MRCC->PWRCTRL &= MRCC_RTCM_Disable;
??MRCC_RTCMConfig_0:
        LDR      R1,??MRCC_RTCMConfig_1  ;; 0x60000028
        LDR      R1,[R1, #+0]
        LDR      R2,??MRCC_RTCMConfig_1+0x4  ;; 0xfbffffff
        ANDS     R2,R2,R1
        LDR      R1,??MRCC_RTCMConfig_1  ;; 0x60000028
        STR      R2,[R1, #+0]
// 1126   }  
// 1127 }
??MRCC_RTCMConfig_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??MRCC_RTCMConfig_1:
        DC32     0x60000028
        DC32     0xfbffffff
        CFI EndBlock cfiBlock24
// 1128 
// 1129 /*******************************************************************************
// 1130 * Function Name  : MRCC_SetBuilderCounter
// 1131 * Description    : Sets the builder counter value which defines the delay for
// 1132 *                  the 4MHz main oscillator (OSC4M) clock to be stabilized.
// 1133 * Input          : - BuilderCounter: defines the delay for the OSC4M oscillator
// 1134 *                    clock to be stabilized.
// 1135 * Output         : None
// 1136 * Return         : None
// 1137 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock25 Using cfiCommon0
        CFI Function MRCC_SetBuilderCounter
        THUMB
// 1138 void MRCC_SetBuilderCounter(u8 BuilderCounter)
// 1139 { 
// 1140   *(u8 *) 0x60000026 = BuilderCounter;
MRCC_SetBuilderCounter:
        LDR      R1,??MRCC_SetBuilderCounter_0  ;; 0x60000026
        STRB     R0,[R1, #+0]
// 1141 }
        BX       LR               ;; return
        Nop      
        DATA
??MRCC_SetBuilderCounter_0:
        DC32     0x60000026
        CFI EndBlock cfiBlock25
// 1142 
// 1143 /*******************************************************************************
// 1144 * Function Name  : MRCC_GetCKSYSCounter
// 1145 * Description    : Gets the result of the delay applied to CK_SYS before
// 1146 *                  starting the CPU.
// 1147 * Input          : None
// 1148 * Output         : None
// 1149 * Return         : SCOUNT value.
// 1150 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock26 Using cfiCommon0
        CFI Function MRCC_GetCKSYSCounter
        THUMB
// 1151 u16 MRCC_GetCKSYSCounter(void)
// 1152 {
// 1153   return((u16)(MRCC->RFSR & 0x0FFF));
MRCC_GetCKSYSCounter:
        LDR      R0,??MRCC_GetCKSYSCounter_0  ;; 0x60000024
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+20       ;; ZeroExtS R0,R0,#+20,#+20
        LSRS     R0,R0,#+20
        BX       LR               ;; return
        Nop      
        DATA
??MRCC_GetCKSYSCounter_0:
        DC32     0x60000024
        CFI EndBlock cfiBlock26
// 1154 }
// 1155 
// 1156 /*******************************************************************************
// 1157 * Function Name  : MRCC_GetFlagStatus
// 1158 * Description    : Checks whether the specified MRCC flag is set or not.
// 1159 * Input          : - MRCC_FLAG: specifies the flag to check.
// 1160 *                    This parameter can be one of the following values:
// 1161 *                          - MRCC_FLAG_LOCK: PLL Locked flag
// 1162 *                          - MRCC_FLAG_LOCKIF: PLL Lock Interrupt status flag
// 1163 *                          - MRCC_FLAG_CKSEL: CK_SYS source staus flag
// 1164 *                          - MRCC_FLAG_CKOSCSEL: CK_OSC clock source staus flag
// 1165 *                          - MRCC_FLAG_NCKD: No Clock Detected flag
// 1166 *                          - MRCC_FLAG_SWR: Software Reset flag
// 1167 *                          - MRCC_FLAG_WDGR: Watchdog Reset flag
// 1168 *                          - MRCC_FLAG_EXTR: External Reset flag
// 1169 *                          - MRCC_FLAG_WKP: Wake-Up flag
// 1170 *                          - MRCC_FLAG_STDB: STANDBY flag
// 1171 *                          - MRCC_FLAG_BCOUNT:   Builder Counter Flag
// 1172 *                          - MRCC_FLAG_OSC32KRDY: Oscillator 32K Ready
// 1173 *                          - MRCC_FLAG_CKRTCOK: CK_RTC OK
// 1174 *                          - MRCC_FLAG_LPDONE: Low Power Bit Sequence has been performed
// 1175 *                          - MRCC_FLAG_LP: Low Power Mode Entry
// 1176 * Output         : None
// 1177 * Return         : The new state of MRCC_FLAG (SET or RESET).
// 1178 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock27 Using cfiCommon0
        CFI Function MRCC_GetFlagStatus
        THUMB
// 1179 FlagStatus MRCC_GetFlagStatus(u8 MRCC_FLAG)
// 1180 {
MRCC_GetFlagStatus:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R1,R0
// 1181   u32 MRCCReg = 0, FlagPos = 0;
        MOVS     R0,#+0
        MOVS     R2,R0
        MOVS     R0,#+0
        MOVS     R3,R0
// 1182   u32 StatusReg = 0;
        MOVS     R0,#+0
        MOVS     R4,R0
// 1183 
// 1184   /* Get the MRCC register index */
// 1185   MRCCReg = MRCC_FLAG >> 5;
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+5
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        MOVS     R2,R0
// 1186 
// 1187   /* Get the flag position */
// 1188   FlagPos = MRCC_FLAG & MRCC_FLAG_Mask;
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        LSLS     R0,R1,#+27       ;; ZeroExtS R0,R1,#+27,#+27
        LSRS     R0,R0,#+27
        MOVS     R3,R0
// 1189 
// 1190   if(MRCCReg == 1) /* The flag to check is in CLKCTL register */
        CMP      R2,#+1
        BNE      ??MRCC_GetFlagStatus_0
// 1191   {
// 1192     StatusReg = MRCC->CLKCTL;
        LDR      R0,??MRCC_GetFlagStatus_1  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R4,R0
        B        ??MRCC_GetFlagStatus_2
// 1193   }
// 1194   else if (MRCCReg == 2) /* The flag to check is in RFSR register */
??MRCC_GetFlagStatus_0:
        CMP      R2,#+2
        BNE      ??MRCC_GetFlagStatus_3
// 1195   {
// 1196     StatusReg = MRCC->RFSR;
        LDR      R0,??MRCC_GetFlagStatus_1+0x4  ;; 0x60000024
        LDR      R0,[R0, #+0]
        MOVS     R4,R0
        B        ??MRCC_GetFlagStatus_2
// 1197   }
// 1198   else if(MRCCReg == 3) /* The flag to check is in PWRCTRL register */
??MRCC_GetFlagStatus_3:
        CMP      R2,#+3
        BNE      ??MRCC_GetFlagStatus_2
// 1199   {
// 1200     StatusReg = MRCC->PWRCTRL;
        LDR      R0,??MRCC_GetFlagStatus_1+0x8  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R4,R0
// 1201   }
// 1202   
// 1203   if((StatusReg & (1 << FlagPos))!= RESET)
??MRCC_GetFlagStatus_2:
        MOVS     R0,#+1
        LSLS     R0,R0,R3
        ANDS     R0,R0,R4
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
// 1204   {
// 1205     return SET;
// 1206   }
// 1207   else
// 1208   {
// 1209     return RESET;
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??MRCC_GetFlagStatus_1:
        DC32     0x60000020
        DC32     0x60000024
        DC32     0x60000028
        CFI EndBlock cfiBlock27
// 1210   }
// 1211 }
// 1212 
// 1213 /*******************************************************************************
// 1214 * Function Name  : MRCC_ClearFlag
// 1215 * Description    : Clears the MRCC’s pending flags.
// 1216 * Input          : - MRCC_FLAG: specifies the flag to clear.
// 1217 *                    This parameter can be one of the following values:
// 1218 *                          - MRCC_FLAG_NCKD: No Clock Detected flag
// 1219 *                          - MRCC_FLAG_SWR: Software Reset flag
// 1220 *                          - MRCC_FLAG_WDGR: Watchdog Reset flag
// 1221 *                          - MRCC_FLAG_EXTR: External Reset flag
// 1222 *                          - MRCC_FLAG_WKP: Wake-Up flag
// 1223 *                          - MRCC_FLAG_STDB: STANDBY flag
// 1224 *                          - MRCC_FLAG_LPDONE: Low Power Bit Sequence has been performed
// 1225 * Output         : None
// 1226 * Return         : None
// 1227 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock28 Using cfiCommon0
        CFI Function MRCC_ClearFlag
        THUMB
// 1228 void MRCC_ClearFlag(u8 MRCC_FLAG)
// 1229 {
MRCC_ClearFlag:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
// 1230   u32 MRCCReg = 0, FlagPos = 0;
        MOVS     R3,#+0
        MOVS     R1,R3
        MOVS     R3,#+0
        MOVS     R2,R3
// 1231 
// 1232   /* Get the MRCC register index */
// 1233   MRCCReg = MRCC_FLAG >> 5;
        MOVS     R3,R0
        LSLS     R3,R3,#+24       ;; ZeroExtS R3,R3,#+24,#+24
        LSRS     R3,R3,#+24
        LSRS     R3,R3,#+5
        LSLS     R3,R3,#+24       ;; ZeroExtS R3,R3,#+24,#+24
        LSRS     R3,R3,#+24
        MOVS     R1,R3
// 1234 
// 1235   /* Get the flag position */
// 1236   FlagPos = MRCC_FLAG & MRCC_FLAG_Mask;
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSLS     R3,R0,#+27       ;; ZeroExtS R3,R0,#+27,#+27
        LSRS     R3,R3,#+27
        MOVS     R2,R3
// 1237 
// 1238   if(MRCCReg == 1) /* The flag to clear is in CLKCTL register */
        CMP      R1,#+1
        BNE      ??MRCC_ClearFlag_0
// 1239   {
// 1240     MRCC->CLKCTL &= ~(1 << FlagPos);
        LDR      R3,??MRCC_ClearFlag_1  ;; 0x60000020
        LDR      R3,[R3, #+0]
        MOVS     R4,#+1
        LSLS     R4,R4,R2
        BICS     R3,R3,R4
        LDR      R4,??MRCC_ClearFlag_1  ;; 0x60000020
        STR      R3,[R4, #+0]
        B        ??MRCC_ClearFlag_2
// 1241   }
// 1242   else if (MRCCReg == 2) /* The flag to clear is in RFSR register */
??MRCC_ClearFlag_0:
        CMP      R1,#+2
        BNE      ??MRCC_ClearFlag_3
// 1243   {
// 1244     MRCC->RFSR &= ~(1 << FlagPos);
        LDR      R3,??MRCC_ClearFlag_1+0x4  ;; 0x60000024
        LDR      R3,[R3, #+0]
        MOVS     R4,#+1
        LSLS     R4,R4,R2
        BICS     R3,R3,R4
        LDR      R4,??MRCC_ClearFlag_1+0x4  ;; 0x60000024
        STR      R3,[R4, #+0]
        B        ??MRCC_ClearFlag_2
// 1245   }
// 1246   else if(MRCCReg == 3) /* The flag to clear is in PWRCTRL register */
??MRCC_ClearFlag_3:
        CMP      R1,#+3
        BNE      ??MRCC_ClearFlag_2
// 1247   {
// 1248     MRCC->PWRCTRL &= ~(1 << FlagPos);
        LDR      R3,??MRCC_ClearFlag_1+0x8  ;; 0x60000028
        LDR      R3,[R3, #+0]
        MOVS     R4,#+1
        LSLS     R4,R4,R2
        BICS     R3,R3,R4
        LDR      R4,??MRCC_ClearFlag_1+0x8  ;; 0x60000028
        STR      R3,[R4, #+0]
// 1249   }
// 1250 }
??MRCC_ClearFlag_2:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??MRCC_ClearFlag_1:
        DC32     0x60000020
        DC32     0x60000024
        DC32     0x60000028
        CFI EndBlock cfiBlock28
// 1251 
// 1252 /*******************************************************************************
// 1253 * Function Name  : MRCC_GetITStatus
// 1254 * Description    : Checks whether the specified MRCC interrupt has occurred or not.
// 1255 * Input          : - MRCC_IT: specifies the MRCC interrupt source to check.
// 1256 *                    This parameter can be one of the following values:
// 1257 *                          - MRCC_IT_LOCK: PLL lock interrupt
// 1258 *                          - MRCC_IT_NCKD: No Clock detected interrupt
// 1259 * Output         : None
// 1260 * Return         : The new state of MRCC_IT (SET or RESET).
// 1261 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock29 Using cfiCommon0
        CFI Function MRCC_GetITStatus
        THUMB
// 1262 ITStatus MRCC_GetITStatus(u32 MRCC_IT)
// 1263 {
MRCC_GetITStatus:
        MOVS     R1,R0
// 1264   /* Check the specified interrupt pending bit */
// 1265   if((MRCC->CLKCTL & (MRCC_IT >> 1)) != RESET)
        LDR      R0,??MRCC_GetITStatus_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R2,R1
        LSRS     R2,R2,#+1
        ANDS     R2,R2,R0
        MOVS     R0,R2
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
// 1266   {
// 1267     return SET;
// 1268   }
// 1269   else
// 1270   {
// 1271     return RESET;
        BX       LR               ;; return
        DATA
??MRCC_GetITStatus_0:
        DC32     0x60000020
        CFI EndBlock cfiBlock29
// 1272   }
// 1273 }
// 1274 
// 1275 /*******************************************************************************
// 1276 * Function Name  : MRCC_ClearITPendingBit
// 1277 * Description    : Clears the MRCC’s interrupt pending bits.
// 1278 * Input          : - MRCC_IT: specifies the interrupt pending bit to clear.
// 1279 *                    This parameter can be any combination of the following
// 1280 *                    values:
// 1281 *                          - MRCC_IT_LOCK: PLL lock interrupt
// 1282 *                          - MRCC_IT_NCKD: No Clock detected interrupt
// 1283 * Output         : None
// 1284 * Return         : None
// 1285 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock30 Using cfiCommon0
        CFI Function MRCC_ClearITPendingBit
        THUMB
// 1286 void MRCC_ClearITPendingBit(u32 MRCC_IT)
// 1287 {
// 1288   /* Clear the specified interrupt pending bit */
// 1289   MRCC->CLKCTL &= ~(MRCC_IT >> 1);
MRCC_ClearITPendingBit:
        LDR      R1,??MRCC_ClearITPendingBit_0  ;; 0x60000020
        LDR      R1,[R1, #+0]
        MOVS     R2,R0
        LSRS     R2,R2,#+1
        BICS     R1,R1,R2
        LDR      R2,??MRCC_ClearITPendingBit_0  ;; 0x60000020
        STR      R1,[R2, #+0]
// 1290 }
        BX       LR               ;; return
        DATA
??MRCC_ClearITPendingBit_0:
        DC32     0x60000020
        CFI EndBlock cfiBlock30
// 1291 
// 1292 /*******************************************************************************
// 1293 * Function Name  : MRCC_WaitForOSC4MStartUp
// 1294 * Description    : Waits for OSC4M start-up.
// 1295 * Input          : None
// 1296 * Output         : None
// 1297 * Return         : An ErrorStatus enumuration value:
// 1298 *                         - SUCCESS: OSC4M oscillator is stable and ready to use
// 1299 *                         - ERROR: no clock is detected on OSC4M
// 1300 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock31 Using cfiCommon0
        CFI Function MRCC_WaitForOSC4MStartUp
        THUMB
// 1301 ErrorStatus MRCC_WaitForOSC4MStartUp(void)
// 1302 {
MRCC_WaitForOSC4MStartUp:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
// 1303   u32 StartUpCounter = 0;
        MOVS     R0,#+0
        MOVS     R4,R0
// 1304 
// 1305   do
// 1306   {
// 1307     /* Clear No Clock Detected flag */
// 1308     if(MRCC_GetFlagStatus(MRCC_FLAG_NCKD) != RESET)
??MRCC_WaitForOSC4MStartUp_0:
        MOVS     R0,#+50
        BL       MRCC_GetFlagStatus
        CMP      R0,#+0
        BEQ      ??MRCC_WaitForOSC4MStartUp_1
// 1309     {
// 1310       MRCC_ClearFlag(MRCC_FLAG_NCKD);
        MOVS     R0,#+50
        BL       MRCC_ClearFlag
// 1311     }
// 1312 
// 1313     StartUpCounter++;
??MRCC_WaitForOSC4MStartUp_1:
        ADDS     R4,R4,#+1
// 1314 
// 1315   }while((MRCC_GetFlagStatus(MRCC_FLAG_BCOUNT) == RESET)&&
// 1316          (StartUpCounter != OSC4MStartUp_TimeOut));
        MOVS     R0,#+88
        BL       MRCC_GetFlagStatus
        CMP      R0,#+0
        BNE      ??MRCC_WaitForOSC4MStartUp_2
        CMP      R4,#+254
        BNE      ??MRCC_WaitForOSC4MStartUp_0
// 1317   
// 1318   if(MRCC_GetFlagStatus(MRCC_FLAG_BCOUNT) != RESET)
??MRCC_WaitForOSC4MStartUp_2:
        MOVS     R0,#+88
        BL       MRCC_GetFlagStatus
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
// 1319   {
// 1320     return SUCCESS;
// 1321   }
// 1322   else
// 1323   {
// 1324     return ERROR;
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock31
// 1325   }  
// 1326 }
// 1327 
// 1328 /*******************************************************************************
// 1329 * Function Name  : SetCKSYS_FREEOSC
// 1330 * Description    : Selects FREEOSC as CK_SYS clock source.
// 1331 * Input          : None
// 1332 * Output         : None
// 1333 * Return         : An ErrorStatus enumuration value:
// 1334 *                         - SUCCESS: Clock configuration succeeded
// 1335 *                         - ERROR: Clock configuration failed
// 1336 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock32 Using cfiCommon0
        CFI Function SetCKSYS_FREEOSC
        THUMB
// 1337 static ErrorStatus SetCKSYS_FREEOSC(void)
// 1338 {
SetCKSYS_FREEOSC:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
// 1339   /* Check if the PLL is enabled */
// 1340   if((MRCC->CLKCTL & MRCC_PLLEN_Set_Mask) != RESET)
        LDR      R0,??SetCKSYS_FREEOSC_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+7
        BPL      ??SetCKSYS_FREEOSC_1
// 1341   {  
// 1342     if((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) == RESET)
        LDR      R0,??SetCKSYS_FREEOSC_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+8
        BMI      ??SetCKSYS_FREEOSC_2
// 1343     { /* CK_PLL1 used as Ck_SYS clock source*/
// 1344 
// 1345       if((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET)
        LDR      R0,??SetCKSYS_FREEOSC_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BPL      ??SetCKSYS_FREEOSC_3
// 1346       {/* Check if CK_RTC source clock is present*/ 
// 1347         if((MRCC->PWRCTRL & MRCC_CKRTCSEL_Set_Mask) == RESET) 
        LDR      R0,??SetCKSYS_FREEOSC_0+0x4  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R1,#+192
        LSLS     R1,R1,#+18       ;; #+50331648
        ANDS     R1,R1,R0
        CMP      R1,#+0
        BNE      ??SetCKSYS_FREEOSC_3
// 1348         {
// 1349           /* CK_RTC disabled*/
// 1350           return ERROR;
        MOVS     R0,#+0
        B        ??SetCKSYS_FREEOSC_4
// 1351         }
// 1352       }
// 1353       
// 1354       /* Select CK_OSC as CK_SYS clock source */
// 1355       MRCC->CLKCTL |= MRCC_CKSEL_Set_Mask;
??SetCKSYS_FREEOSC_3:
        LDR      R0,??SetCKSYS_FREEOSC_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+16       ;; #+8388608
        ORRS     R1,R1,R0
        LDR      R0,??SetCKSYS_FREEOSC_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1356     }  
// 1357     
// 1358     /* Disable PLL */
// 1359     MRCC->CLKCTL &= MRCC_PLLEN_Reset_Mask;
??SetCKSYS_FREEOSC_2:
        LDR      R0,??SetCKSYS_FREEOSC_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_FREEOSC_0+0x8  ;; 0xfeffffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_FREEOSC_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1360   }
// 1361 
// 1362   /* Select CK_PLL1 as CK_SYS clock source */
// 1363   MRCC->CLKCTL &= MRCC_CKSEL_Reset_Mask;
??SetCKSYS_FREEOSC_1:
        LDR      R0,??SetCKSYS_FREEOSC_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_FREEOSC_0+0xC  ;; 0xff7fffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_FREEOSC_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1364 
// 1365   if((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) == RESET)
        LDR      R0,??SetCKSYS_FREEOSC_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+16       ;; #+8388608
        ANDS     R1,R1,R0
        SUBS     R0,R1,#+1
        SBCS     R0,R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
// 1366   {
// 1367     return SUCCESS;
??SetCKSYS_FREEOSC_4:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        DATA
??SetCKSYS_FREEOSC_0:
        DC32     0x60000020
        DC32     0x60000028
        DC32     0xfeffffff
        DC32     0xff7fffff
        CFI EndBlock cfiBlock32
// 1368   }
// 1369   else
// 1370   {
// 1371     return ERROR;
// 1372   }
// 1373 }
// 1374 
// 1375 /*******************************************************************************
// 1376 * Function Name  : SetCKSYS_OSC4M
// 1377 * Description    : Selects 4MHz main oscillator (OSC4M) as CK_SYS clock source.
// 1378 * Input          : PLL_State: specifies the PLL state.
// 1379 * Output         : None
// 1380 * Return         : An ErrorStatus enumuration value:
// 1381 *                         - SUCCESS: Clock configuration succeeded
// 1382 *                         - ERROR: Clock configuration failed
// 1383 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock33 Using cfiCommon0
        CFI Function SetCKSYS_OSC4M
        THUMB
// 1384 static ErrorStatus SetCKSYS_OSC4M(u32 PLL_State)
// 1385 {
SetCKSYS_OSC4M:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
// 1386 /* If OSC4M is not present, exit ---------------------------------------------*/      
// 1387   if(((MRCC->CLKCTL & MRCC_NCKDF_Set_Mask) != RESET) || 
// 1388      ((MRCC->CLKCTL & MRCC_OSC4MOFF_Set_Mask) != RESET) ) 
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+13
        BMI      ??SetCKSYS_OSC4M_1
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+14
        BPL      ??SetCKSYS_OSC4M_2
// 1389   {
// 1390     /* OSC4M disabled or OSC4M clock is not present*/
// 1391     return ERROR;
??SetCKSYS_OSC4M_1:
        MOVS     R0,#+0
        B        ??SetCKSYS_OSC4M_3
// 1392   }
// 1393 
// 1394 /* Else configure CKSEL and CKOSCSEL bits ------------------------------------*/          
// 1395   if((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET)
??SetCKSYS_OSC4M_2:
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BPL      ??SetCKSYS_OSC4M_4
// 1396   { /* CK_RTC used as CK_OSC clock */   
// 1397   
// 1398     if((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) != RESET) 
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+8
        BPL      ??SetCKSYS_OSC4M_5
// 1399     {
// 1400       /* Select CK_PLL1 as CK_SYS clock source */
// 1401       MRCC->CLKCTL &= MRCC_CKSEL_Reset_Mask;
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_OSC4M_0+0x4  ;; 0xff7fffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1402     }
// 1403     
// 1404     /* Clear CKOSCSEL bit ----------------------------------------------------*/
// 1405     /* Execute CKOSCSEL bit writing sequence */
// 1406     WriteCKOSCSELBit();
??SetCKSYS_OSC4M_5:
        BL       WriteCKOSCSELBit
// 1407 
// 1408     /* Check if CKOSCSEL is set to 0 */
// 1409     if((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET)
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BPL      ??SetCKSYS_OSC4M_4
// 1410     {
// 1411       return ERROR;
        MOVS     R0,#+0
        B        ??SetCKSYS_OSC4M_3
// 1412     }
// 1413   }  
// 1414  
// 1415   /* Select CK_OSC as CK_SYS clock source */
// 1416   MRCC->CLKCTL |= MRCC_CKSEL_Set_Mask;
??SetCKSYS_OSC4M_4:
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+16       ;; #+8388608
        ORRS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1417 
// 1418   if((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) != RESET)
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+8
        BPL      ??SetCKSYS_OSC4M_6
// 1419   {
// 1420     if(PLL_State == MRCC_PLL_Disabled)
        LDR      R0,??SetCKSYS_OSC4M_0+0x8  ;; 0xfeffffff
        CMP      R4,R0
        BNE      ??SetCKSYS_OSC4M_7
// 1421     {
// 1422       /* Disable PLL */
// 1423       MRCC->CLKCTL &= MRCC_PLLEN_Reset_Mask;
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_OSC4M_0+0x8  ;; 0xfeffffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4M_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1424     }
// 1425     
// 1426     return SUCCESS;
??SetCKSYS_OSC4M_7:
        MOVS     R0,#+1
        B        ??SetCKSYS_OSC4M_3
// 1427   }
// 1428   else
// 1429   {
// 1430     return ERROR;
??SetCKSYS_OSC4M_6:
        MOVS     R0,#+0
??SetCKSYS_OSC4M_3:
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        DATA
??SetCKSYS_OSC4M_0:
        DC32     0x60000020
        DC32     0xff7fffff
        DC32     0xfeffffff
        CFI EndBlock cfiBlock33
// 1431   }  
// 1432 }
// 1433 
// 1434 /*******************************************************************************
// 1435 * Function Name  : SetCKSYS_OSC4MPLL
// 1436 * Description    : Selects 4MHz main oscillator (OSC4M) followed by PLL as
// 1437 *                  CK_SYS clock source.
// 1438 * Input          : PLL_Mul: specifies the PLL factor.
// 1439 * Output         : None
// 1440 * Return         : An ErrorStatus enumuration value:
// 1441 *                         - SUCCESS: Clock configuration succeeded
// 1442 *                         - ERROR: Clock configuration failed
// 1443 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock34 Using cfiCommon0
        CFI Function SetCKSYS_OSC4MPLL
        THUMB
// 1444 static ErrorStatus SetCKSYS_OSC4MPLL(u32 PLL_Mul)
// 1445 {
SetCKSYS_OSC4MPLL:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
// 1446   /* Check if 4MHz main oscillator clock is present */
// 1447   if(((MRCC->CLKCTL & MRCC_NCKDF_Set_Mask) == RESET) && 
// 1448      ((MRCC->CLKCTL & MRCC_OSC4MOFF_Set_Mask) == RESET)) 
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+13
        BPL      .+4
        B        ??SetCKSYS_OSC4MPLL_1
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+14
        BPL      .+4
        B        ??SetCKSYS_OSC4MPLL_1
// 1449   {    
// 1450     if(((MRCC->CLKCTL & MRCC_PLLEN_Set_Mask) != RESET) &&
// 1451        ((MRCC->CLKCTL & MRCC_MX_Set_Mask) == PLL_Mul))
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+7
        BPL      ??SetCKSYS_OSC4MPLL_2
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+192
        LSLS     R1,R1,#+21       ;; #+402653184
        ANDS     R1,R1,R0
        CMP      R1,R4
        BNE      ??SetCKSYS_OSC4MPLL_2
// 1452     {
// 1453       /* Select CK_PLL1 as CK_SYS clock source */
// 1454       MRCC->CLKCTL &= MRCC_CKSEL_Reset_Mask;
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_OSC4MPLL_0+0x4  ;; 0xff7fffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1455 
// 1456       if((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) == RESET)
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+16       ;; #+8388608
        ANDS     R1,R1,R0
        SUBS     R0,R1,#+1
        SBCS     R0,R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
// 1457       {
// 1458         return SUCCESS;
        B        ??SetCKSYS_OSC4MPLL_3
// 1459       }
// 1460       else
// 1461       {
// 1462         return ERROR;
// 1463       }
// 1464     }
// 1465     else
// 1466     {
// 1467       /* If CK_RTC is selected as CK_OSC clock source */
// 1468       if((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET)
??SetCKSYS_OSC4MPLL_2:
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BPL      ??SetCKSYS_OSC4MPLL_4
// 1469       {
// 1470         if((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) != RESET)
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+8
        BPL      ??SetCKSYS_OSC4MPLL_5
// 1471         {
// 1472           /* Clear CKSEL bit */
// 1473           MRCC->CLKCTL &= MRCC_CKSEL_Reset_Mask;
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_OSC4MPLL_0+0x4  ;; 0xff7fffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1474         }
// 1475 
// 1476         /* Clear CKOSCSEL bit ------------------------------------------------*/
// 1477         /* Execute CKOSCSEL bit writing sequence */
// 1478         WriteCKOSCSELBit();
??SetCKSYS_OSC4MPLL_5:
        BL       WriteCKOSCSELBit
// 1479       
// 1480         /* Check if CKOSCSEL is set to 0 */
// 1481         if((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) != RESET)
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BPL      ??SetCKSYS_OSC4MPLL_4
// 1482         {
// 1483           return ERROR;
        MOVS     R0,#+0
        B        ??SetCKSYS_OSC4MPLL_3
// 1484         }
// 1485       }
// 1486 
// 1487       /* Select CK_OSC as CK_SYS clock source */
// 1488       MRCC->CLKCTL |= MRCC_CKSEL_Set_Mask;
??SetCKSYS_OSC4MPLL_4:
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+16       ;; #+8388608
        ORRS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1489 
// 1490       /* Disable PLL */
// 1491       MRCC->CLKCTL &= MRCC_PLLEN_Reset_Mask;
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_OSC4MPLL_0+0x8  ;; 0xfeffffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1492 
// 1493       /* Configure PLL factor */
// 1494       if(PLL_Mul == MRCC_PLL_Mul_16)
        CMP      R4,#+0
        BNE      ??SetCKSYS_OSC4MPLL_6
// 1495       {
// 1496         MRCC->CLKCTL &= MRCC_MX_Reset_Mask;
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_OSC4MPLL_0+0xC  ;; 0xe7ffffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        STR      R1,[R0, #+0]
        B        ??SetCKSYS_OSC4MPLL_7
// 1497       }
// 1498       else if((PLL_Mul == MRCC_PLL_Mul_15) || (PLL_Mul == MRCC_PLL_Mul_14) ||
// 1499             (PLL_Mul == MRCC_PLL_Mul_12))
??SetCKSYS_OSC4MPLL_6:
        MOVS     R0,#+128
        LSLS     R0,R0,#+20       ;; #+134217728
        CMP      R4,R0
        BEQ      ??SetCKSYS_OSC4MPLL_8
        MOVS     R0,#+128
        LSLS     R0,R0,#+21       ;; #+268435456
        CMP      R4,R0
        BEQ      ??SetCKSYS_OSC4MPLL_8
        MOVS     R0,#+192
        LSLS     R0,R0,#+21       ;; #+402653184
        CMP      R4,R0
        BNE      ??SetCKSYS_OSC4MPLL_7
// 1500       {
// 1501         /* Clear MX[1:0] bits */
// 1502         MRCC->CLKCTL &= MRCC_MX_Reset_Mask;
??SetCKSYS_OSC4MPLL_8:
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_OSC4MPLL_0+0xC  ;; 0xe7ffffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1503         /* Set MX[1:0] bits according to PLL_Mul value */
// 1504         MRCC->CLKCTL |= PLL_Mul;
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        ORRS     R0,R0,R4
        LDR      R1,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        STR      R0,[R1, #+0]
// 1505       }
// 1506        
// 1507       if(Main_Oscillator == 4000000)
// 1508       {/* 4 MHz external Quartz oscillator used as main oscillator */
// 1509         /* Disable Oscillator Divider by 2 */
// 1510         MRCC->CLKCTL &= MRCC_XTDIV2_Reset_Mask;
// 1511       }
// 1512       else if(Main_Oscillator == 8000000)
// 1513       {/* 8 MHz external Quartz oscillator used as main oscillator */
// 1514         /* Enable Oscillator Divider by 2 */
// 1515         MRCC->CLKCTL |= MRCC_XTDIV2_Set_Mask;
??SetCKSYS_OSC4MPLL_7:
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+8        ;; #+32768
        ORRS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1516       }
// 1517 
// 1518       /* Enable PLL */
// 1519       MRCC->CLKCTL |= MRCC_PLLEN_Set_Mask;
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+17       ;; #+16777216
        ORRS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1520    
// 1521       /* Wait until the PLL is locked */
// 1522       while((MRCC->CLKCTL & MRCC_LOCK_Mask) == RESET)
??SetCKSYS_OSC4MPLL_9:
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BMI      ??SetCKSYS_OSC4MPLL_10
// 1523       {
// 1524         /* If OSC4M clock disapear or the PLL is disabled, exit */
// 1525         if(((MRCC->CLKCTL & MRCC_NCKDF_Set_Mask) != RESET) ||
// 1526            ((MRCC->CLKCTL & MRCC_PLLEN_Set_Mask) == RESET))       
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+13
        BMI      ??SetCKSYS_OSC4MPLL_11
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+7
        BMI      ??SetCKSYS_OSC4MPLL_9
// 1527         {
// 1528           return ERROR;
??SetCKSYS_OSC4MPLL_11:
        MOVS     R0,#+0
        B        ??SetCKSYS_OSC4MPLL_3
// 1529         }
// 1530       }
// 1531 
// 1532       /* Select CK_PLL1 as CK_SYS clock source */
// 1533       MRCC->CLKCTL &= MRCC_CKSEL_Reset_Mask;
??SetCKSYS_OSC4MPLL_10:
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_OSC4MPLL_0+0x4  ;; 0xff7fffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1534 
// 1535       if((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) == RESET)
        LDR      R0,??SetCKSYS_OSC4MPLL_0  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+16       ;; #+8388608
        ANDS     R1,R1,R0
        SUBS     R0,R1,#+1
        SBCS     R0,R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
// 1536       {
// 1537         return SUCCESS;
        B        ??SetCKSYS_OSC4MPLL_3
// 1538       }
// 1539       else
// 1540       {
// 1541         return ERROR;
// 1542       }
// 1543     }
// 1544   }
// 1545   else 
// 1546   {
// 1547     /* OSC4M disabled or OSC4M clock is not present*/
// 1548     return ERROR;
??SetCKSYS_OSC4MPLL_1:
        MOVS     R0,#+0
??SetCKSYS_OSC4MPLL_3:
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        DATA
??SetCKSYS_OSC4MPLL_0:
        DC32     0x60000020
        DC32     0xff7fffff
        DC32     0xfeffffff
        DC32     0xe7ffffff
        CFI EndBlock cfiBlock34
// 1549   }
// 1550 }
// 1551 
// 1552 /*******************************************************************************
// 1553 * Function Name  : SetCKSYS_RTC
// 1554 * Description    : Selects RTC clock (CK_RTC) as CK_SYS clock source.
// 1555 * Input          : PLL_State: specifies the PLL state.
// 1556 * Output         : None
// 1557 * Return         : An ErrorStatus enumuration value:
// 1558 *                         - SUCCESS: Clock configuration succeeded
// 1559 *                         - ERROR: Clock configuration failed
// 1560 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock35 Using cfiCommon0
        CFI Function SetCKSYS_RTC
        THUMB
// 1561 static ErrorStatus SetCKSYS_RTC(u32 PLL_State)
// 1562 {
SetCKSYS_RTC:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
// 1563   /* Check if CK_RTC clock is enabled and ready to use */
// 1564   if(((MRCC->PWRCTRL & MRCC_CKRTCSEL_Set_Mask) != RESET)||
// 1565      ((MRCC->CLKCTL & MRCC_CKRTCOK_Mask) == RESET))
        LDR      R0,??SetCKSYS_RTC_0  ;; 0x60000028
        LDR      R0,[R0, #+0]
        MOVS     R1,#+192
        LSLS     R1,R1,#+18       ;; #+50331648
        ANDS     R1,R1,R0
        CMP      R1,#+0
        BNE      ??SetCKSYS_RTC_1
        LDR      R0,??SetCKSYS_RTC_0+0x4  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+4
        BMI      ??SetCKSYS_RTC_2
// 1566   {
// 1567 /* Configure CK_RTC as Ck_SYS clock source -----------------------------------*/
// 1568     if((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) == RESET)
??SetCKSYS_RTC_1:
        LDR      R0,??SetCKSYS_RTC_0+0x4  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BMI      ??SetCKSYS_RTC_3
// 1569     { 
// 1570       /* Select CK_PLL1 as CK_SYS clock source */
// 1571       MRCC->CLKCTL &= MRCC_CKSEL_Reset_Mask;
        LDR      R0,??SetCKSYS_RTC_0+0x4  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_RTC_0+0x8  ;; 0xff7fffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_RTC_0+0x4  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1572     
// 1573       /* Set CKOSCSEL bit ----------------------------------------------------*/
// 1574       /* Execute CKOSCSEL bit writing sequence */
// 1575       WriteCKOSCSELBit();
        BL       WriteCKOSCSELBit
// 1576       
// 1577       /* Check if CKOSCSEL is set to 1 */
// 1578       if((MRCC->CLKCTL & MRCC_CKOSCSEL_Set_Mask) == RESET)
        LDR      R0,??SetCKSYS_RTC_0+0x4  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+10
        BMI      ??SetCKSYS_RTC_3
// 1579       {
// 1580          return ERROR;
        MOVS     R0,#+0
        B        ??SetCKSYS_RTC_4
// 1581       }
// 1582     }
// 1583     
// 1584     /* Select CK_OSC as CK_SYS clock source */
// 1585     MRCC->CLKCTL |= MRCC_CKSEL_Set_Mask;          
??SetCKSYS_RTC_3:
        LDR      R0,??SetCKSYS_RTC_0+0x4  ;; 0x60000020
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+16       ;; #+8388608
        ORRS     R1,R1,R0
        LDR      R0,??SetCKSYS_RTC_0+0x4  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1586     
// 1587     if((MRCC->CLKCTL & MRCC_CKSEL_Set_Mask) != RESET)
        LDR      R0,??SetCKSYS_RTC_0+0x4  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+8
        BPL      ??SetCKSYS_RTC_5
// 1588     {
// 1589       if(PLL_State == MRCC_PLL_Disabled)
        LDR      R0,??SetCKSYS_RTC_0+0xC  ;; 0xfeffffff
        CMP      R4,R0
        BNE      ??SetCKSYS_RTC_6
// 1590       {
// 1591         /* Disable PLL */
// 1592         MRCC->CLKCTL &= MRCC_PLLEN_Reset_Mask;
        LDR      R0,??SetCKSYS_RTC_0+0x4  ;; 0x60000020
        LDR      R0,[R0, #+0]
        LDR      R1,??SetCKSYS_RTC_0+0xC  ;; 0xfeffffff
        ANDS     R1,R1,R0
        LDR      R0,??SetCKSYS_RTC_0+0x4  ;; 0x60000020
        STR      R1,[R0, #+0]
// 1593       }
// 1594     
// 1595       return SUCCESS;
??SetCKSYS_RTC_6:
        MOVS     R0,#+1
        B        ??SetCKSYS_RTC_4
// 1596     }
// 1597     else
// 1598     {
// 1599       return ERROR;
??SetCKSYS_RTC_5:
        MOVS     R0,#+0
        B        ??SetCKSYS_RTC_4
// 1600     }    
// 1601   }
// 1602   else
// 1603   {      
// 1604     /* CK_RTC disabled */
// 1605     return ERROR;
??SetCKSYS_RTC_2:
        MOVS     R0,#+0
??SetCKSYS_RTC_4:
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??SetCKSYS_RTC_0:
        DC32     0x60000028
        DC32     0x60000020
        DC32     0xff7fffff
        DC32     0xfeffffff
        CFI EndBlock cfiBlock35
// 1606   }  
// 1607 }
// 1608 
// 1609 /*******************************************************************************
// 1610 * Function Name  : WriteLPBit
// 1611 * Description    : Executes the Low Power bit writing sequence.
// 1612 * Input          : None
// 1613 * Output         : None
// 1614 * Return         : None
// 1615 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock36 Using cfiCommon0
        CFI Function WriteLPBit
        THUMB
// 1616 static void WriteLPBit(void)
// 1617 {
WriteLPBit:
        PUSH     {R0,R4}
        CFI R4 Frame(CFA, -4)
        CFI CFA R13+8
// 1618   u32 Tmp = 0, Tmp1 = 0, Tmp2 = 0;
        MOVS     R3,#+0
        MOVS     R0,R3
        MOVS     R3,#+0
        MOVS     R1,R3
        MOVS     R3,#+0
        MOVS     R2,R3
// 1619 
// 1620   /* Clear LP_DONE flag */
// 1621   MRCC->PWRCTRL &= MRCC_LPDONE_Reset_Mask;
        LDR      R3,??WriteLPBit_0  ;; 0x60000028
        LDR      R3,[R3, #+0]
        MOVS     R4,#+128
        BICS     R3,R3,R4
        LDR      R4,??WriteLPBit_0  ;; 0x60000028
        STR      R3,[R4, #+0]
// 1622 
// 1623   Tmp = MRCC->PWRCTRL;
        LDR      R3,??WriteLPBit_0  ;; 0x60000028
        LDR      R3,[R3, #+0]
        MOVS     R0,R3
// 1624   Tmp1 = Tmp | MRCC_LP_Set_Mask;
        MOVS     R3,#+1
        ORRS     R3,R3,R0
        MOVS     R1,R3
// 1625   Tmp2 = Tmp & MRCC_LP_Reset_Mask;
        LSRS     R3,R0,#+1
        LSLS     R3,R3,#+1
        MOVS     R2,R3
// 1626 
// 1627   /* Set LP bit */
// 1628   MRCC->PWRCTRL = Tmp1;
        LDR      R3,??WriteLPBit_0  ;; 0x60000028
        STR      R1,[R3, #+0]
// 1629 
// 1630   /* Set LP bit */
// 1631   MRCC->PWRCTRL = Tmp1;
        LDR      R3,??WriteLPBit_0  ;; 0x60000028
        STR      R1,[R3, #+0]
// 1632 
// 1633   /* Reset LP bit */
// 1634   MRCC->PWRCTRL = Tmp2;
        LDR      R3,??WriteLPBit_0  ;; 0x60000028
        STR      R2,[R3, #+0]
// 1635 
// 1636   /* Set LP bit */
// 1637   MRCC->PWRCTRL = Tmp1;
        LDR      R3,??WriteLPBit_0  ;; 0x60000028
        STR      R1,[R3, #+0]
// 1638 
// 1639   /* Read LP bit*/
// 1640   Tmp = MRCC->PWRCTRL;  
        LDR      R3,??WriteLPBit_0  ;; 0x60000028
        LDR      R3,[R3, #+0]
        MOVS     R0,R3
// 1641 }
        POP      {R3,R4}
        CFI R4 SameValue
        CFI CFA R13+0
        BX       LR               ;; return
        Nop      
        DATA
??WriteLPBit_0:
        DC32     0x60000028
        CFI EndBlock cfiBlock36
// 1642 
// 1643 /*******************************************************************************
// 1644 * Function Name  : WriteCKOSCSELBit
// 1645 * Description    : Executes the CKOSCSEL bit writing sequence.
// 1646 * Input          : None
// 1647 * Output         : None
// 1648 * Return         : None
// 1649 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock37 Using cfiCommon0
        CFI Function WriteCKOSCSELBit
        THUMB
// 1650 static void WriteCKOSCSELBit(void)
// 1651 {
// 1652   u32 Tmp = 0, Tmp1 = 0, Tmp2 = 0;
WriteCKOSCSELBit:
        MOVS     R3,#+0
        MOVS     R0,R3
        MOVS     R3,#+0
        MOVS     R1,R3
        MOVS     R3,#+0
        MOVS     R2,R3
// 1653 
// 1654   Tmp = MRCC->CLKCTL;
        LDR      R3,??WriteCKOSCSELBit_0  ;; 0x60000020
        LDR      R3,[R3, #+0]
        MOVS     R0,R3
// 1655   Tmp1 = Tmp | MRCC_CKOSCSEL_Set_Mask;
        MOVS     R3,#+128
        LSLS     R3,R3,#+14       ;; #+2097152
        ORRS     R3,R3,R0
        MOVS     R1,R3
// 1656   Tmp2 = Tmp & MRCC_CKOSCSEL_Reset_Mask;
        LDR      R3,??WriteCKOSCSELBit_0+0x4  ;; 0xffdfffff
        ANDS     R3,R3,R0
        MOVS     R2,R3
// 1657 
// 1658   /* Set CKOSCSEL bit */
// 1659   MRCC->CLKCTL = Tmp1;
        LDR      R3,??WriteCKOSCSELBit_0  ;; 0x60000020
        STR      R1,[R3, #+0]
// 1660 
// 1661   /* Set CKOSCSEL bit */
// 1662   MRCC->CLKCTL = Tmp1;
        LDR      R3,??WriteCKOSCSELBit_0  ;; 0x60000020
        STR      R1,[R3, #+0]
// 1663 
// 1664   /* Reset CKOSCSEL bit */
// 1665   MRCC->CLKCTL = Tmp2;
        LDR      R3,??WriteCKOSCSELBit_0  ;; 0x60000020
        STR      R2,[R3, #+0]
// 1666 
// 1667   /* Set CKOSCSEL bit */
// 1668   MRCC->CLKCTL = Tmp1;
        LDR      R3,??WriteCKOSCSELBit_0  ;; 0x60000020
        STR      R1,[R3, #+0]
// 1669   
// 1670   /* Read CKOSCSEL bit */
// 1671   Tmp = MRCC->CLKCTL;
        LDR      R3,??WriteCKOSCSELBit_0  ;; 0x60000020
        LDR      R3,[R3, #+0]
        MOVS     R0,R3
// 1672 }
        BX       LR               ;; return
        DATA
??WriteCKOSCSELBit_0:
        DC32     0x60000020
        DC32     0xffdfffff
        CFI EndBlock cfiBlock37

        END
// 1673 /******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
// 
// 3 378 bytes in section .text
// 
// 3 378 bytes of CODE memory
//
//Errors: none
//Warnings: none
