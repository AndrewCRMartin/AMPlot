/****************************************************************************

   Program:    MoG
   File:       HPGL.c
   
   Version:    V2.0
   Date:       25.03.91
   Function:   HPGL plotting support commands
   
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
   V1.1  28.05.92    ANSIed
   V2.0  25.06.92    Modified for AMPlot2; floats->doubles

****************************************************************************/
#define HPGL_MAIN 1

/***************************************************************************/
/* This is used to fix the dimensions of vertical text. Not sure if this
   is a bug in the PLT: HPGL interpreter or is standard HPGL.
*/
#define FIXVERT

#define MAXBUFF   160            /* This must be as in "amplot.h"          */

/***************************************************************************/
/* Includes
*/
#ifdef FPU
#include <m68881.h>
#endif

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef AMIGA
#include <dos/dosextens.h>
#else
#include "macros.h"
#endif

#include "port.h"
#include "myio.h"
#include "hpgl.h"
#include "ps.h"
#include "csclip.h"

/* STATICTEXT is used to store text details. Character arrays are stored
   within the structure. Taken from "amplot.h"
*/
typedef struct
{
   double      PSSize,              /* PostScript font size (pt)           */
               x,                   /* x-position                          */
               y;                   /* y-position                          */
   int         AmigaSize;           /* Amiga screen font size              */
   char        text[MAXBUFF],       /* Text                                */
               PSFont[MAXBUFF],     /* PostScript font                     */
               AmigaFont[MAXBUFF];  /* Amiga screen font                   */
}  STATICTEXT;

/***************************************************************************/
/* Globals
*/
static int     HPGL_Debug = FALSE,
               FontHeight,                /* Font size in plotter units    */
               FontWidth;
static double  FontHcm,                   /* Font size in cm               */
               FontWcm;

/***************************************************************************/
/* Externals
*/
extern char *SimplifyText(char *text);
extern int PS2AmigaFont(STATICTEXT *stext);
extern int WedgeClip(double xc, double yc, double r, double m, double c,
          double alpha1, double alpha2, double *x1, double *y1,
          double *x2, double *y2);

/***************************************************************************/
/*>HPGLInit(double xmargin, double ymargin)
   ----------------------------------------
   Initialise an HPGL plot. The parameters specify the unprintable margins
   on the output device.
   25.06.92 Taken from MoG. Changed to support offsets. Added parameters.
   29.06.92 Added initialisation of alternate font.
*/
int HPGLInit(double xmargin, 
         double ymargin)
{
   char        buffer[80];
   int         xoff,
               yoff,
               AltFont;
   STATICTEXT  temptext;
   
   strcpy(temptext.PSFont,"Alternate");
   
   AltFont = PS2AmigaFont(&temptext);

   if((char *)getenv("HPGLDEBUG") != (char *)NULL)
      HPGL_Debug = TRUE;
   
   xoff = (int)((PSxoffset - xmargin) * 1024);
   yoff = (int)((PSyoffset - ymargin) * 1024);

   if(HPGL_Debug) myputs(HPGLFile, "\n>>Init:\n");
   
   sprintf(buffer,"IN; IP%d,%d,%d,%d;\n",xoff,yoff,
                                         (int)(PSxpicsize*1024)+xoff,
                                         (int)(PSypicsize*1024)+yoff);
   myputs(HPGLFile,buffer);

   sprintf(buffer,"SC0,10000,0,10000;DT\\;\n");
   myputs(HPGLFile,buffer);
   
   sprintf(buffer,"CA%d;SS;\n",AltFont);
   myputs(HPGLFile,buffer);

   return(0);
}

/***************************************************************************/
/*>HPGLPen(int num)
   ----------------
   Select a Pen
   25.06.92 Taken from MoG
   08.09.92 Changed to store pen first (having seen IntroCAD output)
*/
int HPGLPen(int num)
{
   char buffer[80];
   
   if(HPGL_Debug) myputs(HPGLFile, "\n>>Pen:\n");

   sprintf(buffer,"SP;PU;SP%1d;\n",num);
   myputs(HPGLFile,buffer);

   return(0);
}

/***************************************************************************/
/*>HPGLMove(double x, double y)
   --------------------------
   Move on HPGL plot
   25.06.92 Taken from MoG
*/
int HPGLMove(double x,
         double y)
{
   char buffer[80];
   
   if(HPGL_Debug) myputs(HPGLFile, "\n>>Move:\n");
   
   sprintf(buffer,"PU;PA%d, %d;\n", (int)(10000*x),(int)(10000*y));
   myputs(HPGLFile,buffer);

   return(0);
}

/***************************************************************************/
/*>HPGLDraw(double x, double y)
   --------------------------
   Draw on HPGL plot
   25.06.92 Taken from MoG
*/
int HPGLDraw(double x,
         double y)
{
   char buffer[80];
   
   if(HPGL_Debug) myputs(HPGLFile, "\n>>Draw:\n");
   
   sprintf(buffer,"PD;PA%d, %d;\n",(int)(10000*x),(int)(10000*y));
   myputs(HPGLFile,buffer);

   return(0);
}

