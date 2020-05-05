//******************************************************************************
//                                                                    
// File name: Tasks_Desc.c                                              
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   VSCP with RTOS v1.0 (PICos18 2.10)                                 			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   Kind of OIL file where all the features of the tasks    
//            are described.                                          
//                                                                    
//           rom_desc_tsk foo = {                                     
//             0x..,    Priority level [0:15],15 the most significant 
//             0x....,  Stack address                                 
//             0x....,  Start adress of the task                      
//             0x..,    Task state at start [RUN/WAIT/READY/SUSPEND]  
//             0x..,    Identification number of the task             
//             0x....   Stack size                                    
//             };                                                     
//                                                                    
// History:                                                           
// 2007/03/07 [MAP] Create this file from PICos18 Tutorial            
//                                                                    
//            PICos18 is free software; you can redistribute it       
//            and/or modify it under the terms of the GNU General     
//            Public License as published by the Free Software        
//            Foundation; either version 2, or (at your option)       
//            any later version.                                      
//                                                                    
//            PICos18 is distributed in the hope that it will be      
//            useful, but WITHOUT ANY WARRANTY; without even the      
//            implied warranty of MERCHANTABILITY or FITNESS FOR A    
//            PARTICULAR PURPOSE.  See the GNU General Public         
//            License for more details.                               
//                                                                    
//            You should have received a copy of the GNU General      
//            Public License along with gpsim; see the file           
//            COPYING.txt. If not, write to the Free Software         
//            Foundation, 59 Temple Place - Suite 330,                
//            Boston, MA 02111-1307, USA.                             
//                                                                    
//          > A special exception to the GPL can be applied should    
//            you wish to distribute a combined work that includes    
//            PICos18, without being obliged to provide the source    
//            code for any proprietary components.                    
//******************************************************************************

#include "Define_VSCP.h"
#include <TypeDefs.h>


/**********************************************************************
 * --------------------- COUNTER & ALARM DEFINITION -------------------
 **********************************************************************/
Counter Counter_list[] = 
  {
   /*******************************************************************
    * -------------------------- First counter ------------------------
    *******************************************************************/
   {
     {
       200,                                /* maxAllowedValue        */
        10,                                /* ticksPerBase           */
       100                                 /* minCycle               */
     },
     0,                                    /* CounterValue           */
     0                                     /* Nbr of Tick for 1 CPT  */
   }
  };

Counter Counter_kernel = 
  {
    {
      65535,                              /* maxAllowedValue        */
          1,                              /* ticksPerBase           */
          0                               /* minCycle               */
    },
    0,                                    /* CounterValue           */
    0                                     /* Nbr of Tick for 1 CPT  */
  };

AlarmObject Alarm_list[] = 
  {
   /*******************************************************************
    * -------------------------- Alarm Led ---------------------------
    *******************************************************************/
   {
     OFF,                                  /* State                   */
     ALARM_LED,                            /* AlarmValue              */
     0,                                    /* Cycle                   */
     &Counter_kernel,                      /* ptrCounter              */
     VSCP_INIT_ID,                         /* TaskIDActivate          */
     LED_EVENT,		                         /* EventToPost             */
     0                                     /* CallBack                */
   },
   
      /*******************************************************************
    * -------------------------- Alarm INIT ---------------------------
    *******************************************************************/
   {
     OFF,                                  /* State                   */
     ALARM_INIT,                           /* AlarmValue              */
     0,                                    /* Cycle                   */
     &Counter_kernel,                      /* ptrCounter              */
     VSCP_INIT_ID,									       /* TaskIDActivate          */
     INIT_EVENT,	                         /* EventToPost             */
     0                                     /* CallBack                */
   },

      /*******************************************************************
    * -------------------------- Alarm PROTOCOL-------------------------
    *******************************************************************/
   {
     OFF,                                  /* State                   */
     ALARM_PROT,                           /* AlarmValue              */
     0,                                    /* Cycle                   */
     &Counter_kernel,                      /* ptrCounter              */
     VSCP_PROT_ID,									       /* TaskIDActivate          */
     PROT_EVENT,		                       /* EventToPost             */
     0                                     /* CallBack                */
   },
   
   /*******************************************************************
    * -------------------------- Alarm Loop ---------------------------
    *******************************************************************/
   {
     OFF,                                  /* State                   */
     ALARM_LOOP,                           /* AlarmValue              */
     0,                                    /* Cycle                   */
     &Counter_kernel,                      /* ptrCounter              */
     VSCP_LOOP_ID,                         /* TaskIDActivate          */
     TIC_EVENT,		                         /* EventToPost             */
     0                                     /* CallBack                */
   },
   

 };

#define _ALARMNUMBER_          sizeof(Alarm_list)/sizeof(AlarmObject)
#define _COUNTERNUMBER_        sizeof(Counter_list)/sizeof(Counter)
uint08_t ALARMNUMBER    = _ALARMNUMBER_;
uint08_t COUNTERNUMBER  = _COUNTERNUMBER_;
uint32_t global_counter = 0;


/**********************************************************************
 * --------------------- COUNTER & ALARM DEFINITION -------------------
 **********************************************************************/
Resource Resource_list[] = 
  {
   {
      RESOURCE_EE_PRIO,                  /* priority           */
       0,                                /* Task prio          */
       0,                                /* lock               */
   }
  };
  
