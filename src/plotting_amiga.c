/****************************************************************************

   Program:    AMPlot2
   File:       Plotting.c
   
   Version:    V1.0
   Date:       06.04.92
   Function:   Device independent plotting routines.
   
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

****************************************************************************/
/* Includes
*/
#include "includes.h"
#ifdef AMIGA
#include <graphics/gfxmacros.h>     /* SetDrPt() macro is defined here     */
#endif

#include "hpgl.h"





/***************************************************************************/
/*>ScreenMark(struct RastPort *RasP, int xscreen, int yscreen, 
              int mark, double dpsize)
   -----------------------------------------------------------
   Draw the appropriate mark on the Amiga screen
   06.05.92 Original
*/
int ScreenMark(struct RastPort *RasP,
           int             xscreen, 
           int             yscreen, 
           int             mark, 
           double          dpsize)
{
   int         size,
               j,
               *offset;
   static int  warned   = FALSE;
   
   size = (int)(dpsize / 4.0);
   
   switch(mark)
   {
   case 0:                             /* No mark                          */
      break;
   case 1:                             /* Cross                            */
      Move(RasP,  xscreen-size,     yscreen-size);
      Draw(RasP,  xscreen+size,     yscreen+size);
      Move(RasP,  xscreen-size,     yscreen+size);
      Draw(RasP,  xscreen+size,     yscreen-size);
      break;
   case 2:                             /* Open square                      */
      Move(RasP,  xscreen-size,     yscreen-size);
      Draw(RasP,  xscreen-size,     yscreen+size);
      Draw(RasP,  xscreen+size,     yscreen+size);
      Draw(RasP,  xscreen+size,     yscreen-size);
      Draw(RasP,  xscreen-size,     yscreen-size);
      break;
   case 3:                             /* Filled square                    */
      RectFill(RasP,xscreen-size,yscreen-size,xscreen+size,yscreen+size);
      break;
   case 4:                             /* Open circle                      */
      if((offset = (int *)malloc((size+1) * sizeof(int)))==NULL)
      {
         if(!warned)
         {
            ReqMessage(MainWind,"No memory for circle",0);
            warned = TRUE;
            break;
         }
      }

      for(j=0; j<=size; j++) 
         offset[j] = (int)(0.5+sqrt((double)((size*size) - (j*j))));

      Move(RasP,xscreen,yscreen+size);
      /* First quadrant */
      for(j=1;j<=size;j++)   Draw(RasP, xscreen+j, yscreen+offset[j]);
      /* Second quadrant */
      for(j=size-1;j>=0;j--) Draw(RasP, xscreen+j, yscreen-offset[j]);
      /* Third quadrant */
      for(j=1;j<=size;j++)   Draw(RasP, xscreen-j, yscreen-offset[j]);
      /* Fourth quadrant */
      for(j=size-1;j>=0;j--) Draw(RasP, xscreen-j, yscreen+offset[j]);

      free(offset);
      break;
   case 5:                             /* Filled circle                    */
      if((offset = (int *)malloc((size+1) * sizeof(int)))==NULL)
      {
         if(!warned)
         {
            ReqMessage(MainWind,"No memory for circle",0);
            warned = TRUE;
            break;
         }
      }

      for(j=0; j<=size; j++) 
         offset[j] = (int)(0.5+sqrt((double)((size*size) - (j*j))));

      AreaMove(RasP,xscreen,yscreen+size);
      /* First quadrant */
      for(j=1;j<=size;j++)   AreaDraw(RasP, xscreen+j, yscreen+offset[j]);
      /* Second quadrant */
      for(j=size-1;j>=0;j--) AreaDraw(RasP, xscreen+j, yscreen-offset[j]);
      /* Third quadrant */
      for(j=1;j<=size;j++)   AreaDraw(RasP, xscreen-j, yscreen-offset[j]);
      /* Fourth quadrant */
      for(j=size-1;j>=0;j--) AreaDraw(RasP, xscreen-j, yscreen+offset[j]);
      AreaEnd(RasP);
   
      free(offset);
      break;
   case 6:                             /* Open diamond                     */
      Move(RasP,     xscreen-size,  yscreen);
      Draw(RasP,     xscreen,       yscreen+size);
      Draw(RasP,     xscreen+size,  yscreen);
      Draw(RasP,     xscreen,       yscreen-size);
      Draw(RasP,     xscreen-size,  yscreen);
      break;
   case 7:                             /* Filled diamond                   */
      AreaMove(RasP, xscreen-size,  yscreen);
      AreaDraw(RasP, xscreen,       yscreen+size);
      AreaDraw(RasP, xscreen+size,  yscreen);
      AreaDraw(RasP, xscreen,       yscreen-size);
      AreaDraw(RasP, xscreen-size,  yscreen);
      AreaEnd(RasP);
      break;
   case 8:                             /* Open triangle                    */
      Move(RasP,     xscreen-size,  yscreen+size);
      Draw(RasP,     xscreen,       yscreen-size);
      Draw(RasP,     xscreen+size,  yscreen+size);
      Draw(RasP,     xscreen-size,  yscreen+size);
      break;
   case 9:                             /* Filled triangle                  */
      AreaMove(RasP, xscreen-size,  yscreen+size);
      AreaDraw(RasP, xscreen,       yscreen-size);
      AreaDraw(RasP, xscreen+size,  yscreen+size);
      AreaDraw(RasP, xscreen-size,  yscreen+size);
      AreaEnd(RasP);
      break;
   case 10:                            /* Open inverted triangle           */
      Move(RasP,     xscreen-size,  yscreen-size);
      Draw(RasP,     xscreen,       yscreen+size);
      Draw(RasP,     xscreen+size,  yscreen-size);
      Draw(RasP,     xscreen-size,  yscreen-size);
      break;
   case 11:                            /* Filled inverted triangle         */
      AreaMove(RasP, xscreen-size,  yscreen-size);
      AreaDraw(RasP, xscreen,       yscreen+size);
      AreaDraw(RasP, xscreen+size,  yscreen-size);
      AreaDraw(RasP, xscreen-size,  yscreen-size);
      AreaEnd(RasP);
      break;
   case 12:                            /* Plus sign                        */
      Move(RasP,  xscreen-size,     yscreen);
      Draw(RasP,  xscreen+size,     yscreen);
      Move(RasP,  xscreen,          yscreen+size);
      Draw(RasP,  xscreen,          yscreen-size);
      break;
   default:
      break;
   }

   Move(RasP,xscreen,yscreen);

   return(0);
}

