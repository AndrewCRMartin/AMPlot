/****************************************************************************

   Program:    AMPlot2
   File:       Piechart.c

   Version:    V1.0
   Date:       16.06.92
   Function:   Piechart plotting routines
   
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
/*>double trueangle(double opp, double adj)
   ----------------------------------------
   This routine returns the true angle (between 0 and 2PI) given the 
   opposite and adjacent, taking account of positive and negative distances.
   
   17.06.92 Adapted from *old* version
*/
double trueangle(double opp, double adj)
{
   double theta;
   
   if(adj==0.0)
   {
      if(opp > 0.0) theta = PI/2.0;
      else          theta = 3*PI/2.0;
   }
   else
   {
      theta = atan(opp/adj);
      if(opp<0.0 && adj>0.0) theta += (2*PI);
      if(adj<0.0) theta += PI;
   }
   return(theta);
}

/***************************************************************************/
/*>ScreenToPieData(int xscreen, int yscreen, double *ang, double *dist)
   --------------------------------------------------------------------
   Converts screen coordinates to pie data coordinates. Returns the angle
   and distance.
   
   16.06.92 Framework
   19.06.92 Original; changed to assume equal X & Y scaling
*/
int ScreenToPieData(int    xscreen, 
                int    yscreen, 
                double *ang,
                double *dist)
{
   int   XScrCent,
         YScrCent,
         ScrDist;
         
   /* Find screen centre of pie */
   DataToScreen(0.0, 0.0, &XScrCent, &YScrCent);

   /* Calc angle of the intput screen coords. */
   *ang = trueangle((double)(YScrCent-yscreen), (double)(xscreen-XScrCent));
   
   /* Calc dist in screen coords */
   ScrDist = (xscreen-XScrCent)*(xscreen-XScrCent) +
             (yscreen-YScrCent)*(yscreen-YScrCent);
   ScrDist = (int)sqrt((double)ScrDist);
   
   /* Convert to data coordinates */
   *dist = ScrDist / graph.XScale;     /* graph.XScale == graph.YScale     */

   return(0);
}

/***************************************************************************/
/*>PieGraph(int dest, FH *fh)
   -----------------------------------------
   Plots a pie chart to specified destination.
   27.03.92 Framework
   12.06.92 Stopped replot from Rexx from infinite looping
   16.06.92 Original
   17.06.92 Continued...
   22.06.92 Corrected box to use graph.xmin, etc., rather than PIEBOUNDS
   02.07.92 Moved Rexx launch command to DoPlot()
   10.07.92 Added DoGraphTitle()
*/
int PieGraph(int               dest,
         FH *fh)
{
   double   total,
            angle     = 0.0,
            lastangle = 0.0;
   int      j,
            pattern,
            eject;
   FILLITEM *f;
   
   /* If we've got some labels, set the font (use X-label font) */
   if(barlabels) AMSetFont(dest, fh, &XLabel);
   
   /* Find total of data in pie */
   for(j=0, total=0.0; j<data->ndata; j++) total += data->x[j];

   /* For each data point */
   for(j=0; j<data->ndata; j++)
   {
      /* Find the boundary angles for this slice */
      lastangle = angle;
      angle    += (2.0 * PI * data->x[j] / total);

      /* See if we have a fill for this slice */
      pattern   = 0;
      if(filllist)
      {
         for(f=filllist; f; NEXT(f))
         {
            if(f->angle > lastangle && f->angle < angle)
               pattern = f->pattern;
         }
      }
      
      if(data->ep[j] != 0.0) eject = TRUE;
      else                   eject = FALSE;
      
      /* Now draw the wedge */
      if(j == data->ndata-1)
         Wedge(dest, fh, lastangle, 2.0*PI, pattern, eject, j);
      else
         Wedge(dest, fh, lastangle, angle,  pattern, eject, j);
   }

   /* If the graph is boxed draw it */
   if(graph.boxed)
   {
      AMMove(dest, fh,  graph.xmin, graph.ymin);
      AMDraw(dest, fh,  graph.xmax, graph.ymin);
      AMDraw(dest, fh,  graph.xmax, graph.ymax);
      AMDraw(dest, fh,  graph.xmin, graph.ymax);
      AMDraw(dest, fh,  graph.xmin, graph.ymin);
      AMEndLine(dest, fh);
   }

#ifdef SEPPIEREQ
   DoExtraPieLabels(dest, fh);
#else
   DoExtraLabels(dest, fh);
#endif

   DoPieKey(dest, fh);
   DoGraphTitle(dest, fh);

   return(0);
}

