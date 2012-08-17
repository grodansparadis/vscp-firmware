///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:21 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\gediminas\CAN4USB\project\app\75x_it.c   /
//    Command line =  D:\development\gediminas\CAN4USB\project\app\75x_it.c   /
//                    -lC D:\development\gediminas\CAN4USB\project\RELEASE\Li /
//                    st\ -lA D:\development\gediminas\CAN4USB\project\RELEAS /
//                    E\List\ -o D:\development\gediminas\CAN4USB\project\REL /
//                    EASE\Obj\ --no_cse --no_unroll --no_inline              /
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
//                    5x_it.s                                                 /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME `75x_it`

        RTMODEL "__dlib_file_descriptor", "0"

        EXTERN CAN_ISR
        EXTERN TB_ClearFlag
        EXTERN Tim0Handler
        EXTERN Tim1Handler
        EXTERN USB_HIGH_ISR
        EXTERN USB_ISR
        EXTERN blink
        EXTERN sysTICs

        PUBLIC ADC_IRQHandler
        PUBLIC Abort_Handler
        PUBLIC CAN_IRQHandler
        PUBLIC DMA_IRQHandler
        PUBLIC EXTIT_IRQHandler
        PUBLIC FIQ_Handler
        PUBLIC FLASHSMI_IRQHandler
        PUBLIC I2C_IRQHandler
        PUBLIC MRCC_IRQHandler
        PUBLIC PWM_EM_IRQHandler
        PUBLIC PWM_OC123_IRQHandler
        PUBLIC PWM_UP_IRQHandler
        PUBLIC Prefetch_Handler
        PUBLIC RTC_IRQHandler
        PUBLIC SSP0_IRQHandler
        PUBLIC SSP1_IRQHandler
        PUBLIC SWI_Handler
        PUBLIC TB_IRQHandler
        PUBLIC TIM0_IC12_IRQHandler
        PUBLIC TIM0_OC1_IRQHandler
        PUBLIC TIM0_OC2_IRQHandler
        PUBLIC TIM0_UP_IRQHandler
        PUBLIC TIM1_IC12_IRQHandler
        PUBLIC TIM1_OC1_IRQHandler
        PUBLIC TIM1_OC2_IRQHandler
        PUBLIC TIM1_UP_IRQHandler
        PUBLIC TIM2_IC12_IRQHandler
        PUBLIC TIM2_OC1_IRQHandler
        PUBLIC TIM2_OC2_IRQHandler
        PUBLIC TIM2_UP_IRQHandler
        PUBLIC UART0_IRQHandler
        PUBLIC UART1_IRQHandler
        PUBLIC UART2_IRQHandler
        PUBLIC USB_HP_IRQHandler
        PUBLIC USB_LP_IRQHandler
        PUBLIC Undefined_Handler
        PUBLIC WAKUP_IRQHandler
        
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
        
