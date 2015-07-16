/****************************************************************************

   Program:    AMPlot2
   File:       handlers.c
   
   Version:    V1.1
   Date:       04.03.92
   Function:   Handle menu functions.
   
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

*****************************************************************************

   Usage:
   ======

*****************************************************************************

   Revision History:
   =================
   V1.1     27.04.92
   Changed all sprintf's to gadgets from %.3lg to %lg

****************************************************************************/
#include "includes.h"


#define ISCHECKED(x) ((x)->Flags & CHECKED)

/***************************************************************************/
/*>ToggleLogX(APTR object)
   -----------------------
   Toggle log X-axis 
   04.03.92 Framework
   31.03.92 Original
   10.04.92 Added check on data and rescale
   29.04.92 Added NULL and Rexx checks
   05.05.92 Added check for Rexx empty graph
   07.07.92 Now always toggles, even if from datafile or Rexx. Removed check
            on rexx.fromrexx
   21.07.92 Stopped clearing graph.userbounds if object==NULL. i.e. this has
            come from the datafile.
   24.07.92 If came from menu, now checks menu CHECK status rather than
            toggling
*/
int ToggleLogX(APTR object)
{
   int      i,
            ok;
   DATASET  *d;
   
   if(object == NULL)   TOGGLE(graph.logx);
#ifdef AMIGA
   else                 graph.logx = ISCHECKED(MenuLogX);
#endif

   ok = TRUE;

   /* Check the data are all positive */
   if(graph.logx)
   {
#ifdef REXX_SUPPORT
      if(rexx.empty)       /* Called from a Rexx `empty' graph  */
      {
         if(graph.xmin <= 0.0)
            graph.logx = FALSE;
         else
            SetScaling(FALSE);

         return(0);
      }
#endif
      
      for(d=data;d;NEXT(d))
      {
         for(i=0;i<d->ndata;i++)
         {
            if(d->x[i] <= 0.0)
            {
               ok = FALSE;
               goto breakout;
            }
         }
      }
breakout:
      if(!ok)
      {
         graph.logx = FALSE;
#ifdef AMIGA
         Uncheck(MenuLogX);
#endif
      }
   }
   
   /* If all was OK, redo scaling */
   if(ok)
   {
      if(object != NULL 
#ifdef AMIGA
         || rexx.replot
#endif
        ) /* If from menu or ARexx */
      {
         graph.userbounds = FALSE;
         graph.userticks  = FALSE;
      }
      SetScaling(FALSE);
   }
   
   return(0);
}

/***************************************************************************/
/*>ToggleLogY(APTR object)
   -----------------------
   Toggle log Y-axis 
   04.03.92 Framework
   31.03.92 Original
   10.04.92 Added check on data and rescale
   29.04.92 Added NULL and Rexx checks
   05.05.92 Added check for Rexx empty graph
   09.06.92 Added check for barcharts
   07.07.92 Now always toggles, even if from datafile or Rexx. Removed check
            on rexx.fromrexx
   21.07.92 Stopped clearing graph.userbounds if object==NULL. i.e. this has
            come from the datafile.
   24.07.92 If came from menu, now checks menu CHECK status rather than
            toggling
*/
int ToggleLogY(APTR object)
{
   int      i,
            ok;
   DATASET  *d;
   
   if(object == NULL)   TOGGLE(graph.logy);
#ifdef AMIGA
   else                 graph.logy = ISCHECKED(MenuLogY);
#endif

   ok = TRUE;

   /* Not allowed with barcharts */
   if(graph.logy && graph.style == STYLE_BAR)
   {
      graph.logy = FALSE;
#ifdef AMIGA
      Uncheck(MenuLogY);
#endif
      return(0);
   }


   /* Check the data are all positive */
   if(graph.logy)
   {
#ifdef AMIGA
      if(rexx.empty)          /* Called from a Rexx `empty' graph  */
      {
         if(graph.ymin <= 0.0)
            graph.logy = FALSE;
         else
            SetScaling(FALSE);

         return(0);
      }
#endif
      
      for(d=data;d;NEXT(d))
      {
         for(i=0;i<d->ndata;i++)
         {
            if(d->y[i] <= 0.0)
            {
               ok = FALSE;
               goto breakout;
            }
         }
      }
breakout:
      if(!ok)
      {
         graph.logy = FALSE;
#ifdef AMIGA
         Uncheck(MenuLogY);
#endif
      }
   }
   
   /* If all was OK, redo scaling */
   if(ok)
   {
      if(object != NULL 
#ifdef AMIGA
         || rexx.replot
#endif
        ) /* If from menu or ARexx */
      {
         graph.userbounds = FALSE;
         graph.userticks  = FALSE;
      }
      SetScaling(FALSE);
   }

   return(0);
}

