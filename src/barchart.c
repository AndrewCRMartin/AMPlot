/***************************************************************************

   Program:    AMPlot2
   File:       barchart.c
   
   Version:    V1.0
   Date:       08.06.92
   Function:   Barchart plot style
   
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
/* Includes
*/
#include "includes.h"

/**************************************************************************/
/*>BarGraph(int dest, FH *fh)
   -----------------------------------------
   27.03.92 Framework
   08.06.92 Original: Outline and Overlay styles only
   09.06.92 Added Grouped and Stacked styles
   12.06.92 Stopped replot from Rexx from infinite looping
   02.07.92 Moved Rexx launch command to DoPlot()
*/
int BarGraph(int               dest,
         FH *fh)
{
   DATASET  *d;
   int      NData,
            PlotNum,
            i;
   double   XStart,              /* Left of current bar                    */
            XEnd,                /* Right of the last bar actually drawn   */
            *YTotal = NULL;      /* Array of y heights for stacked style   */
            
   
   /* First, if its grouped or stacked, check the data is applicable and,
      if not, cancel these settings and re-enter the routine.
   */
   if(barstyle.style == STYLE_GROUPED ||
      barstyle.style == STYLE_STACKED)
   {
      int      ok;               /* Flag for data OK                       */
      double   width;            /* Width of data bar                      */
      
      /* Check data. Each dataset must have same number of points and x 
         widths must be the same.
      */
      ok = TRUE;
      NData = 0;
      width = data->x[1] - data->x[0];
      
      for(d=data; d; NEXT(d))
      {
         /* Check number of data points */
         if(NData != 0)
         {
            if(d->ndata != NData)
            {
               ok = FALSE;
               break;
            }
         }
         else
         {
            NData = d->ndata;
         }
         
         /* Check widths */
         for(i=1; i<d->ndata; i++)
         {
            if((d->x[i] - d->x[i-1]) != width)
            {
               ok = FALSE;
               break;
            }
         }
      }
      
      
      if(!ok)
      {
         switch(barstyle.style)
         {
         case STYLE_GROUPED:
            ReqMessage2(MainWind,"Grouped style not allowed","with this dataset",0);
            break;
         case STYLE_STACKED:
            ReqMessage2(MainWind,"Stacked style not allowed","with this dataset",0);
            break;
         default:
            break;
         }
            
         barstyle.style = STYLE_OVERLAYED;
         SetScaling(FALSE);

         /* Recurse */
         BarGraph(dest, fh);
         return(0);
      }
   }
   
   /* If we've got the stacked style, we need to create an array to store the
      total Y heights of each bar.
   */
   if(barstyle.style == STYLE_STACKED)
   {
      if((YTotal = (double *)malloc(NData * sizeof(double))) == NULL)
      {
         ReqMessage(MainWind,"No memory for stacked style",0);
         barstyle.style = STYLE_OVERLAYED;
         /* Recurse */
         BarGraph(dest, fh);
         return(0);
      }
      else  /* Clear our new array */
      {
         for(i=0; i<NData; i++) YTotal[i] = 0.0;
      }
   }

   DrawAxes(dest,fh,1);    /* Grid only  */
   
   for(d=data, PlotNum = 0; d; NEXT(d), PlotNum++)
   {
      AMSetPen(dest,fh,d->pen);
      AMSetLineStyle(dest,fh,d->linestyle);

      XStart = XEnd = graph.xmin;
      AMMove(dest,fh,XStart,0.0);

      if(barstyle.style == STYLE_OUTLINE)       /* The OUTLINE style       */
      {
         for(i=0; i<d->ndata; i++)
         {
            /* Actually draw or move */
            if((graph.clip) && ((d->x[i]   < graph.xmin) ||
                                (d->x[i]   > graph.xmax) ||
                                (d->y[i]   < graph.ymin) ||
                                (d->y[i]   > graph.ymax)))
            {
               XStart = d->x[i];
               AMMove(dest,fh,XStart,d->y[i]);
            }
            else
            {
               AMDraw(dest,fh,XStart,d->y[i]);
               XStart = XEnd = d->x[i];
               AMDraw(dest,fh,XStart,d->y[i]);
            }
         }
         /* Finish the last line */
         AMDraw(dest,fh,XEnd,0.0);
      }
      else if(barstyle.style == STYLE_GROUPED   /* The GROUPED style       */
              && graph.nplots > 1)
      {
         double   xlast,
                  xnow;
         
         xlast = graph.xmin + ((d->x[0] - graph.xmin) * PlotNum / graph.nplots);
         
         for(i=0; i<d->ndata; i++)
         {
            if(i==0)
               xnow = xlast + (d->x[0] - graph.xmin)/graph.nplots;
            else
               xnow = xlast + (d->x[i] - d->x[i-1])/graph.nplots;
               
            doBar(dest, fh, xlast, 0.0, xnow, d->y[i], PlotNum, d->pen);

            if(i==0)
               xlast += (d->x[0] - graph.xmin);
            else
               xlast += (d->x[i] - d->x[i-1]);
         }
      }
      else if(barstyle.style == STYLE_STACKED   /* The STACKED style       */
              && graph.nplots > 1)
      {
         double xlast;
         
         xlast = graph.xmin;
         
         for(i=0; i<d->ndata; i++)
         {
            doBar(dest, fh, xlast,   YTotal[i],
                            d->x[i], YTotal[i] + d->y[i], PlotNum, d->pen);
            xlast = d->x[i];
            YTotal[i] += d->y[i];
         }
      }
      else                                      /* Simple overlayed style  */
      {
         double xlast;
         
         xlast = graph.xmin;
         
         for(i=0; i<d->ndata; i++)
         {
            doBar(dest, fh, xlast, 0.0, d->x[i], d->y[i], PlotNum, d->pen);
            xlast = d->x[i];
         }
      }
   }
   AMSetLineStyle(dest,fh,0);
   
   if(barstyle.style != STYLE_GROUPED && barstyle.style != STYLE_STACKED)
   {
      DoErrorBars(dest, fh, TRUE);
      DoFitting(dest, fh);
   }
   
   DrawAxes(dest,fh,2);    /* Not grid */

   DoExtraLabels(dest, fh);
   DoLineKey(dest, fh);
   DoGraphTitle(dest, fh);

   if(YTotal != NULL)
   {
      free(YTotal);
      YTotal = NULL;
   }
   
   return(0);
}

