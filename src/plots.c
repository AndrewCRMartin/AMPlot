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
#ifdef AMIGA
#define AMIGA_MEM
#endif

/**************************************************************************/
/* Includes
*/
#include "includes.h"


#ifdef AMIGA_MEM
#include <exec/memory.h>
#endif

/**************************************************************************/
/*>SetScaling(force)
   ----------------
   Calculates scaling for the graph by looking through all min and max
   values. The force flag overrides user specified bounds and ticks.
   27.03.92 Original
   31.03.92 Calls CalcBounds() and CalcTicks()
   07.04.92 Only modifies bounds and ticks if these haven't been set in the
            datafile.
   10.04.92 Added log support
   14.04.92 Added calc. of maxdata
   29.04.92 Added setting of label precision, check on data presence
   05.05.92 Changed to support empty Rexx graphs
   17.06.92 Changed to support Pie charts correctly; added force flag
   19.06.92 Changed so scaling in both directions is same for Pies
   03.07.92 Added graph.usersub handling
*/
int SetScaling(int force)
{
   double   XSize,
            YSize,
            xtick,
            ytick,
            xsub,
            ysub;
   DATASET  *d;
   
   /* Find max number of datapoints in any one plot */
   graph.maxdata = 0;
   for(d=data; d; NEXT(d)) 
      if(d->ndata > graph.maxdata) graph.maxdata = d->ndata;
          
   if(graph.maxdata)
   {
      if(graph.style==STYLE_PIE)
      {
         graph.xmax     =  PIEBOUNDS;
         graph.xmin     = -PIEBOUNDS;
         graph.ymax     =  PIEBOUNDS;
         graph.ymin     = -PIEBOUNDS;
      }
      else
      {
         /* Calculate `best' bounds and ticks */
         if(!graph.userbounds || force)
            CalcBounds(  &graph.xmin,  &graph.xmax,  &graph.ymin, &graph.ymax);

         CalcTicks(0, &xtick, &ytick, &xsub, &ysub);
         if(!graph.userticks || force)
         {
            graph.xtick = xtick;
            graph.ytick = ytick;
         }
         if(!graph.usersub || force)
         {
            graph.xsub = xsub;
            graph.ysub = ysub;
         }

         if(!graph.userprec || force)
         {
            if(log10(graph.xsub) < 0.0)
               graph.xprecision = (int)ceil(fabs(log10(graph.xtick)));
            if(log10(graph.ysub) < 0.0) 
               graph.yprecision = (int)ceil(fabs(log10(graph.ytick)));
         }
      }

   }
   
   if(graph.maxdata
#ifdef REXX_SUPPORT 
      || rexx.empty
#endif
)
   {
      /* Calculate scaling from the bounds */
      if(graph.logx)
         XSize             = log10(graph.xmax)-log10(graph.xmin);
      else
         XSize             = graph.xmax-graph.xmin;
      
      if(graph.logy)
         YSize             = log10(graph.ymax)-log10(graph.ymin);
      else
         YSize             = graph.ymax-graph.ymin;
   
      graph.XScale      = display.width/XSize;
      graph.XPScale     = 1.0/XSize;
      graph.YScale      = display.height/YSize;
      graph.YPScale     = 1.0/YSize;

      if(graph.style == STYLE_PIE)  /* Set scaling same in both directions */
      {
         if(graph.XScale < graph.YScale)
         {
            graph.YScale   = graph.XScale;
            graph.YPScale  = graph.XPScale;
         }
         else
         {
            graph.XScale   = graph.YScale;
            graph.XPScale  = graph.YPScale;
         }
      }
   }

   return(0);
}

/**************************************************************************/
/*>DoPlot(int destination, FH *fh)
   ----------------------------------------------
   Main plot dispatch routine. Selects plotting routine based on graph
   style variable, sending the graph to the device named by destination.
   27.03.92 Original
   06.04.92 Checks data exists
   07.05.92 Added call to AMInitPlot(), etc
   09.06.92 Changed call to DrawAxes() to add new parameter
   12.06.92 Added WaitPointer()
   02.07.92 Moved Rexx launch so it always comes from here. Added
            WaitForRexx() (effectively making the Rexx call synchronous)
            rexx.fromrexx is now cleared in the main IDCMP loop when a
            menu or gadget event occurs and by the REXX ENDPLOT command.
   03.07.92 Added path check for ARexx macro
   07.07.92 Added check on rexx.rexxplot
   20.07.92 Added DEMO_VERSION conditional code
   23.09.92 Added DoExtraLabels(), DoLineKey(), DoGraphTitle() to empty
            graph code.

*/
int DoPlot(int destination,
       FH *fh)
{
#ifdef DEMO_VERSION
   if(destination != DEST_SCREEN)
   {
      WriteMessage("Plotting disabled in demo version!");
      return(0);
   }
#endif

   WaitPointer(MainWind);

   if(data)
   {
      
      AMInitPlot(destination, fh, output.xsize,    output.ysize,
                                  output.xoffset,  output.yoffset);
      switch(graph.style)
      {
      case STYLE_LINE:
         LineGraph(destination,fh);
         break;
      case STYLE_SCATTER:
         ScatterGraph(destination,fh);
         break;
      case STYLE_BAR:
         BarGraph(destination,fh);
         break;
      case STYLE_PIE:
         PieGraph(destination,fh);
         break;
      default:
         break;
      }
#ifdef REXX_SUPPORT
      if(!rexx.fromrexx)
      {
         /* If a Rexx macro has been specified and this call hasn't come from
            Rexx, execute the macro
         */
         if(rexx.RexxMacro[0])
         {
            if((strchr(rexx.RexxMacro,':')==NULL) &&
               (strchr(rexx.RexxMacro,'/')==NULL))
            {
               WriteMessage("Full path must be specified for ARexx macro");
            }
            else
            {
               LaunchCmd(rexx.RexxMacro);
               rexx.fromrexx=TRUE;
               WaitForRexx();
            }
         }

         /* End the plot if it didn't come from Rexx */
         if(!rexx.rexxplot) AMEndPlot(destination, fh);
      }
#else
      AMEndPlot(destination, fh);
#endif
   }
#ifdef REXX_SUPPORT
   else if(rexx.empty)     /* Empty graph from ARexx */
   {
      if(destination == DEST_SCREEN) WriteMessage(" ");
      
      AMInitPlot(destination, fh, output.xsize,    output.ysize,
                                  output.xoffset,  output.yoffset);
      DrawAxes(destination,fh,0);

      if(!rexx.fromrexx)
      {
         /* If a Rexx macro has been specified and this call hasn't come from
            Rexx, execute the macro
         */
         if(rexx.RexxMacro[0])
         {
            if((strchr(rexx.RexxMacro,':')==NULL) &&
               (strchr(rexx.RexxMacro,'/')==NULL))
            {
               WriteMessage("Full path must be specified for ARexx macro");
            }
            else
            {
               LaunchCmd(rexx.RexxMacro);
               rexx.fromrexx=TRUE;
               WaitForRexx();
            }
         }

         /* End the plot if it didn't come from Rexx */
         if(!rexx.rexxplot) AMEndPlot(destination, fh);
      }

      DoExtraLabels(destination, fh);
      DoLineKey(destination, fh);
      DoGraphTitle(destination, fh);
   }
#endif
   
   ClearPointer(MainWind);
   
   return(0);
}

