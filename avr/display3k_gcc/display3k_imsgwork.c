// (C) 2010 Stefan Langer

switch (vscp_imsg.class)
	{
	case VSCP_CLASS1_MEASUREMENT:
		{
		switch (vscp_imsg.type)
			{
			case VSCP_TYPE_MEASUREMENT_ELECTRIC_CURRENT:
				{
				//receviec one of my current measurements
				sprintf(txt, "ADST=%hu", vscp_imsg.data[5]);
				LCD_Print(txt, 8, 54, 1, 1, 1, red, white);
				//LCD_Print(txt, 1, 64, 1, 1, 1, red, white);
				//read current from message and convert to integer
				array_read(vscp_imsg.data, 1, 4, (void*)&sens_current_raw);
				sprintf(txt, "ADRW=%ld      ", sens_current_raw);
				LCD_Print(txt, 64, 54, 1, 1, 1, red, white);
				//LCD_Print(txt, 48, 64, 1, 1, 1, red, white);
				//now convert to real current
				sens_current=sens_current_raw; // type cast
				sens_current=(sens_current-sens_current_off)-((sens_temp-21)*sens_current_offslope);
				sens_current*=sens_current_rng;
				if (sens_current<0)
					{
					sprintf_P(txt,PSTR("Q=%0.3f mW/cm2      "), sens_current);
					}
					else
					{
					sprintf_P(txt,PSTR("Q=+%0.3f mW/cm2      "), sens_current);
					}
				LCD_Print(txt, 8, 64, 2, 1, 2, blue, white);
				}
				break;
			case VSCP_TYPE_MEASUREMENT_TEMPERATURE:
				{
				//receive temp measurement
				array_read(vscp_imsg.data, 1, 2, (void*)&sens_temp_raw);
				sprintf(txt, "TMRW=%ld      ", sens_temp_raw);
				LCD_Print(txt, 8, 104, 1, 1, 1, red, white);
				sens_temp=sens_temp_raw; // type cast
				sens_temp=((sens_temp-169.6)*sens_temp_rng)+sens_temp_off;
				if (sens_temp<0)
					{
					sprintf_P(txt,PSTR("T=%.1f DegC  "), sens_temp);
					}
					else
					{
					sprintf_P(txt,PSTR("T=+%.1f DegC  "), sens_temp);
					}
				LCD_Print(txt, 8, 114, 2, 1, 1, blue, white);
				}
				break;
			}
		}
		break;
	}