/***************************************************************************/
/*>HPGLLText(double x, double y, char *string)
   -----------------------------------------
   Write text on HPGL plot
   25.06.92 Taken from MoG
   29.06.92 Changed to use HPGLShowText().
*/
int HPGLLText(double x,
          double y,
          char   *string)
{
   char buffer[80];
   
   if(HPGL_Debug) myputs(HPGLFile, "\n>>LText:\n");
   
   sprintf(buffer,"PU;PA%d, %d;",
           (int)(10000.0 * x),(int)(10000.0 * y));
   myputs(HPGLFile,buffer);
   
   HPGLShowText(string,0,(int)(10000.0 * x),(int)(10000.0 * y));

   return(0);
}

/***************************************************************************/
/*>HPGLFont(int font, double size)
   -------------------------------
   Set font for HPGL plot
   25.06.92 Taken from MoG
   29.06.92 Corrected CA to CS and added SS. Changed to use global width
            and height variables. Altered width to 1/2.4 * height
*/
int HPGLFont(int    font,
         double size)
{
   char   buffer[80];
   double width;
          
   /* Character dimensions in centimeters. We convert pts to cm, then divide
      by 1.7 to get a better size. Width is set to half height
   */
   FontHcm = size * 2.54 / (1.7 * 72.0);
   FontWcm = FontHcm/2.4;
   
   if(HPGL_Debug) myputs(HPGLFile, "\n>>Font:\n");
   
   sprintf(buffer,"PU;CS%d;SS;SI%5.3f, %5.3f;\n",font,FontWcm,FontHcm);
   myputs(HPGLFile,buffer);


   /* Character height in scaled units */
   FontHeight = (int)((size * 10000.0) / (72.0 * PSypicsize));

   /* Character width from height in cm. This conversion is empirical!     */
   width     =  FontHcm * 0.6154;      /* cm          */
   width     /= 2.54;                  /* in          */
   width     *= 10000.0/PSxpicsize;    /* Graph units */
   FontWidth =  (int)width;

   return(0);
}

/***************************************************************************/
/*>HPGLCBText(double x, double y, double offset, char *text)
   ---------------------------------------------------------
   25.06.92 Framework
   26.06.92 Original
   29.06.92 Added SimplifyText(). Changed to use HPGLShowText().
   06.07.92 Modified x-pos for wider font width
*/
int HPGLCBText(double x, 
           double y, 
           double offset, 
           char   *text)
{
   char     buffer[80];
   int      xpos,
            ypos;
   
   if(HPGL_Debug) myputs(HPGLFile, "\n>>CBText:\n");
   
   xpos = (int)(10000.0 * x);
   xpos -= strlen(SimplifyText(text)) * FontWidth / 2;
   xpos += FontWidth / 6;
   
   ypos = (int)(10000.0 * y);
   ypos -= (int)(offset * FontHeight);

   sprintf(buffer,"PU;PA%d, %d;", xpos, ypos);
   myputs(HPGLFile,buffer);

   HPGLShowText(text,0,xpos,ypos);
   
   return(0);
}

/***************************************************************************/
/*>HPGLROffText(double x, double y, double offset, char *text)
   -----------------------------------------------------------
   25.06.92 Framework
   26.06.92 Original
   29.06.92 Added SimplifyText(). Changed to use HPGLShowText().
   06.07.92 Modified x-pos for wider font width
*/
int HPGLROffText(double x, 
             double y, 
             double offset, 
             char   *text)
{
   char     buffer[80];
   int      xpos,
            ypos;
   
   if(HPGL_Debug) myputs(HPGLFile, "\n>>ROffText:\n");
   
   /* Base position */
   xpos = (int)(10000.0 * x);
   /* Right justify */
   xpos -= (strlen(SimplifyText(text)) * FontWidth);
   xpos += FontWidth/6;
   /* Convert offset from pt to plotter units */
   xpos += (int)((offset * 10000.0) / (72.0 * PSxpicsize));
   
   ypos = (int)(10000.0 * y);
   /* Centre y-height */
   ypos -= (int)(1.0 * FontHeight / 3.0);
   
   sprintf(buffer,"PU;PA%d, %d;\n", xpos, ypos);
   myputs(HPGLFile,buffer);
   HPGLShowText(text,0,xpos,ypos);
   
   return(0);
}

/***************************************************************************/
/*>HPGLLCText(double x, double y, char *text)
   ------------------------------------------
   25.06.92 Framework
   26.06.92 Original
   29.06.92 Changed to use HPGLShowText().
*/
int HPGLLCText(double x, 
           double y, 
           char   *text)
{
   char  buffer[80];
   int   xpos,
         ypos;
         
   if(HPGL_Debug) myputs(HPGLFile, "\n>>LCText:\n");
   
   xpos = (int)(10000.0 * x);
   ypos = (int)(10000.0 * y);

   /* Centre y-height */
   ypos -= (int)(1.0 * FontHeight / 3.0);
   
   sprintf(buffer,"PU;PA%d, %d;", xpos, ypos);
   myputs(HPGLFile,buffer);
   HPGLShowText(text,0,xpos,ypos);
   
   return(0);
}

