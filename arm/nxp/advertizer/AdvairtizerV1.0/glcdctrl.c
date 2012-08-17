/**********************************************************************
 * // backlight PWM generation
 * // 44Khz
 ***********************************************************************/
#include <global.h>

static U8 GLCD_contrast;

void glcdctrl_backlight_init(void){
        PINSEL1 = (PINSEL1 & ~(1<<11)) | (1<<10); // PWM5 function select register
	PWMPCR &= ~(1<<5);                        // single edge mode for PWM5
	PWMPCR |= (1<<13);                        // enable outputs for PWM5
                                                  // set power-on values for Contrast and Brightness
	glcdctrl_SetContrast(GLCD_CONTRAST_DEFAULT);
	PWMTCR = PWMCR_ENABLE | (1<<3);           // Counter Enable & PWM Enable
  }
void glcdctrl_blink(U8 rep){
  U8 temp = glcdctrl_GetContrast();
    for (U8 i=0;i<=rep;i++){
      for (U8 ia=0;ia<=100;ia+=10){
        glcdctrl_SetContrast(ia);
        pause(ONE_MS*10);
      }
    }
  glcdctrl_SetContrast(temp);
}
// Parameter: percent duty (75 = on for 75% of MR0, off for 25%)
void glcdctrl_SetContrast(unsigned int contr){
        if (contr>100) contr=100;
	GLCD_contrast = contr;
	PWMMR5 = (PWM_MR5INIT*contr)/100L;
	PWMLER |= (1<<5);                       // Enable PWM Match5 Latch
  }

U8 glcdctrl_GetContrast(void){
	return GLCD_contrast;
  }


