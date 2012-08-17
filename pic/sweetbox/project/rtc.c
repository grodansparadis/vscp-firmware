#pragma config FPLLMUL = MUL_18, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
 4#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2
 5
 6//**********************************RTCC FUNCTIONS*************************************
 7
 8int InitRTCC()
 9{
10    SYSTEMConfigPerformance(72000000L);
11    RtccInit();            // init the RTCC
12    while(RtccGetClkStat()!=RTCC_CLK_ON);    // wait for the SOSC to be
13    //actually running and RTCC to have its clock source, could wait here at most 32ms
14    
        
      // let's set the current date
15    {
16        tm.l=0;            //Why do clocks always start at 12:00??
17        tm.sec=0x30;    //Set the Seconds
18        tm.min=0x29;    //set the Minutes
19        tm.hour=0x21;    //Set the Hour
20
21        dt.wday=6;        //Set Weekday to Sunday
22        dt.mday=0x05;    //on the 5th
23        dt.mon=0x10;    //of Oct
24        dt.year=0x08;    //of 2008.
25        RtccSetTimeDate(tm.l, dt.l);    //Set time and date into registers....
26    }
27    //mRtccSelectSecPulseOutput();        // select the seconds clock pulse as the function of the RTCC output pin
28    mRtccSelectAlarmPulseOutput();        // select the alarm pulse as the function of the RTCC output pin
29    mRtccOutputEnable();            // enable the Output pin of the RTCC
30    return 1;
31}//End InitRTCC
32//***********************************************************************************
33int OpenRTCC()
34{
35    RtccOpen(tm.l, dt.l, 0);            // set time, date and calibration in a single operation
36    //check to see if the RTCC is running: check the SYNC bit
37    while(mRtccGetSync());                // wait sync to be low
38    while(!mRtccGetSync());                // wait to be high
39    while(mRtccGetSync());                // wait sync to be low again
40    // other RTCC operations
41    // adjust the RTCC timing
42    RtccSetCalibration(200);            // value to calibrate with at each minute
43    // enabling the RTCC output pin
44    //mRtccSelectSecPulseOutput();        // select the seconds clock pulse as the function of the RTCC output pin
45    mRtccSelectAlarmPulseOutput();        // select the alarm pulse as the function of the RTCC output pin
46    mRtccOutputEnable();                // enable the Output pin of the RTCC
47    //Enable Interrupts
48    mRtccEnableInt();
49    return 1;
50}
51//***********************************************************************************
52int AlarmRTCC(int AlrmMinutes)    //To set an alram for the RTCC
53{
54    tAlrm.l=tm.l;
55    dAlrm.l=dt.l;
56    tAlrm.min+=AlrmMinutes;            //Set for nn Mins
57    RtccChimeDisable();                //Disable Chime, no rollover allowed!!
58    RtccSetAlarmRptCount(5);        //10 Alarms
59    RtccSetAlarmRpt(RTCC_RPT_MIN);    //repeat every minute
60    RtccSetAlarmTimeDate(tAlrm.l,dAlrm.l);
61    RtccAlarmEnable();
62}
63//***********************************************************************************
64/*    the following will update the RTCC calibration value.*/
65void CalibrateRTCC(void)
66{
67    int cal=0x3FD;            //10 Bits adjustment, -3 in value
68    if(RTCCON&0x8000)        //if RTCC is On
69    {
70        unsigned int t0,t1;
71        do
72        {
73            t0=RTCTIME;
74            t1=RTCTIME;
75        }while(t0!=t1);//End Do        //Read valid time value
76        if((t0&0xFF)==00)
77        {                            //were at 2nd 00, wait for auto-adjust to be performed
78            while(!(RTCCON&0x2));    //wait until second half
79        }//end if
80    }//End If
81}
82//******************************END RTCC FUNCTIONS************************************