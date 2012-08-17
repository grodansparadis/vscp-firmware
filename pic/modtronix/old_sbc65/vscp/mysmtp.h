/*
 * Initialise SMTP mail server adress
 */
#define SMPT_SRVR1 (192)
#define SMPT_SRVR2 (168)
#define SMPT_SRVR3 (1)
#define SMPT_SRVR4 (100)


/*
 * SMTP State Machine states list
 */
typedef enum _SM_SMTP
{
   SM_SMTP_STDBY,
   SM_SMTP_ARP,
   SM_SMTP_CONNECT,
   SM_SMTP_WELCOME,
   SM_SMTP_HELO,
   SM_SMTP_FROM,
   SM_SMTP_TO,
   SM_SMTP_DATA1,
   SM_SMTP_DATA2,
   SM_SMTP_DATA3,
   SM_SMTP_DATA_HEADERS,
   SM_SMTP_DATA_MESSAGE,
   SM_SMTP_DATA_END,
   SM_SMTP_QUIT,
   SM_SMTP_DONE

} SM_SMTP;


typedef enum _SMTP_COMMAND
{
    SMTP_CMD_HELO,
    SMTP_CMD_FROM,
    SMTP_CMD_TO,
    SMTP_CMD_DATA,
    SMTP_CMD_DATA_HEADERS,
    SMTP_CMD_DATA_HEADERS2,
    SMTP_CMD_DATA_HEADERS3,
    SMTP_CMD_DATA_MESSAGE,
    SMTP_CMD_DATA_END,
    SMTP_CMD_QUIT,

} SMTP_COMMAND;


/**************** functions prototypes ******************************

/*********************************************************************
 * Function:        void SMTPInit(void)
 *
 * PreCondition:    TCP module is already initialized.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initializes internal variables of SMTP client
 *
 * Note:
 ********************************************************************/
void SMTPInit(void)	;


/*********************************************************************
 * Function:        void SMTPSend(void)
 *
 * PreCondition:    SMTP module is already initialized.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Triggers sending of mail
 *
 * Note:
 ********************************************************************/
void SMTPSend(void);

/*********************************************************************
 * Function:        void SMTP Client State machine(void)
 *
 * PreCondition:    FTPInit() must already be called.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:
 *
 * Note:            This function acts as a task (similar to one in
 *                  RTOS).  This function performs its task in
 *                  co-operative manner.  Main application must call
 *                  this function repeatdly to ensure it can send
 *                  mails when requested.
 ********************************************************************/
void SMTPClient(void) ;