// D:\development\gediminas\CAN4USB\project\app\75x_it.c
//    1 /******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
//    2 * File Name          : 75x_it.c
//    3 * Author             : MCD Application Team
//    4 * Date First Issued  : 03/10/2006
//    5 * Description        : Main Interrupt Service Routines.
//    6 *                      This file can be used to describe all the exceptions
//    7 *                      subroutines that may occur within user application.
//    8 *                      When an interrupt happens, the software will branch
//    9 *                      automatically to the corresponding routine according
//   10 *                      to the interrupt vector loaded in the PC register.
//   11 *                      The following routines are all empty, user can write code
//   12 *                      for exceptions handlers and peripherals IRQ interrupts.
//   13 ********************************************************************************
//   14 * History:
//   15 * 07/17/2006 : V1.0
//   16 * 03/10/2006 : V0.1
//   17 ********************************************************************************
//   18 * THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
//   19 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
//   20 * AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
//   21 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
//   22 * CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
//   23 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
//   24 *******************************************************************************/
//   25 
//   26 /* Includes ------------------------------------------------------------------*/
//   27 #include "75x_lib.h"
//   28 #include "includes.h"
//   29 
//   30 extern void Tim1Handler( void );
//   31 extern void Tim0Handler( void );
//   32 extern void USB_ISR( void );
//   33 extern void USB_HIGH_ISR( void );
//   34 //extern void Uart0Isr( void );
//   35 //extern void Uart1Isr( void );
//   36 //extern void Uart2Isr( void );
//   37 extern void CAN_ISR( void );
//   38 extern void blink( void );
//   39 
//   40 extern Int32U sysTICs;
//   41 
//   42 /* Private typedef -----------------------------------------------------------*/
//   43 /* Private define ------------------------------------------------------------*/
//   44 /* Private macro -------------------------------------------------------------*/
//   45 /* Private variables ---------------------------------------------------------*/
//   46 
//   47 /* Private function prototypes -----------------------------------------------*/
//   48 /* Private functions ---------------------------------------------------------*/
//   49 
//   50 /*******************************************************************************
//   51 * Function Name  : Undefined_Handler
//   52 * Description    : This function handles Undefined instruction exception.
//   53 * Input          : None
//   54 * Output         : None
//   55 * Return         : None
//   56 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function Undefined_Handler
        THUMB
//   57 void Undefined_Handler(void)
//   58 {
//   59 }
Undefined_Handler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock0
//   60 
//   61 /*******************************************************************************
//   62 * Function Name  : FIQ_Handler
//   63 * Description    : This function handles FIQ exception.
//   64 * Input          : None
//   65 * Output         : None
//   66 * Return         : None
//   67 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function FIQ_Handler
        THUMB
//   68 void FIQ_Handler(void)
//   69 {
//   70 }
FIQ_Handler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock1
//   71 
//   72 /*******************************************************************************
//   73 * Function Name  : SWI_Handler
//   74 * Description    : This function handles SW exception.
//   75 * Input          : None
//   76 * Output         : None
//   77 * Return         : None
//   78 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function SWI_Handler
        THUMB
//   79 void SWI_Handler(void)
//   80 {
//   81 }
SWI_Handler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock2
//   82 
//   83 /*******************************************************************************
//   84 * Function Name  : Prefetch_Handler
//   85 * Description    : This function handles preftetch abort exception.
//   86 * Input          : None
//   87 * Output         : None
//   88 * Return         : None
//   89 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function Prefetch_Handler
        THUMB
//   90 void Prefetch_Handler(void)
//   91 {
//   92 }
Prefetch_Handler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock3
//   93 
//   94 /*******************************************************************************
//   95 * Function Name  : Abort_Handler
//   96 * Description    : This function handles data abort exception.
//   97 * Input          : None
//   98 * Output         : None
//   99 * Return         : None
//  100 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function Abort_Handler
        THUMB
//  101 void Abort_Handler(void)
//  102 {
//  103 }
Abort_Handler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock4
//  104 
//  105 /*******************************************************************************
//  106 * Function Name  : WAKUP_IRQHandler
//  107 * Description    : This function handles External line 15(WAKUP) interrupt
//  108 *                  request.
//  109 * Input          : None
//  110 * Output         : None
//  111 * Return         : None
//  112 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function WAKUP_IRQHandler
        THUMB
//  113 void WAKUP_IRQHandler(void)
//  114 {
//  115 }
WAKUP_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock5
//  116 
//  117 /*******************************************************************************
//  118 * Function Name  : TIM2_OC2_IRQHandler
//  119 * Description    : This function handles TIM2 Output Compare 2 interrupt request.
//  120 * Input          : None
//  121 * Output         : None
//  122 * Return         : None
//  123 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function TIM2_OC2_IRQHandler
        THUMB