/**************************************************************************/
/*>doBar(int dest, FH *fh, double x1, double y1,
      double x2, double y2, int PlotNum, int pen)
   ------------------------------------------------------------
   Draw a bar, filled if required.
   08.06.92 Original
   09.06.92 Corrected treatment of Stacked style
*/   
int doBar(int               dest,
      FH *fh,
      double            x1, 
      double            y1,
      double            x2, 
      double            y2,
      int               PlotNum,
      int               pen)
{
   double   shrink;
   int      pattern = 0;

   if(x2 < x1)                /* Swap x if necessary                       */
   {
      double temp;
      
      temp = x1;
      x1   = x2;
      x2   = temp;
   }

   if(y2 < y1)                /* Swap y if necessary                       */
   {
      double temp;
      
      temp = y1;
      y1   = y2;
      y2   = temp;
   }

   if(barstyle.shrink!=0.0)   /* Calculate shrink for this bar             */
   {
      if(barstyle.style == STYLE_GROUPED)
         shrink = (x2-x1)/barstyle.shrink;
      else if(barstyle.style == STYLE_STACKED)
         shrink = ((barstyle.shrinkall)?1:0) * (x2-x1)/barstyle.shrink;
      else
         shrink = ((PlotNum + ((barstyle.shrinkall)?1:0)) * (x2-x1)/barstyle.shrink);
   }
   else
   {
      shrink = 0.0;
   }
   
   /* Modify coordinates by shrink factor */
   x1 += shrink;
   x2 -= shrink;
   
   if(filllist != NULL)    /* See if there's a fill in this bar            */
   {
      FILLITEM *p;
      
      for(p=filllist; p; NEXT(p))
      {
         /* Find the *last* entry in the fill list which matches */
         if((p->x > x1 && p->x < x2 && p->y > y1 && p->y < y2) ||
            (barstyle.fillall && p->dataset == PlotNum+1))
            pattern = p->pattern;
      }
   }
   
   AMRect(dest, fh, x1, y1, x2, y2, pattern, pen);
   
   return(0);
}

/**************************************************************************/
/*>int FindBarDataset(double x, double y)
   --------------------------------------
   Given data coordinates, find which barchart dataset contains these.
   These are numbered from 1. (0 indicates no dataset).
   09.06.92 Original
*/
int FindBarDataset(double x,
                   double y)
{
   int      PlotNum,
            i;
   double   xlast,
            *YTotal = NULL;
   DATASET  *d;
            
   if(barstyle.style == STYLE_STACKED)
   {
      if((YTotal = (double *)malloc(data->ndata * sizeof(double))) == NULL)
      {
         ReqMessage(MainWind,"No memory for stacked bars",0);
         return(0);
      }
      else  /* Clear our new array */
      {
         for(i=0; i<data->ndata; i++) YTotal[i] = 0.0;
      }
   }
   
   for(d=data, PlotNum=0; d; NEXT(d), PlotNum++)
   {
      for(i=0; i<d->ndata; i++)
      {
         if(barstyle.style == STYLE_GROUPED        /* The GROUPED style    */
            && graph.nplots > 1)
         {
            double   xnow;
            
            xlast = graph.xmin + ((d->x[0] - graph.xmin) * PlotNum / graph.nplots);
            
            for(i=0; i<d->ndata; i++)
            {
               if(i==0)
                  xnow = xlast + (d->x[0] - graph.xmin)/graph.nplots;
               else
                  xnow = xlast + (d->x[i] - d->x[i-1])/graph.nplots;
                  
               if(x > xlast && 
                  x < xnow  && 
                  ((y > 0.0 && y < d->y[i]) || (y < 0.0 && y > d->y[i])))
                  return(PlotNum+1);
                  
               if(i==0)
                  xlast += (d->x[0] - graph.xmin);
               else
                  xlast += (d->x[i] - d->x[i-1]);
            }
         }
         else if(barstyle.style == STYLE_STACKED   /* The STACKED style    */
                 && graph.nplots > 1)
         {
            xlast = graph.xmin;
            
            for(i=0; i<d->ndata; i++)
            {
               if(x   > xlast     && 
                  x   < d->x[i]   && 
                  ((y > YTotal[i] && y < YTotal[i] + d->y[i]) ||
                   (y < YTotal[i] && y > YTotal[i] + d->y[i])))
                  {
                     if(YTotal)
                     {
                        free(YTotal);
                        YTotal = NULL;
                     }
                     return(PlotNum+1);
                  }

               xlast = d->x[i];
               YTotal[i] += d->y[i];
            }
         }
         else
         {
            xlast = graph.xmin;
            
            for(i=0; i<d->ndata; i++)
            {
               if(x > xlast   && 
                  x < d->x[i] && 
                  ((y > 0.0   && y < d->y[i]) ||
                   (y < 0.0   && y > d->y[i])))
                  return(PlotNum+1);

               xlast = d->x[i];
            }
         }
      }
   }
   
   if(YTotal)
   {
      free(YTotal);
      YTotal = NULL;
   }
   return(0);
}
