/****************************************************************************

   Program:    AMPlot2
   File:       axes.c
   
   Version:    V1.0
   Date:       31.03.92
   Function:   Handle menu and gadget functions.
   
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
#include "includes.h"

/***************************************************************************/
/*>CalcBounds(double *xmin, double *xmax, double *ymin, double *ymax)
   ------------------------------------------------------------------
   Calculates clean bounds from the max and min values in the plots
   31.03.92 Framework
   06.04.92 Original
   10.04.92 Added log support.
   09.06.92 Changed to support stacked bars
*/
int CalcBounds(double *xmin,
           double *xmax,
           double *ymin,
           double *ymax)
{
   DATASET *p;
   double  XSize,
           YSize,
           *YTotal;
   int     i;
   
   *xmin = 0.0;
   *xmax = 0.0;
   *ymin = 0.0;
   *ymax = 0.0;
   
   if(data != NULL)
   {
      *xmin = data->xmin;
      *xmax = data->xmax;
      *ymin = data->ymin;
      *ymax = data->ymax;
   
      for(p=data->next;p;NEXT(p))
      {
         if(p->xmin < *xmin) *xmin = p->xmin;
         if(p->xmax > *xmax) *xmax = p->xmax;
         if(p->ymin < *ymin) *ymin = p->ymin;
         if(p->ymax > *ymax) *ymax = p->ymax;
      }

      if(graph.style == STYLE_PIE)
      {
         *xmin = 0.0;   *xmax = 100.0;
         *ymin = 0.0;   *ymax = 100.0;
      }
      else
      {
         if(graph.logx)
         {
            *xmin = alog(floor(log10(*xmin)));
            *xmax = alog(ceil(log10(*xmax)));
         }
         else
         {
            XSize = *xmax - *xmin;
            RoundDown(xmin,XSize);
            RoundUp(xmax,XSize);
         }
         if(graph.logy)
         {
            *ymin = alog(floor(log10(*ymin)));
            *ymax = alog(ceil(log10(*ymax)));
         }
         else
         {
            YSize = *ymax - *ymin;
            RoundDown(ymin,YSize);
            RoundUp(ymax,YSize);
         }


         /* Modify the Y min and max if it's a stacked barchart */
         if(graph.style==STYLE_BAR  && barstyle.style == STYLE_STACKED)
         {
            if((YTotal = (double *)malloc(graph.maxdata * sizeof(double)))==NULL)
            {
               ReqMessage(MainWind,"No memory for stacked bar scaling",0);
            }
            else
            {
               /* Find total Y values at each position */
               for(i=0; i<graph.maxdata; i++) YTotal[i] = 0.0;
               for(p=data; p; NEXT(p))
                  for(i=0; i<p->ndata; i++)
                     YTotal[i] += p->y[i];
                     
               /* Find min and max Y */
               *ymin = *ymax = YTotal[0];
               for(i=1; i<graph.maxdata; i++)
               {
                  if(YTotal[i] < *ymin) *ymin = YTotal[i];
                  if(YTotal[i] > *ymax) *ymax = YTotal[i];
               }
               
               /* Round off these values */
               if(graph.logy)
               {
                  *ymin = alog(floor(log10(*ymin)));
                  *ymax = alog(ceil(log10(*ymax)));
               }
               else
               {
                  RoundDown(ymin, *ymax - *ymin);
                  RoundUp(ymax,   *ymax - *ymin);
                  
                  if(*ymin > 0.0) *ymin = 0.0;
                  if(*ymax < 0.0) *ymax = 0.0;
               }
               
               free(YTotal);
            }
         }
      }
   }

   return(0);
}