/**************************************************************************/
/*>LineGraph(int destination, FH *fh)
   -------------------------------------------------
   27.03.92 Framework
   07.04.92 Original - no dp marks, error bars, fitting, key or labels.
   08.04.92 Added calls to do these. Added linestyle.
   06.05.92 Added clearing of linestyle
   09.06.92 Changed call to DrawAxes() to add new parameter
   12.06.92 Stopped replot from Rexx from infinite looping
   02.07.92 Moved Rexx launch command to DoPlot()
*/
int LineGraph(int               dest,
          FH *fh)
{
   DATASET  *d;
   int      i;
   
   DrawAxes(dest,fh,0);

   for(d=data; d; NEXT(d))
   {
      AMSetPen(dest,fh,d->pen);
      AMSetLineStyle(dest,fh,d->linestyle);
      AMMove(dest,fh,d->x[0],d->y[0]);
      for(i=1;i<d->ndata;i++)
      {
         if((graph.clip) && ((d->x[i]   < graph.xmin) ||
                             (d->x[i-1] < graph.xmin) ||
                             (d->x[i]   > graph.xmax) ||
                             (d->y[i]   < graph.ymin) ||
                             (d->y[i-1] < graph.ymin) ||
                             (d->y[i-1] > graph.ymax) ||
                             (d->y[i]   > graph.ymax)))
            AMMove(dest,fh,d->x[i],d->y[i]);
         else
            AMDraw(dest,fh,d->x[i],d->y[i]);
      }
      AMEndLine(dest,fh);
   }
   AMSetLineStyle(dest,fh,0);
   
   DoDPMarks(dest, fh);
   DoErrorBars(dest, fh, FALSE);
   DoFitting(dest, fh);
   DoExtraLabels(dest, fh);
   DoLineKey(dest, fh);
   DoGraphTitle(dest, fh);

   return(0);
}

/**************************************************************************/
/*>ScatterGraph(int destination, FH *fh)
   ----------------------------------------------------
   Plots a scatter plot to specified destination.
   27.03.92 Framework
   06.05.92 Original
   09.06.92 Changed call to DrawAxes() to add new parameter
   12.06.92 Stopped replot from Rexx from infinite looping
   02.07.92 Moved Rexx launch command to DoPlot()
   27.07.92 Added AMEndLine() for joined lines.
*/
int ScatterGraph(int               dest,
             FH *fh)
{
   DATASET  *d;
   int      i;
   
   DrawAxes(dest,fh,0);

   /* Draw any lines for linked pens */
   for(d=data; d; NEXT(d))
   {
      if(pens[d->pen - 1].join)
      {
         AMSetPen(dest,fh,d->pen);
         AMSetLineStyle(dest,fh,d->linestyle);
         AMMove(dest,fh,d->x[0],d->y[0]);
         for(i=1;i<d->ndata;i++)
         {
            if((graph.clip) && ((d->x[i]   < graph.xmin) ||
                                (d->x[i-1] < graph.xmin) ||
                                (d->x[i]   > graph.xmax) ||
                                (d->y[i]   < graph.ymin) ||
                                (d->y[i-1] < graph.ymin) ||
                                (d->y[i-1] > graph.ymax) ||
                                (d->y[i]   > graph.ymax)))
               AMMove(dest,fh,d->x[i],d->y[i]);
            else
               AMDraw(dest,fh,d->x[i],d->y[i]);
         }
         AMEndLine(dest,fh);
      }
   }
   AMSetLineStyle(dest,fh,0);
   
   DoDPMarks(dest, fh);
   DoErrorBars(dest, fh, FALSE);
   DoFitting(dest, fh);
   DoExtraLabels(dest, fh);
   DoLineKey(dest, fh);
   DoGraphTitle(dest, fh);

   return(0);
}

/**************************************************************************/
/*>DoGraphTitle(int dest, FH *fh)
   ---------------------------------------------
   Displays the graph title
   07.04.92 Original
   06.05.92 Added check that there is a title
*/
int DoGraphTitle(int                 dest, 
             FH   *fh)
{
   if(GTitle.text[0] != '\0')
   {
      AMSetPen(dest, fh, 6);
      AMSetFont(dest, fh, &GTitle);
      AMCBText(dest, fh, GTitle.x, GTitle.y, GTitle.text);
   }

   return(0);
}