/***************************************************************************/
/*>HPGLSetDash(int style)
   ----------------------
   25.06.92 Framework
   26.06.92 Original
*/
int HPGLSetDash(int style)
{
   if(HPGL_Debug) myputs(HPGLFile, "\n>>SetDash:\n");
   
   switch(style)
   {
   case 0:                                   /* Solid line     */
      myputs(HPGLFile,"LT;\n");
      break;
   case 1:                                   /* ............   */
      myputs(HPGLFile,"LT1,2;\n");
      break;
   case 2:                                   /* -.-.-.-.-.-.   */
      myputs(HPGLFile,"LT4,3;\n");
      break;
   case 3:                                   /* ------------   */
      myputs(HPGLFile,"LT2,3;\n");
      break;
   case 4:                                   /* -..-..-..-..   */
      myputs(HPGLFile,"LT6,3;\n");
      break;
   case 5:                                   /* --.--.--.--.   */
      myputs(HPGLFile,"LT5,3;\n");
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>HPGLMark(double x, double y, int mark, double size)
   ---------------------------------------------------
   25.06.92 Framework
   26.06.92 Original
*/
int HPGLMark(double x, 
         double y, 
         int    mark, 
         double size)
{
   char     buffer[160];
   int      xpos,
            ypos,
            xpltsize,
            ypltsize;
   double   ang;
       
   if(HPGL_Debug) myputs(HPGLFile, "\n>>Mark:\n");
   
   xpos     = (int)(10000.0 * x);
   ypos     = (int)(10000.0 * y);
   size    /= 4.0;
   xpltsize = (int)((size * 10000.0) / (72.0 * PSxpicsize));
   ypltsize = (int)((size * 10000.0) / (72.0 * PSypicsize));

   switch(mark)
   {
   case 0:                             /* No mark                          */
      break;
   case 1:                             /* Cross                            */
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PU;PA%d,%d;PD;PA%d,%d;\n",
              xpos-xpltsize,ypos-ypltsize,xpos+xpltsize,ypos+ypltsize,
              xpos-xpltsize,ypos+ypltsize,xpos+xpltsize,ypos-ypltsize);
      myputs(HPGLFile,buffer);
      break;
   case 2:                             /* Open square                      */
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PA%d,%d;PA%d,%d;PA%d,%d;\n",
              xpos-xpltsize,ypos-ypltsize,
              xpos+xpltsize,ypos-ypltsize,
              xpos+xpltsize,ypos+ypltsize,
              xpos-xpltsize,ypos+ypltsize,
              xpos-xpltsize,ypos-ypltsize);
      myputs(HPGLFile,buffer);
      break;
   case 3:                             /* Filled square (square + cross)   */
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PA%d,%d;PA%d,%d;PA%d,%d;\n",
              xpos-xpltsize,ypos-ypltsize,
              xpos+xpltsize,ypos-ypltsize,
              xpos+xpltsize,ypos+ypltsize,
              xpos-xpltsize,ypos+ypltsize,
              xpos-xpltsize,ypos-ypltsize);
      myputs(HPGLFile,buffer);
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PU;PA%d,%d;PD;PA%d,%d;\n",
              xpos-xpltsize,ypos-ypltsize,xpos+xpltsize,ypos+ypltsize,
              xpos-xpltsize,ypos+ypltsize,xpos+xpltsize,ypos-ypltsize);
      myputs(HPGLFile,buffer);
      break;
   case 4:                             /* Open circle                      */
      sprintf(buffer,"PU;PA%d,%d;PD;\n",(int)(xpos+xpltsize),ypos);
      myputs(HPGLFile,buffer);
      for(ang=0.0; ang <= 2*PI; ang += ((2*PI)/18.0))
      {
         double   xr,
                  yr;
         xr = size * cos(ang);
         yr = size * sin(ang);
         xpltsize = (int)((xr * 10000.0) / (72.0 * PSxpicsize));
         ypltsize = (int)((yr * 10000.0) / (72.0 * PSypicsize));
         
         sprintf(buffer,"PA%d,%d;",xpos+xpltsize,ypos+ypltsize);
         myputs(HPGLFile,buffer);
      }
      myputs(HPGLFile,"\n");
      break;
   case 5:                             /* Filled circle (circle + cross)   */
      sprintf(buffer,"PU;PA%d,%d;PD;\n",(int)(xpos+xpltsize),ypos);
      myputs(HPGLFile,buffer);
      for(ang=0.0; ang <= 2*PI; ang += ((2*PI)/18.0))
      {
         double   xr,
                  yr;
         xr = size * cos(ang);
         yr = size * sin(ang);
         xpltsize = (int)((xr * 10000.0) / (72.0 * PSxpicsize));
         ypltsize = (int)((yr * 10000.0) / (72.0 * PSypicsize));
         
         sprintf(buffer,"PA%d,%d;",xpos+xpltsize,ypos+ypltsize);
         myputs(HPGLFile,buffer);
      }
      myputs(HPGLFile,"\n");
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PU;PA%d,%d;PD;PA%d,%d;\n",
              (int)(xpos-xpltsize/1.414),(int)(ypos-ypltsize/1.414),
              (int)(xpos+xpltsize/1.414),(int)(ypos+ypltsize/1.414),
              (int)(xpos-xpltsize/1.414),(int)(ypos+ypltsize/1.414),
              (int)(xpos+xpltsize/1.414),(int)(ypos-ypltsize/1.414));
      myputs(HPGLFile,buffer);
      break;
   case 6:                             /* Open diamond                     */
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PA%d,%d;PA%d,%d;PA%d,%d;\n",
              xpos-xpltsize,  ypos,
              xpos,           ypos-ypltsize,
              xpos+xpltsize,  ypos,
              xpos,           ypos+ypltsize,
              xpos-xpltsize,  ypos);
      myputs(HPGLFile,buffer);
      break;
   case 7:                             /* Filled diamond (diamond + plus)  */
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PA%d,%d;PA%d,%d;PA%d,%d;\n",
              xpos-xpltsize,  ypos,
              xpos,           ypos-ypltsize,
              xpos+xpltsize,  ypos,
              xpos,           ypos+ypltsize,
              xpos-xpltsize,  ypos);
      myputs(HPGLFile,buffer);
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PU;PA%d,%d;PD;PA%d,%d;\n",
              xpos-xpltsize,ypos,
              xpos+xpltsize,ypos,
              xpos,ypos+ypltsize,
              xpos,ypos-ypltsize);
      myputs(HPGLFile,buffer);
      break;
   case 8:                             /* Open triangle                    */
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PA%d,%d;PA%d,%d;\n",
              xpos-xpltsize,  ypos-ypltsize,
              xpos,           ypos+ypltsize,
              xpos+xpltsize,  ypos-ypltsize,
              xpos-xpltsize,  ypos-ypltsize);
      myputs(HPGLFile,buffer);
      break;
   case 9:                             /* Filled triangle (tri + line)     */
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PA%d,%d;PA%d,%d;\n",
              xpos-xpltsize,  ypos-ypltsize,
              xpos,           ypos+ypltsize,
              xpos+xpltsize,  ypos-ypltsize,
              xpos-xpltsize,  ypos-ypltsize);
      myputs(HPGLFile,buffer);
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",
              xpos,           ypos-ypltsize,
              xpos,           ypos+ypltsize);
      myputs(HPGLFile,buffer);
      break;
   case 10:                            /* Open inverted triangle           */
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PA%d,%d;PA%d,%d;\n",
              xpos-xpltsize,  ypos+ypltsize,
              xpos,           ypos-ypltsize,
              xpos+xpltsize,  ypos+ypltsize,
              xpos-xpltsize,  ypos+ypltsize);
      myputs(HPGLFile,buffer);
      break;
   case 11:                            /* Filled inverted triangle         */
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PA%d,%d;PA%d,%d;\n",
              xpos-xpltsize,  ypos+ypltsize,
              xpos,           ypos-ypltsize,
              xpos+xpltsize,  ypos+ypltsize,
              xpos-xpltsize,  ypos+ypltsize);
      myputs(HPGLFile,buffer);
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",
              xpos,           ypos-ypltsize,
              xpos,           ypos+ypltsize);
      myputs(HPGLFile,buffer);
      break;
   case 12:                            /* Plus sign                        */
      sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;PU;PA%d,%d;PD;PA%d,%d;\n",
              xpos-xpltsize,ypos,
              xpos+xpltsize,ypos,
              xpos,ypos+ypltsize,
              xpos,ypos-ypltsize);
      myputs(HPGLFile,buffer);
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>HPGLFillRect(double x1, double y1, double x2, double y2, int pattern,
                double spacing)
   ---------------------------------------------------------------------
   25.06.92 Framework