/***************************************************************************/
/*>RoundUp(double *value, double Size)
   -----------------------------------
   Takes a value and a size (to gain order of magnitude of variable range)
   and rounds the value down to a clean value.
   06.04.92 Original
*/
int RoundUp(double *value,
        double Size)
{
   double   val,
            LastValue,
            Work,
            TenPower,
            Factor;
   int      Negative,
            LogValue,
            Mult;
   
   if(*value==0.0) return(0);
   
   val = *value;
   if(val < 0.0)
   {
      val  = -val;
      Negative = -1;
   }
   else
   {
      Negative = 1;
   }
   LogValue = log10(val);
   TenPower = (int)LogValue;
   if (LogValue < 0) TenPower--;
   Factor   = pow((double)10,(double)TenPower);
   Mult     = (int)(val/Factor);
   if((Negative == 1) && ((val/Factor) - Mult)) Mult++;
   val     = Mult * Factor;
   val     = val * Negative;
   LastValue = *value;

   while (((val - *value) > Size/3.0) && val != LastValue)
   {
      LastValue = val;
      Work = val - *value;
#ifdef DEBUG
      printf("val %g, Work %g\n",val,Work);
#endif
      if(Work < 0.0)
      {
         Work  = -Work;
         Negative = -1;
      }
      else
      {
         Negative = 1;
      }
      LogValue = log10(Work);
      TenPower = (int)LogValue;
      if (LogValue < 0) TenPower--;
      Factor   = pow((double)10,(double)TenPower);
      Mult     = (int)(Work/Factor);
      Work     = Mult * Factor;
      Work     = Work * Negative;

      val = val - Work;

      if(val < *value)
      {
         val = LastValue;
         break;
      }
   }
   *value = val;
   return(0);
}

/***************************************************************************/
/*>RoundDown(double *value,double Size)
   -----------------------------------
   Takes a value and a size (to gain order of magnitude of variable range)
   and rounds the value up to a clean value.
   06.04.92 Original
*/
int RoundDown(double *value,
          double Size)
{
   double   val,
            LastValue,
            Work,
            TenPower,
            Factor;
   int      Negative,
            LogValue,
            Mult;
   
   if(*value==0.0) return(0);
   
   val = *value;
   
   if(val < 0.0)
   {
      val  = -val;
      Negative = -1;
   }
   else
   {
      Negative = 1;
   }
   LogValue = log10(val);
   TenPower = (int)LogValue;
   if (LogValue < 0.0) TenPower--;
   Factor   = pow((double)10,(double)TenPower);
   Mult     = (int)(val/Factor);
   if((Negative == -1) && ((val/Factor) - Mult)) Mult++;
   val    = Mult * Factor;
   if((val< 10.0) && (val>= 1.0) && (Negative==1)) val = 0.0;
   val    = val * Negative;
   LastValue = *value;

   while (((*value - val) > Size/3.0) && val != LastValue)
   {
      LastValue = val;
      Work = *value - val;
#ifdef DEBUG
      printf("val %g, Work %g\n",val,Work);
#endif
      if(Work < 0.0)
      {
         Work  = -Work;
         Negative = -1;
      }
      else
      {
         Negative = 1;
      }
      LogValue = log10(Work);
      TenPower = (int)LogValue;
      if (LogValue < 0.0) TenPower--;
      Factor   = pow((double)10,(double)TenPower);
      Mult     = (int)(Work/Factor);
      Work    = Mult * Factor;
      Work    = Work * Negative;
      val = val + Work;
      
      if(val > *value)
      {
         val = LastValue;
         break;
      }
   }
   *value = val;
   return(0);       
}