/**************************************************************************/
/*>DoErrorBars(int dest, FH *fh, int barchart)
   ----------------------------------------------------------
   Draws the error bars
   08.04.92 Original
   08.06.92 Added correct handling of barcharts, corrected -ve array access
   13.07.92 Divided error bar crosswidth by 2.0
   22.07.92 Added check on error data > 0.0 and moved AMEndLine() so it
            works if the cross bar is of zero width.
*/
int DoErrorBars(int               dest, 
            FH *fh,
            int               barchart)
{
   DATASET  *d;
   int      i;
   double   XStart,           /* Start of current bar    */
            xpos;             /* Current plot position   */
   
   if(graph.errors && graph.columns > 2)
   {
      for(d=data; d; NEXT(d))
      {
         XStart = graph.xmin;
         AMSetPen(dest,fh,d->pen);

         for(i=0;i<d->ndata;i++)
         {
            if(barchart)
               xpos = XStart + (d->x[i] - XStart)/2.0;
            else
               xpos = d->x[i];

            if((graph.clip) && ((xpos      < graph.xmin) ||
                                (xpos      > graph.xmax) ||
                                (d->y[i]   < graph.ymin) ||
                                (d->y[i]   > graph.ymax)))
            {
               ; /* Do nothing */
            }
            else
            {
               /* Draw the error bar */
               AMMove(dest, fh, xpos, d->y[i] + d->ep[i]);
               AMDraw(dest, fh, xpos, d->y[i] - d->em[i]);
               /* And the cross parts */
               if(graph.ebarwidth != 0.0)
               {
                  if(d->ep[i] > 0.0)
                  {
                     AMLine(dest, fh, xpos, d->y[i] + d->ep[i],
                                      -(graph.ebarwidth / 2.0), 0.0);
                     AMLine(dest, fh, xpos, d->y[i] + d->ep[i],
                                      +(graph.ebarwidth / 2.0), 0.0);
                  }
                  if(d->em[i] > 0.0)
                  {
                     AMLine(dest, fh, xpos, d->y[i] - d->em[i],
                                      -(graph.ebarwidth / 2.0), 0.0);
                     AMLine(dest, fh, xpos, d->y[i] - d->em[i],
                                      +(graph.ebarwidth / 2.0), 0.0);
                  }
               }
               AMEndLine(dest,fh);
               
               XStart = d->x[i];
            }
         }
      }
   }

   return(0);
}

/**************************************************************************/
/*>DoExtraLabels(int dest, FH *fh)
   ----------------------------------------------
   Displays the extra labels
   08.04.92 Original
   06.05.92 Added check that there is some text
*/
int DoExtraLabels(int               dest,
              FH *fh)
{
   TEXTLIST *l;
   
   AMSetPen(dest, fh, 6);
   for(l=labels; l; NEXT(l))
   {
      if(l->stext.text[0] != '\0')
      {
         AMSetFont(dest, fh, &(l->stext));
         AMText(dest, fh, l->stext.x, l->stext.y, l->stext.text);
      }
   }
   
   return(0);
}

/**************************************************************************/
/*>DoDPMarks(int dest, FH *fh)
   ------------------------------------------
   Draws the datapoint marks as appropriate
   08.04.92 Framework
   06.05.92 Original
   15.05.92 Added gotdp check before setting pen.
*/
int DoDPMarks(int               dest,
          FH *fh)
{
   DATASET  *d;
   int      i,
            gotdp;
   
   for(d=data; d; NEXT(d))
   {
      gotdp = FALSE;
      for(i=0; i<d->ndata; i++) if(pens[d->pen - 1].mark) gotdp = TRUE;
      if(gotdp) AMSetPen(dest,fh,d->pen);

      for(i=0; i<d->ndata; i++)
      {
         if((graph.clip) && ((d->x[i] < graph.xmin) ||
                             (d->x[i] > graph.xmax) ||
                             (d->y[i] < graph.ymin) ||
                             (d->y[i] > graph.ymax)))
            ;  /* Out of bounds, do nothing */
         else
            if(pens[d->pen - 1].mark)
               AMDPMark(dest, fh, d->x[i], d->y[i], pens[d->pen - 1].mark);
      }
   }
   
   return(0);
}

/**************************************************************************/
/*>double alog(double x)
   ---------------------
   Calculates the anti-log of a number
   10.04.92 Original
*/
double alog(double x)
{
   double   retval;
   int      factor=0;
   
   if(x < 0.0)
   {
      while(x < 0.0)
      {
         factor++;
         x += 1.0;
      }
      retval  = pow((double)10.0, x);
      retval /= pow((double)10.0, (double)factor);
   }
   else
   {
      retval = pow((double)10.0, x);
   }
   return(retval);
}

/***************************************************************************/
/*>DoFitting(int dest, FH *fh)
   ------------------------------------------
   Calls the required fitting routines to draw fitted curves
   08.04.92 Framework
   14.04.92 Original
   30.04.92 Added Rexx fit, changed call to DoSVDFit()
   01.05.92 Changed calls to DoSVDFit(), added conditions for writing info.
   12.06.92 Moved WaitPointer() to DoPlot()
   03.07.92 Added messages.
*/
int DoFitting(int               dest, 
          FH *fh)
{
   if(fitting.regression)  
   {
      if(fitting.regress_write)
      {
         DoSVDFit(dest, fh, fitting.regress_npoly, fitting.regress_tol,
                            "AMPlotFit.data",      &funcs);
         WriteMessage("Fitting information written to AMPlotFit.data");
      }
      else
      {
         DoSVDFit(dest, fh, fitting.regress_npoly, fitting.regress_tol,
                            NULL,                  &funcs);
      }
   }
   if(fitting.robust)      DoRobust(dest, fh);
   if(fitting.fourier)     DoSmooth(dest, fh);
#ifdef REXX_SUPPORT
   if(fitting.rexx)
   {
      if(fitting.rexx_write)
      {
         DoSVDFit(dest, fh, fitting.rexx_npoly,    fitting.rexx_tol,
                            "AMPlotRexxFit.data",  &RexxFunc);
         WriteMessage("Fitting information written to AMPlotRexxFit.data");
      }
      else
      {
         DoSVDFit(dest, fh, fitting.rexx_npoly,    fitting.rexx_tol,
                            NULL,                  &RexxFunc);
      }

   }
#endif
   
   return(0);
}