//  124 void TIM2_OC2_IRQHandler(void)
//  125 {
//  126 }
TIM2_OC2_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock6
//  127 
//  128 /*******************************************************************************
//  129 * Function Name  : TIM2_OC1_IRQHandler
//  130 * Description    : This function handles TIM2 Output Compare 1 interrupt request.
//  131 * Input          : None
//  132 * Output         : None
//  133 * Return         : None
//  134 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function TIM2_OC1_IRQHandler
        THUMB
//  135 void TIM2_OC1_IRQHandler(void)
//  136 {
//  137 }
TIM2_OC1_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock7
//  138 
//  139 /*******************************************************************************
//  140 * Function Name  : TIM2_IC12_IRQHandler
//  141 * Description    : This function handles TIM2 Input Capture 1 & 2 interrupt
//  142 *                  request.
//  143 * Input          : None
//  144 * Output         : None
//  145 * Return         : None
//  146 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function TIM2_IC12_IRQHandler
        THUMB
//  147 void TIM2_IC12_IRQHandler(void)
//  148 {
//  149 }
TIM2_IC12_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock8
//  150 
//  151 /*******************************************************************************
//  152 * Function Name  : TIM2_UP_IRQHandler
//  153 * Description    : This function handles TIM2 Update interrupt request.
//  154 * Input          : None
//  155 * Output         : None
//  156 * Return         : None
//  157 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function TIM2_UP_IRQHandler
        THUMB
//  158 void TIM2_UP_IRQHandler(void)
//  159 {
//  160 }
TIM2_UP_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock9
//  161 
//  162 /*******************************************************************************
//  163 * Function Name  : TIM1_OC2_IRQHandler
//  164 * Description    : This function handles TIM1 Output Compare 2 interrupt request.
//  165 * Input          : None
//  166 * Output         : None
//  167 * Return         : None
//  168 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon0
        CFI Function TIM1_OC2_IRQHandler
        THUMB
//  169 void TIM1_OC2_IRQHandler(void)
//  170 {
//  171 }
TIM1_OC2_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock10
//  172 
//  173 /*******************************************************************************
//  174 * Function Name  : TIM1_OC1_IRQHandler
//  175 * Description    : This function handles TIM1 Output Compare 1 interrupt request.
//  176 * Input          : None
//  177 * Output         : None
//  178 * Return         : None
//  179 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon0
        CFI Function TIM1_OC1_IRQHandler
        THUMB
//  180 void TIM1_OC1_IRQHandler(void)
//  181 {
//  182 }
TIM1_OC1_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock11
//  183 
//  184 /*******************************************************************************
//  185 * Function Name  : TIM1_IC12_IRQHandler
//  186 * Description    : This function handles TIM1 Input Capture 1 & 2 interrupt
//  187 *                  request.
//  188 * Input          : None
//  189 * Output         : None
//  190 * Return         : None
//  191 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock12 Using cfiCommon0
        CFI Function TIM1_IC12_IRQHandler
        THUMB
//  192 void TIM1_IC12_IRQHandler(void)
//  193 {
//  194 }
TIM1_IC12_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock12
//  195 
//  196 /*******************************************************************************
//  197 * Function Name  : TIM1_UP_IRQHandler
//  198 * Description    : This function handles TIM1 Update interrupt request.
//  199 * Input          : None
//  200 * Output         : None
//  201 * Return         : None
//  202 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock13 Using cfiCommon0
        CFI Function TIM1_UP_IRQHandler
        THUMB
//  203 void TIM1_UP_IRQHandler(void)
//  204 {
TIM1_UP_IRQHandler:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  205   Tim1Handler();
        BL       Tim1Handler
//  206 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock13
//  207 
//  208 /*******************************************************************************
//  209 * Function Name  : TIM0_OC2_IRQHandler
//  210 * Description    : This function handles TIM0 Output Compare 2 interrupt request.
//  211 * Input          : None
//  212 * Output         : None
//  213 * Return         : None
//  214 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock14 Using cfiCommon0
        CFI Function TIM0_OC2_IRQHandler
        THUMB