/***************************************************************************/
/*>CalcTicks(int usebuff, double *xtick, 
             double *ytick, double *xsub, double *ysub)
   ----------------------------------------------------
   Calculates ticks and sub-ticks. If usebuff is set, these values are
   calculated based on bounds in the requester buffers; else the values in
   the graph variables are used.
   31.03.92 Framework
   06.04.92 Original
   10.04.92 Added log support
*/
int CalcTicks(int    usebuff,
          double *xtick,
          double *ytick,
          double *xsub,
          double *ysub)
{
   double   xmin,  xmax,
            ymin,  ymax,
            XSize, YSize;
   int      TenPowerX,
            TenPowerY,
            NTick;
            
   *xtick = 0.0;
   *ytick = 0.0;
   *xsub  = 0.0;
   *ysub  = 0.0;

   if(usebuff)
   {
      sscanf(BoundsXMinBuff, "%lf",&xmin);
      sscanf(BoundsXMaxBuff, "%lf",&xmax);
      sscanf(BoundsYMinBuff, "%lf",&ymin);
      sscanf(BoundsYMaxBuff, "%lf",&ymax);
   }
   else
   {
      xmin = graph.xmin;
      xmax = graph.xmax;
      ymin = graph.ymin;
      ymax = graph.ymax;
   }

   /*** Do the x-axis, giving a number of ticks between 4 and 10           */
   if(graph.logx)
   {
      *xtick = 1.0;
      *xsub  = 0.1;
   }
   else
   {
      XSize          = xmax - xmin;
      if(XSize != 0.0)
      {
         TenPowerX   = (int)log10(XSize);
         *xtick      = pow((double)10.0, (double)TenPowerX);
   
         NTick       = (int)(XSize / *xtick);
         if(NTick < 4)
         {
            *xtick   = pow((double)10.0, (double)(TenPowerX-1));
            NTick    = (int)(XSize / *xtick);
         }
         if(NTick > 10)
         {
            *xtick   = pow((double)10.0, (double)TenPowerX);
            NTick    = (int)(XSize / *xtick);
         }
         while(NTick < 4)
         {
            *xtick   = *xtick / 2.0;
            NTick    = (int)(XSize / *xtick);
         }
   
         /*** Now the x-subticks                                              */
         if(NTick > 5) *xsub = *xtick /  2.0;
         else          *xsub = *xtick / 10.0;
      }
   }
         
   /*** Do the y-axis, giving a number of ticks between 4 and 10           */
   if(graph.logy)
   {
      *ytick = 1.0;
      *ysub  = 0.1;
   }
   else
   {
      YSize          = ymax - ymin;
      if(YSize != 0.0)
      {
         TenPowerY   = (int)log10(YSize);
         *ytick      = pow((double)10.0, (double)TenPowerY);
      
         NTick       = (int)(YSize / *ytick);
         if(NTick < 4)
         {
            *ytick   = pow((double)10.0, (double)(TenPowerY-1));
            NTick    = (int)(YSize / *ytick);
         }
         if(NTick > 10)
         {
            *ytick   = pow((double)10.0, (double)TenPowerY);
            NTick    = (int)(YSize / *ytick);
         }
         while(NTick < 4)
         {
            *ytick   = *ytick / 2.0;
            NTick    = (int)(YSize / *ytick);
         }
   
         /*** Now the y-subticks                                                 */
         if(NTick > 5) *ysub = *ytick /  2.0;
         else          *ysub = *ytick / 10.0;
      }
   }
         
   return(0);
}

