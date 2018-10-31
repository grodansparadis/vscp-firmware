
void vscp_link_parser( const char cmd ) {

    char *p = cmd;
    char *pcmd;

    // Remove whitespace from command
    while( *p && ( *p == " " ) ) {
        p++;
    }

    pcmd = p;

    if ( NULL != ( p == strstr( pcmd, "noop" ) ) ) {
        p += 4;
        vscp_link_doCmdNoop( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "help" ) ) ) {
        p += 4;
        vscp_link_doCmdHelp( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "quit" ) ) ) {
        p += 4;
        vscp_link_doCmdQuit( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "user" ) ) ) {
        p +=4;
        vscp_link_doCmdUser( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "pass" ) ) ) {
        p +=4;
        vscp_link_doCmdPassword( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "challenge" ) ) ) {
        p +=9;
        vscp_link_doCmdChallenge( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "send" ) ) ) {
        p +=4;
        vscp_link_doCmdSend( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "retr" ) ) ) {
        p +=4;
        vscp_link_doCmdRetrive( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "rcvloop" ) ) ) {
        p +=7;
        vscp_link_doCmdRcvLoop( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "quitloop" ) ) ) {
        p +=8;
        vscp_link_doCmdQuitLoop( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "cdta" ) ) ) {
        p +=4;
        vscp_link_doCmdCmdCheckData( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "chkdata" ) ) ) {
        p +=7;
        vscp_link_doCmdCheckData( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "clra" ) ) ) {
        p +=4;
        vscp_link_doCmdClearAll( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "clrall" ) ) ) {
        p +=6;
        vscp_link_doCmdClearAll( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "stat" ) ) ) {
        p +=4;
        vscp_link_doCmdStatistics( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "info" ) ) ) {
        p +=4;
        vscp_link_doCmdInfo( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "chid" ) ) ) {
        p +=4;
        vscp_link_doCmdCetChannelId( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "getchid" ) ) ) {
        p +=7;
        vscp_link_doCmdGetChannelId( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "sgid" ) ) ) {
        p +=4;
        vscp_link_doCmdSetChannelId( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "setguid" ) ) ) {
        p +=7;
        vscp_link_doCmdSetChannelId( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "ggid" ) ) ) {
        p +=4;
        vscp_link_doCmdGetGUID( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "getguid" ) ) ) {
        p +=7;
        vscp_link_doCmdGetGUID( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "vers" ) ) ) {
        p +=4;
        vscp_link_doCmdGetVersion( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "version" ) ) ) {
        p +=7;
        vscp_link_doCmdGetVersion( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "sflt" ) ) ) {
        p +=4;
        vscp_link_doCmdSetFilter( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "setfilter" ) ) ) {
        p +=9;
        vscp_link_doCmdSetFilter( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "smsk" ) ) ) {
        p +=4;
        vscp_link_doCmdSetMask( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "setmask" ) ) ) {
        p +=7;
        vscp_link_doCmdSetMask( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "wcyd" ) ) ) {
        p +=4;
        vscp_link_doCmdWhatCanYouDo();
    }
    else if ( NULL != ( p == strstr( pcmd, "whatcanyoudo" ) ) ) {
        p +=12;
        vscp_link_doCmdWhatCanYouDo( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "+" ) ) ) {
        p +=1;
        vscp_link_doCmdCommandAgain( p );
    }
    else if ( NULL != ( p == strstr( pcmd, "dm" ) ) ) {
        p +=2;
        vscp_link_doCmdDM( p );
    }
#ifdef VSCP_LINK_ENABLE_CMD_VAR
    else if ( NULL != ( p == strstr( pcmd, "var" ) ) ) {
        p +=3;
        vscp_link_doCmdVariable( p );
    }
#endif   
#ifdef VSCP_LINK_ENABLE_CMD_INTERFACE
    else if ( NULL != ( p == strstr( pcmd, "interface" ) ) ) {
        p +=9;
        vscp_link_doCmdInterface( p );
    }
#endif  
    else {
        vscp_link_reply("-OK - Unknown command\r\n");
    }
}



///////////////////////////////////////////////////////////////////////////////
// doCmdDM
//
// enable
// disable
// list
// add
// delete
// reset
// clrtrig
// clrerr
// save
// load
//

void vscp_link_doCmdDM( const char *cmd )
{
    char *p = cmd;
    char *pcmd;

    // Remove whitespace from command
    while( *p && ( *p == " " ) ) {
        p++;
    }

    pcmd = p;

    if ( NULL != ( p == strstr( pcmd, "enable" ) ) ) {
        p +=6;
        
    }
    else if ( NULL != ( p == strstr( pcmd, "disable" ) ) ) {
        p +=7;
        
    }
    else if ( NULL != ( p == strstr( pcmd, "list" ) ) ) {
        p +=4;
        
    }
    else if ( NULL != ( p == strstr( pcmd, "add" ) ) ) {
        p +=3;
        
    }
    else if ( NULL != ( p == strstr( pcmd, "delete" ) ) ) {
        p +=6;
        
    }
    else if ( NULL != ( p == strstr( pcmd, "reset" ) ) ) {
        p +=5;
        
    }
    else if ( NULL != ( p == strstr( pcmd, "clrtrig" ) ) ) {
        p +=7;
        
    }
    else if ( NULL != ( p == strstr( pcmd, "clrerr" ) ) ) {
        p +=6;
        
    }
    else if ( NULL != ( p == strstr( pcmd, "save" ) ) ) {
        p +=6;
        
    }
    else if ( NULL != ( p == strstr( pcmd, "load" ) ) ) {
        p +=6;
        
    }
    else {
        vscp_link_reply("-OK - DM - unknown sub command\r\n");
    }
}

///////////////////////////////////////////////////////////////////////////////
// doCmdVar
//
// list
// write
// read
// readvalue
// writevalue
// readnote
// writenote
// reset
// readreset
// remove
// readremove
// length
// save
// load
//

///////////////////////////////////////////////////////////////////////////////
// doCmdInterface
//
// list
// close
//