/**************************************************************************/
/*>DoLineKey(int dest, FH *fh)
   ------------------------------------------
   Creates a key for line graph.
   08.04.92 Original.
   27.04.92 Fixed for logs
   06.05.92 Added clearing of linestyle
   07.05.92 Corrected scaling
   10.06.92 Added barchart support
   06.07.92 Altered spacing to max(marksize,fontsize)
*/
int DoLineKey(int               dest,
          FH *fh)
{
   double   x,                /* Coords of start of key                    */
            y,
            xend,             /* x-coord of end of key line                */
            xhalf,            /* x-coord of half way along key line        */
            xgap,             /* x-coord of end of key line & gap          */
            fontsize,         /* Size of font (pt)                         */
            length = 40.0,    /* Length of line (in final coords)          */
            dlen,             /* Length in data coords                     */
            gap = 2.0,        /* Gap between line and text (final coords)  */
            dgap,             /* Gap in data coords                        */
            swatch,           /* Size of bar chart swatch                  */
            dswatch_x,        /* Swatch in data coords                     */
            dswatch_y1,
            dswatch_y2,
            xscale,           /* PS or screen scaling                      */
            yscale,           /* PS or screen scaling                      */
            ystep;            /* Step along y-axis for each item           */
   DATASET  *d;               /* Pointer for current dataset               */
   int      PlotNum;          /* Dataset number                            */
   
   /* Set scaling */
   xscale   = graph.XScale;
   yscale   = graph.YScale;
          
   fontsize = data->key.PSSize;
   x        = data->key.x;          /* Start of key                        */
   y        = data->key.y;

   ystep    = MAX(fontsize,graph.dpsize);
            


   /* Return if log axes and values invalid */
   if(graph.logx && x <= 0.0) return(0);
   if(graph.logy && y <= 0.0) return(0);
   
   /* Set font if there's any text */
   for(d=data; d; NEXT(d))
   {
      if(d->key.text[0] != '\0')
      {
         AMSetFont(dest, fh, &(data->key));
         break;
      }
   }
   
   /* For the moment we'll make barchart swatches 1pt smaller than the
      font size, but not bigger than 20pt.
   */
   swatch = fontsize - 1.0;
   if(swatch > 20.0) swatch = 20.0;
   
   /* Convert length of line and gap in final coords to data coords */
   if(graph.logx)
   {
      xhalf     = alog(log10(x) + length/(2.0 * xscale));
      xend      = alog(log10(x) + length/xscale);
      xgap      = alog(log10(x) + (length+gap)/xscale);
      
      dswatch_x = alog(log10(xhalf) + swatch/xscale);
   }
   else
   {
      dlen      = length / xscale;
      dgap      = gap    / xscale;

      xhalf     = x + (dlen/2.0);
      xend      = x + dlen;
      xgap      = xend + dgap;
      
      dswatch_x = xhalf + swatch/xscale;
   }
   
   if(graph.logy)
   {
      dswatch_y1 = alog(log10(y) - swatch/(2.0 * yscale));
      dswatch_y2 = alog(log10(y) + swatch/(2.0 * yscale));
   }
   else
   {
      dswatch_y1 = y - swatch/(2.0 * yscale);
      dswatch_y2 = y + swatch/(2.0 * yscale);
   }

   for(d=data, PlotNum=0; d; NEXT(d), PlotNum++)
   {
      if(d->key.text[0] != '\0')
      {
         /* Set pen and line style */
         AMSetPen(dest, fh, d->pen);
   
         /* Draw a line */
         if(graph.style == STYLE_LINE || 
           (graph.style == STYLE_SCATTER && pens[d->pen - 1].join))
         {
            AMSetLineStyle(dest, fh, d->linestyle);
            AMMove(dest, fh, x,    y);
            AMDraw(dest, fh, xend, y);
            AMEndLine(dest,fh);
         }
         AMSetLineStyle(dest, fh, 0);

         if(graph.style == STYLE_LINE || graph.style == STYLE_SCATTER)
         {
            /* Add a datapoint mark */
            AMDPMark(dest, fh, xhalf, y, pens[d->pen - 1].mark);
         }
         else if(graph.style == STYLE_BAR)
         {
            /* Do a swatch */
            FILLITEM *f;
            int      pattern = 0;
            
            for(f=filllist; f; NEXT(f))
               if(f->dataset == PlotNum+1) pattern = f->pattern;
   
            AMRect(dest, fh, xhalf,     dswatch_y1, 
                             dswatch_x, dswatch_y2, pattern, d->pen);
            AMMove(dest, fh, xhalf,     dswatch_y1);
            AMDraw(dest, fh, dswatch_x, dswatch_y1);
            AMEndLine(dest, fh);
         }
         
         /* Reset pen for text */
         AMSetPen(dest, fh, 6);
   
         /* Write the text */
         AMCText(dest, fh, xgap, y, d->key.text);
   
         /* Step along y by fontsize */
         if(graph.logy)
         {
            y           = alog(log10(y) - ystep/yscale);
            dswatch_y1  = alog(log10(dswatch_y1) - ystep/yscale);
            dswatch_y2  = alog(log10(dswatch_y2) - ystep/yscale);
         }
         else
         {
            y          -= ystep/yscale;
            dswatch_y1 -= ystep/yscale;
            dswatch_y2 -= ystep/yscale;
         }
      }
   }
   
   return(0);
}