/**************************************************************************/
/*>DrawAxes(int dest, FH *fh, int parts)
   ----------------------------------------------------
   27.03.92 Framework
   06.04.92 Original
   07.04.92 All ticks in pen 6; added clipping; added grid; added titles
   08.04.92 Added follow zero option
   10.04.92 Added log support
   14.04.92 Changed max checks to tick/10 rather than tick/2
   27.04.92 Added .1 to max on log tick steps and tick/100 step otherwise
   29.04.92 Added check on subtick size > 0.0
   09.06.92 Added parameter to define which bits get drawn
   11.06.92 Added parameter to DrawXTick() to count tick number (for bar
            labels).
   22.07.92 Added special parameter to DrawXSub() and DrawYSub(). This
            causes labels to be drawn by subticks in log axes, if the
            span is only one log value.
   24.07.92 graph.subtick must also be set for special to be true.
   15.09.92 Added special code for PS output, to improve last tick on
            unboxed axes.
*/
int DrawAxes(int               dest,
         FH *fh,
         int               parts)
{
   double   tick,
            subtick,
            axis_x,
            axis_y;
   int      ticknum = 0;
            
   
   /* Set plain line style */
   AMSetLineStyle(dest, fh, 0);

   if(graph.fzero)
   {
      axis_x = (graph.logx) ? graph.xmin : 0.0;
      axis_y = (graph.logy) ? graph.ymin : 0.0;
   }
   else
   {
      axis_x = graph.xmin;
      axis_y = graph.ymin;
   }
   
   if(parts == 0 || parts == 1)
   {
      /* Do the grid */
      if(graph.grid)
      {
         /* Use pen 5 for the grid */
         AMSetPen(dest, fh, 5);
   
         /* X-axis grid */
         if(graph.logx)
         {
            for(tick=log10(graph.xmin); tick<log10(graph.xmax); tick+=1.0)
            {
               AMMove(dest, fh, alog(tick), graph.ymin);
               AMDraw(dest, fh, alog(tick), graph.ymax);
               if(graph.subtick)
               {
                  for(subtick=2 * alog(tick);
                      subtick < graph.xmax && subtick < alog(tick+1.0)-(alog(tick)/10.0);
                      subtick += alog(tick))
                  {
                     AMMove(dest, fh, subtick, graph.ymin);
                     AMDraw(dest, fh, subtick, graph.ymax);
                  }
               }
            }
         }
         else        /* Not logs */
         {
            for(tick=graph.xmin+graph.xtick;
                tick<graph.xmax-(graph.xtick/10.0);
                tick+=graph.xtick)
            {
               if(tick != axis_x)   /* Don't overdraw axes */
               {
                  AMMove(dest, fh, tick, graph.ymin);
                  AMDraw(dest, fh, tick, graph.ymax);
               }
            }
         }
         AMEndLine(dest,fh);

         /* Y-axis grid */
         if(graph.logy)
         {
            for(tick=log10(graph.ymin); tick<log10(graph.ymax); tick+=1.0)
            {
               AMMove(dest, fh, graph.xmin, alog(tick));
               AMDraw(dest, fh, graph.xmax, alog(tick));
               if(graph.subtick)
               {
                  for(subtick=2 * alog(tick);
                      subtick < graph.ymax && subtick < alog(tick+1.0)-(alog(tick)/10.0);
                      subtick += alog(tick))
                  {
                     AMMove(dest, fh, graph.xmin, subtick);
                     AMDraw(dest, fh, graph.xmax, subtick);
                  }
               }
            }
         }
         else
         {
            for(tick=graph.ymin+graph.ytick;
                tick<graph.ymax-(graph.ytick/10.0);
                tick+=graph.ytick)
            {
               if(tick != axis_y)   /* Don't overdraw axes */
               {
                  AMMove(dest, fh, graph.xmin, tick);
                  AMDraw(dest, fh, graph.xmax, tick);
               }
            }
         }
         AMEndLine(dest,fh);
      }
   }

   if(parts==0 || parts==2)
   {
      /* Use Pen 6 for the axes and labels */
      AMSetPen(dest, fh, 6);
   
      /* Draw the actual axes */
      if(dest == DEST_PS)  myputs(fh,"2 setlinecap\n");  /* 15.09.92 */
      AMMove(dest, fh, axis_x, graph.ymax);    /* y-axis   */
      AMDraw(dest, fh, axis_x, graph.ymin);
      AMMove(dest, fh, graph.xmin, axis_y);    /* x-axis   */
      AMDraw(dest, fh, graph.xmax, axis_y);
      if(graph.boxed)
      {
         if(graph.fzero)
         {
            if(axis_y == graph.ymin && axis_x == graph.xmin)
            {
               /* Same as not fzero */
               AMDraw(dest, fh, graph.xmax, graph.ymax);
               AMDraw(dest, fh, graph.xmin, graph.ymax);
            }
            else if(axis_y == graph.ymin)
            {
               /* Only need to draw sides and top */
               AMMove(dest, fh, graph.xmax, graph.ymin);
               AMDraw(dest, fh, graph.xmax, graph.ymax);
               AMDraw(dest, fh, graph.xmin, graph.ymax);
               AMDraw(dest, fh, graph.xmin, graph.ymin);
            }
            else if(axis_x == graph.xmin)
            {
               /* Only need to draw top, bottom and right */
               AMMove(dest, fh, graph.xmin, graph.ymin);
               AMDraw(dest, fh, graph.xmax, graph.ymin);
               AMDraw(dest, fh, graph.xmax, graph.ymax);
               AMDraw(dest, fh, graph.xmin, graph.ymax);
            }
            else
            {
               /* Need to draw whole box */
               AMMove(dest, fh, graph.xmin, graph.ymin);
               AMDraw(dest, fh, graph.xmax, graph.ymin);
               AMDraw(dest, fh, graph.xmax, graph.ymax);
               AMDraw(dest, fh, graph.xmin, graph.ymax);
               AMDraw(dest, fh, graph.xmin, graph.ymin);
            }
         }
         else
         {
            /* Draw 2 remaining sides */
            AMDraw(dest, fh, graph.xmax, graph.ymax);
            AMDraw(dest, fh, graph.xmin, graph.ymax);
         }
      }
      AMEndLine(dest,fh);
      if(dest == DEST_PS)  myputs(fh,"0 setlinecap\n");  /* 15.09.92 */
   
      /* Do ticks on the X-axis */
      AMSetFont(dest, fh, &XLabel);
      if(graph.logx)
      {
         int special;
         
         special = (graph.subtick && (log10(graph.xmax) - log10(graph.xmin) < 2.0) ? 
                   TRUE : FALSE);
         
         for(tick=log10(graph.xmin), ticknum = 0;
             tick<=log10(graph.xmax)+0.1;
             tick+=1.0, ticknum++)
         {
            DrawXTick(dest, fh, alog(tick), axis_y, ticknum);
            for(subtick=2 * alog(tick);
                subtick < graph.xmax && subtick < alog(tick+1.0)-(alog(tick)/10.0);
                subtick += alog(tick))
                DrawXSub(dest, fh, subtick, axis_y, special);
         }
      }
      else
      {
         for(tick=graph.xmin, ticknum = 0;
             tick<graph.xmax+(graph.xtick/100.0);
             tick+=graph.xtick, ticknum++)
         {
            DrawXTick(dest, fh, tick, axis_y, ticknum);
            if(graph.subtick && graph.xsub > 0.0)
            {
               for(subtick=tick+graph.xsub;
                   subtick < graph.xmax && subtick<tick+graph.xtick;
                   subtick+=graph.xsub)
                   DrawXSub(dest, fh, subtick, axis_y, FALSE);
            }
         }
      }
   
      /* Do ticks on the Y-axis */
      AMSetFont(dest, fh, &YLabel);
      if(graph.logy)
      {
         int special;
         
         special = (graph.subtick && (log10(graph.ymax) - log10(graph.ymin) < 2.0) ?
                   TRUE : FALSE);

         for(tick=log10(graph.ymin); tick<=log10(graph.ymax)+0.1; tick+=1.0)
         {
            DrawYTick(dest, fh, axis_x, alog(tick));
            for(subtick=2 * alog(tick);
                subtick < graph.ymax && subtick < alog(tick+1.0)-(alog(tick)/10.0);
                subtick += alog(tick))
                DrawYSub(dest, fh, axis_x, subtick, special);
         }
      }
      else
      {
         for(tick=graph.ymin; tick<=graph.ymax+(graph.ytick/100.0); tick+=graph.ytick)
         {
            DrawYTick(dest, fh, axis_x, tick);
            if(graph.subtick && graph.ysub > 0.0)
            {
               for(subtick=tick+graph.ysub; 
                   subtick < graph.ymax && subtick<tick+graph.ytick; 
                   subtick+=graph.ysub)
                   DrawYSub(dest, fh, axis_x, subtick, FALSE);
            }
         }
      }
      
      /* Do the x-axis title */
      XAxisTitle(dest, fh);
   
      /* Do the y-axis title */
      YAxisTitle(dest, fh);
   }

   return(0);
}

