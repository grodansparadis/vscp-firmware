#include "P18CXXX.INC"
#include "P18MACRO.INC"
	
	GLOBAL  RotateLeftEmData
    GLOBAL  RotateRightEmData
	EXTERN 	EmData
	
	CODE

RotateLeftEmData        
		LFSR		FSR0,EmData + 5
        RLCF		POSTDEC0,F,ACCESS	; EmData[5] << 1
		RLCF		POSTDEC0,F,ACCESS	; EmData[4] << 1
		RLCF		POSTDEC0,F,ACCESS	; EmData[3] << 1
		RLCF		POSTDEC0,F,ACCESS	; EmData[2] << 1
		RLCF		POSTDEC0,F,ACCESS	; EmData[1] << 1
		RLCF		POSTDEC0,F,ACCESS	; EmData[0] << 1
		RETURN    

RotateRightEmData        
		LFSR		FSR0,EmData
		RRCF		POSTINC0,F,ACCESS	; EmData[0] >> 1
		RRCF		POSTINC0,F,ACCESS	; EmData[1] >> 1
		RRCF		POSTINC0,F,ACCESS	; EmData[2] >> 1
		RRCF		POSTINC0,F,ACCESS	; EmData[3] >> 1
		RRCF		POSTINC0,F,ACCESS	; EmData[4] >> 1
        RRCF		POSTINC0,F,ACCESS	; EmData[5] >> 1
		RETURN   

		END