/***************************************************************************/
/*>DoSmooth(int dest, FH *fh)
   -----------------------------------------
   Performs and plots Fourier smoothing.
   27.04.92 Original
   06.05.92 Added clearing of linestyle
*/
int DoSmooth(int               dest,
         FH *fh)
{
   DATASET *d;
   
   double *y;
   int    nmax,
          j,
          nmin,
          dsetnum;


   for(d=data, dsetnum = 1; d; NEXT(d), dsetnum++)
   {
   
      /* Allocate working array for smoothing */
      nmin = d->ndata + 2.0 * fitting.smooth_pts;  /* Minimum size         */
      nmax = 2;
      while(nmax<nmin) nmax *= 2;                  /* Next > power of 2    */
      if((y = (double *)malloc(nmax * sizeof(double))) == NULL)
      {
         char buffer[24];
         
         sprintf(buffer,"Dataset %d",dsetnum);
         ReqMessage2(MainWind,"No memory for smoothing", buffer, 0); /* 29.05.97 Hadd extra "buffer" */
         continue;
      }

      /* Copy data into working array */
      if(graph.logy)
         for(j=0; j<d->ndata; j++)  y[j] = log10(d->y[j]);
      else
         for(j=0; j<d->ndata; j++)  y[j] = d->y[j];
      
      /* Do the smoothing */
      if(smooft(y, d->ndata, nmax, fitting.smooth_pts))
      {
         /* This should never occur, since the array is allocated to
            the correct size before calling smooft(), but just in case...
         */
         char buffer[24];
         
         sprintf(buffer,"Dataset %d",dsetnum);
         ReqMessage2(MainWind,buffer,"No memory for smoothing", 0); /* 29.05.97 Hadd extra "buffer" */
         continue;
      }
      
      /* Plot the smoothed data */
      AMSetPen(dest,fh,d->pen);
      AMSetLineStyle(dest,fh,d->linestyle);
      
      if(graph.logy)
      {
         AMMove(dest,fh,d->x[0],alog(y[0]));
         for(j=1; j<d->ndata; j++)
         {
            if((graph.clip) && ((d->x[j]        < graph.xmin) ||
                                (d->x[j-1]      < graph.xmin) ||
                                (d->x[j]        > graph.xmax) ||
                                (alog(y[j])     < graph.ymin) ||
                                (alog(y[j-1])   < graph.ymin) ||
                                (alog(y[j-1])   > graph.ymax) ||
                                (alog(y[j])     > graph.ymax)))
               AMMove(dest,fh,d->x[j],alog(y[j]));
            else
               AMDraw(dest,fh,d->x[j],alog(y[j]));
         }
      }
      else
      {
         AMMove(dest,fh,d->x[0],y[0]);
         for(j=1; j<d->ndata; j++)
         {
            if((graph.clip) && ((d->x[j]   < graph.xmin) ||
                                (d->x[j-1] < graph.xmin) ||
                                (d->x[j]   > graph.xmax) ||
                                (y[j]      < graph.ymin) ||
                                (y[j-1]    < graph.ymin) ||
                                (y[j-1]    > graph.ymax) ||
                                (y[j]      > graph.ymax)))
               AMMove(dest,fh,d->x[j],y[j]);
            else
               AMDraw(dest,fh,d->x[j],y[j]);
         }
      }
      AMEndLine(dest,fh);

      free(y);    /* Free allocated memory */
   }

   AMSetLineStyle(dest,fh,0);

   return(0);
}

/***************************************************************************/
/*>DoRobust(int dest, FH *fh)
   -----------------------------------------
   Performs and plots Robust fitting
   27.04.92 Original
   28.04.92 Now clips correctly
   06.05.92 Added clearing of linestyle
   01.07.92 Added writing coefficients to message window
   07.08.92 When displaying coefficients, only puts a + sign if actually
            required.
*/
int DoRobust(int               dest,
         FH *fh)
{
   DATASET *d;
   
   int    dsetnum,
          j,
          retval;
   double a,
          b,
          abdev,
          *x = NULL,
          *y = NULL,
          x1,   x2,
          y1,   y2;
   char   buffer[40];
          
   /* If we're using log axes allocate space for log values */
   if(graph.logx)
   {
      if((x = (double *)malloc(graph.maxdata * sizeof(double))) == NULL)
      {
         ReqMessage(MainWind,"No memory for fitting",0);
         return(0);
      }
   }
   if(graph.logy)
   {
      if((y = (double *)malloc(graph.maxdata * sizeof(double))) == NULL)
      {
         if(x) free(x);
         ReqMessage(MainWind,"No memory for fitting",0);
         return(0);
      }
   }

   for(d=data, dsetnum = 1; d; NEXT(d), dsetnum++)
   {
      /* If we've got log axes, work out the logs; 
         otherwise, just set the array pointer.
      */
      if(graph.logx)
         for(j=0; j<d->ndata; j++) x[j] = log10(d->x[j]);
      else
         x = d->x;

      if(graph.logy)
         for(j=0; j<d->ndata; j++) y[j] = log10(d->y[j]);
      else
         y = d->y;

      /* Do the fitting */
      retval = medfit(x, y, d->ndata, &a, &b, &abdev);
      if(retval == -1)
      {
         /* No memory */
         char buffer[24];
         
         sprintf(buffer,"Dataset %d",dsetnum);
         ReqMessage2(MainWind,buffer,"No memory for fitting", 0); /* 29.05.97 Hadd extra "buffer" */
         continue;
      }
      else if(retval == 1)
      {
         /* Iter count exceeded */
         char buffer[40];
         
         sprintf(buffer,"bracketing dataset %d",dsetnum);
         ReqMessage2(MainWind,"Iteration count exceeded while", buffer, 0);
         continue;
      }
         
      /* Display fitting data */
      if(dest == DEST_SCREEN)
      {
         WriteMessage("Robust fit:");
         sprintf(buffer,"y = %f %c%fx",a,(b>0.0 ? '+' : ' '),b);
         WriteMessage(buffer);
      }
      
      /* Plot the smoothed data */
      AMSetPen(dest,fh,d->pen);
      AMSetLineStyle(dest,fh,d->linestyle);

      /* Log versions */
      x1 = (graph.logx) ? log10(graph.xmin) : graph.xmin;
      x2 = (graph.logx) ? log10(graph.xmax) : graph.xmax;
      /* non-log versions */
      y1 = (graph.logy) ? alog(a + b*x1)  : a + b*x1;
      y2 = (graph.logy) ? alog(a + b*x2)  : a + b*x2;

      if(graph.clip)       /* Clip to fit within bounds */
      {
         if(y1 < graph.ymin)
         {
            y1 = (graph.logy) ? log10(graph.ymin) : graph.ymin;
            x1 = (y1 - a)/b;
            
            if(graph.logy) y1 = alog(y1);
         }
         
         if(y1 > graph.ymax)
         {
            y1 = (graph.logy) ? log10(graph.ymax) : graph.ymax;
            x1 = (y1 - a)/b;
            
            if(graph.logy) y1 = alog(y1);
         }
         
         if(y2 < graph.ymin)
         {
            y2 = (graph.logy) ? log10(graph.ymin) : graph.ymin;
            x2 = (y2 - a)/b;
            
            if(graph.logy) y2 = alog(y2);
         }
   
         if(y2 > graph.ymax)
         {
            y2 = (graph.logy) ? log10(graph.ymax) : graph.ymax;
            x2 = (y2 - a)/b;
            
            if(graph.logy) y2 = alog(y2);
         }
      }

      if(graph.logx)
      {
         x1 = alog(x1);
         x2 = alog(x2);
      }
      
      /* Actually draw the line */
      AMMove(dest,fh,x1,y1);
      AMDraw(dest,fh,x2,y2);
      AMEndLine(dest,fh);
   }

   AMSetLineStyle(dest,fh,0);
   
   if(graph.logx && x) free(x);
   if(graph.logy && y) free(y);
   
   return(0);
}