/**************************************************************************/
/*>XAxisTitle(int dest, FH *fh)
   -------------------------------------------
   Writes the title on the x-axis
   07.04.92 Original
   13.04.92 Fixed for log support
   06.05.92 Added check that there is a title
   07.05.92 Changed positioning for non-screen display
*/
int XAxisTitle(int                dest, 
           FH  *fh)
{
   double xpos,
          ypos;
          
   if(XTitle.text[0] != '\0')
   {
      AMSetFont(dest, fh, &XTitle);
   
      if(graph.logx)
         xpos = alog(log10(graph.xmin) + (log10(graph.xmax)-log10(graph.xmin))/2.0);
      else
         xpos = graph.xmin + (graph.xmax-graph.xmin)/2.0;
   
      if(dest == DEST_SCREEN)
         ypos = (graph.logy) ? graph.ymin / alog(XLabel.PSSize/graph.YScale)
                             : graph.ymin - XLabel.PSSize/graph.YScale;
      else
         ypos = (graph.logy) ? graph.ymin / alog(1.5*XLabel.PSSize/graph.YScale)
                             : graph.ymin - 1.5*XLabel.PSSize/graph.YScale;
   
      AMCTText(dest, fh, xpos, ypos, XTitle.text);
   }

   return(0);
}

/**************************************************************************/
/*>YAxisTitle(int dest, FH *fh)
   -------------------------------------------
   Writes the title on the y-axis
   07.04.92 Original
   06.05.92 Added check that there is a title
*/
int YAxisTitle(int                dest, 
           FH  *fh)
{
   if(YTitle.text[0] != '\0')
   {
      AMSetFont(dest, fh, &YTitle);
      AMYTitle(dest, fh, graph.xmin, graph.ymin, YTitle.text);
   }

   return(0);
}