/***************************************************************************/
/*>Wedge(int dest, FH *fh, double lastangle,
         double endangle, int pattern, int eject, int wedge)
   ---------------------------------------------------------
   Calls the routine to draw a Piechart wedge in the specified pattern
   16.06.92 Framework
   18.06.92 Added eject
   19.06.92 Added labelling of wedge with barlabel (and wedge parameter).
   22.06.92 Moved label to 1.1 * PIESIZE (rather than PIESIZE).
            Added check on barstyle.uselabels.
   23.06.92 Now just returns if lastangle and angle are identical.
*/
int Wedge(int               dest, 
      FH *fh,
      double            lastangle, 
      double            endangle, 
      int               pattern,
      int               eject,
      int               wedge)
{
   double   xc,
            yc,
            midang,
            labdist;
   
   if(endangle == lastangle) return(0);

   lastangle += piestyle.start;
   endangle  += piestyle.start;
   midang     = lastangle + (endangle - lastangle)/2.0;
   labdist    = 1.1 * PIESIZE;
   
   AMSetPen(dest, fh, 1);
   if(eject)
   {
      xc = cos(midang) * piestyle.eject * PIESIZE/100.0;
      yc = sin(midang) * piestyle.eject * PIESIZE/100.0;
      labdist += piestyle.eject * PIESIZE/100.0;
   }
   else
   {
      xc = 0.0;
      yc = 0.0;
   }

   AMWedge(dest, fh, xc, yc, lastangle, endangle, pattern);

   if(barlabels != NULL && barstyle.uselabels && !graph.usekey)
   {
      /* Now place the label */
      xc = cos(midang) * labdist;   
      yc = sin(midang) * labdist;   
   
      /* Place label according to quadrant */
      if(midang <= PI/2.0 || midang >= 3.0*PI/2.0) /* Left justify text    */
         AMText(dest, fh, xc, yc, barlabels[wedge].text);
      else                                         /* Right justify text   */
         AMRText(dest, fh, xc, yc, 0.0, barlabels[wedge].text);
   }
   
   return(0);
}

/***************************************************************************/
/*>DoExtraPieLabels(int dest, FH *fh)
   -------------------------------------------------
   Displays the extra labels
   19.06.92 Original (based on DoExtraLabels() )
*/
int DoExtraPieLabels(int               dest,
                 FH *fh)
{
   TEXTLIST *l;
   double   xc,
            yc;
   
   AMSetPen(dest, fh, 6);
   for(l=labels; l; NEXT(l))
   {
      if(l->stext.text[0] != '\0')
      {
         AMSetFont(dest, fh, &(l->stext));

         /* Now place the label */
         xc = cos(l->stext.x) * l->stext.y;   
         yc = sin(l->stext.x) * l->stext.y;   
      
         /* Place label */
         AMText(dest, fh, xc, yc, l->stext.text);
      }
   }
   
   return(0);
}


/**************************************************************************/
/*>DoPieKey(int dest, FH *fh)
   -----------------------------------------
   Creates a key for piecharts
   23.06.92 Original
*/
int DoPieKey(int               dest,
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
            lastangle = 0.0,
            angle     = 0.0,
            total;
   FILLITEM *f;
   int      j,
            pattern;
   
   /* Check this is a pie graph */
   if(graph.style == STYLE_PIE && graph.usekey == TRUE)
   {
      /* Set scaling */
      xscale = graph.XScale;
      yscale = graph.YScale;
      
      fontsize = data->key.PSSize;
      x        = data->key.x;       /* Start of key                        */
      y        = data->key.y;
      
      /* If we've got some labels, set the font (use key font) */
      if(barlabels) AMSetFont(dest, fh, &(data->key));
   
      /* For the moment we'll make barchart swatches 1pt smaller than the
         font size, but not bigger than 20pt.
      */
      swatch = fontsize - 1.0;
      if(swatch > 20.0) swatch = 20.0;
      
      /* Convert length of line and gap in final coords to data coords */
      dlen      = length / xscale;
      dgap      = gap    / xscale;
   
      xhalf     = x + (dlen/2.0);
      xend      = x + dlen;
      xgap      = xend + dgap;
      
      dswatch_x = xhalf + swatch/xscale;
      
      dswatch_y1 = y - swatch/(2.0 * yscale);
      dswatch_y2 = y + swatch/(2.0 * yscale);
      
      /* Find total of data in pie */
      for(j=0, total=0.0; j<data->ndata; j++) total += data->x[j];

      for(j=0; j<data->ndata; j++)
      {
         /* Find the boundary angles for this slice */
         lastangle = angle;
         angle    += (2.0 * PI * data->x[j] / total);
   
         if(barlabels[j].text[0] != '\0')
         {
            AMSetPen(dest, fh, 1);
   
            /* See if we have a fill for this slice */
            pattern = 0;
            if(filllist)
            {
               for(f=filllist; f; NEXT(f))
               {
                  if(f->angle > lastangle && f->angle < angle)
                     pattern = f->pattern;
               }
            }
   
            AMRect(dest, fh, xhalf,     dswatch_y1, 
                             dswatch_x, dswatch_y2, pattern, 1);
            AMMove(dest, fh, xhalf,     dswatch_y1);
            AMDraw(dest, fh, dswatch_x, dswatch_y1);
            AMEndLine(dest, fh);
   
               
            /* Reset pen for text */
            AMSetPen(dest, fh, 6);
         
            /* Write the text */
            AMCText(dest, fh, xgap, y, barlabels[j].text);
         
            /* Step along y by fontsize */
            y          -= fontsize/yscale;
            dswatch_y1 -= fontsize/yscale;
            dswatch_y2 -= fontsize/yscale;
         }
      }
   }
   return(0);
}