/***************************************************************************/
/*>DoSVDFit(int dest, FH *fh, int np, double tol, char *write,
            void (*Func)(double xf, double *pf, int nf))
   -----------------------------------------------------
   Performs and plots SVD linear regression fitting. If write is non-NULL,
   a file will be written with the parameters, variance and covariance
   info.
   14.04.92 Original
   27.04.92 Added no memory condition for svdfit()
   30.04.92 Added function parameter
   01.05.92 Added np, tol and write as parameters
   05.05.92 Corrected check on *write to check on write
   06.05.92 Added clearing of linestyle
   01.07.92 Added writing coefficients to message window. Required memory
            allocation for text buffer.
*/
int DoSVDFit(int               dest,
         FH *fh,
         int               np,      /* Number of polynomials for SVDFIT    */
         double            tol,
         char              *write,  /* Pointer to name of output data      */
         void (*Func)(double xf, double *pf, int nf))
{
   DATASET  *d;
   double   *sigma   = NULL,     /* Standard deviations for SVDFIT         */
            *coef    = NULL,     /* Coefficients returned by SVDFIT        */
            **u      = NULL,     /* U-matrix returned by SVDFIT            */
            **v      = NULL,     /* V-matrix returned by SVDFIT            */
            *w       = NULL,     /* W-vector returned by SVDFIT            */
            *powers  = NULL,     /* Powers from expansion of SVDFIT        */
            *x       = NULL,     /* Data arrays for log fitting            */
            *y       = NULL,
            chisq,               /* Accuracy of fitting by SVDFIT          */
            xstart,              /* Bounds of fitting                      */
            xstop,
            xstep,               /* Step size for drawing curve            */
            xval,                /* Stepped x value                        */
            yval,                /* Fitted y value                         */
            xtiny;               /* Sub step along x to reach boundary     */
   int      error    = TRUE,     /* Flag for error on memory allocation    */
            j,                   /* Counter                                */
            dsetnum,             /* Counter for dataset number             */
            InBounds = TRUE,     /* Flag for curve being in graph bounds   */
            retval;              /* Return value from svdfit()             */
   FH                  *Fh = NULL;  /* File for output info                */
   char     *buffer  = NULL;     /* Buffer for coefficient display         */

/*** Allocate space for working matrices ***/
#ifdef AMIGA_MEM
   /* Standard deviations (sigma) */
   if((sigma = (double *)AllocMem((ULONG)(graph.maxdata * sizeof(double)),
                                   MEMF_CLEAR)) == NULL)
      goto nomem;
   for(j=0; j<graph.maxdata; j++) sigma[j] = 1.0;
      
   /* U-matrix which is maxdata x npoly */
   if((u = (double **)AllocMem((ULONG)(graph.maxdata * sizeof(double *)),
                               MEMF_CLEAR)) == NULL)
      goto nomem;
   for(j=0; j<graph.maxdata; j++) u[j] = NULL;
   for(j=0; j<graph.maxdata; j++)
      if((u[j] = (double *)AllocMem((ULONG)(np * sizeof(double)),
                                    MEMF_CLEAR)) == NULL)
         goto nomem;

   /* V-matrix which is npoly x npoly */
   if((v = (double **)AllocMem((ULONG)(np * sizeof(double *)),
                               MEMF_CLEAR)) == NULL)
      goto nomem;
   for(j=0; j<np; j++) v[j] = NULL;
   for(j=0; j<np; j++)
      if((v[j] = (double *)AllocMem((ULONG)(np * sizeof(double)),
                                    MEMF_CLEAR)) == NULL)
         goto nomem;

   /* W-vector is returned by SVDFIT */
   if((w = (double *)AllocMem((ULONG)(np * sizeof(double)),
                              MEMF_CLEAR)) == NULL)
      goto nomem;

   /* Coefficient vector returned by SVDFIT */
   if((coef = (double *)AllocMem((ULONG)(np * sizeof(double)),
                                 MEMF_CLEAR)) == NULL)
      goto nomem;
      
   /* Powers vector from expansion */
   if((powers = (double *)AllocMem((ULONG)(np * sizeof(double)),
                                   MEMF_CLEAR)) == NULL)
      goto nomem;

   /* If we're using log axes allocate space for log values */
   if(graph.logx)
   {
      if((x = (double *)AllocMem((ULONG)(graph.maxdata * sizeof(double)),
                                 MEMF_CLEAR)) == NULL)
         goto nomem;
   }
   if(graph.logy)
   {
      if((y = (double *)AllocMem((ULONG)(graph.maxdata * sizeof(double)),
                                 MEMF_CLEAR)) == NULL)
         goto nomem;
   }
   
   /* We don't check this; we just can't display message if there's 
      no memory.
   */
   buffer = (char *)AllocMem((ULONG)(15 * np * sizeof(char)),MEMF_CLEAR);
#else
   /* Standard deviations (sigma) */
   if((sigma = (double *)malloc(graph.maxdata * sizeof(double))) == NULL)
      goto nomem;
   for(j=0; j<graph.maxdata; j++) sigma[j] = 1.0;
   
   /* U-matrix which is maxdata x npoly */
   if((u = (double **)malloc(graph.maxdata * sizeof(double *))) == NULL)
      goto nomem;
   for(j=0; j<graph.maxdata; j++) u[j] = NULL;
   for(j=0; j<graph.maxdata; j++)
      if((u[j] = (double *)malloc(np * sizeof(double))) == NULL)
         goto nomem;

   /* V-matrix which is npoly x npoly */
   if((v = (double **)malloc(np * sizeof(double *))) == NULL)
      goto nomem;
   for(j=0; j<np; j++) v[j] = NULL;
   for(j=0; j<np; j++)
      if((v[j] = (double *)malloc(np * sizeof(double))) == NULL)
         goto nomem;

   /* W-vector is returned by SVDFIT */
   if((w = (double *)malloc(np * sizeof(double))) == NULL)
      goto nomem;

   /* Coefficient vector returned by SVDFIT */
   if((coef = (double *)malloc(np * sizeof(double))) == NULL)
      goto nomem;
      
   /* Powers vector from expansion */
   if((powers = (double *)malloc(np * sizeof(double))) == NULL)
      goto nomem;

   /* If we're using log axes allocate space for log values */
   if(graph.logx)
   {
      if((x = (double *)malloc(graph.maxdata * sizeof(double))) == NULL)
         goto nomem;
   }
   if(graph.logy)
   {
      if((y = (double *)malloc(graph.maxdata * sizeof(double))) == NULL)
         goto nomem;
   }

   /* We don't check this; we just can't display message if there's 
      no memory.
   */
   buffer = (char *)malloc(15 * np * sizeof(char));
#endif

   /* Everything OK, so clear the error flag */
   error = FALSE;


   /* Open the output file if appropriate */
   if(write)
   {
#ifdef AMIGA
      if((Fh = (FH *)Open(write,MODE_NEWFILE))==NULL)
#else
      if((Fh = fopen(write,"w"))==NULL)
#endif
         ReqMessage2(MainWind,"Unable to open information file:",write,0);
   }



/*** Now the actual fitting and plotting ***/
   for(d=data, dsetnum=1; d; NEXT(d),dsetnum++)
   {
      /* If we've got log axes, work out the logs; 
         otherwise, just set the array pointer.
      */
      if(graph.logx)
         for(j=0;j<d->ndata;j++) x[j] = log10(d->x[j]);
      else
         x = d->x;

      if(graph.logy)
         for(j=0;j<d->ndata;j++) y[j] = log10(d->y[j]);
      else
         y = d->y;

      /* Do the actual fit. Returns 1 if there was an error, -1 if no memory */
      retval = svdfit(x,y,sigma,d->ndata,tol,
                      coef,np,u,v,w,graph.maxdata,np,&chisq,Func);
      if(retval == 1)
      {
         char buffer[24];
         
         sprintf(buffer,"Dataset %d",dsetnum);
         ReqMessage3(MainWind,buffer,"must have at least as many datapoints",
                                     "as the polynomial degree.",0);
            
         continue;
      }
      else if(retval == -1)
      {
         ReqMessage(MainWind,"No memory for fitting!",0);
         continue;
      }
      
      /* Write coefficients to message window */
      if(dest == DEST_SCREEN && buffer!=NULL)
      {
         char tempbuff[60];
         buffer[0] = '\0';
         
         sprintf(tempbuff,"Linear regression coefficients, Dataset %d:",dsetnum);
         WriteMessage(tempbuff);
         
         for(j=0; j<np; j++)
         {
            sprintf(tempbuff,"%7.4g ",coef[j]);
            strcat(buffer,tempbuff);
         }
         WriteMessage(buffer);
      }
      
      /* If we've opened an information file, write data to it */
      if(Fh) WriteFitInfo(Fh, dsetnum, coef, v, np, w);

      /* Prepare to plot */
      AMSetPen(dest,Fh,d->pen);
      AMSetLineStyle(dest,Fh,d->linestyle);
         
      /* Calculate an appropriate step size */
      if(graph.logx) xstep = (log10(graph.xmax) - log10(graph.xmin)) / 100.0;
      else           xstep = (graph.xmax - graph.xmin) / 100.0;

      /* Expand and plot the first datapoint */
      xval = (graph.logx) ? log10(graph.xmin) : graph.xmin;
      Func(xval, powers, np);
      yval = expand(powers, coef, np);
      if(graph.logy) yval = alog(yval);
      if(yval >= graph.ymin && yval <= graph.ymax)
         AMMove(dest, Fh, graph.xmin, yval);
      else
         InBounds = FALSE;

      /* Expand and plot remaining datapoints */
      xstart = (graph.logx) ? log10(graph.xmin) : graph.xmin;
      xstop  = (graph.logx) ? log10(graph.xmax) : graph.xmax;

      for(xval=xstart+xstep; xval<=xstop; xval+=xstep)
      {
#ifdef AMIGA
         /* See if user has asked to quit with RMB */
         if(CheckIDCMP(MainWind)) goto nomem;    
#endif

         Func(xval, powers, np);
         yval = expand(powers,coef,np);
         if(graph.logy) yval = alog(yval);
            
         if(yval < graph.ymin || yval > graph.ymax)
         {
            /* If we are out of bounds */
            if(InBounds)
            {
               /* Crossing boundary from OK to not. */
               InBounds = FALSE;  /* Set the flag */

               /* Draw to the boundary */
               for(xtiny=xval-xstep;xtiny<=xval;xtiny+=xstep/50.0)
               {
#ifdef AMIGA
                  /* See if user has asked to quit with RMB */
                  if(CheckIDCMP(MainWind)) goto nomem;    
#endif

                  Func(xtiny, powers, np);
                  yval = expand(powers,coef,np);
                  if(graph.logy) yval = alog(yval);

                  if(yval>=graph.ymin && yval<=graph.ymax)
                  {
                     if(graph.logx) AMDraw(dest, Fh, alog(xtiny), yval);
                     else           AMDraw(dest, Fh, xtiny,       yval);
                  }
                  else
                  {
                     break;
                  }
               }
            }
            /* If we're not crossing the boundary, don't do anything */
         }
         else
         {
            /* We're in bounds */
            if(!InBounds)
            {
               /* Crossing boundary from not OK to OK */

               InBounds = TRUE;   /* Set the flag */
               for(xtiny=xval-xstep;xtiny<=xval;xtiny+=xstep/50.0)
               {
#ifdef AMIGA
                  /* See if user has asked to quit with RMB */
                  if(CheckIDCMP(MainWind)) goto nomem;    
#endif

                  Func(xtiny,powers,np);
                  yval = expand(powers,coef,np);
                  if(graph.logy) yval = alog(yval);

                  if(yval>=graph.ymin && yval<=graph.ymax)
                  {
                     /* We've just got in bounds, so move to this point */
                     if(graph.logx) AMMove(dest, Fh, alog(xtiny), yval);
                     else           AMMove(dest, Fh, xtiny,       yval);
                     break;
                  }
               }
            }
            else
            {
               /* We were already in bounds, so just draw */
               if(graph.logx) AMDraw(dest, Fh, alog(xval), yval);
               else           AMDraw(dest, Fh, xval,       yval);
            }
         }
      }
      AMEndLine(dest,fh);
   }

   AMSetLineStyle(dest,fh,0);

   /* Central cleanup with error message if appropriate */
nomem:
#ifdef AMIGA_MEM

   for(j=0; j<graph.maxdata; j++)
      if(u[j])         FreeMem(u[j],  (ULONG)(np            * sizeof(double)));
   for(j=0; j<np;        j++)
      if(v[j])         FreeMem(v[j],  (ULONG)(np            * sizeof(double)));
   if(sigma)           FreeMem(sigma, (ULONG)(graph.maxdata * sizeof(double)));
   if(u)               FreeMem(u,     (ULONG)(graph.maxdata * sizeof(double *)));
   if(v)               FreeMem(v,     (ULONG)(np            * sizeof(double *)));
   if(w)               FreeMem(w,     (ULONG)(np            * sizeof(double)));
   if(coef)            FreeMem(coef,  (ULONG)(np            * sizeof(double)));
   if(powers)          FreeMem(powers,(ULONG)(np            * sizeof(double)));
   if(graph.logx && x) FreeMem(x,     (ULONG)(graph.maxdata * sizeof(double)));
   if(graph.logy && y) FreeMem(y,     (ULONG)(graph.maxdata * sizeof(double)));
   if(buffer)          FreeMem(buffer,(ULONG)(15 * np       * sizeof(char)));
#else
   for(j=0; j<graph.maxdata; j++)   if(u[j]) free(u[j]);
   for(j=0; j<np;            j++)   if(v[j]) free(v[j]);
   if(sigma)                                 free(sigma);
   if(coef)                                  free(coef);
   if(w)                                     free(w);
   if(powers)                                free(powers);
   if(u)                                     free(u);
   if(v)                                     free(v);
   if(graph.logx && x)                       free(x);
   if(graph.logy && y)                       free(y);
   if(buffer)                                free(buffer);
#endif

#ifdef AMIGA
   if(Fh) Close((BPTR)Fh);
#else
   if(Fh) fclose(Fh);
#endif

   if(error)   ReqMessage(MainWind,"No memory for fitting",0);

   return(0);
}

