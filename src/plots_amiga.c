/***************************************************************************

   Program:    AMPlot2
   File:       Plots.c
   
   Version:    V1.0
   Date:       27.03.92
   Function:   Line and scatter plot styles and scaling
   
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
               
****************************************************************************

   This program is copyright. Any copying without the permission of
   SciTech Software is illegal.

****************************************************************************

   Description:
   ============

****************************************************************************

   Usage:
   ======

****************************************************************************

   Revision History:
   =================

***************************************************************************/
/* Define this to use Amiga memory allocation for fitting
*/
#define AMIGA_MEM

/**************************************************************************/
/* Includes
*/
#include "includes.h"

#include <exec/memory.h>

/***************************************************************************/
/*>CheckIDCMP(struct Window *wind)
   -------------------------------
   See if the user has clicked the RMB. If so, post a requester to see
   if the user wants to abort.
   01.05.92 Original
*/
int CheckIDCMP(struct Window *wind)
{
   /* Variables for IDCMP handling */
   struct   IntuiMessage *message;
   ULONG    MIClass;
   int      retval = 0,
            done   = FALSE;

   /* Handle messages from the display window */
   while (message = (struct IntuiMessage *)GetMsg(wind->UserPort))
   {
      MIClass   = message->Class;
      ReplyMsg((struct Message *)message);

      if(!done)
      {
         if(MIClass==MENUPICK)
         {
            retval = ReqMessage(wind,"Abort?",1);
            done = TRUE;
         }
      }
   }

   return(retval);

}