//  215 void TIM0_OC2_IRQHandler(void)
//  216 {
//  217 }
TIM0_OC2_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock14
//  218 
//  219 /*******************************************************************************
//  220 * Function Name  : TIM0_OC1_IRQHandler
//  221 * Description    : This function handles TIM0 Output Compare 1 interrupt request.
//  222 * Input          : None
//  223 * Output         : None
//  224 * Return         : None
//  225 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock15 Using cfiCommon0
        CFI Function TIM0_OC1_IRQHandler
        THUMB
//  226 void TIM0_OC1_IRQHandler(void)
//  227 {
//  228 }
TIM0_OC1_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock15
//  229 
//  230 /*******************************************************************************
//  231 * Function Name  : TIM0_IC12_IRQHandler
//  232 * Description    : This function handles TIM0 Input Capture 1 & 2 interrupt
//  233 *                  request.
//  234 * Input          : None
//  235 * Output         : None
//  236 * Return         : None
//  237 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock16 Using cfiCommon0
        CFI Function TIM0_IC12_IRQHandler
        THUMB
//  238 void TIM0_IC12_IRQHandler(void)
//  239 {
//  240 }
TIM0_IC12_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock16
//  241 
//  242 /*******************************************************************************
//  243 * Function Name  : TIM0_UP_IRQHandler
//  244 * Description    : This function handles TIM0 Update interrupt request.
//  245 * Input          : None
//  246 * Output         : None
//  247 * Return         : None
//  248 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock17 Using cfiCommon0
        CFI Function TIM0_UP_IRQHandler
        THUMB
//  249 void TIM0_UP_IRQHandler(void)
//  250 {
TIM0_UP_IRQHandler:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  251   Tim0Handler();
        BL       Tim0Handler
//  252 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock17
//  253 
//  254 /*******************************************************************************
//  255 * Function Name  : PWM_OC123_IRQHandler
//  256 * Description    : This function handles PWM Output Compare 1,2&3 interrupt
//  257 *                  request.
//  258 * Input          : None
//  259 * Output         : None
//  260 * Return         : None
//  261 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock18 Using cfiCommon0
        CFI Function PWM_OC123_IRQHandler
        THUMB
//  262 void PWM_OC123_IRQHandler(void)
//  263 {
//  264 }
PWM_OC123_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock18
//  265 
//  266 /*******************************************************************************
//  267 * Function Name  : PWM_EM_IRQHandler
//  268 * Description    : This function handles PWM Emergency interrupt request.
//  269 * Input          : None
//  270 * Output         : None
//  271 * Return         : None
//  272 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock19 Using cfiCommon0
        CFI Function PWM_EM_IRQHandler
        THUMB
//  273 void PWM_EM_IRQHandler(void)
//  274 {
//  275 }
PWM_EM_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock19
//  276 
//  277 /*******************************************************************************
//  278 * Function Name  : PWM_UP_IRQHandler
//  279 * Description    : This function handles PWM Update interrupt request.
//  280 * Input          : None
//  281 * Output         : None
//  282 * Return         : None
//  283 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock20 Using cfiCommon0
        CFI Function PWM_UP_IRQHandler
        THUMB
//  284 void PWM_UP_IRQHandler(void)
//  285 {
//  286 }
PWM_UP_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock20
//  287 
//  288 /*******************************************************************************
//  289 * Function Name  : I2C_IRQHandler
//  290 * Description    : This function handles I2C global interrupt request.
//  291 * Input          : None
//  292 * Output         : None
//  293 * Return         : None
//  294 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock21 Using cfiCommon0
        CFI Function I2C_IRQHandler
        THUMB
//  295 void I2C_IRQHandler(void)
//  296 {
//  297 }
I2C_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock21
//  298 
//  299 /*******************************************************************************
//  300 * Function Name  : SSP1_IRQHandler
//  301 * Description    : This function handles SSP1 interrupt request.
//  302 * Input          : None
//  303 * Output         : None
//  304 * Return         : None
//  305 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock22 Using cfiCommon0
        CFI Function SSP1_IRQHandler
        THUMB