/***************************************************************************/
/*>WriteFitInfo(FH   *Fh, int dsetnum,
                double *coef, double *v[], int ma, double *w)
   ----------------------------------------------------------
   Call routines to calculate variance and covariance matrices and output
   some information.

   01.05.92 Original
*/
int WriteFitInfo(FH   *Fh,
             int                 dsetnum,
             double              *coef,
             double              *v[],
             int                 ma,
             double              *w)
{
   int      error = TRUE,
            i, j;
   double   **cvm,
            *var;
   char     buffer[200];
   
#ifdef AMIGA_MEM
   /* CVM-matrix which is npoly x npoly */
   if((cvm = (double **)AllocMem((ULONG)(ma * sizeof(double *)),
                                  MEMF_CLEAR)) == NULL)
      goto nomem;
   for(j=0; j<ma; j++) cvm[j] = NULL;
   for(j=0; j<ma; j++)
      if((cvm[j] = (double *)AllocMem((ULONG)(ma * sizeof(double)),
                                    MEMF_CLEAR)) == NULL)
         goto nomem;
         
   if((var = (double *)AllocMem((ULONG)(ma * sizeof(double)),
                                MEMF_CLEAR)) == NULL)
      goto nomem;
#else
   /* CVM-matrix which is npoly x npoly */
   if((cvm = (double **)malloc(ma * sizeof(double *))) == NULL)
      goto nomem;
   for(j=0; j<ma; j++) cvm[j] = NULL;
   for(j=0; j<ma; j++)
      if((cvm[j] = (double *)malloc(ma * sizeof(double))) == NULL)
         goto nomem;
   if((var = (double *)malloc(ma * sizeof(double))) == NULL)
      goto nomem;
#endif

   error = FALSE;
   
   
   /* Calculate variance */
   if(svdvar(v, ma, w, var))
   {
      error = TRUE;
      goto nomem;
   }
   
   /* Calculate covariance */
   if(svdcov(v, ma, w, cvm))
   {
      error = TRUE;
      goto nomem;
   }
   
   
   /* Specify the dataset number */
   sprintf(buffer,"Dataset: %d", dsetnum);
   myputs(Fh, buffer);
   
   
   /* Print the coeficients */
   myputs(Fh,"\nCoefficients:\n");
   buffer[0] = '\0';
   for(j=0; j<ma; j++)
   {
      char buff2[32];
      
      sprintf(buff2,"%10.4g ",coef[j]);   /* 29.05.97 lg -> g */
      strcat(buffer,buff2);
   }
   myputs(Fh, buffer);
   
   /* Print the variance array */
   myputs(Fh,"\nVariance:\n");
   buffer[0] = '\0';
   for(j=0; j<ma; j++)
   {
      char buff2[32];
      
      sprintf(buff2,"%10.4g ",var[j]);   /* 29.05.97 lg -> g */
      strcat(buffer,buff2);
   }
   myputs(Fh, buffer);
   
   /* Print the covariance matrix */
   myputs(Fh,"\nCovariance:\n");
   for(i=0; i<ma; i++)
   {
      buffer[0] = '\0';
      for(j=0; j<ma; j++)
      {
         char buff2[32];
         
         sprintf(buff2,"%10.4g ",cvm[i][j]);   /* 29.05.97 lg -> g */
         strcat(buffer,buff2);
      }
      myputs(Fh, buffer);
      myputs(Fh, "\n");
   }
   

nomem:

#ifdef AMIGA_MEM
   for(j=0; j<ma; j++) if(cvm[j])   FreeMem(cvm[j],(ULONG)(ma * sizeof(double)));
   if(cvm)                          FreeMem(cvm,   (ULONG)(ma * sizeof(double *)));
#else
   for(j=0; j<ma; j++) if(cvm[j])   free(cvm[j]);
   if(cvm)                          free(cvm);
#endif

   if(error)   ReqMessage(MainWind,"No memory for writing info.",0);

   return(0);
}