*/
int HPGLFillRect(double x1, 
             double y1, 
             double x2, 
             double y2, 
             int    pattern,
             double spacing)
{
   char  buffer[80];
   int   x1lim,   y1lim,
         x2lim,   y2lim,
         x1p,     y1p,
         x2p,     y2p,
         xstep,   ystep,
         count;
         
   if(HPGL_Debug) myputs(HPGLFile, "\n>>FillRect:\n");
   
   x1lim = (int)(x1 * 10000.0);
   x2lim = (int)(x2 * 10000.0);
   y1lim = (int)(y1 * 10000.0);
   y2lim = (int)(y2 * 10000.0);
   xstep = (int)((spacing * 10000.0)/(72.0 * PSxpicsize));
   ystep = (int)((spacing * 10000.0)/(72.0 * PSypicsize));
   
   if(x2lim < x1lim)
   {
      int temp = x1lim;

      x1lim    = x2lim;
      x2lim    = temp;
   }
   
   if(y2lim < y1lim)
   {
      int temp = y1lim;

      y1lim    = y2lim;
      y2lim    = temp;
   }
   
   switch(pattern)
   {
   case 0:
      break;
   case 1:
      for(count = y1lim - 2*(x2lim-x1lim); count < y2lim; count += ystep)
      {
         x1p = x1lim;
         x2p = x2lim;
         y1p = count;
         y2p = y1p + (x2lim-x1lim)*PSxpicsize/PSypicsize;
         
         if(!CSClip(&x1p,&y1p,&x2p,&y2p,x1lim,y1lim,x2lim,y2lim))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",x1p,y1p,x2p,y2p);
            myputs(HPGLFile,buffer);
         }
      }
      break;
   case 2:
      for(count = y1lim; count < y2lim + 2*(x2lim-x1lim); count += ystep)
      {
         x1p = x1lim;
         x2p = x2lim;
         y1p = count;
         y2p = y1p - (x2lim-x1lim)*PSxpicsize/PSypicsize;
         
         if(!CSClip(&x1p,&y1p,&x2p,&y2p,x1lim,y1lim,x2lim,y2lim))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",x1p,y1p,x2p,y2p);
            myputs(HPGLFile,buffer);
         }
      }
      break;
   case 3:
      for(count = y1lim; count < y2lim; count += ystep)
      {
         sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",x1lim,count,x2lim,count);
         myputs(HPGLFile,buffer);
      }
      break;
   case 4:
      for(count = x1lim; count < x2lim; count += xstep)
      {
         sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",count,y1lim,count,y2lim);
         myputs(HPGLFile,buffer);
      }
      break;
   case 5:
      for(count = y1lim; count < y2lim; count += ystep)
      {
         sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",x1lim,count,x2lim,count);
         myputs(HPGLFile,buffer);
      }
      for(count = x1lim; count < x2lim; count += xstep)
      {
         sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",count,y1lim,count,y2lim);
         myputs(HPGLFile,buffer);
      }
      break;
   case 6:
      for(count = y1lim - 2*(x2lim-x1lim); count < y2lim; count += ystep)
      {
         x1p = x1lim;
         x2p = x2lim;
         y1p = count;
         y2p = y1p + (x2lim-x1lim)*PSxpicsize/PSypicsize;
         
         if(!CSClip(&x1p,&y1p,&x2p,&y2p,x1lim,y1lim,x2lim,y2lim))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",x1p,y1p,x2p,y2p);
            myputs(HPGLFile,buffer);
         }
      }
      for(count = y1lim; count < y2lim + 2*(x2lim-x1lim); count += ystep)
      {
         x1p = x1lim;
         x2p = x2lim;
         y1p = count;
         y2p = y1p - (x2lim-x1lim)*PSxpicsize/PSypicsize;
         
         if(!CSClip(&x1p,&y1p,&x2p,&y2p,x1lim,y1lim,x2lim,y2lim))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",x1p,y1p,x2p,y2p);
            myputs(HPGLFile,buffer);
         }
      }
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>HPGLVText(double x, double y, double xoff, char *text, int TitleFont,
             double TitleSize, char *label, int LabelFont, double LabelSize)
   -------------------------------------------------------------------------
   Write vertical text centred on x,y offset back along x by the size of
   label and by xoff in pts
   The FIXVERT code is used to correct character dimensions. Not sure if it
   is a bug in the PLT: interpreter which requires this, or whether it is
   standard HPGL.

   25.06.92 Framework
   29.06.92 Original
   06.07.92 Modified x-pos for wider font width
