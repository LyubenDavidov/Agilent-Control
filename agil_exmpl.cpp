#include <stdio.h>
#include <stdlib.h>
#include <visa.h>

/* This function checks and displays errors, using the error query of 
the instrument; 
Call this function after every command to make sure your commands 
are correct */


void checkError(ViSession session, ViStatus err_status )
{
    ViStatus error;
    ViChar errMsg[256];
    /* queries what kind of error occurred */
    error = viQueryf(session,"%s\n","%t","SYST:ERR?",errMsg);
    /*if this command times out, a system error is probable;
    check the GPIB bus communication */
    if (error == VI_ERROR_TMO)
    {
        printf("System Error!\n");
        exit(1);
    }

    else
    {
        /* display the error number and the error message */
        if(errMsg[0] != '+')
        printf("error:%ld - - > %s\n", err_status,errMsg);
    }
 
}

int main (void)
{
    ViStatus errStatus; /*return error code from visa call */
    ViSession defaultRM; /*default visa resource manager variable*/
    ViSession vi; /*current session handle */
    ViChar replyBuf[256]; /*buffer holding answers from the instrument*/
    ViChar c;

    /* Initialize visa resource manger */
    errStatus = viOpenDefaultRM (&defaultRM);
    if(errStatus < VI_SUCCESS)
    {
        printf("Failed to open VISA Resource manager\n"); 
        exit(errStatus); 
    }

    /* Open session to GPIB device at address 11; the VI_NULL parameters 3,4 are mandatory and not used for VISA 1.0*/
    errStatus = viOpen (defaultRM, "GPIB::11::INSTR", VI_NULL,VI_NULL,&vi); //"GPIB::11::INSTR"
    if(errStatus < VI_SUCCESS)
    {
        printf("Failed to open instrument\n"); 
        printf("End of program, press Enter key to quit\n");
	    getchar();
        exit(errStatus); 
    }

    /* set timeout to 20 sec; this should work for all commands except 
    for zeroing or READ commands with averaging times greater than the
    timeout */

    errStatus = viSetAttribute(vi,VI_ATTR_TMO_VALUE,20000);
    checkError(vi,errStatus);

    /* get the identification string of the instrument mainframe*/
    errStatus = viQueryf(vi,"%s\n","%t","*IDN?",replyBuf);
    if(errStatus < VI_SUCCESS)
    {
        checkError(vi,errStatus);
    }

    else printf("%s",replyBuf);

    /* identify the installed modules */
    errStatus = viQueryf(vi,"%s\n","%t","*OPT?",replyBuf);
    if(errStatus < VI_SUCCESS)
    {
        checkError(vi,errStatus);
    }

    else printf("%s",replyBuf);

    /* get information about the available options of a slot */ 
    errStatus = viQueryf(vi,"%s","%t","SLOT1:OPT?\n",replyBuf);
    if(errStatus < VI_SUCCESS)
    {
        checkError(vi,errStatus);
    }

    else printf("%s",replyBuf);

    /*loop, until a key is pressed */
    while(!scanf("%c",&c));

    /*close the session */
    viClose(vi); 
}

