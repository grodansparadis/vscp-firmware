// FILE: testif.c


/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2019 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *	This file is part of VSCP - Very Simple Control Protocol
 *	https://www.vscp.org
 *
 * ******************************************************************************
 */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "../../common/vscp_link.h"

void main()
{
    uint8_t guid[16];
    char buf[512];
    int i,j,k;
    int rv;

    printf("GUID test 1\n");
    const char *strGUID0 = "";  
    memset( guid, 0xff, 16 );  
    rv = parseGuid( strGUID0, NULL, guid );
    if ( rv ) printf("GUID="" fail\n");
    for ( i=0; i<16; i++ ) {
        if ( 0 != guid[i] ) {
            printf("GUID="" fail. Index=%d\n", i);
        } 
    }

    printf("GUID test 2\n");
    const char *strGUID1 = "-"; 
    memset( guid, 0xff, 16 );
    rv = parseGuid( strGUID1, NULL, guid );
    if ( rv ) printf("GUID='-' fail\n");
    for ( int i=0; i<16; i++ ) {
        if ( 0 != guid[i] ) {
            printf("GUID='-' fail. Index=%d\n", i);
        } 
    }

    printf("GUID test 3\n");
    const char *strGUID2 = "00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF"; 
    const uint8_t GUID2_array[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
    memset( guid, 0xff, 16 );   
    rv = parseGuid( strGUID2, NULL, guid );
    if ( VSCP_ERROR_SUCCESS != rv ) printf("GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF' parser return fail\n");
    for ( i=0; i<16; i++ ) {
        if ( GUID2_array[i] != guid[i] ) {
            printf("GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF' fail. Index=%d\n", i);
        } 
    }

    printf("GUID test 4\n");
    const char *strGUID3 = "00:11:22:33:44:55:66:77:88:99:AA:BB:CC:"; 
    const uint8_t GUID3_array[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0x00,0x00,0x00};
    memset( guid, 0xff, 16 );   
    rv = parseGuid( strGUID3, NULL, guid );
    if ( VSCP_ERROR_SUCCESS != rv ) printf("GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:' parser return fail\n");
    for ( i=0; i<16; i++ ) {
        if ( GUID3_array[i] != guid[i] ) {
            printf("GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:' fail. Index=%d\n", i);
        } 
    }

    printf("GUID test 5\n");
    const char *strGUID4 = "00:11:22:33:44:55:66:77:88:99:AA:BB:CC"; 
    const uint8_t GUID4_array[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0x00,0x00,0x00};
    memset( guid, 0xff, 16 );   
    rv = parseGuid( strGUID4, NULL, guid );
    if ( VSCP_ERROR_SUCCESS != rv ) printf("GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC' parser return fail\n");
    for ( i=0; i<16; i++ ) {
        if ( GUID4_array[i] != guid[i] ) {
            printf("GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC' fail. Index=%d\n", i);
        } 
    }

    printf("GUID test 6\n");
    const char *strGUID5 = "00:11:22:33:44:YW:66:77:88:99:AA:BB:CC:01:02:03"; 
    const uint8_t GUID5_array[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0x01,0x02,0x03};
    memset( guid, 0xff, 16 );   
    rv = parseGuid( strGUID5, NULL, guid );
    if ( VSCP_ERROR_SUCCESS == rv ) printf("GUID='00:11:22:33:44:YW:66:77:88:99:AA:BB:CC:01:02:03' parser return success but should have returned failure.\n");
    
    printf("GUID test 7\n");
    const char *strGUID6 = "00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF,11,22,33,44,55"; 
    const uint8_t GUID6_array[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
    memset( guid, 0xff, 16 );   
    rv = parseGuid( strGUID6, NULL, guid );
    if ( VSCP_ERROR_SUCCESS != rv ) printf("GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF,11,22,33,44,55' parser return fail\n");
    for ( i=0; i<16; i++ ) {
        if ( GUID6_array[i] != guid[i] ) {
            printf("GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF,11,22,33,44,55' fail. Index=%d\n", i);
        } 
    }

    // ------------------------------------------------------------------------

    printf("Filter test 1 (filter)\n");
    vscpEventFilter evfilter;
    memset( &evfilter, 0, sizeof(vscpEventFilter) );
    if ( VSCP_ERROR_SUCCESS  != parseFilter( "2,0x20,6,00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF", 
                                        &evfilter ) ) {
        printf("Failed on filter test 1\n"); 
    }
    if ( evfilter.filter_priority != 2 ) printf("Filter test 1 - filter_priority fail.\n");
    if ( evfilter.filter_class != 32 ) printf("Filter test 1 - filter_class fail.\n");
    if ( evfilter.filter_type != 6 ) printf("Filter test 1 - filter_type fail.\n");
    for ( i=0; i<16; i++ ) {
        if ( GUID2_array[i] != evfilter.filter_GUID[i] ) {
            printf("Filter test 1 GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF' fail. Index=%d\n", i);
        } 
    }

    // ------------------------------------------------------------------------

    printf("Filter test 2 (mask)\n");
    memset( &evfilter, 0, sizeof(vscpEventFilter) );
    if ( VSCP_ERROR_SUCCESS  != parseMask( "0x22,1,077,00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF", 
                                        &evfilter ) ) {
        printf("Failed on filter test 2\n"); 
    }
    if ( evfilter.mask_priority != 34 ) printf("Filter test 2 - mask_priority fail.\n");
    if ( evfilter.mask_class != 1 ) printf("Filter test 2 - mask_class fail.\n");
    if ( evfilter.mask_type != 63 ) printf("Filter test 2 - mask_type fail.\n");
    for ( i=0; i<16; i++ ) {
        if ( GUID2_array[i] != evfilter.mask_GUID[i] ) {
            printf("Filter test 1 GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF' fail. Index=%d\n", i);
        } 
    }

    // ------------------------------------------------------------------------

    printf("Event test 1 (standard)\n");
    vscpEvent ev;
    memset( &ev, 0, sizeof(vscpEvent) );
    if ( VSCP_ERROR_SUCCESS  != parseEvent( "1,0x21,3,1234567,2062-11-17T19:32:44,7654321,00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF,11,0x22,33,0x44,55", &ev ) ) {
        printf("Failed on event test 1\n"); 
    }
    if ( ev.head != 1 ) printf("Event test 1, head failed.\n");
    if ( ev.vscp_class != 0x21 ) printf("Event test 1, vscp_class failed.\n");
    if ( ev.vscp_type != 3 ) printf("Event test 1, vscp_type failed.\n");
    if ( ev.obid != 1234567 ) printf("Event test 1, obid failed.\n");
    if ( ev.year != 2062 ) printf("Event test 1, year failed.\n");
    if ( ev.month != 11 ) printf("Event test 1, month failed.\n");
    if ( ev.day != 17 ) printf("Event test 1, day failed.\n");
    if ( ev.hour != 19 ) printf("Event test 1, hour failed.\n");
    if ( ev.minute != 32 ) printf("Event test 1, minute failed.\n");
    if ( ev.second != 44 ) printf("Event test 1, second failed.\n");
    if ( ev.timestamp != 7654321 ) printf("Event test 1, timestamp failed.\n");
    if ( ev.sizeData != 5 ) printf("Event test 1, sizeData failed (%d).\n", (int)ev.sizeData );
    for ( i=0; i<16; i++ ) {
        if ( GUID2_array[i] != ev.GUID[i] ) {
            printf("Event test 1, GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF' fail. Index=%d\n", i);
        } 
    }
    if ( NULL == ev.pdata ) {
        printf("Event test 1, pdata == NULL, Failure\n");
    }
    else {
        if ( ev.pdata[0] != 11 ) printf("Event test 1, Data byte 0 failed.\n");
        if ( ev.pdata[1] != 0x22 ) printf("Event test 1, Data byte 1 failed.\n");
        if ( ev.pdata[2] != 33 ) printf("Event test 1, Data byte 2 failed.\n");
        if ( ev.pdata[3] != 0x44 ) printf("Event test 1, Data byte 3 failed.\n");
        if ( ev.pdata[4] != 55 ) printf("Event test 1, Data byte 4 failed.\n");
    }


    // ------------------------------------------------------------------------


    printf("Event test 2 (standard)\n");
    memset( &ev, 0, sizeof(vscpEvent) );
    if ( VSCP_ERROR_SUCCESS  != parseEvent( "1,0x21,3,,,,00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF,11,0x22,33,0x44,55", &ev ) ) {
        printf("Failed on event test 2\n"); 
    }
    if ( ev.head != 1 ) printf("Event test 2, head failed.\n");
    if ( ev.vscp_class != 0x21 ) printf("Event test 2, vscp_class failed.\n");
    if ( ev.vscp_type != 3 ) printf("Event test 2, vscp_type failed.\n");
    if ( ev.obid != 0 ) printf("Event test 2, obid failed.\n");
    if ( ev.year != 0 ) printf("Event test 2, year failed.\n");
    if ( ev.month != 0 ) printf("Event test 2, month failed.\n");
    if ( ev.day != 0 ) printf("Event test 2, day failed.\n");
    if ( ev.hour != 0 ) printf("Event test 2, hour failed.\n");
    if ( ev.minute != 0 ) printf("Event test 2, minute failed.\n");
    if ( ev.second != 0 ) printf("Event test 2, second failed.\n");
    if ( ev.timestamp != 0 ) printf("Event test 2, timestamp failed.\n");
    if ( ev.sizeData != 5 ) printf("Event test 2, sizeData failed (%d).\n", (int)ev.sizeData );
    for ( i=0; i<16; i++ ) {
        if ( GUID2_array[i] != ev.GUID[i] ) {
            printf("Event test 2, GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF' fail. Index=%d\n", i);
        } 
    }
    if ( NULL == ev.pdata ) {
        printf("Event test 2, pdata == NULL, Failure\n");
    }
    else {
        if ( ev.pdata[0] != 11 ) printf("Event test 2, Data byte 0 failed.\n");
        if ( ev.pdata[1] != 0x22 ) printf("Event test 2, Data byte 1 failed.\n");
        if ( ev.pdata[2] != 33 ) printf("Event test 2, Data byte 2 failed.\n");
        if ( ev.pdata[3] != 0x44 ) printf("Event test 2, Data byte 3 failed.\n");
        if ( ev.pdata[4] != 55 ) printf("Event test 2, Data byte 4 failed.\n");
    }


    // ------------------------------------------------------------------------


    printf("EventEx test 1 (extended)\n");
    vscpEventEx evex;
    memset( &evex, 0, sizeof(vscpEventEx) );
    if ( VSCP_ERROR_SUCCESS  != parseEventEx( "1,0x21,3,1234567,2062-11-17T19:32:44,7654321,00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF,11,0x22,33,0x44,55", &evex ) ) {
        printf("Failed on eventex test 1\n"); 
    }
    if ( evex.head != 1 ) printf("EventEx test 1, head failed.\n");
    if ( evex.vscp_class != 0x21 ) printf("EventEx test 1, vscp_class failed.\n");
    if ( evex.vscp_type != 3 ) printf("EventEx test 1, vscp_type failed.\n");
    if ( evex.obid != 1234567 ) printf("EventEx test 1, obid failed.\n");
    if ( evex.year != 2062 ) printf("EventEx test 1, year failed.\n");
    if ( evex.month != 11 ) printf("EventEx test 1, month failed.\n");
    if ( evex.day != 17 ) printf("EventEx test 1, day failed.\n");
    if ( evex.hour != 19 ) printf("EventEx test 1, hour failed.\n");
    if ( evex.minute != 32 ) printf("EventEx test 1, minute failed.\n");
    if ( evex.second != 44 ) printf("EventEx test 1, second failed.\n");
    if ( evex.timestamp != 7654321 ) printf("EventEx test 1, timestamp failed.\n");
    if ( evex.sizeData != 5 ) printf("EventEx test 1, sizeData failed (%d).\n", (int)evex.sizeData );
    for ( i=0; i<16; i++ ) {
        if ( GUID2_array[i] != evex.GUID[i] ) {
            printf("EventEx test 1, GUID='00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF' fail. Index=%d\n", i);
        } 
    }
    if ( evex.data[0] != 11 ) printf("EventEx test 1, Data byte 0 failed.\n");
    if ( evex.data[1] != 0x22 ) printf("EventEx test 1, Data byte 1 failed.\n");
    if ( evex.data[2] != 33 ) printf("EventEx test 1, Data byte 2 failed.\n");
    if ( evex.data[3] != 0x44 ) printf("EventEx test 1, Data byte 3 failed.\n");
    if ( evex.data[4] != 55 ) printf("EventEx test 1, Data byte 4 failed.\n");


    // ------------------------------------------------------------------------


    printf("Event to string test\n");

    if ( VSCP_ERROR_SUCCESS  != parseEvent( "1,0x21,3,1234567,2062-11-17T19:32:44,7654321,00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF,011,0x22,33,0x44,55", &ev ) ) {
        printf("Event to string test failed creating event\n"); 
    }

    if ( VSCP_ERROR_SUCCESS  != eventToString( &ev, buf, sizeof( buf ) ) ) {
        printf("Failed to convert event to string\n");
    }
    else {
        printf("IN :%s\n", "1,0x21,3,1234567,2062-11-17T19:32:44,7654321,00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF,011,0x22,33,0x44,55" );
        printf("OUT:%s\n", buf );
    }

}