*/
int HPGLVText(double x, 
          double y, 
          double xoff, 
          char   *text, 
          int    TitleFont, 
          double TitleSize, 
          char   *label, 
          int    LabelFont, 
          double LabelSize)
{
   double   LabelWidth;
   char     buffer[240];

#ifdef FIXVERT
   double   width,
            fixwidth,
            height,
            fixheight;
#endif
   
   if(HPGL_Debug) myputs(HPGLFile, "\n>>LVText:\n");

   /* Find size of label */
   LabelWidth  = strlen(SimplifyText(label)) * 
                 (LabelSize * 10000.0) / (2.0 * 72.0 * PSxpicsize);
   /* Convert offset from points to plotter units */
   xoff *= 10000.0 / (72.0 * PSxpicsize);
   
   /* Convert x & y to plotter coordinates */
   x *= 10000.0;
   y *= 10000.0;

   /* Modify the x-coordinate to account for the offsets */
   x -= LabelWidth;
   x += xoff;
   
   /* Now find the y-start to centre the string vertically */
   y -= strlen(SimplifyText(text)) * FontWidth / 2;
   y += FontWidth/6;
   
#ifdef FIXVERT
   /* Character dimensions in centimeters. We convert pts to cm, then divide
      by 1.7 to get a better size. Width is set to half height
   */
   height    = TitleSize * 2.54 / (1.7 * 72.0);
   width     = height/2.0;

   fixwidth  = width  * PSxpicsize/PSypicsize;
   fixheight = height * PSypicsize/PSxpicsize;
   
   sprintf(buffer,"PU;SI%5.3f, %5.3f;\n",fixwidth,fixheight);
   myputs(HPGLFile,buffer);
#endif

   /* Now output the text */
   sprintf(buffer,"PU;PA%d,%d;DR0,1;",(int)x,(int)y);
   myputs(HPGLFile,buffer);
   HPGLShowText(text,1,(int)x,(int)y);
   myputs(HPGLFile,"DR;\n");
   
#ifdef FIXVERT
   sprintf(buffer,"PU;SI%5.3f, %5.3f;\n",width,height);
   myputs(HPGLFile,buffer);
#endif

   return(0);
}