#define _RESOURCENUMBER_       sizeof(Resource_list)/sizeof(Resource)
uint08_t RESOURCENUMBER = _RESOURCENUMBER_;

/**********************************************************************
 * ----------------------- TASK & STACK DEFINITION --------------------
 **********************************************************************/
#define DEFAULT_STACK_SIZE      128

DeclareTask(VSCP_InitAddress);
DeclareTask(VSCP_ProtEngine);
DeclareTask(VSCP_Loop);
DeclareTask(VSCP_DrvCAN);

#ifdef RMT_DRV              //include only if remote control decoder is present
	DeclareTask(RMT_Drv);
#endif

// to avoid any C18 map error : regroup the stacks into blocks
// of 256 bytes (except the last one).

#pragma		udata      STACK_A
volatile uint08_t stack_can_drv[2*DEFAULT_STACK_SIZE];
#pragma		udata      STACK_B
volatile uint08_t stack_init[DEFAULT_STACK_SIZE];
volatile uint08_t stack_prot[DEFAULT_STACK_SIZE];
#pragma		udata      STACK_C
volatile uint08_t stack_loop[DEFAULT_STACK_SIZE];
/////////// remote control driver ifdef
#ifdef RMT_DRV              //include only if remote control decoder is present
	volatile uint08_t stack_rmt_drv[DEFAULT_STACK_SIZE];
#endif
/////////// remote control driver ifdef

/**********************************************************************
 * ---------------------- TASK DESCRIPTOR SECTION ---------------------
 **********************************************************************/
#pragma		romdata		DESC_ROM
const rom uint16_t descromarea;
/**********************************************************************
// * ----------------------- VSCP_INIT task -----------------------------
// **********************************************************************/
rom_desc_tsk rom_desc_VSCP_Init = {
	VSCP_INIT_PRIO,                    /* prioinit from 0 to 15       */
	stack_init,                        /* stack address (16 bits)     */
	VSCP_InitAddress,                  /* start address (16 bits)     */
	READY,		                         /* state at init phase         */
	VSCP_INIT_ID,                      /* id_tsk from 1 to 15         */
	sizeof(stack_init)                 /* stack size    (16 bits)     */
};

/**********************************************************************
 * ------------------------ VSCP_PROTOCOL task ------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_VSCP_Prot = {
	VSCP_PROT_PRIO,                     /* prioinit from 0 to 15       */
	stack_prot,                        	/* stack address (16 bits)     */
	VSCP_ProtEngine,                    /* start address (16 bits)     */
	SUSPENDED,                          /* state at init phase         */
	VSCP_PROT_ID,                      	/* id_tsk from 1 to 15         */
	sizeof(stack_prot)                  /* stack size    (16 bits)     */
};

///**********************************************************************
// * ------------------------- VSCP_LOOP task -------------------------
// **********************************************************************/
rom_desc_tsk rom_desc_VSCP_LOOP = {
	VSCP_LOOP_PRIO,                /* prioinit from 0 to 15       */
	stack_loop,                    /* stack address (16 bits)     */
	VSCP_Loop,                     /* start address (16 bits)     */
	SUSPENDED,		                 /* state at init phase         */
	VSCP_LOOP_ID,                  /* id_tsk from 0 to 15         */
	sizeof(stack_loop)             /* ctx address   (16 bits)     */
};

/**********************************************************************
 * ------------------------- VSCP_DRIVER task -------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_VSCP_DRV = {
	VSCP_DRV_PRIO,                 /* prioinit from 0 to 15       */
	stack_can_drv,                 /* stack address (16 bits)     */
	VSCP_DrvCAN,                   /* start address (16 bits)     */
	READY,		                     /* state at init phase         */
	VSCP_DRV_ID,                   /* id_tsk from 0 to 15         */
	sizeof(stack_can_drv)          /* ctx address   (16 bits)     */
};

/////////// remote control driver ifdef
#ifdef RMT_DRV              //include only if remote control decoder is present

				/**********************************************************************
				 * ------------------------- REMOTE_DRIVER task -------------------------
				 **********************************************************************/
				rom_desc_tsk rom_desc_RMT_DRV = {
					RMT_DRV_PRIO,                  /* prioinit from 0 to 15       */
					stack_rmt_drv,                 /* stack address (16 bits)     */
					RMT_Drv,											 /* start address (16 bits)     */
					READY,		                 		 /* state at init phase         */
					RMT_DRV_ID,                    /* id_tsk from 0 to 15         */
					sizeof(stack_rmt_drv)          /* ctx address   (16 bits)     */
				};
#endif	
/////////// remote control driver ifdef

/**********************************************************************
 * --------------------- END TASK DESCRIPTOR SECTION ------------------
 **********************************************************************/
rom_desc_tsk end = {
	0x00,                              /* prioinit from 0 to 15       */
	0x00,                              /* stack address               */
	0x0000,                            /* adr_tsk in 16 bits          */
	0x00,                              /* state at init phase         */
	0x00,                              /* id_tsk from 1 to 15         */
	0x0000                             /* stack size    (16 bits)     */
};

volatile rom uint16_t * taskdesc_addr = (&(descromarea)+1);
	
/* End of File : taskdesc.c */