//  306 void SSP1_IRQHandler(void)
//  307 {
//  308 }
SSP1_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock22
//  309 
//  310 /*******************************************************************************
//  311 * Function Name  : SSP0_IRQHandler
//  312 * Description    : This function handles SSP0 interrupt request.
//  313 * Input          : None
//  314 * Output         : None
//  315 * Return         : None
//  316 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock23 Using cfiCommon0
        CFI Function SSP0_IRQHandler
        THUMB
//  317 void SSP0_IRQHandler(void)
//  318 {
//  319 }
SSP0_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock23
//  320 
//  321 /*******************************************************************************
//  322 * Function Name  : UART2_IRQHandler
//  323 * Description    : This function handles UART2 global interrupt request.
//  324 * Input          : None
//  325 * Output         : None
//  326 * Return         : None
//  327 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock24 Using cfiCommon0
        CFI Function UART2_IRQHandler
        THUMB
//  328 void UART2_IRQHandler(void)
//  329 {
//  330   //Uart2Isr();
//  331 }
UART2_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock24
//  332 
//  333 /*******************************************************************************
//  334 * Function Name  : UART1_IRQHandler
//  335 * Description    : This function handles UART1 global interrupt request.
//  336 * Input          : None
//  337 * Output         : None
//  338 * Return         : None
//  339 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock25 Using cfiCommon0
        CFI Function UART1_IRQHandler
        THUMB
//  340 void UART1_IRQHandler(void)
//  341 {
//  342   //Uart1Isr();
//  343 }
UART1_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock25
//  344 
//  345 /*******************************************************************************
//  346 * Function Name  : UART0_IRQHandler
//  347 * Description    : This function handles UART0 global interrupt request.
//  348 * Input          : None
//  349 * Output         : None
//  350 * Return         : None
//  351 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock26 Using cfiCommon0
        CFI Function UART0_IRQHandler
        THUMB
//  352 void UART0_IRQHandler(void)
//  353 {
//  354   //Uart0Isr();
//  355 }
UART0_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock26
//  356 
//  357 /*******************************************************************************
//  358 * Function Name  : CAN_IRQHandler
//  359 * Description    : This function handles CAN global interrupt request.
//  360 * Input          : None
//  361 * Output         : None
//  362 * Return         : None
//  363 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock27 Using cfiCommon0
        CFI Function CAN_IRQHandler
        THUMB
//  364 void CAN_IRQHandler(void)
//  365 {
CAN_IRQHandler:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  366   CAN_ISR();
        BL       CAN_ISR
//  367 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock27
//  368 
//  369 /*******************************************************************************
//  370 * Function Name  : USBLP_IRQHandler
//  371 * Description    : This function handles USB Low Priority event interrupt
//  372 *                  request.
//  373 * Input          : None
//  374 * Output         : None
//  375 * Return         : None
//  376 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock28 Using cfiCommon0
        CFI Function USB_LP_IRQHandler
        THUMB
//  377 void USB_LP_IRQHandler(void)
//  378 {
USB_LP_IRQHandler:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  379   USB_ISR();
        BL       USB_ISR
//  380 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock28
//  381 
//  382 /*******************************************************************************
//  383 * Function Name  : USBHP_IRQHandler
//  384 * Description    : This function handles USB High Priority event interrupt
//  385 *                  request.
//  386 * Input          : None
//  387 * Output         : None
//  388 * Return         : None
//  389 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock29 Using cfiCommon0
        CFI Function USB_HP_IRQHandler
        THUMB
//  390 void USB_HP_IRQHandler(void)
//  391 {
USB_HP_IRQHandler:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  392   USB_HIGH_ISR();
        BL       USB_HIGH_ISR