/***************************************************************************/
/*>HPGLArc(double xc, double yc, double dlength, double startangle, 
           double endangle, int pattern)
   ----------------------------------------------------------------
   25.06.92 Framework
   29.06.92 Original
*/
int HPGLArc(double xc, 
        double yc, 
        double dlength, 
        double startangle, 
        double endangle, 
        int    pattern,
        double hatchstep)
{
   char   buffer[80];
   double x,
          y,
          angle,
          yoff,
          step,
          x1,  y1,
          x2,  y2;
   
   if(HPGL_Debug) myputs(HPGLFile, "\n>>Arc:\n");
   
   step = hatchstep / (72.0 * PSxpicsize);


/***                                ***
 ***  Fill the arc if appropriate.  ***
 ***                                ***/

   switch(pattern)
   {
   case 1:
      for(yoff = -2.0; yoff < 2.0; yoff += step)
      {
         if(WedgeClip(xc,yc,dlength,1.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",(int)(10000.0 * x1),
                                                      (int)(10000.0 * y1),
                                                      (int)(10000.0 * x2),
                                                      (int)(10000.0 * y2));
            myputs(HPGLFile,buffer);
         }
      }
      break;
   case 2:
      for(yoff = -2.0; yoff < 2.0; yoff += step)
      {
         if(WedgeClip(xc,yc,dlength,-1.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",(int)(10000.0 * x1),
                                                      (int)(10000.0 * y1),
                                                      (int)(10000.0 * x2),
                                                      (int)(10000.0 * y2));
            myputs(HPGLFile,buffer);
         }
      }
      break;
   case 3:
      for(yoff = -2.0; yoff < 2.0; yoff += step)
      {
         if(WedgeClip(xc,yc,dlength,0.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",(int)(10000.0 * x1),
                                                      (int)(10000.0 * y1),
                                                      (int)(10000.0 * x2),
                                                      (int)(10000.0 * y2));
            myputs(HPGLFile,buffer);
         }
      }
      break;
   case 4:
      for(yoff = -2.0; yoff < 2.0; yoff += step)
      {
         if(WedgeClip(xc,yc,dlength,-9999.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",(int)(10000.0 * x1),
                                                      (int)(10000.0 * y1),
                                                      (int)(10000.0 * x2),
                                                      (int)(10000.0 * y2));
            myputs(HPGLFile,buffer);
         }
      }
      break;
   case 5:
      for(yoff = -2.0; yoff < 2.0; yoff += step)
      {
         if(WedgeClip(xc,yc,dlength,1.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",(int)(10000.0 * x1),
                                                      (int)(10000.0 * y1),
                                                      (int)(10000.0 * x2),
                                                      (int)(10000.0 * y2));
            myputs(HPGLFile,buffer);
         }
      }
      for(yoff = -2.0; yoff < 2.0; yoff += step)
      {
         if(WedgeClip(xc,yc,dlength,-1.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",(int)(10000.0 * x1),
                                                      (int)(10000.0 * y1),
                                                      (int)(10000.0 * x2),
                                                      (int)(10000.0 * y2));
            myputs(HPGLFile,buffer);
         }
      }
      break;
   case 6:
      for(yoff = -2.0; yoff < 2.0; yoff += step)
      {
         if(WedgeClip(xc,yc,dlength,0.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",(int)(10000.0 * x1),
                                                      (int)(10000.0 * y1),
                                                      (int)(10000.0 * x2),
                                                      (int)(10000.0 * y2));
            myputs(HPGLFile,buffer);
         }
      }
      for(yoff = -2.0; yoff < 2.0; yoff += step)
      {
         if(WedgeClip(xc,yc,dlength,-9999.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
         {
            sprintf(buffer,"PU;PA%d,%d;PD;PA%d,%d;\n",(int)(10000.0 * x1),
                                                      (int)(10000.0 * y1),
                                                      (int)(10000.0 * x2),
                                                      (int)(10000.0 * y2));
            myputs(HPGLFile,buffer);
         }
      }
      break;
   default:
      break;
   }


/***                                ***
 ***  Now draw the wedge itself.    ***
 ***                                ***/

   /* Move to centre and draw out to start of arc */
   sprintf(buffer,"PU; PA%d,%d;PD;\n",  (int)(10000.0 * xc),
                                        (int)(10000.0 * yc));
   myputs(HPGLFile,buffer);
   
   if(startangle < endangle)
   {
      for(angle = startangle; angle < endangle; angle += (5.0 * PI / 180.0))
      {
         /* Calculate the point in the arc */
         x = dlength * cos(angle);
         y = dlength * sin(angle);
         
         /* Draw to this position */
         sprintf(buffer,"PA%d,%d;", (int)(10000.0 * x) + (int)(10000.0 * xc),
                                    (int)(10000.0 * y) + (int)(10000.0 * yc));
         myputs(HPGLFile,buffer);
      }
   }
   else
   {
      for(angle = startangle; angle < 2*PI; angle += (5.0 * PI / 180.0))
      {
         /* Calculate the start point of the arc */
         x = dlength * cos(angle);
         y = dlength * sin(angle);
         
         /* Draw to this position */
         sprintf(buffer,"PA%d,%d;", (int)(10000.0 * x) + (int)(10000.0 * xc),
                                    (int)(10000.0 * y) + (int)(10000.0 * yc));
         myputs(HPGLFile,buffer);
      }
      
      for(angle -= 2*PI; angle < endangle; angle += (5.0 * PI / 180.0))
      {
         /* Calculate the start point of the arc */
         x = dlength * cos(angle);
         y = dlength * sin(angle);
         
         /* Draw to this position */
         sprintf(buffer,"PA%d,%d;", (int)(10000.0 * x) + (int)(10000.0 * xc),
                                    (int)(10000.0 * y) + (int)(10000.0 * yc));
         myputs(HPGLFile,buffer);
      }
   }
   
   /* Draw to end angle */
   x = dlength * cos(endangle);
   y = dlength * sin(endangle);
   
   sprintf(buffer,"PA%d,%d;", (int)(10000.0 * x) + (int)(10000.0 * xc),
                              (int)(10000.0 * y) + (int)(10000.0 * yc));
   myputs(HPGLFile,buffer);
   
   /* Close the wedge */
   sprintf(buffer,"\nPA%d,%d;\n",   (int)(10000.0 * xc),
                                    (int)(10000.0 * yc));
   myputs(HPGLFile,buffer);
   
   return(0);
}

/***************************************************************************/
/*>HPGLShowText(char *text, int orientation, int XBase, int YBase)
   ---------------------------------------------------------------
   Displays the text, raising or lowering as appropriate and selecting
   alternate font where required.
   29.06.92 Original based on PostScript version.
*/
int HPGLShowText(char *text, 
             int  orientation,
             int  XBase,
             int  YBase)
{
   char     buffer[MAXBUFF],
            OutBuff[MAXBUFF];
   int      i, j,
            chcount,
            first = TRUE;
   double   fixwidth,
            fixheight;
   
   /* Walk along the string */
   for(i=0, j=0, chcount=0; i<strlen(text) && j<MAXBUFF-1; i++)
   {
      switch(text[i])
      {
      case '\\':           /* Should interpret next character as Greek     */
         /* Finish the current string */
         if(j)
         {
            buffer[j] = '\0';
            sprintf(OutBuff,"LB%s\\;",buffer);
            myputs(HPGLFile,OutBuff);
            j = 0;
         }
         /* Output the next character as Greek */
         sprintf(OutBuff,"SA;LB%c\\;SS;",text[++i]);
         myputs(HPGLFile,OutBuff);
         chcount++;
         break;
      case '^':            /* Should raise next character                  */
         /* Finish the current string */
         if(j)
         {
            buffer[j] = '\0';
            sprintf(OutBuff,"LB%s\\;",buffer);
            myputs(HPGLFile,OutBuff);
            j = 0;
         }
         
         if(first)
         {
            if(orientation) chcount++;
            first = FALSE;
         }

         /* Move to the shifted position */
         if(orientation)                                    /* VERTICAL    */
            sprintf(OutBuff,"PU;PA%d,%d;", XBase-FontHeight/2,
                    (int)(YBase+(PSxpicsize*chcount*FontWidth/PSypicsize)));
         else                                               /* HORIZONTAL  */
            sprintf(OutBuff,"PU;PA%d,%d;", XBase + chcount*FontWidth,
                                           YBase+FontHeight/2);
         myputs(HPGLFile,OutBuff);
         
         /* If necessary build string */
         if(text[++i] == '{')
         {
            while(text[++i] != '}' && text[i] != '\0' && j<MAXBUFF-1)
            {
               buffer[j++] = text[i];
               chcount++;
            }
         }
         else
         {
            buffer[j++] = text[i];
            chcount++;
         }
         /* Output raised string */
         if(j)
         {
            buffer[j] = '\0';
            if(orientation)
            {
               fixwidth  = FontWcm * PSxpicsize/PSypicsize;
               fixheight = FontHcm * PSypicsize/PSxpicsize;
            }
            else
            {
               fixwidth  = FontWcm;
               fixheight = FontHcm;
            }
            sprintf(OutBuff,"PU;SI%f,%f;LB%s\\;PU;SI%f,%f;",fixwidth,fixheight/2.0,
                                                            buffer,
                                                            fixwidth,fixheight);
            myputs(HPGLFile,OutBuff);
            j = 0;
         }
         /* Reset position */
         if(orientation)                                    /* VERTICAL    */
            sprintf(OutBuff,"PU;PA%d,%d;", XBase,
                    (int)(YBase+(PSxpicsize*chcount*FontWidth/PSypicsize)));
         else                                               /* HORIZONTAL  */
            sprintf(OutBuff,"PU;PA%d,%d;", XBase + chcount*FontWidth,
                                           YBase);
         myputs(HPGLFile,OutBuff);
         break;
      case '_':            /* Should lower next character                  */
         /* Finish the current string */
         if(j)
         {
            buffer[j] = '\0';
            sprintf(OutBuff,"LB%s\\;",buffer);
            myputs(HPGLFile,OutBuff);
            j = 0;
         }

         if(first)
         {
            if(orientation) chcount++;
            first = FALSE;
         }

         /* Move to the shifted position */
         if(orientation)                                    /* VERTICAL    */
            sprintf(OutBuff,"PU;PA%d,%d;", XBase+FontHeight/4,
                    (int)(YBase+(PSxpicsize*chcount*FontWidth/PSypicsize)));
         else                                               /* HORIZONTAL  */
            sprintf(OutBuff,"PU;PA%d,%d;", XBase + chcount*FontWidth,
                                           YBase-FontHeight/4);
         myputs(HPGLFile,OutBuff);

         /* If necessary build string */
         if(text[++i] == '{')
         {
            while(text[++i] != '}' && text[i] != '\0' && j<MAXBUFF-1)
            {
               buffer[j++] = text[i];
               chcount++;
            }
         }
         else
         {
            buffer[j++] = text[i];
            chcount++;
         }

         /* Output lowered string */
         if(j)
         {
            buffer[j] = '\0';
            if(orientation)
            {
               fixwidth  = FontWcm * PSxpicsize/PSypicsize;
               fixheight = FontHcm * PSypicsize/PSxpicsize;
            }
            else
            {
               fixwidth  = FontWcm;
               fixheight = FontHcm;
            }
            sprintf(OutBuff,"PU;SI%f,%f;LB%s\\;PU;SI%f,%f;",fixwidth,fixheight/2.0,
                                                            buffer,
                                                            fixwidth,fixheight);
            myputs(HPGLFile,OutBuff);
            j = 0;
         }
         /* Reset position */
         if(orientation)                                    /* VERTICAL    */
            sprintf(OutBuff,"PU;PA%d,%d;", XBase,
                    (int)(YBase+(PSxpicsize*chcount*FontWidth/PSypicsize)));
         else                                               /* HORIZONTAL  */
            sprintf(OutBuff,"PU;PA%d,%d;", XBase + chcount*FontWidth,
                                           YBase);
         myputs(HPGLFile,OutBuff);

         break;
      default:             /* An ordinary character                        */
         buffer[j++] = text[i];
         chcount++;
         break;
      }
   }
   
   if(j)
   {
      buffer[j] = '\0';
      sprintf(OutBuff,"LB%s\\;",buffer);
      myputs(HPGLFile,OutBuff);
      j = 0;
   }

   if(strlen(text)) myputs(HPGLFile,"\n");

   return(0);
}

/***************************************************************************/
/*>HPGLCTText(double x, double y, double offset, char *text)
   ---------------------------------------------------------
   Center Top justify text at x,y with y-offset in pts.
   25.06.92 Framework
   26.06.92 Original
   29.06.92 Added SimplifyText(). Changed to use HPGLShowText().
   01.07.92 Corrected y-positioning. Changed offset to be in pts rather
            than a multiplier of font size.
   06.07.92 Modified x-pos for wider font width
*/
int HPGLCTText(double x, 
           double y, 
           double offset, 
           char   *text)
{
   char     buffer[80];
   int      xpos,
            ypos;
   
   if(HPGL_Debug) myputs(HPGLFile, "\n>>CTText:\n");
   
   xpos = (int)(10000.0 * x);
   xpos -= (strlen(SimplifyText(text)) * FontWidth / 2.0);
   xpos += FontWidth/6;
   
   ypos = (int)(10000.0 * y);
   /* Move down by height of font */
   ypos -= FontHeight;
   /* Move by offset */
   ypos += (int)((offset * 10000.0) / (72.0 * PSypicsize));

   sprintf(buffer,"PU;PA%d, %d;\n", xpos, ypos);
   myputs(HPGLFile,buffer);
   
   HPGLShowText(text,0,xpos,ypos);
   
   return(0);
}

