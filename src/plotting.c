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
/* Fill patterns
*/
#ifdef AMIGA
WORDCHIP FillPat0[] = 
{
   0xFFFF,
   0xFFFF,
   0xFFFF,
   0xFFFF
};
WORDCHIP FillPat1[] = 
{
   0x8080,
   0x0000,
   0x0808,
   0x0000
};
WORDCHIP FillPat2[] = 
{
   0x8888,
   0x0000,
   0x2222,
   0x0000
};
WORDCHIP FillPat3[] = 
{
   0x8888,
   0x2222,
   0x8888,
   0x2222
};
WORDCHIP FillPat4[] = 
{
   0x9696,
   0x6969,
   0x9696,
   0x6969
};
WORDCHIP FillPat5[] = 
{
   0xAAAA,
   0x5555,
   0xAAAA,
   0x5555
};
WORDCHIP FillPat6[] = 
{
   0xFFFF,
   0xFFFF,
   0xFFFF,
   0xFFFF
};
#endif

/***************************************************************************/
/*>AMMove(int dest, FH *fh, double x, double y)
   -----------------------------------------------------------
   Move to a position specified in data coordinates.
   06.04.92 Handles screen
   10.04.92 Added log support
   29.04.92 Added check on log bounds
   07.05.92 Added PS support
   25.06.92 Added HPGL support
   16.07.92 Added DR2D support
*/
int AMMove(int                 dest,
       FH   *fh,
       double              x,
       double              y)
{
#ifdef AMIGA
   int xscreen,
       yscreen;
#endif

   if((graph.logx && x <= 0.0) || (graph.logy && y <= 0.0)) return(0);

   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      if(graph.logx)
         xscreen = display.xoff   + (log10(x)-log10(graph.xmin)) * graph.XScale;
      else
         xscreen = display.xoff   + (x-graph.xmin) * graph.XScale;

      if(graph.logy)
         yscreen = display.WYSize - 
                   (display.yoff + (log10(y)-log10(graph.ymin)) * graph.YScale);
      else
         yscreen = display.WYSize - 
                   (display.yoff + (y-graph.ymin) * graph.YScale);

      Move(rp,xscreen,yscreen);
#endif
      break;
   case DEST_PS:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      PSMove(x,y);
      break;
   case DEST_HPGL:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      HPGLMove(x,y);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      x *= output.xsize;
      y *= output.ysize;

      DR2DMove(x,y);
#endif
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>AMDraw(int dest, FH *fh, double x, double y)
   -----------------------------------------------------------
   Draw to a position specified in data coordinates.
   06.04.92 Handles screen
   10.04.92 Added log support
   29.04.92 Added check on log bounds
   07.05.92 Added PS support
   25.06.92 Added HPGL support
   16.07.92 Added DR2D support
*/
int AMDraw(int                 dest,
       FH   *fh,
       double              x,
       double              y)
{
#ifdef AMIGA
   int xscreen,
       yscreen;
#endif
   
   if((graph.logx && x <= 0.0) || (graph.logy && y <= 0.0)) return(0);

   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      if(graph.logx)
         xscreen = display.xoff   + (log10(x)-log10(graph.xmin)) * graph.XScale;
      else
         xscreen = display.xoff   + (x-graph.xmin) * graph.XScale;

      if(graph.logy)
         yscreen = display.WYSize - 
                   (display.yoff + (log10(y)-log10(graph.ymin)) * graph.YScale);
      else
         yscreen = display.WYSize - 
                   (display.yoff + (y-graph.ymin) * graph.YScale);

      Draw(rp,xscreen,yscreen);
#endif
      break;
   case DEST_PS:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      PSDraw(x,y);
      break;
   case DEST_HPGL:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      HPGLDraw(x,y);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      x *= output.xsize;
      y *= output.ysize;

      DR2DDraw(x,y);
#endif
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>AMSetPen(int dest, FH *fh, int pen)
   --------------------------------------------------
   Change pen
   06.04.92 Handles screen
   07.05.92 Added PS support
   25.06.92 Added HPGL support
   16.07.92 Added DR2D support
*/   
int AMSetPen(int               dest,
         FH *fh,
         int               pen)
{
   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      SetAPen(rp,pens[pen-1].screencolour);
#endif
      break;
   case DEST_PS:
      PSThick(pens[pen-1].thickness);
      break;
   case DEST_HPGL:
      HPGLPen(pen);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      DR2DThick(pens[pen-1].thickness);
#endif
      break;
   default:
      break;
   }
   return(0);
}

/***************************************************************************/
/*>AMLine(int dest, FH *fh, 
          double x1, double y1, double x2, double y2)
   --------------------------------------------------
   Draw a line from x1,y1 in data coordinates of length x2,y2 in device
   coordinates (approximately pt's)
   06.04.92 Handles screen
   10.04.92 Added log support
   29.04.92 Added check on log bounds
   07.05.92 Added PS support, corrected calc of y2screen to subtract y2
   25.06.92 Added HPGL support
   16.07.92 Added DR2D support
   17.07.92 Corrected conversion of offset to inches for DR2D
*/
int AMLine(int                 dest,
       FH   *fh,
       double              x1,
       double              y1,
       double              x2,
       double              y2)
{
#ifdef AMIGA
   int x1screen,
       y1screen,
       x2screen,
       y2screen;
#endif
   
   if((graph.logx && x1 <= 0.0) || (graph.logy && y1 <= 0.0)) return(0);

   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      if(graph.logx)
         x1screen = display.xoff + (log10(x1)-log10(graph.xmin)) * graph.XScale;
      else
         x1screen = display.xoff + (x1-graph.xmin) * graph.XScale;

      if(graph.logy)
         y1screen = display.WYSize - 
                    (display.yoff + (log10(y1)-log10(graph.ymin)) * graph.YScale);
      else
         y1screen = display.WYSize - 
                    (display.yoff + (y1-graph.ymin) * graph.YScale);

      x2screen = x1screen + (int)x2;
      y2screen = y1screen - (int)y2;
      Move(rp,x1screen,y1screen);
      Draw(rp,x2screen,y2screen);
#endif
      break;
   case DEST_PS:
      if(graph.logx)
         x1 = (log10(x1)-log10(graph.xmin)) * graph.XPScale;
      else
         x1 = (x1-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y1 = ((log10(y1)-log10(graph.ymin)) * graph.YPScale);
      else
         y1 = (y1-graph.ymin) * graph.YPScale;

      PSMove(x1, y1);
      PSDrawRel(x2, y2);
      break;
   case DEST_HPGL:
      if(graph.logx)
         x1 = (log10(x1)-log10(graph.xmin)) * graph.XPScale;
      else
         x1 = (x1-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y1 = ((log10(y1)-log10(graph.ymin)) * graph.YPScale);
      else
         y1 = (y1-graph.ymin) * graph.YPScale;

      x2 = x1 + x2 / (72.0 * output.xsize);
      y2 = y1 + y2 / (72.0 * output.ysize);

      HPGLMove(x1, y1);
      HPGLDraw(x2, y2);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      if(graph.logx)
         x1 = (log10(x1)-log10(graph.xmin)) * graph.XPScale;
      else
         x1 = (x1-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y1 = ((log10(y1)-log10(graph.ymin)) * graph.YPScale);
      else
         y1 = (y1-graph.ymin) * graph.YPScale;

      x1 *= output.xsize;
      y1 *= output.ysize;
      
      x2 /= 72.0;    /* Convert points to inches */
      y2 /= 72.0;
      x2 += x1;
      y2 += y1;

      DR2DMove(x1, y1);
      DR2DDraw(x2, y2);
#endif
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>AMCTText(int dest, FH *fh, double x, double y, char *text)
   -------------------------------------------------------------------------
   Centre-top justify text at position in data coordinates. Used for X-axis
   title and X-tick labels
   06.04.92 Handles screen
   07.04.92 Fix to positioning
   10.04.92 Added log support
   29.04.92 Added check on log bounds
   07.05.92 Added PS support
   25.06.92 Added HPGL support
   01.07.92 Changed for new versions of PSCTText() and HPGLCTText() which
            take offset in points.
   16.07.92 Added DR2D support
*/
int AMCTText(int               dest,
         FH *fh,
         double            x,
         double            y,
         char              *text)
{
#ifdef AMIGA
   int xscreen,
       yscreen;
#endif
   
   if((graph.logx && x <= 0.0) || (graph.logy && y <= 0.0)) return(0);

   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      text = SimplifyText(text);
      if(graph.logx)
         xscreen = display.xoff + (log10(x)-log10(graph.xmin)) * graph.XScale;
      else
         xscreen = display.xoff + (x-graph.xmin) * graph.XScale;
      xscreen -= (int)(TextLength(rp,text,strlen(text))/2);

      if(graph.logy)
         yscreen = display.WYSize - 
                   (display.yoff + (log10(y)-log10(graph.ymin)) * graph.YScale);
      else
         yscreen = display.WYSize - 
                   (display.yoff + (y-graph.ymin) * graph.YScale);
      yscreen += rp->TxBaseline;
      yscreen += output.ticklen;

      Move(rp,xscreen,yscreen);
      Text(rp,text,strlen(text));
#endif
      break;
      
   case DEST_PS:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      PSCTText(x, y, -output.ticklen, text);
      break;
   case DEST_HPGL:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      HPGLCTText(x, y, -output.ticklen, text);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;
      
      x *= output.xsize;
      y *= output.ysize;

      DR2DCTText(x, y, -output.ticklen, text);
#endif
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>AMCBText(int dest, FH *fh, double x, double y, char *text)
   -------------------------------------------------------------------------
   Centre-bottom justify text at position in data coordinates
   07.04.92 Handles screen
   10.04.92 Added log support
   29.04.92 Added check on log bounds
   07.05.92 Added PS support
   25.06.92 Added HPGL support
   16.07.92 Added DR2D support
*/
int AMCBText(int               dest,
         FH *fh,
         double            x,
         double            y,
         char              *text)
{
#ifdef AMIGA
   int xscreen,
       yscreen;
#endif
   
   if((graph.logx && x <= 0.0) || (graph.logy && y <= 0.0)) return(0);

   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      text = SimplifyText(text);

      if(graph.logx)
         xscreen = display.xoff + (log10(x)-log10(graph.xmin)) * graph.XScale;
      else
         xscreen = display.xoff + (x-graph.xmin) * graph.XScale;
      xscreen -= (int)(TextLength(rp,text,strlen(text))/2);

      if(graph.logy)
         yscreen = display.WYSize - 
                   (display.yoff + (log10(y)-log10(graph.ymin)) * graph.YScale);
      else
         yscreen = display.WYSize - 
                   (display.yoff + (y-graph.ymin) * graph.YScale);

      Move(rp,xscreen,yscreen);
      Text(rp,text,strlen(text));
#endif
      break;
      
   case DEST_PS:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      PSCBText(x, y, 0.0, text);
      break;
   case DEST_HPGL:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      HPGLCBText(x, y, 0.0, text);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;
      
      x *= output.xsize;
      y *= output.ysize;

      DR2DCBText(x, y, text);
#endif
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>AMRText(int dest, FH *fh, double x, double y, 
           double offset, char *text)
   ------------------------------------------------------------
   Right justify text at position in data coordinates; offset is an x-offset
   specified in device coordinates (pt)
   
   06.04.92 Handles screen
   07.04.92 Fix to positioning
   10.04.92 Added log support
   29.04.92 Added check on log bounds
   07.05.92 Added PS support and offset
   25.06.92 Added HPGL support
   16.07.92 Added DR2D support
*/
int AMRText(int               dest,
        FH *fh,
        double            x,
        double            y,
        double            offset,
        char              *text)
{
#ifdef AMIGA
   int xscreen,
       yscreen;
#endif
   
   if((graph.logx && x <= 0.0) || (graph.logy && y <= 0.0)) return(0);

   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      text = SimplifyText(text);

      if(graph.logx)
         xscreen =  display.xoff + (log10(x)-log10(graph.xmin)) * graph.XScale;
      else
         xscreen =  display.xoff + (x-graph.xmin) * graph.XScale;
      xscreen -= TextLength(rp,text,strlen(text));
      xscreen += offset;

      if(graph.logy)
         yscreen =  display.WYSize - 
                    (display.yoff + (log10(y)-log10(graph.ymin)) * graph.YScale);
      else
         yscreen =  display.WYSize - 
                    (display.yoff + (y-graph.ymin) * graph.YScale);
      yscreen += rp->TxBaseline/2;

      Move(rp,xscreen,yscreen);
      Text(rp,text,strlen(text));
#endif
      break;
      
   case DEST_PS:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      PSROffText(x, y, offset, text);
      break;
   case DEST_HPGL:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      HPGLROffText(x, y, offset, text);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;
      
      x *= output.xsize;
      y *= output.ysize;

      DR2DROffText(x, y, offset, text);
#endif
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>AMText(int dest, FH *fh, double x, double y, char *text)
   -----------------------------------------------------------------------
   Left justify text at position in data coordinates
   08.04.92 Handles screen
   10.04.92 Added log support
   29.04.92 Added check on log bounds
   07.05.92 Added PS support
   25.06.92 Added HPGL support
   16.07.92 Added DR2D support
*/
int AMText(int               dest,
       FH *fh,
       double            x,
       double            y,
       char              *text)
{
#ifdef AMIGA
   int xscreen,
       yscreen;
#endif
   
   if((graph.logx && x <= 0.0) || (graph.logy && y <= 0.0)) return(0);

   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      text = SimplifyText(text);

      if(graph.logx)
         xscreen =  display.xoff + (log10(x)-log10(graph.xmin)) * graph.XScale;
      else
         xscreen =  display.xoff + (x-graph.xmin) * graph.XScale;
      if(graph.logy)
         yscreen =  display.WYSize - 
                    (display.yoff + (log10(y)-log10(graph.ymin)) * graph.YScale);
      else
         yscreen =  display.WYSize - 
                    (display.yoff + (y-graph.ymin) * graph.YScale);

      Move(rp,xscreen,yscreen);
      Text(rp,text,strlen(text));
#endif
      break;
      
   case DEST_PS:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      PSLText(x,y,text);
      break;
   case DEST_HPGL:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      HPGLLText(x,y,text);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;
      
      x *= output.xsize;
      y *= output.ysize;

      DR2DLText(x,y,text);
#endif
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>AMCText(int dest, FH *fh, double x, double y, char *text)
   ------------------------------------------------------------------------
   Left/centre height justify text at position in data coordinates
   08.04.92 Handles screen
   10.04.92 Added log support
   29.04.92 Added check on log bounds
   06.05.92 Fix to height centering
   07.05.92 Added PS support
   08.05.92 Corrected Y-pos for PS
   25.06.92 Added HPGL support
   16.07.92 Added DR2D support
*/
int AMCText(int               dest,
        FH *fh,
        double            x,
        double            y,
        char              *text)
{
#ifdef AMIGA
   int xscreen,
       yscreen;
#endif
   
   if((graph.logx && x <= 0.0) || (graph.logy && y <= 0.0)) return(0);

   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      text = SimplifyText(text);

      if(graph.logx)
         xscreen  = display.xoff + (log10(x)-log10(graph.xmin)) * graph.XScale;
      else
         xscreen  = display.xoff + (x-graph.xmin) * graph.XScale;
      if(graph.logy)
         yscreen  = display.WYSize - 
                    (display.yoff + (log10(y)-log10(graph.ymin)) * graph.YScale);
      else
         yscreen  = display.WYSize - 
                    (display.yoff + (y-graph.ymin) * graph.YScale);
         yscreen += rp->TxBaseline/2;

      Move(rp,xscreen,yscreen);
      Text(rp,text,strlen(text));
#endif
      break;
      
   case DEST_PS:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      PSLCText(x,y,text);
      break;
   case DEST_HPGL:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;

      HPGLLCText(x,y,text);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;
      
      x *= output.xsize;
      y *= output.ysize;

      DR2DLCText(x,y,text);
#endif
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>AMSetFont(int dest, FH *fh, STATICTEXT *stext)
   -------------------------------------------------------------
   Sets the current font.
   07.04.92 Framework
   05.05.92 Original for Screen
   07.05.92 Added PS support
   25.06.92 Added HPGL support.
   29.06.92 Modified to use new PS2AmigaFont() for HPGL
   13.07.92 Added DEF_FONT parameter to SetAmigaFont()
   16.07.92 Added DR2D support
*/
int AMSetFont(int                 dest, 
          FH   *fh, 
          STATICTEXT          *stext)
{
   int FontNum;
   
   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      PS2AmigaFont(stext);
      SetAmigaFont(MainWind, stext->AmigaFont, (int)stext->PSSize, DEF_FONT);
#endif
      break;
   case DEST_PS:
      CorrectPSCase(stext);
      PSFont(stext->PSFont, stext->PSSize);
      break;
   case DEST_HPGL:
      FontNum = PS2AmigaFont(stext);
      HPGLFont(FontNum, stext->PSSize);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      DR2DSetFont(stext->PSFont, stext->PSSize);
#endif
      break;
   default:
      break;
   }
   return(0);
}

/***************************************************************************/
/*>AMYTitle(int dest, FH *fh, double x, double y, char *text)
   -------------------------------------------------------------------------
   Writes vertical text for the y-axis title. The specified x-position
   is offset by the width of the y-axis labels.
   07.04.92 Framework
   10.04.92 Added log support
   13.04.92 Fixed y-pos in logs.
   06.05.92 Corrected to use passed text rather than direct from YTitle
   08.05.92 Added PS support
   25.06.92 Corrected bug in label font size for PS. Added HPGL support.
   29.06.92 Modified to call PS2AmigaFont() for HPGL.
   13.07.92 Changed calc of label width, so if TxWidth < TxHeight/4,
            TxHeight is used instead. Seems to be a bug in TxWidth of
            Times.font. Also if TxWidth > TxHeight, use TxHeight
   16.07.92 Added DR2D support
   17.07.92 Changed buffer and label to [40] --- just in case!
   24.07.92 Added extras parameter to ftostr
*/
int AMYTitle(int dest, 
         FH *fh, 
         double x, 
         double y, 
         char *text)
{
   char buffer[40],
        label[40];
   int  label_width,
        TitleFontNum,
        LabelFontNum;
#ifdef AMIGA
   int  ypos,
        xscreen, yscreen,
        height;
   char *ptr;
#endif
   
   /* Print min and max labels to see what size they are */
   ftostr(buffer, 40, graph.ymin, graph.yprecision);
   label_width = strlen(buffer);
   strcpy(label,buffer);

   ftostr(buffer, 40, graph.ymax, graph.yprecision);
   if(strlen(buffer)>label_width)
   {
      label_width = strlen(buffer);
      strcpy(label, buffer);
   }
   
   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      text = SimplifyText(text);

      /* Get approx. width of y-labels */
      AMSetFont(dest, fh, &YLabel);
      if(rp->TxWidth < rp->TxHeight/4 || rp->TxWidth > rp->TxHeight)
         label_width *= rp->TxHeight;
      else
         label_width *= rp->TxWidth;

      AMSetFont(dest, fh, &YTitle);
      
      /* Calc pixel size of y-title as stacked characters */
      height = strlen(text) * rp->TxHeight;
      
      /*** 
       *** Calc x-position of title
       ***/
      /* Position specified in call             */
      if(graph.logx)
         xscreen =  display.xoff + (log10(x)-log10(graph.xmin)) * graph.XScale;
      else
         xscreen =  display.xoff + (x-graph.xmin) * graph.XScale;
      /* Offset for y-labels                    */
      xscreen -= label_width;
      /* Offset for tick-marks                  */
      xscreen -= output.ticklen;
      /* Offset for width of character & space  */
      xscreen -= 2 * rp->TxWidth;
      
      ptr  = text;
      
      /*** 
       *** Calc y-position for starting title
       ***/
      /* Centre-point of y-axis */
      if(graph.logy)
         y = (log10(graph.ymax)-log10(graph.ymin))/2.0;
      else
         y = (graph.ymax-graph.ymin)/2.0;

      /* Convert to screen coordinates */
      ypos = display.yoff + y * graph.YScale;
      /* Now offset by half the length of the title */
      for(yscreen = ypos - height/2;
          *ptr;
          ptr++, yscreen += rp->TxHeight)
      {
         Move(rp,xscreen,yscreen);
         Text(rp,ptr,1);
      }
#endif
      break;
   case DEST_PS:
      strcat(label, "  ");  /* Add a space between the title and the label */
      PSVText(0.0, 0.5, -output.ticklen,
              text,  YTitle.PSFont, YTitle.PSSize,
              label, YLabel.PSFont, YLabel.PSSize);
      break;
   case DEST_HPGL:
      TitleFontNum = PS2AmigaFont(&YTitle);
      LabelFontNum = PS2AmigaFont(&YLabel);
      strcat(label, "  ");  /* Add a space between the title and the label */
      HPGLVText(0.0, 0.5, -output.ticklen,
              text,  TitleFontNum, YTitle.PSSize,
              label, LabelFontNum, YLabel.PSSize);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      strcat(label, "  ");  /* Add a space between the title and the label */
      DR2DVText(0.0, output.ysize/2.0, -output.ticklen,
                text,  YTitle.PSFont, YTitle.PSSize,
                label, YLabel.PSFont, YLabel.PSSize);
#endif
      break;
   default:
      break;
   }

   return(0);
}

/***************************************************************************/
/*>char *ftostr(char *str, int maxlen, double x, int precision)
   ------------------------------------------------------------
   Convert a double to a string using precision decimal places. If precision
   is negative, use e-form, otherwise use f-form.
   07.07.92 Changed to stop value of -0.0
   24.07.92 Added maxlen parameter. If the f-form will exceed this number
            of characters we switch to e-form autmatically
   28.07.92 If precision -ve, is never < maxlen-5
*/
char *ftostr(char    *str,
             int     maxlen,
             double  x,
             int     precision)
{
   char     fmt[8],
            *ptr;
   int      i;
   double   val;
   
   /* If usiing e-form, check whether string will fit */
   if(precision >= 0)
   {
      double lgx;
      int    len;
      
      lgx = log10(ABS(x));       /* Number of pre-dp places - 1            */
      if(lgx < 0.0) lgx = 0.1;   /* i.e. If x<1.0, 1 p-dp after rounding   */ 
      len = (int)ceil(lgx);      /* Round up to number of pre-dp places    */
      len += precision + 2;      /* dp and '\0'                            */
      
      if(len > maxlen)           /* String too long                        */
      {
         if(precision == 0)   precision  = -1;  /* Set to -1               */
         else                 precision *= -1;  /* Switch to e-form        */
      }
   }
   
   if(precision < -1*(maxlen-5)) precision = -1*(maxlen-5);

   if(precision >= 0)
   {
      sprintf(fmt,"%%.%df",precision);
   }
   else
   {
      precision = abs(precision);
      sprintf(fmt,"%%.%de",precision);
   }
   sprintf(str,fmt,x);

   if(!strchr(str,'e'))                   /* Not e-form                    */
   {
      if(precision == 0)
      {
         /* Terminate string at . */
         ptr = str;
         while(*ptr)
         {
            if(*ptr++ == '.')
            {
               *ptr = '\0';
               break;
            }
         }
      }
      else
      {
         if((ptr = strchr(str,'.')))      /* Contains a decimal point      */
         {
            ptr++;
            for(i=0; *ptr; ptr++, i++);
            if(i<precision)               /* Fewer places than precision   */
            {
               /* Pad with zeros */
               for( ; i<precision; i++) *ptr++ = '0';
               *ptr = '\0';
            }
         }
         else                             /* No decimal point              */
         {
            /* Move to end of string */
            for(ptr = str; *ptr; ptr++);
            /* Put in a dp */
            *ptr++ = '.';
            /* Pad with zeros */
            for(i=0; i<precision; i++) *ptr++ = '0';
            *ptr = '\0';
         }
      }
   }
/*
// Attempt to raise the power. Need to add curly brackets
//   else                                   // Is e form.
//   {
//      *strchr(str,'e') = '^';
//   }
//
*/
      
   if(str[strlen(str)-1] == '.') str[strlen(str)-1] = '\0';
   
   /* Strip any leading spaces                        */
   while(str[0] == ' ')
      for(i=0; i<strlen(str); ) str[i] = str[++i];

   /* Remove any leading minus sign if value is 0.0   */
   sscanf(str,"%lf",&val);
   if(val == 0.0 && str[0] == '-')
      for(i=0; i<strlen(str); ) str[i] = str[++i];

   return(str);
}

/***************************************************************************/
/*>AMSetLineStyle(int dest, FH *fh, int style)
   ----------------------------------------------------------
   Set the line style
   08.04.92 Framework
   07.05.92 Original (screen & PS)
   25.06.92 Added HPGL support. Removed static store of style.
   05.07.92 Corrected line patterns. They don't have commas!
   16.07.92 Added DR2D support
*/
int AMSetLineStyle(int               dest,
               FH *fh, 
               int               style)
{
#ifdef AMIGA
   USHORT Pattern[6]       = {0xFFFF,        /* Screen line patterns       */
                              0xCCCC,
                              0xF6F6,
                              0xF0F0,
                              0xF198,
                              0xF3CC};
#endif
   char   PSPattern[6][16] = {"",            /* PostScript line patterns   */
                              "2",
                              "4 1 2 1",
                              "4",
                              "4 3 2 2 2 3",
                              "4 2 4 2 2 2"};
   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      SetDrPt(rp,Pattern[style]);
#endif
      break;
   case DEST_PS:
      if(style == 0) PSClearDash();
      else           PSSetDash(PSPattern[style]);
      break;
   case DEST_HPGL:
      HPGLSetDash(style);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      DR2DSetDash(style);
#endif
      break;
   default:
      break;
   }
   return(0);
}

/***************************************************************************/
/*>AMDPMark(int dest, FH *fh, double x, double y, int mark)
   -----------------------------------------------------------------------
   Draws a datapoint mark
   08.04.92 Framework
   10.04.92 Added log support
   29.04.92 Added check on log bounds
   06.05.92 Added call to ScreenMark()
   07.05.92 Added PS support
   25.06.92 Added HPGL support
   30.06.92 Removed call to AMEndLine()
   16.07.92 Added DR2D support
*/
int AMDPMark(int               dest,
         FH *fh, 
         double            x, 
         double            y, 
         int               mark)
{
#ifdef AMIGA
   int xscreen,
       yscreen;
#endif

   if((graph.logx && x <= 0.0) || (graph.logy && y <= 0.0)) return(0);

   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      if(graph.logx)
         xscreen = display.xoff   + (log10(x)-log10(graph.xmin)) * graph.XScale;
      else
         xscreen = display.xoff   + (x-graph.xmin) * graph.XScale;

      if(graph.logy)
         yscreen = display.WYSize - 
                   (display.yoff + (log10(y)-log10(graph.ymin)) * graph.YScale);
      else
         yscreen = display.WYSize - 
                   (display.yoff + (y-graph.ymin) * graph.YScale);

      ScreenMark(rp, xscreen, yscreen, mark, graph.dpsize);
#endif
      break;
   case DEST_PS:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;
      PSMark(x, y, mark, graph.dpsize);
      break;
   case DEST_HPGL:
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;
      HPGLMark(x, y, mark, graph.dpsize);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      if(graph.logx)
         x = (log10(x)-log10(graph.xmin)) * graph.XPScale;
      else
         x = (x-graph.xmin) * graph.XPScale;

      if(graph.logy)
         y = ((log10(y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (y-graph.ymin) * graph.YPScale;
      x *= output.xsize;
      y *= output.ysize;
      DR2DMark(x, y, mark, graph.dpsize);
#endif
      break;
   default:
      break;
   }
   
   return(0);
}

/***************************************************************************/
/*>char *SimplifyText(char *string)
   ---------------------------------
   Removes control codes from a string for screen display, etc
   06.05.92 Original
*/
char *SimplifyText(char *string)
{
   static char retstring[MAXBUFF];
   int         i, j;
   
   /* If the string is too long, just return it */
   if(strlen(string) > MAXBUFF) return(string);
   
   /* Walk along the string */
   for(i=0, j=0; i<strlen(string) && j<MAXBUFF-1; i++)
   {
      switch(string[i])
      {
      case '\\':           /* Should interpret next character as Greek     */
         retstring[j++] = string[++i];
         break;
      case '^':            /* Should raise next character                  */
         if(string[++i] == '{')
            while(string[++i] != '}' && string[i] != '\0' && j<MAXBUFF-1)
               retstring[j++] = string[i];
         else
            retstring[j++] = string[i];
         break;
      case '_':            /* Should lower next character                  */
         if(string[++i] == '{')
            while(string[++i] != '}' && string[i] != '\0' && j<MAXBUFF-1)
               retstring[j++] = string[i];
         else
            retstring[j++] = string[i];
         break;
      default:             /* An ordinary character                        */
         retstring[j++] = string[i];
         break;
      }
   }
   
   retstring[j] = '\0';

   return(retstring);
}

/***************************************************************************/
/*>AMInitPlot(int dest, FH *fh, 
              double xsize, double ysize, double xoff, double yoff)
   ----------------------------------------------------------------
   Initialise a device ready for plotting.
   
   07.05.92 Original
   25.06.92 Added HPGL support. Moved setting of PS globals to here.
   02.07.92 Put ClearWindow() in for screen plotting. Added seek to
            start of file for PS and HPGL file plotting.
   16.07.92 Added DR2D support
   17.07.92 Corrected call to InstallDR2DFonts() *before* InitDR2D().
            Changed buffer to [40]. Added bounds calc'n for DR2D.
   20.07.92 Added alternate font parameter to DR2DInit(). Added check on
            DR2DInit() return.
   22.07.92 Consider x-axis labelling in finding max x. Corrected to
            consider precision for log axes
   24.07.92 Added extras parameter to ftostr
   27.07.92 Removed the specification of EPSF offsets since the
            PSFixBoundingBox() routine takes care of all this. Increased
            size of xmax border for DR2D plots.
*/
int AMInitPlot(int                dest,
           FH  *fh, 
           double             xsize, 
           double             ysize, 
           double             xoff, 
           double             yoff)
{
   char     buffer[40];
   int      label_width;
#ifdef DR2D_SUPPORT
   double   EPSFxoff,   EPSFyoff,
            lab1size,   lab2size,
            xmax,       ymax,
            y;
   int      fontnum,
            prec;
#endif

   PSxpicsize      = output.xsize;
   PSypicsize      = output.ysize;
   PSxoffset       = output.xoffset;
   PSyoffset       = output.yoffset;
   
   PSHatchThick    = output.hatchthick;
   PSHatchSpace    = output.hatchspace;

   if(graph.style == STYLE_PIE)
   {
      if(PSxpicsize > PSypicsize) PSxpicsize = PSypicsize;
      else                        PSypicsize = PSxpicsize;
   }
   
   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      ClearWindow(MainWind);
#endif
      break;
   case DEST_PS:
      PSFile = fh;

#ifdef AMIGA
      Seek((BPTR)fh, 0L, OFFSET_BEGINNING);
#else
      rewind(fh);
#endif

      /* Print min and max labels to see what size they are */
      ftostr(buffer, 40, graph.ymin, graph.yprecision);
      label_width = strlen(buffer);
      ftostr(buffer, 40, graph.ymax, graph.yprecision);
      if(strlen(buffer)>label_width) label_width = strlen(buffer);

/*
//      EPSFxoff = label_width * YLabel.PSSize    + 2 * YTitle.PSSize;
//      EPSFyoff = output.ticklen + XLabel.PSSize + 2 * XTitle.PSSize;
*/

      PSInit(output.filename, "AMPlot2. SciTech Software, 1992",
             0.0, 0.0, output.AltFont);
      break;
   case DEST_HPGL:
      HPGLFile = fh;
      
#ifdef AMIGA
      Seek((BPTR)fh, 0L, OFFSET_BEGINNING);
#else
      rewind(fh);
#endif

      HPGLInit(output.xmargin, output.ymargin);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      InstallDR2DFonts();  /* Tell DR2D routines which fonts to use        */

      /* Find the larger y-label */
      fontnum = FindDR2DFont(YLabel.PSFont);
      /* If not logs, just take precision, else sign of precision */
      prec = (graph.logy ? (graph.yprecision>=0 ? 0 : -1) : graph.yprecision);
      ftostr(buffer, 40, graph.ymin, prec);
      lab1size = DR2DStrWidth(buffer, fontnum, (double)(YLabel.PSSize/72.0));
      ftostr(buffer, 40, graph.ymax, prec);
      lab2size = DR2DStrWidth(buffer, fontnum, (double)(YLabel.PSSize/72.0));
      
      /* Calculate x-offset from size of this and y-title */
      if(lab1size > lab2size) EPSFxoff = -lab1size;
      else                    EPSFxoff = -lab2size;
      EPSFxoff -= (2.0 * YTitle.PSSize / 72.0);
      
      /* Calculate y-offset from size of x-labels and x-title              */
      EPSFyoff  = -output.ticklen/72.0;   /* Account for ticks             */
      EPSFyoff -= 2.0*XLabel.PSSize/72.0; /* Account for x-labels & gap    */
      EPSFyoff -= 2.0*XTitle.PSSize/72.0; /* Account for x-title & gap     */

      /* Find y-position of title */
      if(graph.logy)
         y = ((log10(GTitle.y)-log10(graph.ymin)) * graph.YPScale);
      else
         y = (GTitle.y-graph.ymin) * graph.YPScale;

      y *= output.ysize;                  /* Convert to output size        */
      y -= GTitle.PSSize/72.0;            /* Account for descenders        */

      /* Find min y */
      EPSFyoff = MIN(EPSFyoff, y);

      /* Add size of title to find max y */
      y += GTitle.PSSize/72.0;
      ymax = MAX((output.ysize + graph.dpsize/72.0), y);

      /* Find max x. We'll assume, no titles/labels fall outside the graph 
         along x.
      */
      fontnum = FindDR2DFont(XLabel.PSFont);
      /* If not logs, just take precision, else sign of precision */
      prec = (graph.logx ? (graph.xprecision>=0 ? 0 : -1) : graph.xprecision);
      ftostr(buffer, 40, graph.xmax, prec);
      lab1size = DR2DStrWidth(buffer, fontnum, (double)(XLabel.PSSize/72.0));
      xmax = output.xsize + MAX((2.0 * graph.dpsize/72.0), lab1size);

      if(InitDR2D(output.filename, EPSFxoff, xmax, EPSFyoff, ymax, 
                  output.AltFont))
      {
         ReqMessage2(MainWind,"Unable to initialise DR2D file:",
                     output.filename,0);
      }
#endif
      break;
   default:
      break;
   }

   return(0);
}

/***************************************************************************/
/*>AMEndPlot(int dest, FH *fh)
   ------------------------------------------
   Close up a device after plotting.
   
   07.05.92 Original
   25.06.92 Added HPGL support
   01.07.92 Added blank WriteMessage() when plotting to screen
   16.07.92 Added DR2D support
*/
int AMEndPlot(int                dest,
          FH  *fh)
{
   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      WriteMessage(".");
#endif
      break;
   case DEST_PS:
      PSEnd();
      break;
   case DEST_HPGL:
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      EndDR2D();
#endif
      break;
   default:
      break;
   }

   
   return(0);
}

/***************************************************************************/
/*>PSMark(double x, double y, int mark, double dpsize)
   ---------------------------------------------------
   Draw a PostScript data point mark
   07.05.92 Original
   29.06.92 Calls for new DP mark styles added.
*/
int PSMark(double x, 
       double y, 
       int    mark, 
       double dpsize)
{
   dpsize /= 2.0;
   
   switch(mark)
   {
   case 0:                             /* No mark                          */
      break;
   case 1:                             /* Cross                            */
      PSCross(x, y, dpsize);
      break;
   case 2:                             /* Open square                      */
      PSBox(x, y, dpsize);
      break;
   case 3:                             /* Filled square                    */
      PSFillBox(x, y, dpsize);
      break;
   case 4:                             /* Open circle                      */
      PSCircle(x, y, dpsize);
      break;
   case 5:                             /* Filled circle                    */
      PSFillCircle(x, y, dpsize);
      break;
   case 6:                             /* Open diamond                     */
      PSDiamond(x, y, dpsize);
      break;
   case 7:                             /* Filled diamond                   */
      PSFillDiamond(x, y, dpsize);
      break;
   case 8:                             /* Open triangle                    */
      PSTriangle(x, y, dpsize);
      break;
   case 9:                             /* Filled triangle                  */
      PSFillTriangle(x, y, dpsize);
      break;
   case 10:                            /* Open inverted triangle           */
      PSInvTriangle(x, y, dpsize);
      break;
   case 11:                            /* Filled inverted triangle         */
      PSFillInvTriangle(x, y, dpsize);
      break;
   case 12:                            /* Plus sign                        */
      PSPlus(x, y, dpsize);
      break;
   default:
      break;
   }

   return(0);
}

/***************************************************************************/
/*>AMEndLine(int dest, FH *fh)
   ------------------------------------------
   07.05.92 Original
   25.06.92 Added HPGL support
   16.07.92 Added DR2D support
*/
int AMEndLine(int               dest, 
          FH *fh)
{
   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      return(0);
#endif
      break;
   case DEST_PS:
      PSStroke();
      break;
   case DEST_HPGL:
      return(0);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      DR2DStroke();
#endif
      break;
   default:
      break;
   }
   return(0);
}

/***************************************************************************/
/*>DataToScreen(double x, double y, int *xs, int *ys)
   --------------------------------------------------
   Converts data coorindates to screen coordinates
   15.05.92 Original
*/
int DataToScreen(double x,
             double y, 
             int    *xs,
             int    *ys)
{
   if(graph.logx)
      *xs = display.xoff + (log10(x)-log10(graph.xmin)) * graph.XScale;
   else
      *xs = display.xoff + (x-graph.xmin) * graph.XScale;

   if(graph.logy)
      *ys = display.WYSize - 
            (display.yoff + (log10(y)-log10(graph.ymin)) * graph.YScale);
   else
      *ys = display.WYSize - 
            (display.yoff + (y-graph.ymin) * graph.YScale);
   
   return(0);
}

/***************************************************************************/
/*>AMSetPattern(int dest, FH *fh, int pattern)
   ----------------------------------------------------------
   Sets the fill pattern (shades of grey).
   16.07.92 Added DR2D support. Doesn't do anything...
*/
int AMSetPattern(int dest, FH *fh, int pattern)
{
   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      if(pattern)
      {
         switch(pattern)
         {
         case 1:  SetAfPt(rp,&FillPat1[0],2);
                  break;
         case 2:  SetAfPt(rp,&FillPat2[0],2);
                  break;
         case 3:  SetAfPt(rp,&FillPat3[0],2);
                  break;
         case 4:  SetAfPt(rp,&FillPat4[0],2);
                  break;
         case 5:  SetAfPt(rp,&FillPat5[0],2);
                  break;
         case 6:  SetAfPt(rp,&FillPat6[0],2);
                  break;
         default: SetAfPt(rp,&FillPat0[0],2);
                  break;
         }
      }
#endif
      break;
   case DEST_PS:
      if(pattern)
      {
         switch(pattern)
         {
         case 1:  myputs(PSFile,"0.833 setgray\n");
                  break;
         case 2:  myputs(PSFile,"0.667 setgray\n");
                  break;
         case 3:  myputs(PSFile,"0.500 setgray\n");
                  break;
         case 4:  myputs(PSFile,"0.333 setgray\n");
                  break;
         case 5:  myputs(PSFile,"0.167 setgray\n");
                  break;
         case 6:  myputs(PSFile,"0.000 setgray\n");
                  break;
         default: myputs(PSFile,"0.000 setgray\n");
                  break;
         }
      }
      break;
   case DEST_HPGL:
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      /* TODO: Don't think we need this .... */
#endif
      break;
   default:
      break;
   }

   return(0);
}

/***************************************************************************/
/*>AMRect(int dest, FH *fh, double x1, double y1, 
          double x2, double y2, int pattern, int pen)
   ------------------------------------------------------------
   Draws a filled rectangle
   08.06.92 Original
   10.06.92 Reset to black after shaded fill; corrected coords for outline
   11.06.92 Added hatched fill support.
   16.07.92 Added DR2D support
   22.07.92 Fixed outline drawing for -ve bars.
*/
int AMRect(int                 dest,
       FH   *fh,
       double              x1,
       double              y1,
       double              x2,
       double              y2,
       int                 pattern,
       int                 pen)
{
#ifdef AMIGA
   int      x1s,
            y1s,
            x2s,
            y2s;
#endif
   double   x1in = x1,
            y1in = y1,
            x2in = x2,
            y2in = y2;
         
   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      DataToScreen(x1, y1, &x1s, &y1s);
      DataToScreen(x2, y2, &x2s, &y2s);

      if(y1s > y2s)
      {
         int temp = y1s;

         y1s      = y2s;
         y2s      = temp;
      }

      /* Clear this bar */
      SetAfPt(rp,&FillPat0[0],2);
      SetAPen(rp,0);
      SafeRectFill(rp, x1s, y1s+1, x2s, y2s-1);

      AMSetPattern(dest, fh, pattern);

      if(pattern)    /* If filled, fill with our chosen pen and pattern    */
      {
         AMSetPen(dest, fh, pen);
         SafeRectFill(rp, x1s, y1s+1, x2s, y2s-1);
         SetAfPt(rp,&FillPat0[0],2);
      }
      
#endif
      break;
   case DEST_PS:
      if(graph.logx)
      {
         x1 = (log10(x1)-log10(graph.xmin)) * graph.XPScale;
         x2 = (log10(x2)-log10(graph.xmin)) * graph.XPScale;
      }
      else
      {
         x1 = (x1-graph.xmin) * graph.XPScale;
         x2 = (x2-graph.xmin) * graph.XPScale;
      }

      if(graph.logy)
      {
         y1 = ((log10(y1)-log10(graph.ymin)) * graph.YPScale);
         y2 = ((log10(y2)-log10(graph.ymin)) * graph.YPScale);
      }
      else
      {
         y1 = (y1-graph.ymin) * graph.YPScale;
         y2 = (y2-graph.ymin) * graph.YPScale;
      }
      
      if(pattern == 0)
      {
         /* No fill, so fill in white  */
         myputs(PSFile,"1.000 setgray\n");
         myputs(PSFile,"newpath\n");
         PSMove(x1,y1);
         PSDraw(x1,y2);
         PSDraw(x2,y2);
         PSDraw(x2,y1);
         myputs(PSFile,"closepath\n");
         myputs(PSFile,"fill\n");

         /* Reset to black */
         myputs(PSFile,"0.000 setgray\n");
      }
      else                          /* Fill it as chosen                   */
      {
         if(output.hatching)        /* Hatched bars   */
         {
            PSHatchBox(x1,y1,x2,y2,pattern);
         }
         else                       /* Grey bars      */
         {
            AMSetPattern(dest, fh, pattern);
            myputs(PSFile,"newpath\n");
            PSMove(x1,y1);
            PSDraw(x1,y2);
            PSDraw(x2,y2);
            PSDraw(x2,y1);
            myputs(PSFile,"closepath\n");
            myputs(PSFile,"fill\n");
            myputs(PSFile,"stroke\n");
   
            /* Reset to black */
            myputs(PSFile,"0.000 setgray\n");
         }
      }
      break;
   case DEST_HPGL:
      if(pattern)
      {
         if(graph.logx)
         {
            x1 = (log10(x1)-log10(graph.xmin)) * graph.XPScale;
            x2 = (log10(x2)-log10(graph.xmin)) * graph.XPScale;
         }
         else
         {
            x1 = (x1-graph.xmin) * graph.XPScale;
            x2 = (x2-graph.xmin) * graph.XPScale;
         }
   
         if(graph.logy)
         {
            y1 = ((log10(y1)-log10(graph.ymin)) * graph.YPScale);
            y2 = ((log10(y2)-log10(graph.ymin)) * graph.YPScale);
         }
         else
         {
            y1 = (y1-graph.ymin) * graph.YPScale;
            y2 = (y2-graph.ymin) * graph.YPScale;
         }
         HPGLFillRect(x1,y1,x2,y2,pattern,output.hatchspace);
      }
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      if(pattern)
      {
         if(graph.logx)
         {
            x1 = (log10(x1)-log10(graph.xmin)) * graph.XPScale;
            x2 = (log10(x2)-log10(graph.xmin)) * graph.XPScale;
         }
         else
         {
            x1 = (x1-graph.xmin) * graph.XPScale;
            x2 = (x2-graph.xmin) * graph.XPScale;
         }
   
         if(graph.logy)
         {
            y1 = ((log10(y1)-log10(graph.ymin)) * graph.YPScale);
            y2 = ((log10(y2)-log10(graph.ymin)) * graph.YPScale);
         }
         else
         {
            y1 = (y1-graph.ymin) * graph.YPScale;
            y2 = (y2-graph.ymin) * graph.YPScale;
         }
         x1 *= output.xsize;
         x2 *= output.xsize;
         y1 *= output.ysize;
         y2 *= output.ysize;
         DR2DFillRect(x1,y1,x2,y2,pattern,output.hatching,
                      output.hatchspace,output.hatchthick);
      }
#endif
      break;
   default:
      break;
   }
   
   /* Now draw outline around it */
   AMSetPen(dest, fh, pen);
   if(ABS(y1in) < ABS(y2in))
   {
      AMMove(dest, fh, x1in, y1in);
      AMDraw(dest, fh, x1in, y2in);
      AMDraw(dest, fh, x2in, y2in);
      AMDraw(dest, fh, x2in, y1in);
   }
   else
   {
      AMMove(dest, fh, x1in, y2in);
      AMDraw(dest, fh, x1in, y1in);
      AMDraw(dest, fh, x2in, y1in);
      AMDraw(dest, fh, x2in, y2in);
   }
   
   AMEndLine(dest, fh);
  
/* 
//      if(!graph.fzero && (abs(y1-(WYMax-YOff))>2)) Draw(rp,x1s,y1s);
*/
   
   return(0);
}

/***************************************************************************/
/*>CalcWedgeBounds(double xcent, double ycent, double size,
                   double startangle, double endangle,
                   double *x1, double *y1, double *x2, double *y2)
   Given the centre of a wedge, it's radius and start and end angles,
   calculates the coordinates of a bounding rectangle.
   18.06.92 Original
*/
int CalcWedgeBounds(double xcent, 
                double ycent, 
                double size,
                double startangle, 
                double endangle,
                double *x1, 
                double *y1, 
                double *x2, 
                double *y2)
{
   double xstart,
          ystart,
          xend,
          yend,
          starta,       /* Temporary storage of angles               */
          enda;
          
   /* Initially do calculations with circle centred at 0.0, 0.0      */
   xstart = size * cos(startangle);
   ystart = size * sin(startangle);
   xend   = size * cos(endangle);
   yend   = size * sin(endangle);
   
   /* Set x1,y1 to min of these coords and the center of the circle  */
   *x1    = MIN(xstart, xend);
   *x1    = MIN(*x1,    0.0);
   *y1    = MIN(ystart, yend);
   *y1    = MIN(*y1,    0.0);
   
   /* Set x2,y2 to max of these coords and the center of the circle  */
   *x2    = MAX(xstart, xend);
   *x2    = MAX(*x2,    0.0);
   *y2    = MAX(ystart, yend);
   *y2    = MAX(*y2,    0.0);
   
   /* If angles span 0.0, update max x                               */
   if(startangle > endangle)  *x2 = size;
   
   /* If angles span PI/2, update max y                              */
   starta = startangle - PI/2.0;
   if(starta < 0.0)  starta = 2.0*PI + starta;
   enda   = endangle   - PI/2.0;
   if(enda < 0.0)    enda   = 2.0*PI + enda;
   if(starta > enda)          *y2 = size;

   /* If angles span PI, update min x                                */
   starta = startangle - PI;
   if(starta < 0.0)  starta = 2.0*PI + starta;
   enda   = endangle   - PI;
   if(enda < 0.0)    enda   = 2.0*PI + enda;
   if(starta > enda)          *x1 = -size;

   /* If angles span 3PI/2, update min y                             */
   starta = startangle - 3.0 * PI / 2.0;
   if(starta < 0.0)  starta = 2.0*PI + starta;
   enda   = endangle   - 3.0 * PI / 2.0;
   if(enda < 0.0)    enda   = 2.0*PI + enda;
   if(starta > enda)          *y1 = -size;

   /* Finally modify bounds by the centre point                      */
   *x1 += xcent;
   *y1 += ycent;
   *x2 += xcent;
   *y2 += ycent;

   return(0);
}

/***************************************************************************/
/*>AMWedge(int dest, FH *fh, double xcent, double ycent,
          double startangle, double endangle, int pattern)
   --------------------------------------------------------------------
   Draws a filled wedge centred at xcent, ycent from startangle to endangle.
   17.06.92 Original
   18.06.92 Fixed PS support, added hatching and eject support.
   19.06.92 Fixed hatch fill for ejected slices. Now changed to assume 
            X and Y scaling are the same and that the PS output dimensions
            are square.
   23.06.92 Moved arc drawing into PSArc()
   24.06.92 Added framework of hatched fills (commented out).
   25.06.92 Added HPGL support
   16.07.92 Added DR2D support
   17.07.92 Changed DR2D to use MIN(xsize,ysize) for scaling
*/

int AMWedge(int                dest, 
        FH  *fh, 
        double             xcent, 
        double             ycent,
        double             startangle, 
        double             endangle, 
        int                pattern)
{
#ifdef AMIGA
   int      xs,
            ys,
            length;
#endif
   double   x1, y1,
            x2, y2,
            xc, yc,
            xmin, ymin,
            xmax, ymax,
            dlength;
   char     buffer[200];
   
   switch(dest)
   {
   case DEST_SCREEN:
#ifdef AMIGA
      DataToScreen(xcent, ycent, &xs, &ys);
      length = PIESIZE * graph.XScale; /* XScale == YScale                 */

      if(pattern)
      {
/*
//         if(output.hatching)
//         {
//            double yoff;
//
//            for(yoff = -2.0 * PIESIZE; yoff <= 2.0 * PIESIZE; yoff+=10.0)
//            {
//               if(WedgeClip(xcent,ycent,PIESIZE,-9999.0,yoff,startangle,endangle,
//                            &x1,&y1,&x2,&y2))
//               {
//                  AMMove(dest, fh, x1, y1);
//                  AMDraw(dest, fh, x2, y2);
//               }
//            }
//         }
//         else
//         {
*/
         AMSetPattern(dest, fh, pattern);

         ScreenFillArc(rp, xs, ys, length, startangle, endangle);

         /* Clear the pattern */
         SetAfPt(rp,&FillPat0[0],2);

/*
//         }
*/
      }

      ScreenArc(rp, xs, ys, length, startangle, endangle, TRUE);

#endif
      break;
   case DEST_PS:
      /* Find the boundaries of the rectangle which encompasses this
         wedge.
      */
      CalcWedgeBounds(xcent,ycent,(double)PIESIZE,startangle,endangle,
                      &xmin,&ymin,&xmax,&ymax);
            
      endangle   *= 180.0 / PI;
      startangle *= 180.0 / PI;

      /* These are the bounds for the hatch fill */
      x1 = (xmin-graph.xmin) * graph.XPScale;       /* XPScale == YPScale  */
      y1 = (ymin-graph.ymin) * graph.XPScale;
      x2 = (xmax-graph.xmin) * graph.XPScale;
      y2 = (ymax-graph.ymin) * graph.XPScale;
      
      /* The centre of the wedge */
      xc = (xcent-graph.xmin) * graph.XPScale;      /* XPScale == YPScale  */
      yc = (ycent-graph.ymin) * graph.XPScale;

      /* Radius of wedge */
      dlength = PIESIZE * graph.XPScale;
      
      if(pattern)
      {
         if(output.hatching)
         {
            /* Define the clipping path */
            myputs(PSFile,"gsave\n");
            PSArc(xc,yc,dlength,startangle,endangle);
            myputs(PSFile,"clip\n");
            
            /* Do appropriate hatching. */
            switch(pattern)
            {
            case 1:  sprintf(buffer,"%f %f %f %f upLines\n",   x1,y1,x2,y2);
                     myputs(PSFile, buffer);
                     break;
            case 2:  sprintf(buffer,"%f %f %f %f downLines\n", x1,y1,x2,y2);
                     myputs(PSFile, buffer);
                     break;
            case 3:  sprintf(buffer,"%f %f %f %f horizLines\n",x1,y1,x2,y2);
                     myputs(PSFile, buffer);
                     break;
            case 4:  sprintf(buffer,"%f %f %f %f vertLines\n", x1,y1,x2,y2);
                     myputs(PSFile, buffer);
                     break;
            case 5:  sprintf(buffer,"%f %f %f %f upLines\n",   x1,y1,x2,y2);
                     myputs(PSFile, buffer);
                     sprintf(buffer,"%f %f %f %f downLines\n", x1,y1,x2,y2);
                     myputs(PSFile, buffer);
                     break;
            case 6:  sprintf(buffer,"%f %f %f %f horizLines\n",x1,y1,x2,y2);
                     myputs(PSFile, buffer);
                     sprintf(buffer,"%f %f %f %f vertLines\n", x1,y1,x2,y2);
                     myputs(PSFile, buffer);
                     break;
            default:
                     break;
            }
            myputs(PSFile,"grestore\n");
         }
         else
         {
            AMSetPattern(dest, fh, pattern);
   
            PSArc(xc,yc,dlength,startangle,endangle);

            /* Fill what we've just done */
            myputs(PSFile,"fill\n");
   
            /* Reset the grey shade */
            myputs(PSFile,"0.000 setgray\n");
         }
      }

      /* Draw without the fill */
      PSArc(xc,yc,dlength,startangle,endangle);
      PSStroke();

      break;
   case DEST_HPGL:
      /* The centre of the wedge */
      xc = (xcent-graph.xmin) * graph.XPScale;      /* XPScale == YPScale  */
      yc = (ycent-graph.ymin) * graph.XPScale;

      /* Radius of wedge */
      dlength = PIESIZE * graph.XPScale;
      
      HPGLArc(xc,yc,dlength,startangle,endangle,pattern,output.hatchspace);
      break;
   case DEST_DR2D:
#ifdef DR2D_SUPPORT
      /* The centre of the wedge */
      xc = (xcent-graph.xmin) * graph.XPScale;      /* XPScale == YPScale  */
      yc = (ycent-graph.ymin) * graph.XPScale;

      /* Radius of wedge */
      dlength = PIESIZE * graph.XPScale;

      xc *= MIN(output.xsize, output.ysize);
      yc *= MIN(output.xsize, output.ysize);
      dlength *= (output.xsize + output.ysize)/2.0;
      
      DR2DArc(xc,yc,dlength,startangle,endangle,pattern,output.hatching,
              output.hatchspace,output.hatchthick);
#endif
      break;
   default:
      break;
   }

   
   return(0);
}

/***************************************************************************/
/*>WedgeClip(double xc, double yc, double r, double m, double c, 
             double alpha1, double alpha2, double *x1, double *y1, 
             double *x2, double *y2)
   ---------------------------------------------------------------
   Given the centre of a wedge (xc, yc), its radius (r), and boundary angles
   (alpha1, alpha2) and a line's slope (m) and y-offset (c), outputs the
   coordinates of the ends of the line clipped by the wedge. Returns 1 if
   OK, 0 if the line doesn't pass through the wedge.
   FUDGE is defined to account for rounding errors.
   If m is -9999.0, this is treated as a special case for vertical lines
   and c is the x-offset. This is done by rotating the input angles through
   -PI/2, treating the lines as horizontal then rotating the resulting
   coordinates back by PI/2.
   24.06.92 Original
*/
#define FUDGE 0.00001
int WedgeClip(double xc,          /* Centre of circle                          */
          double yc,
          double r,           /* Radius of circle                          */
          double m,           /* Slope of line                             */
          double c,           /* y-offset of line                          */
          double alpha1,      /* Start angle                               */
          double alpha2,      /* End angle                                 */
          double *x1,         /* End of clipped line                       */
          double *y1,
          double *x2,
          double *y2)
{
   double   g1,               /* Slope of radius at alpha1                 */
            g2,               /* Slope of radius at alpha2                 */
            xl1,  yl1,        /* Intersect of line with radius at alpha1   */
            xl2,  yl2,        /* Intersect of line with radius at alpha2   */
            xc1,  yc1,        /* Intersect of line with circle             */
            xc2,  yc2,
            r2,               /* Radius squared                            */
            angle,            /* Used to check intersect validity          */
            sqterm;           /* sqrt term in the solve quadratic          */
   int      ok[4],            /* Flags for valid intersects                */
            nvalid,
            j,
            done,
            special=FALSE;    /* Flag for vertical lines                   */
   
   /* Return if angles identical */
   if(alpha1 > alpha2-FUDGE && alpha1 < alpha2+FUDGE) return(0);

   /* Fix angles to be between 0 and 2PI */
   while(alpha1 >= 2.0 * PI)  alpha1 -= (2.0*PI);
   while(alpha2 >= 2.0 * PI)  alpha2 -= (2.0*PI);
   while(alpha1 < 0.0)        alpha1 += (2.0*PI);
   while(alpha2 < 0.0)        alpha2 += (2.0*PI);

   /* Start off assuming all intersects are valid  */
   ok[0] = ok[1] = ok[2] = ok[3] = TRUE;
   
   /* If m is -9999.0, we treat this as vertical lines   */
   if(m == -9999.0)
   {
      special = TRUE;
      
      alpha1 -= PI/2.0;
      if(alpha1 < 0.0) alpha1 += (2.0 * PI);
      alpha2 -= PI/2.0;
      if(alpha2 < 0.0) alpha2 += (2.0 * PI);
      
      m = 0.0;
   }
   
   g1 = sin(alpha1) / cos(alpha1);
   g2 = sin(alpha2) / cos(alpha2);

   /* Check these are valid i.e. the line will intersect with the radii
      at some point.
   */
   if(g1 > m-FUDGE && g1 < m+FUDGE) ok[0] = FALSE;
   if(g2 > m-FUDGE && g2 < m+FUDGE) ok[1] = FALSE;
   
   r2 = r * r;
   
   if(ok[0])   /* Intersect of line with radius at alpha 1  */
   {
      xl1 = c        / (g1 - m);
      yl1 = (g1 * c) / (g1 - m);
   }
   
   if(ok[1])   /* Intersect of line with radius at alpha 2  */
   {
      xl2 = c        / (g2 - m);
      yl2 = (g2 * c) / (g2 - m);
   }
   
   /* First intersect of line with circle       */
   sqterm = 4.0 * ((m*m*c*c) - ((m*m+1) * (c*c - r*r)));
   sqterm = sqrt(sqterm);
   xc1 = ((-2.0 * m * c) + sqterm) / ((2.0 * m * m) + 2.0);
   yc1 = m * xc1 + c;
   
   /* Second intersect of line with circle      */
   xc2 = ((-2.0 * m * c) - sqterm) / ((2.0 * m * m) + 2.0);
   yc2 = m * xc2 + c;
   
/*** We now have 4 intersects, only 2 of which can be valid ***/
   
   /* Radius intersects are invalid if distance greater than radius */
   if(ok[0])
      if((xl1 * xl1) + (yl1 * yl1) > r2-FUDGE) ok[0] = FALSE;
   if(ok[1])
      if((xl2 * xl2) + (yl2 * yl2) > r2-FUDGE) ok[1] = FALSE;

   /* The intersects are invalid if their angles are outside the range 
      of alpha1 and alpha2
   */
   if(ok[0])
   {
      if(xl1 != 0.0 || yl1 != 0.0)
      {
         angle = trueangle(yl1,xl1);
         if(alpha1 < alpha2)
         {
            if(angle < (alpha1-FUDGE) || angle > (alpha2+FUDGE)) ok[0] = FALSE;
         }
         else
         {
            if(angle < (alpha1-FUDGE) && angle > (alpha2+FUDGE)) ok[0] = FALSE;
         }
      }
   }
   
   if(ok[1])
   {
      if(xl2 != 0.0 || yl2 != 0.0)
      {
         angle = trueangle(yl2,xl2);
         if(alpha1 < alpha2)
         {
            if(angle < (alpha1-FUDGE) || angle > (alpha2+FUDGE)) ok[1] = FALSE;
         }
         else
         {
            if(angle < (alpha1-FUDGE) && angle > (alpha2+FUDGE)) ok[1] = FALSE;
         }
      }
   }
   
   angle = trueangle(yc1,xc1);
   if(alpha1 < alpha2)
   {
      if(angle < (alpha1-FUDGE) || angle > (alpha2+FUDGE)) ok[2] = FALSE;
   }
   else
   {
      if(angle < (alpha1-FUDGE) && angle > (alpha2+FUDGE)) ok[2] = FALSE;
   }
   
   angle = trueangle(yc2,xc2);
   if(alpha1 < alpha2)
   {
      if(angle < (alpha1-FUDGE) || angle > (alpha2+FUDGE)) ok[3] = FALSE;
   }
   else
   {
      if(angle < (alpha1-FUDGE) && angle > (alpha2+FUDGE)) ok[3] = FALSE;
   }
   
   /* Check the number of valid intersects */
   for(j=0, nvalid = 0; j<4; j++) if(ok[j]) nvalid++;

   /* Set coordinates depending on number of valid intersects  */
   if(nvalid == 2)         /* Standard case  */
   {
      done = FALSE;
      for(j=0; j<4; j++)
      {
         if(done)
         {
            if(ok[j])
            {
               switch(j)
               {
               case 0:
                  *x2 = xl1;
                  *y2 = yl1;
                  break;
               case 1:
                  *x2 = xl2;
                  *y2 = yl2;
                  break;
               case 2:
                  *x2 = xc1;
                  *y2 = yc1;
                  break;
               case 3:
                  *x2 = xc2;
                  *y2 = yc2;
                  break;
               default:
                  break;
               }
            }
         }
         else
         {
            if(ok[j])
            {
               switch(j)
               {
               case 0:
                  *x1 = xl1;
                  *y1 = yl1;
                  break;
               case 1:
                  *x1 = xl2;
                  *y1 = yl2;
                  break;
               case 2:
                  *x1 = xc1;
                  *y1 = yc1;
                  break;
               case 3:
                  *x1 = xc2;
                  *y1 = yc2;
                  break;
               default:
                  break;
               }
               done = TRUE;
            }
         }
      }
   }
   else if(nvalid == 3)    /* Special case: either line through centre of  */
   {                       /* circle, or through radius/circle intersect.  */
      if(xl1 > (xl2-FUDGE) && xl1 < (xl2+FUDGE) &&
         yl1 > (yl2-FUDGE) && yl1 < (yl2+FUDGE))
      {
         /* Line through centre */
         *x1 = 0.0;
         *y1 = 0.0;
         if(ok[2])
         {
            *x2 = xc1;
            *y2 = yc1;
         }
         else
         {
            *x2 = xc2;
            *y2 = yc2;
         }
      }
      else
      {
         /* Line though radius/circle intersect, so discard the circle
            intersect
         */
         *x1 = xl1;
         *y1 = yl1;
         *x2 = xl2;
         *y2 = yl2;
      }
   }
   else                    /* Line doesn't intersect wedge                 */
   {
      return(0);
   }
   
   /* Just return, if the line is just a point */
   if(((*x1 - *x2) * (*x1 - *x2) + (*y1 - *y2) * (*y1 - *y2)) < FUDGE) return(0);


   /* If we've got vertical lines, rotate the coords back by PI/2 */
   if(special)
   {
      double temp;
      
      temp = *x1;
      *x1  = -1.0 * (*y1);
      *y1  = temp;

      temp = *x2;
      *x2  = -1.0 * (*y2);
      *y2  = temp;
   }

   /* Modify the coordinates by the specified centre */
   *x1 += xc;  *y1 += yc;
   *x2 += xc;  *y2 += yc;
   
   return(1);
}
   
#ifdef DR2D_SUPPORT
/***************************************************************************/
/*>InstallDR2DFonts(void)
   ----------------------
   Checks all the fonts being used and tells the DR2D routines which ones
   are being used.
   16.07.92 Original
   20.07.92 Added alternate font
*/
void InstallDR2DFonts(void)
{
   TEXTLIST *t;
   
   if(GTitle.text[0]) UseDR2DFont(GTitle.PSFont);
   if(XTitle.text[0]) UseDR2DFont(XTitle.PSFont);
   if(YTitle.text[0]) UseDR2DFont(YTitle.PSFont);
   UseDR2DFont(XLabel.PSFont);
   UseDR2DFont(YLabel.PSFont);
   for(t=labels; t; NEXT(t))
      UseDR2DFont(t->stext.PSFont);
   if(data && graph.usekey) UseDR2DFont(data->key.PSFont);
   UseDR2DFont(output.AltFont);
}
#endif