//  393 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock29
//  394 
//  395 /*******************************************************************************
//  396 * Function Name  : ADC_IRQHandler
//  397 * Description    : This function handles ADC global interrupt request.
//  398 * Input          : None
//  399 * Output         : None
//  400 * Return         : None
//  401 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock30 Using cfiCommon0
        CFI Function ADC_IRQHandler
        THUMB
//  402 void ADC_IRQHandler(void)
//  403 {
//  404 }
ADC_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock30
//  405 
//  406 /*******************************************************************************
//  407 * Function Name  : DMA_IRQHandler
//  408 * Description    : This function handles DMA global interrupt request.
//  409 * Input          : None
//  410 * Output         : None
//  411 * Return         : None
//  412 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock31 Using cfiCommon0
        CFI Function DMA_IRQHandler
        THUMB
//  413 void DMA_IRQHandler(void)
//  414 {
//  415 }
DMA_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock31
//  416 
//  417 /*******************************************************************************
//  418 * Function Name  : EXTIT_IRQHandler
//  419 * Description    : This function handles External lines 14 to 1 interrupt request.
//  420 * Input          : None
//  421 * Output         : None
//  422 * Return         : None
//  423 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock32 Using cfiCommon0
        CFI Function EXTIT_IRQHandler
        THUMB
//  424 void EXTIT_IRQHandler(void)
//  425 {
//  426 }
EXTIT_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock32
//  427 
//  428 /*******************************************************************************
//  429 * Function Name  : MRCC_IRQHandler
//  430 * Description    : This function handles MRCC interrupt request.
//  431 * Input          : None
//  432 * Output         : None
//  433 * Return         : None
//  434 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock33 Using cfiCommon0
        CFI Function MRCC_IRQHandler
        THUMB
//  435 void MRCC_IRQHandler(void)
//  436 {
//  437 }
MRCC_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock33
//  438 
//  439 /*******************************************************************************
//  440 * Function Name  : FLASHSMI_IRQHandler
//  441 * Description    : This function handles Flash and SMI global interrupt request.
//  442 * Input          : None
//  443 * Output         : None
//  444 * Return         : None
//  445 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock34 Using cfiCommon0
        CFI Function FLASHSMI_IRQHandler
        THUMB
//  446 void FLASHSMI_IRQHandler(void)
//  447 {
//  448 }
FLASHSMI_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock34
//  449 
//  450 /*******************************************************************************
//  451 * Function Name  : RTC_IRQHandler
//  452 * Description    : This function handles RTC global interrupt request.
//  453 * Input          : None
//  454 * Output         : None
//  455 * Return         : None
//  456 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock35 Using cfiCommon0
        CFI Function RTC_IRQHandler
        THUMB
//  457 void RTC_IRQHandler(void)
//  458 {
//  459 }
RTC_IRQHandler:
        BX       LR               ;; return
        CFI EndBlock cfiBlock35
//  460 
//  461 /*******************************************************************************
//  462 * Function Name  : TB_IRQHandler
//  463 * Description    : This function handles TB global interrupt request.
//  464 * Input          : None
//  465 * Output         : None
//  466 * Return         : None
//  467 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock36 Using cfiCommon0
        CFI Function TB_IRQHandler
        THUMB
//  468 void TB_IRQHandler(void)
//  469 {
TB_IRQHandler:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  470   sysTICs++;
        LDR      R0,??TB_IRQHandler_0  ;; sysTICs
        LDR      R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??TB_IRQHandler_0  ;; sysTICs
        STR      R0,[R1, #+0]
//  471   blink();
        BL       blink
//  472   TB_ClearFlag( TB_FLAG_Update );
        MOVS     R0,#+1
        BL       TB_ClearFlag
//  473 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??TB_IRQHandler_0:
        DC32     sysTICs
        CFI EndBlock cfiBlock36

        END
//  474 
//  475 /******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
// 
// 154 bytes in section .text
// 
// 154 bytes of CODE memory
//
//Errors: none
//Warnings: none
