/****************************************************************************

   Program:    AMPlot2
   File:       messages.c
   
   Version:    V1.0
   Date:       01.07.92
   Function:   AMPlot graph plotting V2.0
   
   Copyright:  SciTech Software 1992
   Author:     Dr. Andrew C. R. Martin
   Address:    SciTech Software
               23, Stag Leys,
               Ashtead,
               Surrey,
               KT21 2TD.
   Phone:      +44 (0372) 275775
   EMail:      UUCP: cbmuk!cbmuka!scitec!amartin
               JANET: andrew@uk.ac.ox.biop
               
*****************************************************************************

   This program is copyright. Any copying without the permission of
   SciTech Software is illegal.

*****************************************************************************

   Description:
   ============
   This file contains routines to handle an output only message window.
   These routines are not dependent on AMPlot and are generally applicable.
   The Intuition and DOS libraries should be open before using these
   routines.

*****************************************************************************

   Usage:
   ======

*****************************************************************************

   Revision History:
   =================

****************************************************************************/
/* Includes
*/
#include <proto/intuition.h>
#include <proto/console.h>
#include <sts/stslib.h>

/***************************************************************************/
/* Prototypes
*/
#ifndef DOPROTOS
#include "messages_protos.h"
#endif

/***************************************************************************/
/* Statics
*/
static struct Window    *ConWind       = NULL;  /* Window                  */
static struct MsgPort   *ConPort       = NULL;  /* Message port            */
static struct IOStdReq  *ConMsg        = NULL;  /* I/O request block       */

/***************************************************************************/
/*>CreateMessageWindow(struct Screen *scrn, char *title, char *portname,
                       int left, int top, int width, int height)
   ---------------------------------------------------------------------
   Opens a window of the given dimensions and attaches a console for output.
   Returns  0 if OK,
            1 if unable to open message window, or to attach console
   
   01.07.92 Original
   13.07.92 Window sent to back once created.
*/
CreateMessageWindow(struct Screen   *scrn,
                    char            *title,
                    char            *portname,
                    int             left,
                    int             top,
                    int             width,
                    int             height)
{
   struct NewWindow  *nw;
   
   /* Create the message port       */
   if((ConPort = (struct MsgPort *)CreatePort(portname,0)) == NULL)
      return(1);
      
   /* Create I/O Request block      */
   if((ConMsg  = (struct IOStdReq *)CreateStdIO(ConPort)) == NULL)
      return(1);
   
   /* Build the NewWindow structure */
   nw = BuildWindow(scrn,left,top,width,height,title,0,0);
   /* We want no IDCMP messages     */
   nw->IDCMPFlags = 0L;
   /* We don't want to activate the window */
   nw->Flags &= ~WFLG_ACTIVATE;
   
   /* Open the window               */
   if((ConWind = OpenWindow(nw)) == NULL)
      return(1);
    
   WindowToBack(ConWind);
      
   /* Attach console to window      */
   ConMsg->io_Data   = (APTR)ConWind;
   ConMsg->io_Length = sizeof(*ConWind);
   if(OpenDevice("console.device",0,ConMsg,0))
      return(1);
   
   return(0);
}

/***************************************************************************/
/*>RemoveMessageWindow(void)
   -------------------------
   Closes the console and removes the message window.

   01.07.92 Original
*/
RemoveMessageWindow(void)
{
   if(ConMsg)  CloseDevice(ConMsg);
   if(ConMsg)  DeleteStdIO(ConMsg);
   if(ConPort) DeletePort(ConPort);
   
   if(ConWind) CloseWindow(ConWind);
   
   ConMsg  = NULL;
   ConPort = NULL;
   ConWind = NULL;
   
   return(0);
}

/***************************************************************************/
/*>WriteMessage(char *text)
   ------------------------
   Writes a message to the message console window.

   01.07.92 Original
   13.07.92 Only does the WindowToFront() if the text doesn't start with a .
*/
WriteMessage(char *text)
{
   ConMsg->io_Command = CMD_WRITE;
   ConMsg->io_Data = (APTR)text;
   ConMsg->io_Length = -1;
   
   DoIO(ConMsg);
   
   ConMsg->io_Command = CMD_WRITE;
   ConMsg->io_Data = (APTR)"\r\n";
   ConMsg->io_Length = -1;
   
   DoIO(ConMsg);
   
   if(text[0] != '.') WindowToFront(ConWind);
   
   return(0);
}
