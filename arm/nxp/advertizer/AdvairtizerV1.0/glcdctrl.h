#ifndef GLCDCTRL_H_
#define GLCDCTRL_H_

/* Value Range 0-100 (read %) */
#define GLCD_CONTRAST_DEFAULT 50
#define PWM_MR5INIT (U32)((PWMTICSperSEC/PWM_FREQ2) + 0.5)  //PWM-LED Backlight

void glcdctrl_backlight_init(void);
void glcdctrl_SetContrast(unsigned int contr);
U8 glcdctrl_GetContrast(void);
void glcdctrl_blink(U8 rep);
#endif
