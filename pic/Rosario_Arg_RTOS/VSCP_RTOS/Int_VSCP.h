//******************************************************************************
//                                                                    
// File name: Int_VSCP.h                                             
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   VSCP with RTOS v1.0 (PICos18 2.10)                                 			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   General VSCP initialization and kernel start            
//                                                                    
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
#ifndef	_Int_VSCP_H_
#define	_Int_VSCP_H_

//******************************************************************************
// Function you want to call when an IT occurs.
//
//******************************************************************************
  extern void AddOneTick(void);
  extern void CAN_INT(void);
  extern void RMT_INT(void);                     
  void InterruptVectorL(void);
  void InterruptVectorH(void);
  extern void _startup (void);

#endif