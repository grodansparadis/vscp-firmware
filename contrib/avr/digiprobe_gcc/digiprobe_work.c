/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	Application specific worker tasks
*******************************************************************************/

#include "digiprobe_gcc.inc"

void do_work(void)
{
	//do current measurement whenever adc is ready
	if (mcp355xs_read(&sens_current_raw, &adstatus, MCP355X_CONTINOUS))
		{
		//there was really a measurement taken!
		#ifdef DIGIPROBE_USE_FLOAT
			//dbl1 = sens_current_raw;
			dbl2 = (double)sens_current_raw * 1.54123123;
			dbl1 = dbl2 - 3.5452234;
			comp = dbl1;
			accu = dbl1 * 3.141529;
		#endif

		// Do something useful, send a message every second
		vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
		vscp_omsg.flags = VSCP_VALID_MSG + 6;
		vscp_omsg.vscp_class = VSCP_CLASS1_MEASUREMENT;
		vscp_omsg.vscp_type =VSCP_TYPE_MEASUREMENT_ELECTRIC_CURRENT;
		vscp_omsg.data[ 0 ] = 0x60; //data coding integer, default unit and index
		array_write(vscp_omsg.data, 1, 4, (void*)&sens_current_raw);
		vscp_omsg.data[ 5 ] = adstatus;
		vscp_sendEvent();
		//

		if (clock_temp >= 4) {
			clock_temp = 0;
			sens_temp_raw = 0;

			for (samples = 1; samples < 17; samples++)
				{
				sens_temp_raw += adc_read(SENS_TMP_ADCCHN);
				};
			sens_temp_raw = sens_temp_raw / samples;
	
			// Do something useful, send a message every second
			vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
			vscp_omsg.flags = VSCP_VALID_MSG + 3;
			vscp_omsg.vscp_class = VSCP_CLASS1_MEASUREMENT;
			vscp_omsg.vscp_type =VSCP_TYPE_MEASUREMENT_TEMPERATURE;
			vscp_omsg.data[ 0 ] = 0x60; //data coding integer, default unit and index
			array_write(vscp_omsg.data, 1, 2, (void*)&sens_temp_raw);
			vscp_sendEvent();
			}
		else {
			clock_temp +=1;
			};
		}
	else {
		if (clock_current >= 2000) {
			//mcp355x_trigger();
			clock_current=0;
			};
	};

};
	