/***************************************************************************/
/*>ScreenArc(struct RastPort *RasP, int xc, int yc, int size,
             double start, double end, int close)
   ----------------------------------------------------------
   Draws an arc on the Amiga screen, closed to form a slice if required.
   17.06.92 Original
   18.06.92 Fixed arc length for large angles.
*/
int ScreenArc(struct RastPort   *RasP,
          int               xc,
          int               yc,
          int               size,
          double            start,
          double            end,
          int               close)
{
   int      x1,   y1,
            x2,   y2;
   double   length,
            step,
            range,
            ang;
            
   if(end == start) return(0);

   x1 = (int)((double)size *  cos(start));
   y1 = (int)((double)size * -sin(start));
   x2 = (int)((double)size *  cos(end));
   y2 = (int)((double)size * -sin(end));
   
   if(close)      /* Draw in the line */
   {
      Move(RasP, xc, yc);
      Draw(RasP, x1+xc, y1+yc);
   }
   else
   {
      Move(RasP, x1+xc, y1+yc);
   }
   
   /* Find angular size of arc */
   if(end > start)   range = end-start;
   else              range = end + (360.0 - start);
   
   /* Find approximate length of arc. In radians, the following equation
      applies for calc of length of arc:
      Radius = 2 PI r
      Arc length = Radius * (arc angle/2 PI)
   => Arc length = r * arc angle
   */
   length = range * size;
   if(length == 0.0) return(0);
   
   /* Find angular step for approx 5 pixels */
   step = 5.0 * range/length;
   
   /* Draw the arc */
   for(ang=start; ang<=end; ang+=step)
   {
      x1 = (int)((double)size *  cos(ang));
      y1 = (int)((double)size * -sin(ang));
      /* Draw segment */
      Draw(RasP, x1+xc, y1+yc);
   }
   
   /* Draw final segment */
   if((ang-step < end) && (ang-step > end-step)) Draw(RasP, x2+xc, y2+yc);
   
   /* Close if required */
   if(close) Draw(RasP, xc, yc);
   
   return(0);
}

/***************************************************************************/
/*>ScreenFillArc(struct RastPort *RasP, int xc, int yc, int size,
                 double start, double end)
   --------------------------------------------------------------
   Draws a filled arc on the Amiga screen.
   17.06.92 Original
   18.06.92 Fixed arc length for large angles.
*/
int ScreenFillArc(struct RastPort   *RasP,
              int               xc,
              int               yc,
              int               size,
              double            start,
              double            end)
{
   int      x1,   y1,
            x2,   y2;
   double   length,
            step,
            range,
            ang;
            
   if(end == start) return(0);

   x1 = (int)((double)size *  cos(start));
   y1 = (int)((double)size * -sin(start));
   x2 = (int)((double)size *  cos(end));
   y2 = (int)((double)size * -sin(end));
   
   /* Draw in side of slice */
   AreaMove(RasP, xc, yc);
   AreaDraw(RasP, x1+xc, y1+yc);
   
   /* Find angular size of arc */
   if(end > start)  range = end-start;
   else             range = end + (360.0 - start);
   
   /* Find approximate length of arc. In radians, the following equation
      applies for calc of length of arc:
      Radius = 2 PI r
      Arc length = Radius * (arc angle/2 PI)
   => Arc length = r * arc angle
   */
   length = range * size;
   if(length == 0.0) return(0);
   
   /* Find angular step for approx 5 pixels */
   step = 5.0 * range/length;
   
   /* Draw the arc */
   for(ang=start; ang<=end; ang+=step)
   {
      x1 = (int)((double)size *  cos(ang));
      y1 = (int)((double)size * -sin(ang));
      /* Draw segment */
      AreaDraw(RasP, x1+xc, y1+yc);
   }
   
   /* Draw final segment */
   if((ang-step < end) && (ang-step > end-step)) AreaDraw(RasP, x2+xc, y2+yc);
   
   /* Close and end */
   AreaDraw(RasP, xc, yc);
   AreaEnd(RasP);
   
   return(0);
}