/**************************************************************************/
/*>DrawXTick(int dest, FH *fh,
             double x, double y, int ticknum)
   ------------------------------------------
   Draws and labels an x-tickmark.
   06.04.92 Original
   07.04.92 Fix to text positioning
   07.05.92 Corrected call to AMLine() to -ve y
   08.06.92 Corrected to shift labels left rather than right
   11.06.92 Added bar label support and ticknum parameter
   06.07.92 Fixed problem with log axes which span 1.0
   21.07.92 Changed log axes to use *sign* of precision
   22.07.92 Fixed always to use -1 for -ve precision/log axis x<0
   24.07.92 Added extras parameter to ftostr()
*/
int DrawXTick(int               dest,
          FH *fh,
          double            x,
          double            y,
          int               ticknum)
{
   char buffer[24];
   
   AMLine(dest, fh, x, y, 0.0, -output.ticklen);
   AMEndLine(dest,fh);
   if(!graph.noxlabels)
   {
      if(graph.logx)
      {
         /* This code seems to need to be split up to overcome
            rounding errors.
         */
         double   lgx;
         int      prec;

         lgx  = log10(x);
         lgx  = ABS(lgx);
         prec = (int)(lgx+0.1);
         
         if(graph.xprecision >= 0)
         {
            if(x<1.0)   ftostr(buffer,24,x,prec);
            else        ftostr(buffer,24,x,0);
         }
         else
         {
            ftostr(buffer,24,x,-1);
         }
      }
      else
      {
         ftostr(buffer,24,x,graph.xprecision);
      }
      
      if(graph.centrexlabels)
      {
         if(x-graph.xtick/2.0 > graph.xmin)
            AMCTText(dest, fh, x-graph.xtick/2.0, y, buffer);
      }
      else
      {
         AMCTText(dest, fh, x, y, buffer);
      }
   }

   if(barstyle.uselabels         &&
      barlabels != NULL          &&
      ticknum > 0                &&
      ticknum <= barstyle.nbar)
   {
      if(x-graph.xtick/2.0 > graph.xmin)
         AMCTText(dest, fh, x-graph.xtick/2.0, y, barlabels[ticknum-1].text);
   }

   return(0);
}
          
