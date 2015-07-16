/***************************************************************************

   Program:    
   File:       CSClip.c
   
   Version:    V1.1
   Date:       26.02.91
   Function:   Clips a line to fit within a box. Doesn't draw it.
   
   Copyright:  SciTech Software 1991
   Author:     Andrew C. R. Martin
   Address:    SciTech Software
               23, Stag Leys,
               Ashtead,
               Surrey,
               KT21 2TD.
   Phone:      +44 (0372) 275775
   EMail:      UUCP: cbmuk!cbmuka!scitec!amartin
               JANET: andrew@uk.ac.ox.biop
               
   Adapted from Clip() routine by Mike Nelson in AUI Jan 1991, p38--40
               
****************************************************************************

   This program is not in the public domain, but it may be freely copied
   and distributed for no charge providing this header is included.
   The code may be modified as required, but any modifications must be
   documented so that the person responsible can be identified. If someone
   else breaks this code, I don't want to be blamed for code that does not
   work! The code may not be sold commercially without prior permission from
   the author, although it may be given away free with commercial products,
   providing it is made clear that this program is free and that the source
   code is provided with the program.

****************************************************************************

   Description:
   ============
   CSClip() implements Cohen-Sutherland line clipping and returns coordinates
   to fit within the clipping limits.

   This version simply returns the clipped coordinates (in the input
   pointers), so may be used for PostScript clipping, etc.

****************************************************************************

   Usage:
   ======
   CSClip(x1,y1,x2,y2,left,up,right,down)
   Input:   int               left,up     Clip box limits
                              right,down
   I/O:     int               *x1,*y1     Line to draw. Returns clipped
                              *x2,*y2     values.
   Returns: int               1           Nothing to draw
                              0           Draw something

****************************************************************************

   Revision History:
   =================
   V1.1  28.05.92    ANSIed and static'd

***************************************************************************/

#ifdef AMIGA
#include <exec/types.h>
#include <intuition/intuition.h>
#endif

#include "port.h"

#define  SPECIAL 255

/* Globals */
static int     CLP_LineX0,
               CLP_LineX1,
               CLP_LineY0,
               CLP_LineY1;
static UBYTE   CLP_CodeP1,
               CLP_CodeP2,
               CLP_NewCodeP1,
               CLP_NewCodeP2;

/* Function prototypes */
static UBYTE Quadrant(int x,
               int y,
               int left,
               int up,
               int right,
               int down);
static int   ClipEdge(int left,
               int up,
               int right,
               int down);
int   CSClip(  int *x0in,
               int *y0in,
               int *x1in,
               int *y1in,
               int left,
               int up,
               int right,
               int down);


      
int CSClip(int *x0in, int *y0in, int *x1in, int *y1in,
           int left,  int up,    int right, int down)
{
   int   x1temp,
         x0temp,
         y1temp,
         y0temp;
   int   x0,y0,x1,y1;

   /* We copy the input values from the pointers for ease of use              */
   x0 = *x0in;
   y0 = *y0in;
   x1 = *x1in;
   y1 = *y1in;
         
   /* Make copies of the input line coordinates                               */
   CLP_LineX1 = x1temp = x1;
   CLP_LineY1 = y1temp = y1;
   CLP_LineX0 = x0temp = x0;
   CLP_LineY0 = y0temp = y0;

   /* Find the quadrants for the ends of the lines                            */
   CLP_CodeP1 = Quadrant(CLP_LineX0,CLP_LineY0,left,up,right,down);
   CLP_CodeP2 = Quadrant(CLP_LineX1,CLP_LineY1,left,up,right,down);

   /* If one of these was set, then we need to do some clipping               */
   if(CLP_CodeP1||CLP_CodeP2)
   {
      if(CLP_CodeP1 & CLP_CodeP2)
      {  /* If both in same quadrant, we can ignore it                        */
         return(1);
      }
      else
      {  /* They're not the same, we need to do some work                     */
         if(!CLP_CodeP2)      /* See if this end is in the window             */
         {
            x1 = CLP_LineX1;
            y1 = CLP_LineY1;
         }
         else                 /* It wasn't, so we must clip it                */
         {
            CLP_NewCodeP1 = CLP_CodeP1;
            CLP_NewCodeP2 = CLP_CodeP2;
            ClipEdge(left,up,right,down);
            if(CLP_NewCodeP1 == SPECIAL)  /* Special case error condition     */
            {
               return(1);
            }
            else              /* Keep these clipped points                    */
            {
               x1 = CLP_LineX0;
               y1 = CLP_LineY0;
            }
         }
         
         /* Sort out the other point, swapping the coordinates round          */
         CLP_LineX1     = x0temp;
         CLP_LineX0     = x1temp;
         CLP_LineY1     = y0temp;
         CLP_LineY0     = y1temp;
         CLP_NewCodeP1  = CLP_CodeP2;
         CLP_NewCodeP2  = CLP_CodeP1;
         
         if(!CLP_NewCodeP2)   /* See if this end is in the window             */
         {
            x0 = CLP_LineX1;
            y0 = CLP_LineY1;
         }
         else                 /* It wasn't, so we must clip it                */
         {
            ClipEdge(left,up,right,down);
            x0 = CLP_LineX0;
            y0 = CLP_LineY0;
         }
      }
   }
   
   /* O.K, we should now have our required coordinates,
      so we can return the fixed values
   */
   *x0in = x0;
   *y0in = y0;
   *x1in = x1;
   *y1in = y1;
   
   return(0);
}
         
         
/*------------------------------------------------------------------------*/
static UBYTE Quadrant(int x, int y, int left, int up, int right, int down)
{
   UBYTE code = 0;

   if(x<left)  code |= 1;
   if(x>right) code |= 2;
   if(y>down)  code |= 4;
   if(y<up)    code |= 8;

   return(code);
}

/*------------------------------------------------------------------------*/
static int ClipEdge(int left, int up, int right, int down)
{
   int   LineCentreX,
         LineCentreY;
   UBYTE code,
         flag = 0;   /* Used to break loop */
         
   LineCentreX = (CLP_LineX0 + CLP_LineX1) >> 1; /* divide by 2 */
   LineCentreY = (CLP_LineY0 + CLP_LineY1) >> 1; /* divide by 2 */
   
   while(!flag)
   {
      /* Check quadrant for middle of line */
      code = Quadrant(LineCentreX,LineCentreY,left,up,right,down);
      
      if(!(code & CLP_NewCodeP2))   /* Are these in different quads?       */
      {
         CLP_LineX0 = LineCentreX;  /* Move point 0 to the midpoint        */
         CLP_LineY0 = LineCentreY;
      }
      else
      {
         CLP_LineX1 = LineCentreX;  /* Move point 1 to the midpoint        */
         CLP_LineY1 = LineCentreY;
      }
      
      /* Recalc the midpoint */
      LineCentreX = (CLP_LineX0 + CLP_LineX1) >> 1; /* divide by 2 */
      LineCentreY = (CLP_LineY0 + CLP_LineY1) >> 1; /* divide by 2 */
   
      /* Check if we've finished */
      if((LineCentreX == CLP_LineX0)&&(LineCentreY == CLP_LineY0)) flag = 1;
      if((LineCentreX == CLP_LineX1)&&(LineCentreY == CLP_LineY1)) flag = 1;
      
      /* Check for the special case */
      if((code & CLP_NewCodeP1)&&(code & CLP_NewCodeP2))
      {
         flag = 1;
         CLP_NewCodeP1 = SPECIAL;
      }
   }
   return(0);
}