/**************************************************************************/
/*>DrawYTick(int dest, FH *fh, double x, double y)
   --------------------------------------------------------------
   Draws and labels a y-tickmark.
   06.04.92 Original
   07.04.92 Fix to text positioning
   07.05.92 Corrected call to AMLine() to -ve y
   06.07.92 Fixed problem with log axes which span 1.0
   21.07.92 Changed log axes to use *sign* of precision
   22.07.92 Fixed always to use -1 for -ve precision/log axis y<0
   24.07.92 Added extras parameter to ftostr()
*/
int DrawYTick(int               dest,
          FH *fh,
          double            x,
          double            y)
{
   char buffer[24];
   
   AMLine(dest, fh, x, y, -output.ticklen, 0.0);
   AMEndLine(dest,fh);
   if(!graph.noylabels)
   {
      if(graph.logy)
      {
         /* This code seems to need to be split up to overcome
            rounding errors.
         */
         double   lgy;
         int      prec;

         lgy  = log10(y);
         lgy  = ABS(lgy);
         prec = (int)(lgy+0.1);
         
         if(graph.yprecision >= 0)
         {
            if(y<1.0)   ftostr(buffer,24,y,prec);
            else        ftostr(buffer,24,y,0);
         }
         else
         {
            ftostr(buffer,24,y,-1);
         }
      }
      else
      {
         ftostr(buffer,24,y,graph.yprecision);
      }

      AMRText(dest, fh, x, y, -output.ticklen, buffer);
   }
   return(0);
}

/**************************************************************************/
/*>DrawXSub(int dest, FH *fh, double x, double y,
            int special)
   -------------------------------------------------------------
   Draws and labels an x-subtickmark.
   06.04.92 Original
   07.05.92 Corrected call to AMLine() to -ve y
   22.07.92 Added special parameter which puts text by the subtick. This
            is only used for log axes of 1 span.
   24.07.92 Added extras parameter to ftostr()
*/
int DrawXSub(int               dest,
         FH *fh,
         double            x,
         double            y,
         int               special)
{
   char buffer[24];

   if(special)
   {
      AMLine(dest, fh, x, y, 0.0, -output.ticklen);
      AMEndLine(dest,fh);
      if(!graph.noxlabels)
      {
         /* This code seems to need to be split up to overcome
            rounding errors.
         */
         double   lgx;
         int      prec;

         lgx  = log10(graph.xmin);
         lgx  = ABS(lgx);
         prec = (int)(lgx+0.1);
         
         if(graph.xprecision >= 0)
         {
            if(x<1.0)   ftostr(buffer,24,x,prec);
            else        ftostr(buffer,24,x,0);
         }
         else
         {
            ftostr(buffer,24,x,-1);
         }
         
         AMCTText(dest, fh, x, y, buffer);
      }

   }
   else
   {
      AMLine(dest, fh, x, y, 0.0, -output.subticklen);
      AMEndLine(dest,fh);
   }
   return(0);
}

/**************************************************************************/
/*>DrawYSub(int dest, FH *fh, double x, double y,
            int special)
   -------------------------------------------------------------
   Draws and labels a y-subtickmark.
   06.04.92 Original
   07.05.92 Corrected call to AMLine() to -ve y
   22.07.92 Added special parameter which just calls DrawYTick() instead
   24.07.92 Added extras parameter to ftostr()
*/
int DrawYSub(int               dest,
         FH *fh,
         double            x,
         double            y,
         int               special)
{
   char buffer[24];

   if(special)
   {
      AMLine(dest, fh, x, y, -output.ticklen, 0.0);
      AMEndLine(dest,fh);
      if(!graph.noxlabels)
      {
         /* This code seems to need to be split up to overcome
            rounding errors.
         */
         double   lgy;
         int      prec;

         lgy  = log10(graph.ymin);
         lgy  = ABS(lgy);
         prec = (int)(lgy+0.1);
         
         if(graph.yprecision >= 0)
         {
            if(y<1.0)   ftostr(buffer,24,y,prec);
            else        ftostr(buffer,24,y,0);
         }
         else
         {
            ftostr(buffer,24,y,-1);
         }
         
         AMRText(dest, fh, x, y, -output.ticklen, buffer);
      }
   }
   else
   {
      AMLine(dest, fh, x, y, -output.subticklen, 0.0);
      AMEndLine(dest,fh);
   }
   return(0);
}

