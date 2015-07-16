/****************************************************************************

   Program:    
   File:       PSRoutines.c
   
   Version:    V1.12AMIGAFPU
   Date:       22.11.90
   Function:   PostScript Routines
   
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

   This program is not in the public domain, but it may be freely copied
   and distributed for no charge providing this header is included.
   The code may be modified as required, but any modifications must be
   documented so that the person responsible can be identified. If someone
   else breaks this code, I don't want to be blamed for code that does not
   work! The code may not be sold commercially without prior permission from
   the author, although it may be given away free with commercial products,
   providing it is made clear that this program is free and that the source
   code is provided with the program.

*****************************************************************************

   Description:
   ============
      These routines supply rudimentry PostScript support with
      simple commands from C.
      You should #include "PS.h" in your program and assign
      values to the global variables PSxpicsize,PSypicsize,PSxoffset
      and PSyoffset. All these values are in inches. Your plot should
      then run between 0.0 and 1.0 (you need to look after scaling
      to fit within these boundaries yourself).

*****************************************************************************

   Usage:
   ======
      Expects the following globals to be set:
      1. An Amiga FileHandle:       PSFile.
      2. A global flag:             EPSF

      You should start, having opened your PostScript output file
      with a call to PSInit which will set up the scaling and other
      routines within the PostScript program.

      Functions then available are:
      PSCross(X,Y,size)          Draws a cross of size at X,Y
      PSBox(X,Y,size)            Draws a box
      PSFillBox(X,Y,size)        Draws a filled box
      PSCircle(X,Y,size)         Draws a circle
      PSFillCircle(X,Y,size)     Draws a filled circle
      PSDiamond(X,Y,size)        Draws a diamond
      PSFillDiamond(X,Y,size)    Draws a filled diamond

      PSMove(X,Y)                Move to X,Y
      PSDraw(X,Y)                Draw to X,Y
      PSDrawRel(X,Y)             Draw X,Y in relative units
                                        (measured in points)
      PSStroke()                 Actually draw what you've just done
                                        onto the paper
      PSFont(fontname,size)      Select font and pointsize
      PSRText(X,Y,label)         Right justify text
      PSLText(X,Y,label)         Left justify text
      PSCTText(X,Y,Offset,label) Centre top justify text
      PSCBText(X,Y,Offset,label) Centre bottom justify text
      PSLTText(X,Y,label)        Left top justfy text
      PSRTText(X,Y,label)        Right top justfy text
      PSLText2(X1,X,Y1,Y,scale,label)  As original versions, but
      PSRText2(X1,X,Y1,Y,scale,label)  specify the scaling to be
      PSLTText2(X1,X,Y1,Y,scale,label) used (xscale or yscale) and
      PSRTText2(X1,X,Y1,Y,scale,label) an offset scaled position
      PSSetDash(linepatt)        Set dashed lines
      PSClearDash()              Clear dashed lines
      PSThick(thickness)         Set line thickness
      PSEnd()                    End of page

*****************************************************************************

   Revision History:
   =================
      V1.1    05.09.90
      Modified PSFont() to accept abbreviation of Times for
      Times-Roman, added size parameter to PSCross(),
      added xscale and yscale definitions to PSInit(),
      added PSThick()
      added linepatt parameter to PSSetDash()

      V1.2    28.09.90
      Changed PSInit() to define circle procedure
      Added:
      PSBox(X,Y,size)            Draws a box
      PSFillBox(X,Y,size)        Draws a filled box
      PSCircle(X,Y,size)         Draws a circle
      PSFillCircle(X,Y,size)     Draws a filled circle
      PSDiamond(X,Y,size)        Draws a diamond
      PSFillDiamond(X,Y,size)    Draws a filled diamond

      V1.2.1  04.09.90
      Modified Box and Diamond procedures to use closepath.      

      V1.3    18.10.90
      Added:
      PSLTText(X,Y,label)        Left top justfy text
      PSRTText(X,Y,label)        Right top justfy text
      
      V1.3    19.10.90
      Added:
      PSLText2(X1,X,Y1,Y,scale,label)  As original versions, but
      PSRText2(X1,X,Y1,Y,scale,label)  specify the scaling to be
      PSLTText2(X1,X,Y1,Y,scale,label) used (xscale or yscale) and
      PSRTText2(X1,X,Y1,Y,scale,label) an offset scaled position
      
      V1.5AMIGA    13.11.90
      Amiga specific version.
      Now removed PSFile from all the routine calls.
      Expects an Amiga FileHandle called PSFile & uses global PS_pbuf.
      Also added correct commenting to PSInit and PSEnd.
      Expects a global flag EPSF for handling EPSF output.
      Added correctcase() routine;

      V1.6AMIGA    22.11.90
      Fixed PSLText() back to match bottom of text rather than middle.
      PSRText() works as before for Y-axis labels. This _must_
      all be rationalised properly...
      
      V1.7AMIGA    06.02.91
      PSInit() now requires the filename to be passed to go into the
      preamble

      V1.8AMIGA    25.02.91
      PSInit() now also requires a string for the creator line of the 
      preamble to be passed.
      
      V1.9AMIGA    26.02.91
      PSInit() now also requires offsets for EPSF.
      
      V1.10AMIGA   07.05.92
      Changed all floats to double, ANSIfied, added autodoc comments
      Changed PSBox(), etc to take size in pt's
      Added PSROffText(X,Y,offset,text)   Right justified with device offset
      Changed PSInit() to support raise, lower and Greek, added PSShowText()
      and changed all routines to use it. Changed PSFont() to support
      raise and lower
      
      V1.11AMIGA  11.06.92
      Added to Init() max, upLines, downLines, horizLines and vertLines
      
      V1.12AMIGA  12.06.92
      Added spacing and size control for hatching

****************************************************************************/
/* Defines
*/
#define PSMAIN    1
#define MAXBUFF 160

/***************************************************************************/
/* Floating point include
*/
#ifdef FPU
#include <m68881.h>
#endif

/***************************************************************************/
/* Other includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "port.h"
#include "myio.h"
#include "ps.h"
#include <time.h>
#ifdef AMIGA
#include <exec/memory.h>
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

/***************************************************************************/
/* Externs and statics
*/
static char    PS_pbuf[200];
extern char    *SimplifyText(char *text);
static double  PSXMin = 0.0,        /* These are used for BoundingBox info */
               PSXMax = 0.0,
               PSYMin = 0.0,
               PSYMax = 0.0,
               TextHeight = 10.0;

/***************************************************************************/
static int UpdateLimits(double X, double Y, double XOff, double YOff);
static int FixBBox(char *script, int  filelen);

/***************************************************************************/
/*>PSInit(char *FName, char *creator, double EPSFxoff, double EPSFyoff,
          char *AltFont)
   --------------------------------------------------------------------
   Initialises the file writing the Prologue. The filename and creator
   are written into the Prologue and EPSFxoff and EPSFyoff are used to
   calculate the bounding box size for EPSF plots.
   08.05.92 Added definitions of raise, lower and greek. New parameter
            to specify name of alternate font.
   23.06.92 Padded BoundingBox with spaces and setting of PSXMin, etc.
   26.07.92 Correctly divide EPSFxoff and EPSFyoff by 72 when writing
            offsets in prologue.
   15.09.92 Added support for Amiga reencoding. Starting dimensions
            for BoundingBox set to dimensions set by Paper.
*/
int PSInit(char    *FName,
       char    *creator,
       double  EPSFxoff,
       double  EPSFyoff,
       char    *AltFont)
{
   struct tm *time_struc;    /* 29.05.97 Corrected rm to tm */
   long   time_value;
   
   /* Header information */
   if(EPSF)
      myputs(PSFile,"%!PS-Adobe-2.0 EPSF-1.2\n");
   else
      myputs(PSFile,"%!PS-Adobe-2.0\n");
   myputs(PSFile,"%%Creator: ");
   myputs(PSFile,creator);
   myputs(PSFile,"\n");
   myputs(PSFile,"%%For: (Amiga)\n");
   sprintf(PS_pbuf,"%%%%Title: (%s)\n",FName);
   myputs(PSFile,PS_pbuf);
   time(&time_value);
   time_struc = localtime(&time_value);
   sprintf(PS_pbuf,"%%%%CreationDate: %s",asctime(time_struc));
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"%%Pages: 1\n");
   if(EPSF)
   {
      sprintf(PS_pbuf,"%%%%BoundingBox: 0 0 %d %d                   \n",
              (int)(PSxpicsize * 72),
              (int)(PSypicsize * 72));
      myputs(PSFile,PS_pbuf);
      PSXMin = 0.0;
      PSYMin = 0.0;
      PSXMax = EPSFxoff/72.0 + PSxpicsize*72.0;
      PSYMax = EPSFyoff/72.0 + PSypicsize*72.0;
   }

   myputs(PSFile,"%%DocumentFonts: atend\n");
   
   myputs(PSFile,"%%EndComments\n");
   
   myputs(PSFile,"%%---------------Prologue-----------------\n");
   
   sprintf(PS_pbuf,"/xpicsize %7.4f def\n",PSxpicsize);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"/ypicsize %7.4f def\n",PSypicsize);
   myputs(PSFile,PS_pbuf);
   if(EPSF)
   {
      sprintf(PS_pbuf,"/xoffset %5.3f def\n",EPSFxoff/72.0);
      myputs(PSFile,PS_pbuf);
      sprintf(PS_pbuf,"/yoffset %5.3f def\n",EPSFyoff/72.0);
      myputs(PSFile,PS_pbuf);
   }
   else
   {
      sprintf(PS_pbuf,"/xoffset %7.4f def\n",PSxoffset);
      myputs(PSFile,PS_pbuf);
      sprintf(PS_pbuf,"/yoffset %7.4f def\n",PSyoffset);
      myputs(PSFile,PS_pbuf);
   }
   myputs(PSFile,"/xscale { xpicsize 72 mul mul } def\n");
   myputs(PSFile,"/yscale { ypicsize 72 mul mul } def\n");
   myputs(PSFile,"/xunits { xscale xoffset 72 mul add } def\n");
   myputs(PSFile,"/yunits { yscale yoffset 72 mul add } def\n\n");

#ifdef AMIGA
   /* Font re-encoding */
   myputs(PSFile,"% Re-encode font for Amiga special characters\n");
   myputs(PSFile,"/ISOLatin1Enc [\n");
   myputs(PSFile,"StandardEncoding 0 143 getinterval aload\n");
   myputs(PSFile,"/dotlessi/grave/acute/circumflex/tilde/macron/breve/dotaccent/dieresis/.notdef\n");
   myputs(PSFile,"/ring/cedilla/.notdef/quotedblright/ogonek/caron\n");
   myputs(PSFile,"/space/exclamdown/cent/sterling/currency/yen/brokenbar/section/dieresis/copyright\n");
   myputs(PSFile,"/ordfeminine/guillemotleft/logicalnot/hyphen/registered/macron/degree/plusminus\n");
   myputs(PSFile,"/twosuperior/threesuperior/acute/mu/paragraph/periodcentered/cedilla/onesuperior\n");
   myputs(PSFile,"/ordmasculine/guillemotright/onequarter/onehalf/threequarters/questiondown/Agrave\n");
   myputs(PSFile,"/Aacute/Acircumflex/Atilde/Adieresis/Aring/AE/Ccedilla/Egrave/Eacute/Ecircumflex\n");
   myputs(PSFile,"/Edieresis/Igrave/Iacute/Icircumflex/Idieresis/Eth/Ntilde/Ograve/Oacute/Ocircumflex\n");
   myputs(PSFile,"/Otilde/Odieresis/multiply/Oslash/Ugrave/Uacute/Ucircumflex/Udieresis/Yacute/Thorn\n");
   myputs(PSFile,"/germandbls/agrave/aacute/acircumflex/atilde/adieresis/aring/ae/ccedilla/egrave\n");
   myputs(PSFile,"/eacute/ecircumflex/edieresis/igrave/iacute/icircumflex/idieresis/eth/ntilde/ograve\n");
   myputs(PSFile,"/oacute/ocircumflex/otilde/odieresis/divide/oslash/ugrave/uacute/ucircumflex\n");
   myputs(PSFile,"/udieresis/yacute/thorn/ydieresis ] def\n\n");

   myputs(PSFile,"% Blue book full re-encoding program\n");
   myputs(PSFile,"% /reencodedict 5 dict def\n");
   myputs(PSFile,"/ReEncode\n");
   myputs(PSFile,"{ % reencodedict begin\n");
   myputs(PSFile,"	/newencoding exch def /newfontname exch def /basefontname exch def\n");
   myputs(PSFile,"	/basefontdict basefontname findfont def	/newfont basefontdict maxlength dict def\n");
   myputs(PSFile,"	basefontdict\n");
   myputs(PSFile,"	{exch dup dup /FID ne exch /Encoding ne and\n");
   myputs(PSFile,"		{exch newfont 3 1 roll put}{pop pop}ifelse\n");
   myputs(PSFile,"	}forall\n");
   myputs(PSFile,"	newfont /FontName newfontname put newfont /Encoding newencoding put\n");
   myputs(PSFile,"	newfontname newfont definefont pop\n");
   myputs(PSFile,"	% end\n");
   myputs(PSFile,"}def\n\n");

   myputs(PSFile,"% Modified from Gold's re-make font scheme\n");
   myputs(PSFile,"/font {\n");
   myputs(PSFile,"	/newfontname  exch def \n");
   myputs(PSFile,"   /basefontname exch def\n");
   myputs(PSFile,"	dup /sY exch def /sX exch def \n");
   myputs(PSFile,"	FontDirectory newfontname known not\n");
   myputs(PSFile,"	{ basefontname newfontname ISOLatin1Enc ReEncode } if\n");
   myputs(PSFile,"	newfontname findfont [ sX 0 0 sY 0 0 ] makefont setfont\n");
   myputs(PSFile,"}def\n\n");

#else
   myputs(PSFile,"% Simple font command compatible with Amiga re-encoding version\n");
   myputs(PSFile,"/font {\n");
   myputs(PSFile,"        pop\n");
   myputs(PSFile,"        findfont exch\n");
   myputs(PSFile,"        scalefont\n");
   myputs(PSFile,"        setfont\n");
   myputs(PSFile,"}def\n\n");
#endif

   /* The Hatch size and spacing */
   sprintf(PS_pbuf,"/hatchSpace %5.3f def\n",PSHatchSpace);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"/hatchThick %5.3f def\n",PSHatchThick);
   myputs(PSFile,PS_pbuf);

   /* The RJust procedure */
   myputs(PSFile,"/rightJustifyText\n");
   myputs(PSFile,"{  /RightColumn exch def\n");
   myputs(PSFile,"   dup\n");
   myputs(PSFile,"   stringwidth pop\n");
   myputs(PSFile,"   RightColumn exch sub\n");
   myputs(PSFile,"   Line moveto\n");
   myputs(PSFile,"   show\n");
   myputs(PSFile,"}  def\n");

   /* The circle procedure */
   myputs(PSFile,"/circle\n");
   myputs(PSFile,"{  0 360 arc\n");
   myputs(PSFile,"}  def\n");
   
   /* The raise procedure */
   myputs(PSFile,"/raise\n");
   myputs(PSFile,"{  TextHeight 1.5 div FontName FontName1 font\n");
   myputs(PSFile,"   0 TextHeight 2 div     rmoveto\n");
   myputs(PSFile,"   show\n");
   myputs(PSFile,"   0 TextHeight 2 div neg rmoveto\n");
   myputs(PSFile,"   TextHeight FontName FontName1 font\n");
   myputs(PSFile,"}  def\n");
   
   /* The lower procedure */
   myputs(PSFile,"/lower\n");
   myputs(PSFile,"{  TextHeight 1.5 div FontName FontName1 font\n");
   myputs(PSFile,"   0 TextHeight 4 div neg rmoveto\n");
   myputs(PSFile,"   show\n");
   myputs(PSFile,"   0 TextHeight 4 div     rmoveto\n");
   myputs(PSFile,"   TextHeight FontName FontName1 font\n");
   myputs(PSFile,"}  def\n");
   
   /* The greek procedure */
   myputs(PSFile,"/greek\n");
   myputs(PSFile,"{  AltFontName findfont TextHeight scalefont setfont\n");
   myputs(PSFile,"   show\n");
   myputs(PSFile,"   TextHeight FontName FontName1 font\n");
   myputs(PSFile,"}  def\n");
   
   /* Define the alternate font for Greek */
   sprintf(PS_pbuf,"/AltFontName  /%s def\n",AltFont);
   myputs(PSFile,PS_pbuf);
   
   /* The max procedure */
   myputs(PSFile,"/max\n");
   myputs(PSFile,"{  2 copy\n");
   myputs(PSFile,"   lt {exch} if\n");
   myputs(PSFile,"   pop\n");
   myputs(PSFile,"}  def\n");

   /* The horizLines procedure which draws a series of horizontal lines */
   myputs(PSFile,"/horizLines\n");
   myputs(PSFile,"% x1 y1 x2 y2 horizLines\n");
   myputs(PSFile,"{  newpath\n");
   myputs(PSFile,"   hatchThick setlinewidth\n");
   myputs(PSFile,"   /y2 exch def\n");
   myputs(PSFile,"   /x2 exch def\n");
   myputs(PSFile,"   /y1 exch def\n");
   myputs(PSFile,"   /x1 exch def\n");
   myputs(PSFile,"   y1 yunits hatchSpace y2 yunits\n");
   myputs(PSFile,"   {  dup\n");
   myputs(PSFile,"      x1 xunits exch moveto\n");
   myputs(PSFile,"      x2 xunits exch lineto\n");
   myputs(PSFile,"   }  for\n");
   myputs(PSFile,"   stroke\n");
   myputs(PSFile,"}  def\n");

   /* The vertLines procedure which draws a series of vertical lines */
   myputs(PSFile,"/vertLines\n");
   myputs(PSFile,"% x1 y1 x2 y2 vertLines\n");
   myputs(PSFile,"{  newpath\n");
   myputs(PSFile,"   hatchThick setlinewidth\n");
   myputs(PSFile,"   /y2 exch def\n");
   myputs(PSFile,"   /x2 exch def\n");
   myputs(PSFile,"   /y1 exch def\n");
   myputs(PSFile,"   /x1 exch def\n");
   myputs(PSFile,"   x1 xunits hatchSpace x2 xunits\n");
   myputs(PSFile,"   {  dup\n");
   myputs(PSFile,"      y1 yunits moveto\n");
   myputs(PSFile,"      y2 yunits lineto\n");
   myputs(PSFile,"   }  for\n");
   myputs(PSFile,"   stroke\n");
   myputs(PSFile,"}  def\n");

   /* The upLines procedure which draws a series of up slanting lines */
   myputs(PSFile,"/upLines\n");
   myputs(PSFile,"% x1 y1 x2 y2 upLines\n");
   myputs(PSFile,"{  newpath\n");
   myputs(PSFile,"   hatchThick setlinewidth\n");
   myputs(PSFile,"   /y2 exch def\n");
   myputs(PSFile,"   /x2 exch def\n");
   myputs(PSFile,"   /y1 exch def\n");
   myputs(PSFile,"   /x1 exch def\n");
   myputs(PSFile,"   x1 xunits hatchSpace x2 xunits\n");
   myputs(PSFile,"   {  y1 yunits moveto\n");
   myputs(PSFile,"      x2 xunits x1 xunits sub dup rlineto\n");
   myputs(PSFile,"   }  for\n");
   myputs(PSFile,"   stroke\n");
   myputs(PSFile,"   y1 yunits hatchSpace y2 yunits\n");
   myputs(PSFile,"   {  x1 xunits exch moveto\n");
   myputs(PSFile,"      x2 xunits x1 xunits sub dup rlineto\n");
   myputs(PSFile,"   }  for\n");
   myputs(PSFile,"   stroke\n");
   myputs(PSFile,"}  def\n");

   /* The downLines procedure which draws a series of down slanting lines */
   myputs(PSFile,"/downLines\n");
   myputs(PSFile,"% x1 y1 x2 y2 downLines\n");
   myputs(PSFile,"{  newpath\n");
   myputs(PSFile,"   hatchThick setlinewidth\n");
   myputs(PSFile,"   /y2 exch def\n");
   myputs(PSFile,"   /x2 exch def\n");
   myputs(PSFile,"   /y1 exch def\n");
   myputs(PSFile,"   /x1 exch def\n");
   myputs(PSFile,"   x1 xunits hatchSpace x2 xunits\n");
   myputs(PSFile,"   {  y2 yunits moveto\n");
   myputs(PSFile,"      x2 xunits x1 xunits sub dup neg rlineto\n");
   myputs(PSFile,"   }  for\n");
   myputs(PSFile,"   stroke\n");
   myputs(PSFile,"   y2 yunits hatchSpace neg y1 yunits\n");
   myputs(PSFile,"   {  x1 xunits exch moveto\n");
   myputs(PSFile,"      x2 xunits x1 xunits sub dup neg rlineto\n");
   myputs(PSFile,"   }  for\n");
   myputs(PSFile,"   stroke\n");
   myputs(PSFile,"}  def\n");
   

   myputs(PSFile,"%%EndProlog\n");
   myputs(PSFile,"%%Page 1 1\n");
   myputs(PSFile,"%%---------------Script-----------------\n");
   
   return(0);
}

/***************************************************************************/
/*>PSCross(double X, double Y, double size)
   ----------------------------------------
   Draws a cross of size at X,Y
   07.05.92 Removed scaling so size expressed in pt's
   23.06.92 Added UpdateLimits()
   30.06.92 Removed redundant sprintf()'s
*/
int PSCross(double X,
        double Y,
        double size)
{
   size /= 2.0;
   
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits %f sub moveto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits %f add lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits %f sub moveto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits %f add lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);

   myputs(PSFile,"stroke\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}
      
/***************************************************************************/
/*>PSBox(double X, double Y, double size)
   --------------------------------------
   Draws a box of size at X,Y
   07.05.92 Removed scaling so size expressed in pt's
   23.06.92 Added UpdateLimits()
   30.06.92 Removed redundant sprintf()'s
*/
int PSBox(double X,
      double Y,
      double size)
{
   size /= 2.0;
   
   sprintf(PS_pbuf,"newpath\n");
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits %f sub moveto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits %f add lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits %f add lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits %f sub lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);

   myputs(PSFile,"closepath\n");
   myputs(PSFile,"stroke\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSFillBox(double X, double Y, double size)
   ------------------------------------------
   Draws a filled box of size at X,Y
   07.05.92 Removed scaling so size expressed in pt's
   23.06.92 Added UpdateLimits()
   30.06.92 Removed redundant sprintf()'s
*/
int PSFillBox(double X,
          double Y,
          double size)
{
   size /= 2.0;
   
   sprintf(PS_pbuf,"newpath\n");
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits %f sub moveto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits %f add lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits %f add lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits %f sub lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"closepath\n");
   myputs(PSFile,"0 setgray\nfill\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSCircle(double X, double Y, double size)
   -----------------------------------------
   Draws a circle of size at X,Y
   07.05.92 Removed scaling so size expressed in pt's
   23.06.92 Added UpdateLimits()
   30.06.92 Removed redundant sprintf()'s
*/
int PSCircle(double X,
         double Y,
         double size)
{
   size /= 2.0;
   
   myputs(PSFile,"newpath");
   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits %7.4f circle\n",X,Y,size);
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"stroke\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSFillCircle(double X, double Y, double size)
   ---------------------------------------------
   Draws a filled circle of size at X,Y
   07.05.92 Removed scaling so size expressed in pt's
   23.06.92 Added UpdateLimits()
   30.06.92 Removed redundant sprintf()'s
*/
int PSFillCircle(double X,
             double Y,
             double size)
{
   size /= 2.0;
   
   myputs(PSFile,"newpath");
   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits %7.4f circle\n",X,Y,size);
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"0 setgray\nfill\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSDiamond(double X, double Y, double size)
   ------------------------------------------
   Draws a diamond of size at X,Y
   07.05.92 Removed scaling so size expressed in pt's
   23.06.92 Added UpdateLimits()
   30.06.92 Removed redundant sprintf()'s
*/
int PSDiamond(double X,
          double Y,
          double size)
{
   size /= 2.0;
   
   myputs(PSFile,"newpath\n");
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits moveto\n",
           X,size,Y);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits %f add lineto\n",
           X,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits lineto\n",
           X,size,Y);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits %f sub lineto\n",
           X,Y,size);
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"closepath\n");
   myputs(PSFile,"stroke\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSFillDiamond(double X, double Y, double size)
   ----------------------------------------------
   Draws a filled diamond of size at X,Y
   07.05.92 Removed scaling so size expressed in pt's
   23.06.92 Added UpdateLimits()
   30.06.92 Removed redundant sprintf()'s
*/
int PSFillDiamond(double X,
              double Y,
              double size)
{
   size /= 2.0;
   
   sprintf(PS_pbuf,"newpath\n");
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits moveto\n",
           X,size,Y);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits %f add lineto\n",
           X,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits lineto\n",
           X,size,Y);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits %f sub lineto\n",
           X,Y,size);
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"closepath\n");
   myputs(PSFile,"0 setgray\nfill\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSMove(double X, double Y)
   --------------------------
   Move to X,Y
   23.06.92 Added UpdateLimits()
*/
int PSMove(double X,
       double Y)
{
   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits moveto\n",X,Y);
   myputs(PSFile,PS_pbuf);

   UpdateLimits(X,Y,0.0,0.0);

   return(0);
}

/***************************************************************************/
/*>PSDraw(double X, double Y)
   --------------------------
   Draw to X,Y
   23.06.92 Added UpdateLimits()
*/
int PSDraw(double X,
       double Y)
{
   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits lineto\n",X,Y);
   myputs(PSFile,PS_pbuf);

   UpdateLimits(X,Y,0.0,0.0);

   return(0);
}

/***************************************************************************/
/*>PSDrawRel(double X, double Y)
   -----------------------------
   Draw X,Y in relative units (measured in points)
*/
int PSDrawRel(double X,
          double Y)
{
   sprintf(PS_pbuf,"%7.4f %7.4f rlineto\n",X,Y);
   myputs(PSFile,PS_pbuf);

   return(0);
}

/***************************************************************************/
/*>PSStroke(void)
   --------------
   Actually draw what you've just done onto the paper
   30.06.92 Removed redundant sprintf()'s
*/
int PSStroke(void)
{
   myputs(PSFile,"stroke\n");

   return(0);
}

/***************************************************************************/
/*>PSEnd(void)
   -----------
   End of page
*/
int PSEnd(void)
{
   if(!EPSF)myputs(PSFile,"showpage\n");
   myputs(PSFile,"%%Trailer\n");

   return(0);
}

/***************************************************************************/
/*>PSRCText(double X, double Y, char *label)
   -----------------------------------------
   Right justify text centre height.
   07.05.92 Changed x-pos to -6 rather than -4
   23.06.92 Added UpdateLimits()
   26.06.92 Changed to use PSShowText(). Corrected strlen() to use
            SimplifyText()
   30.06.92 Removed use of PSShowText(). This can't be used with the
            current rightJustifyText
   15.09.92 Multiply string width by 0.65
*/
int PSRCText(double X,
         double Y,
         char   *label)
{
   /* Define the current line */
   sprintf(PS_pbuf,"/Line %7.4f yunits TextHeight 2 div sub 2 add def\n",Y);
   myputs(PSFile,PS_pbuf);
   /* Put the text into the file */
   sprintf(PS_pbuf,"(%s)\n",label);
   myputs(PSFile,PS_pbuf);
   /* Ask to Right Justify */
   sprintf(PS_pbuf,"%7.4f xunits 6 sub rightJustifyText\n",X);
   myputs(PSFile,PS_pbuf);

   UpdateLimits(X,Y,-strlen(SimplifyText(label))*TextHeight*0.65,  TextHeight/2.0);
   UpdateLimits(X,Y,-strlen(SimplifyText(label))*TextHeight*0.65, -TextHeight/2.0);

   return(0);
}

/***************************************************************************/
/*>PSRText(double X, double Y, char *label)
   ----------------------------------------
   Right justify text. Not currently used in AMPlot.
   07.05.92 Changed x-pos to -6 rather than -4
   23.06.92 Added UpdateLimits()
   26.06.92 Changed to use PSShowText(). Corrected strlen() to use
            SimplifyText()
   30.06.92 Removed use of PSShowText(). This can't be used with the
            current rightJustifyText
   15.09.92 Multiply string width by 0.65
*/
int PSRText(double X,
        double Y,
        char   *label)
{
   /* Define the current line */
   sprintf(PS_pbuf,"/Line %7.4f yunits def\n",Y);
   myputs(PSFile,PS_pbuf);
   /* Put the text into the file */
   sprintf(PS_pbuf,"(%s)\n",label);
   myputs(PSFile,PS_pbuf);
   /* Ask to Right Justify */
   sprintf(PS_pbuf,"%7.4f xunits 4 sub rightJustifyText\n",X);
   myputs(PSFile,PS_pbuf);

   UpdateLimits(X,Y,-strlen(SimplifyText(label))*TextHeight*0.65,TextHeight);

   return(0);
}

/***************************************************************************/
/*>PSRTText(double X, double Y, char *label)
   -----------------------------------------
   Right top justify text. Not currently used in AMPlot
   23.06.92 Added UpdateLimits()
   26.06.92 Changed to use PSShowText(). Corrected strlen() to use
            SimplifyText()
   30.06.92 Removed use of PSShowText(). This can't be used with the
            current rightJustifyText
   15.09.92 Multiply string width by 0.65
*/
int PSRTText(double X,
         double Y,
         char   *label)
{
   /* Define the current line */
   sprintf(PS_pbuf,"/Line %7.4f yunits TextHeight 2 div sub 2 add \
TextHeight sub def\n",Y);
   myputs(PSFile,PS_pbuf);
   /* Put the text into the file */
   sprintf(PS_pbuf,"(%s)\n",label);
   myputs(PSFile,PS_pbuf);
   /* Ask to Right Justify */
   sprintf(PS_pbuf,"%7.4f xunits 4 sub rightJustifyText\n",X);
   myputs(PSFile,PS_pbuf);

   UpdateLimits(X,Y,-strlen(SimplifyText(label))*TextHeight*0.65, -TextHeight);

   return(0);
}

/***************************************************************************/
/*>PSLText(double X, double Y, char *label)
   ----------------------------------------
   Left justify text
   23.06.92 Added UpdateLimits()
   26.06.92 Changed strlen() to use SimplifyText()
   30.06.92 Removed redundant sprintf()'s
   15.09.92 Multiply string width by 0.65
*/
int PSLText(double X,
        double Y,
        char   *label)
{
   /* Define the current line and position */
   sprintf(PS_pbuf,"%7.4f xunits\n",X);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"/Line %7.4f yunits def\n",Y);
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"Line moveto\n");
   /* Put the text into the file and display */
   PSShowText(label);

   UpdateLimits(X,Y,strlen(SimplifyText(label))*TextHeight*0.65,TextHeight);

   return(0);
}
   
/***************************************************************************/
/*>PSLTText(double X, double Y, char *label)
   -----------------------------------------
   Left top justfy text
   23.06.92 Added UpdateLimits()
   26.06.92 Changed strlen() to use SimplifyText()
   30.06.92 Removed redundant sprintf()'s
   15.09.92 Multiply string width by 0.65
*/
int PSLTText(double X,
         double Y,
         char   *label)
{
   /* Define the current line and position */
   sprintf(PS_pbuf,"%7.4f xunits\n",X);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"/Line %7.4f yunits TextHeight 2 div sub 2 add \
TextHeight sub def\n",Y);
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"Line moveto\n");
   /* Put the text into the file and display */
   PSShowText(label);

   UpdateLimits(X,Y,strlen(SimplifyText(label))*TextHeight*0.65,-TextHeight);

   return(0);
}

/***************************************************************************/
/*>PSRText2(double X1, double X, double Y1, double Y, 
            char *scale, char *label)
   --------------------------------------------------
   Right justify text, but specify scaling to be used and an offset scaled
   position. Not currently used in AMPlot
   23.06.92 Added UpdateLimits()
   26.06.92 Changed to use PSShowText(). Corrected strlen() to use
            SimplifyText()
   30.06.92 Removed use of PSShowText(). This can't be used with the
            current rightJustifyText
   15.09.92 Multiply string width by 0.65
*/
int PSRText2(double X1,
         double X,
         double Y1,
         double Y,
         char   *scale,
         char   *label)
{
   /* Define the current line */
   sprintf(PS_pbuf,"/Line %7.4f yunits %7.4f %6s add TextHeight 2 div sub 7 \
add def\n",Y1,Y,scale);
   myputs(PSFile,PS_pbuf);
   /* Put the text into the file */
   sprintf(PS_pbuf,"(%s)\n",label);
   myputs(PSFile,PS_pbuf);
   /* Ask to Right Justify */
   sprintf(PS_pbuf,"%7.4f xunits %7.4f %6s add 4 sub rightJustifyText\n",
           X1,X,scale);
   myputs(PSFile,PS_pbuf);

   UpdateLimits(X,Y,-strlen(SimplifyText(label))*TextHeight*0.65,TextHeight);

   return(0);
}

/***************************************************************************/
/*>PSRTText2(double X1, double X, double Y1, double Y, 
             char *scale, char *label)
   ---------------------------------------------------
   Right top justify text, but specify scaling to be used and an offset 
   scaled position. Not currently used in AMPlot
   23.06.92 Added UpdateLimits()
   26.06.92 Changed to use PSShowText(). Corrected strlen() to use
            SimplifyText()
   30.06.92 Removed use of PSShowText(). This can't be used with the
            current rightJustifyText
   15.09.92 Multiply string width by 0.65
*/
int PSRTText2(double X1,
          double X,
          double Y1,
          double Y,
          char   *scale,
          char   *label)
{
   /* Define the current line */
   sprintf(PS_pbuf,"/Line %7.4f yunits %7.4f %6s add TextHeight 2 div sub 7 add \
TextHeight sub def\n",Y1,Y,scale);
   myputs(PSFile,PS_pbuf);
   /* Put the text into the file */
   sprintf(PS_pbuf,"(%s)\n",label);
   myputs(PSFile,PS_pbuf);
   /* Ask to Right Justify */
   sprintf(PS_pbuf,"%7.4f xunits %7.4f %6s add 4 sub rightJustifyText\n",
           X1,X,scale);
   myputs(PSFile,PS_pbuf);

   UpdateLimits(X,Y,-strlen(SimplifyText(label))*TextHeight*0.65,-TextHeight);

   return(0);
}

/***************************************************************************/
/*>PSLText2(double X1, double X, double Y1, double Y,
            char *scale, char *label)
   --------------------------------------------------
   Left justify text, but specify scaling to be used and an offset scaled
   position
   23.06.92 Added UpdateLimits()
   26.06.92 Changed strlen() to use SimplifyText()
   30.06.92 Removed redundant sprintf()'s
   15.09.92 Multiply string width by 0.65
*/
int PSLText2(double X1,
         double X,
         double Y1,
         double Y,
         char   *scale,
         char   *label)
{
   /* Define the current line and position */
   sprintf(PS_pbuf,"%7.4f xunits %7.4f %6s add\n",X1,X,scale);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"/Line %7.4f yunits %7.4f %6s add TextHeight 2 div sub 7 add \
def\n",Y1,Y,scale);
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"Line moveto\n");
   /* Put the text into the file and display */
   PSShowText(label);

   UpdateLimits(X,Y,strlen(SimplifyText(label))*TextHeight*0.65,TextHeight);

   return(0);
}
   
/***************************************************************************/
/*>PSLTText2(double X1, double X, double Y1, double Y, 
             char *scale, char *label)
   ---------------------------------------------------
   Left top justify text, but specify scaling to be used and an offset
   scaled position
   23.06.92 Added UpdateLimits()
   26.06.92 Changed strlen() to use SimplifyText()
   30.06.92 Removed redundant sprintf()'s
   15.09.92 Multiply string width by 0.65
*/
int PSLTText2(double X1,
          double X,
          double Y1,
          double Y,
          char   *scale,
          char   *label)
{
   /* Define the current line and position */
   sprintf(PS_pbuf,"%7.4f xunits %7.4f %6s add\n",X1,X,scale);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"/Line %7.4f yunits %7.4f %6s add TextHeight 2 div sub 7 add \
TextHeight sub def\n",Y1,Y,scale);
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"Line moveto\n");
   /* Put the text into the file and display */
   PSShowText(label);

   UpdateLimits(X,Y,strlen(SimplifyText(label))*TextHeight*0.65,-TextHeight);

   return(0);
}
   

/***************************************************************************/
/*>PSCBText(double X, double Y, double Offset, char *label)
   --------------------------------------------------------
   Centers a piece of text with X,Y being the Coords of the BOTTOM centre 
   point
   23.06.92 Added UpdateLimits()
   26.06.92 Changed strlen() to use SimplifyText()
   30.06.92 Removed redundant sprintf()'s
   15.09.92 Multiply string width by 0.65
*/
int PSCBText(double X,
         double Y,
         double Offset,
         char   *label)
{
   char *buffer;
   
   buffer = label;
   while(*buffer == ' ') buffer++;
   
   /* Put the string on the stack */
   sprintf(PS_pbuf,"(%s)\n",SimplifyText(buffer));
   myputs(PSFile,PS_pbuf);
   /* Define the current line */
   sprintf(PS_pbuf,
           "%7.4f yunits 2 sub /Line exch def\n",Y);
   myputs(PSFile,PS_pbuf);
   /* We are now left with label on the stack.
      Find half its width */
   myputs(PSFile,"dup stringwidth pop 2 div\n");
   /* Put the X position on the stack, subtract width/2 */
   sprintf(PS_pbuf,"%7.4f xunits exch sub\n",X);
   myputs(PSFile,PS_pbuf);
   /* We are left with the label and correct X on the stack
      Put Y on, move there and show the text */
   if (Offset == 0.0)
   {
      myputs(PSFile,"Line moveto pop\n");
   }
   else
   {
      sprintf(PS_pbuf,"Line TextHeight %7.4f mul sub moveto pop\n",Offset);
      myputs(PSFile,PS_pbuf);
   }
   PSShowText(buffer);

   UpdateLimits(X,Y,-strlen(SimplifyText(buffer))*TextHeight*0.65/2.0,TextHeight);
   UpdateLimits(X,Y, strlen(SimplifyText(buffer))*TextHeight*0.65/2.0,TextHeight);

   return(0);
}

/***************************************************************************/
/*>char *correctcase(char *font)
   -----------------------------
   Goes through a fontname and fixes case to match the required standard.
*/
char *correctcase(char *font)
{
   int i;
   /* Set the first character to UC */
   font[0] = toupper(font[0]);
   /* Set everything else to lower */
/****
   for(i=1;i<strlen(font);i++)
      font[i] = tolower(font[i]);
****/
   /* Now step through and upper the bits which need to be */
   for(i=1;i<strlen(font);i++)
   {
      /* Anything after a - */
      if(font[i]=='-')
         font[i+1] = toupper(font[i+1]);
      /* Start of the word oblique */
      if(!strncmp(font+i,"oblique",7))
         font[i] = toupper(font[i]);
      /* Start of the word italic */
      if(!strncmp(font+i,"italic",6))
         font[i] = toupper(font[i]);
      /* Start of the word roman */
      if(!strncmp(font+i,"roman",5))
         font[i] = toupper(font[i]);
      /* Start of the word bold */
      if(!strncmp(font+i,"bold",4))
         font[i] = toupper(font[i]);
   }

   return(font);
}

/***************************************************************************/
/*>PSFont(char *fontname, double size)
   -----------------------------------
   Set the font and size
   08.05.92 Changed to support raise and lower
   23.06.92 Set TextHeight
   15.09.92 Changed to support Amiga reencoding
*/
int PSFont(char    *fontname,
       double  size)
{
   char font[80];
   
   StringToUpper(fontname,font);

   if(!strncmp(font,"TIMES",5))
   {
      if(font[5] != '-')
         strcpy(fontname+5,"-Roman\0");
   }
   
   sprintf(PS_pbuf,"/FontName /%s def\n",correctcase(fontname));
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"/FontName1 /%s1 def\n",correctcase(fontname));
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"/TextHeight %f def\n",size);
   myputs(PSFile,PS_pbuf);
   
   myputs(PSFile,"TextHeight FontName FontName1 font\n");

   TextHeight = size;

   return(0);
}

/***************************************************************************/
/*>PSSetDash(char *linepatt)
   -------------------------
   Set a line dash pattern which must be supplied as a string
*/
int PSSetDash(char *linepatt)
{
   sprintf(PS_pbuf,"[%s] 0 setdash\n",linepatt);
   myputs(PSFile,PS_pbuf);

   return(0);
}

/***************************************************************************/
/*>PSClearDash(void)
   -----------------
   Clear the dash pattern to a full line
   30.06.92 Removed redundant sprintf()'s
*/
int PSClearDash(void)
{
   myputs(PSFile,"[] 0 setdash\n");

   return(0);
}

/***************************************************************************/
/*>PSThick(double thickness)
   -------------------------
   Set the line thickness
*/
int PSThick(double thickness)
{
   sprintf(PS_pbuf,"%f setlinewidth\n",thickness);
   myputs(PSFile,PS_pbuf);

   return(0);
}

/***************************************************************************/
/*>PSROffText(double X, double Y, double offset, char *label)
   ----------------------------------------------------------
   Right justify text with offset in device coordinates (points).
   Used to label the y-axis.

   07.05.92 Original
   23.06.92 Added UpdateLimits()
   26.06.92 Changed to use PSShowText(). Corrected strlen() to use
            SimplifyText()
   30.06.92 Removed use of PSShowText(). This can't be used with the
            current rightJustifyText
   15.09.92 Multiply string width by 0.65
*/
int PSROffText(double X,
           double Y,
           double offset,
           char   *label)
{
   /* Define the current line */
   sprintf(PS_pbuf,"/Line %7.4f yunits TextHeight 2 div sub 2 add def\n",Y);
   myputs(PSFile,PS_pbuf);
   /* Put the text into the file */
   sprintf(PS_pbuf,"(%s)\n",label);
   myputs(PSFile,PS_pbuf);
   /* Ask to Right Justify */
   sprintf(PS_pbuf,"%7.4f xunits 4 sub %f add rightJustifyText\n",X,offset);
   myputs(PSFile,PS_pbuf);

   UpdateLimits(X,Y,offset-strlen(SimplifyText(label))*TextHeight*0.65,-TextHeight);

   return(0);
}

/***************************************************************************/
/*>PSVText(double x, double y, double xoff,
           char *text,  char *font,  double size,
           char *label, char *lfont, double lsize)
   --------------------------------------------------------
   Write vertical text centred on x,y offset back along x by the size of
   label and by xoff in pts

   08.05.92 Original
   23.06.92 Added UpdateLimits()
   26.06.92 Changed strlen() to use SimplifyText()
   30.06.92 Removed redundant sprintf()'s
   15.09.92 Multiply string width by 0.65
*/
int PSVText(double x,          /* Data coord position (to be offset)           */
        double y,          /* Data coord position                          */
        double xoff,       /* X-offset in pts                              */
        char   *text,      /* Text to be written                           */
        char   *font,      /* Font in which to write it                    */
        double size,       /* Size of font                                 */
        char   *label,     /* Label to be used to calc x offset            */
        char   *lfont,     /* Font of this label                           */
        double lsize)      /* Size of this label                           */
{
   /* Put the text on the stack */
   sprintf(PS_pbuf,"(%s) dup\n",SimplifyText(text));
   myputs(PSFile,PS_pbuf);

   /* Find the length of the string/2 */
   myputs(PSFile,"stringwidth pop 2 div\n");

   /* Do specified y-pos minus strlen/2 */
   sprintf(PS_pbuf,"%7.4g yunits exch sub\n",y);
   myputs(PSFile,PS_pbuf);

   /* Set font to the offset label font */
   PSFont(lfont, lsize);
   
   /* Calculate the x-offset */
   sprintf(PS_pbuf,"%7.4g xunits (%s) stringwidth pop sub 5 sub %f add exch moveto\n",
           x,label,xoff);
   myputs(PSFile,PS_pbuf);

   /* Set font back */
   PSFont(font, size);
   
   /* Display the actual text */
   myputs(PSFile,"pop 90 rotate ");
   PSShowText(text);
   myputs(PSFile," -90 rotate\n");
   
   UpdateLimits(x,y,xoff-5.0-size-strlen(SimplifyText(label))*0.65*lsize,
                strlen(SimplifyText(text))*size*0.65/2.0);
   UpdateLimits(x,y,xoff-5.0-size-strlen(SimplifyText(label))*0.65*lsize,
               -strlen(SimplifyText(text))*size*0.65/2.0);

   return(0);
}

/***************************************************************************/
/*>PSLCText(double X, double Y, char *label)
   -----------------------------------------
   Left justify text, centred on Y
   08.05.92 Original
   23.06.92 Added UpdateLimits()
   26.06.92 Changed strlen() to use SimplifyText()
   30.06.92 Removed redundant sprintf()'s
   15.09.92 Multiply string width by 0.65
*/
int PSLCText(double X,
         double Y,
         char   *label)
{
   /* Define the current line and position */
   sprintf(PS_pbuf,"%7.4f xunits\n",X);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"/Line %7.4f yunits TextHeight 2 div sub 2 add def\n",Y);
   myputs(PSFile,PS_pbuf);
   myputs(PSFile,"Line moveto\n");
   /* Put the text into the file and display */
   PSShowText(label);

   UpdateLimits(X,Y,strlen(SimplifyText(label))*TextHeight*0.65,  TextHeight/2.0);
   UpdateLimits(X,Y,strlen(SimplifyText(label))*TextHeight*0.65, -TextHeight/2.0);

   return(0);
}

/***************************************************************************/
/*>PSShowText(char *text)
   ----------------------
   Displays text, processing it first if any control codes are found

   08.05.92 Original
*/
int PSShowText(char *text)
{
   char  buffer[MAXBUFF];
   int   i, j;
   
   /* Walk along the string */
   for(i=0, j=0; i<strlen(text) && j<MAXBUFF-1; i++)
   {
      switch(text[i])
      {
      case '\\':           /* Should interpret next character as Greek     */
         /* Finish the current string */
         if(j)
         {
            buffer[j] = '\0';
            sprintf(PS_pbuf,"(%s) show ",buffer);
            myputs(PSFile,PS_pbuf);
            j = 0;
         }
         /* Output the next character as Greek */
         sprintf(PS_pbuf,"(%c) greek ",text[++i]);
         myputs(PSFile,PS_pbuf);
         break;
      case '^':            /* Should raise next character                  */
         /* Finish the current string */
         if(j)
         {
            buffer[j] = '\0';
            sprintf(PS_pbuf,"(%s) show ",buffer);
            myputs(PSFile,PS_pbuf);
            j = 0;
         }
         /* If necessary build string */
         if(text[++i] == '{')
            while(text[++i] != '}' && text[i] != '\0' && j<MAXBUFF-1)
               buffer[j++] = text[i];
         else
            buffer[j++] = text[i];
         /* Output raised string */
         if(j)
         {
            buffer[j] = '\0';
            sprintf(PS_pbuf,"(%s) raise ",buffer);
            myputs(PSFile,PS_pbuf);
            j = 0;
         }
         break;
      case '_':            /* Should lower next character                  */
         /* Finish the current string */
         if(j)
         {
            buffer[j] = '\0';
            sprintf(PS_pbuf,"(%s) show ",buffer);
            myputs(PSFile,PS_pbuf);
            j = 0;
         }
         /* If necessary build string */
         if(text[++i] == '{')
            while(text[++i] != '}' && text[i] != '\0' && j<MAXBUFF-1)
               buffer[j++] = text[i];
         else
            buffer[j++] = text[i];
         /* Output lowered string */
         if(j)
         {
            buffer[j] = '\0';
            sprintf(PS_pbuf,"(%s) lower ",buffer);
            myputs(PSFile,PS_pbuf);
            j = 0;
         }
         break;
      case '(':            /* Need to insert a \, before falling through   */
      case ')':
         buffer[j++] = '\\';
      default:             /* An ordinary character                        */
         buffer[j++] = text[i];
         break;
      }
   }
   
   if(j)
   {
      buffer[j] = '\0';
      sprintf(PS_pbuf,"(%s) show ",buffer);
      myputs(PSFile,PS_pbuf);
      j = 0;
   }

   if(strlen(text)) myputs(PSFile,"\n");

   return(0);
}

/***************************************************************************/
/*>PSHatchBox(double x1, double y1, double x2, double y2, int pattern)
   -------------------------------------------------------------------
   Draws a hatched fill box
   23.06.92 Added UpdateLimits()
*/
int PSHatchBox(double x1, double y1, double x2, double y2, int pattern)
{
   /* First clear to white */
   myputs(PSFile,"1.000 setgray\n");
   myputs(PSFile,"newpath\n");
   PSMove(x1,y1);
   PSDraw(x1,y2);
   PSDraw(x2,y2);
   PSDraw(x2,y1);
   myputs(PSFile,"closepath\n");
   myputs(PSFile,"fill\n");
   
   /* Reset pen to black */
   myputs(PSFile,"0.000 setgray\n");

   /* Define the clipping path */
   myputs(PSFile,"gsave\n");
   myputs(PSFile,"newpath\n");
   PSMove(x1,y1);
   PSDraw(x1,y2);
   PSDraw(x2,y2);
   PSDraw(x2,y1);
   myputs(PSFile,"closepath\n");
   myputs(PSFile,"clip\n");
   
   /* Do the appropriate hatched fill */
   switch(pattern)
   {
   case 1:  sprintf(PS_pbuf,"%f %f %f %f upLines\n",x1,y1,x2,y2);
            myputs(PSFile, PS_pbuf);
            break;
   case 2:  sprintf(PS_pbuf,"%f %f %f %f downLines\n",x1,y1,x2,y2);
            myputs(PSFile, PS_pbuf);
            break;
   case 3:  sprintf(PS_pbuf,"%f %f %f %f horizLines\n",x1,y1,x2,y2);
            myputs(PSFile, PS_pbuf);
            break;
   case 4:  sprintf(PS_pbuf,"%f %f %f %f vertLines\n",x1,y1,x2,y2);
            myputs(PSFile, PS_pbuf);
            break;
   case 5:  sprintf(PS_pbuf,"%f %f %f %f upLines\n",x1,y1,x2,y2);
            myputs(PSFile, PS_pbuf);
            sprintf(PS_pbuf,"%f %f %f %f downLines\n",x1,y1,x2,y2);
            myputs(PSFile, PS_pbuf);
            break;
   case 6:  sprintf(PS_pbuf,"%f %f %f %f horizLines\n",x1,y1,x2,y2);
            myputs(PSFile, PS_pbuf);
            sprintf(PS_pbuf,"%f %f %f %f vertLines\n",x1,y1,x2,y2);
            myputs(PSFile, PS_pbuf);
            break;
   default:
            break;
   }
   myputs(PSFile,"grestore\n");

   UpdateLimits(x1,y1,0.0,0.0);
   UpdateLimits(x2,y2,0.0,0.0);

   return(0);
}

/***************************************************************************/
/*>PSArc(double xc, double yc, double rad, double start, double end)
   -----------------------------------------------------------------
   Draws a closed arc centred on xc, yc of radius rad between specified
   angles (in degrees).
   23.06.92 Original
*/
int PSArc(double xc, 
      double yc, 
      double rad, 
      double start, 
      double end)
{
   double x,
          y;
          
   myputs(PSFile,"newpath\n");

   /* Move to the centre of the arc */
   PSMove(xc,yc);

   /* Now draw the arc */
   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits %7.4f xscale %7.4f %7.4f arc\n",
           xc,yc,rad,start,end);
   myputs(PSFile,PS_pbuf);
      
   /* And close the path */
   myputs(PSFile,"closepath\n");
   
   UpdateLimits(xc,yc,0.0,0.0);

   /* Calc the outer ends of the arc and update */
   x = xc + rad * cos(PI*start/180.0);
   y = yc + rad * sin(PI*start/180.0);
   UpdateLimits(x,y,0.0,0.0);
   x = xc + rad * cos(PI*end/180.0);
   y = yc + rad * sin(PI*end/180.0);
   UpdateLimits(x,y,0.0,0.0);

   return(0);
}

/***************************************************************************/
/*>PSFixBoundingBox(struct Window *wind, char *filename)
   -----------------------------------------------------
   Reads in an EPSF PostScript file, updates the BoundingBox information
   using the stored data and rewrites the file. The window information
   is simply used to post error messages.
   23.06.92 Original
*/
#ifdef AMIGA
#include <exec/memory.h>
#else
#include <fcntl.h>
#endif

#ifdef AMIGA
int PSFixBoundingBox(struct Window *wind,
                 char *filename)
#else
int PSFixBoundingBox(int wind,
                 char *filename)
#endif
{
   FH *file = NULL;
   ULONG             filelen;
   UBYTE             *mem = NULL;
#ifndef AMIGA
   UBYTE             *memp = NULL;
   int               fh;
#endif
   

/*** Read the whole PostScript file into memory ***/
#ifdef AMIGA
   if((file = (FH *)Open(filename, MODE_OLDFILE)) == NULL)
#else
   if((file = fopen(filename, "r")) == NULL)
#endif
   {
      ReqMessage3(wind,"Unable to open file",filename,"to update BoundingBox",0);
      goto Cleanup;
   }

#ifdef AMIGA
   filelen = Seek((BPTR)file, Seek((BPTR)file, 0, OFFSET_END), OFFSET_BEGINNING);
#else
   fseek(file, 0L, SEEK_END);
   filelen = ftell(file);
#endif

#ifdef AMIGA
   if((mem = (UBYTE *)AllocMem(filelen, MEMF_CLEAR)) == NULL)
#else
   if((mem = (UBYTE *)malloc(filelen * sizeof(UBYTE))) == NULL)
#endif
   {
      ReqMessage2(wind,"Insufficient memory to","update BoundingBox",0);
      goto Cleanup;
   }

#ifdef AMIGA
   if(Read((BPTR)file, (void *)mem, filelen) != filelen)
   {
      ReqMessage3(wind,"Error in reading",filename,"to update BoundingBox",0);
      goto Cleanup;
   }
   Close((BPTR)file);
#else
   memp = mem;
   rewind(file);
   while((*(memp++) = fgetc(file))!=(UBYTE)EOF);
   fclose(file);
#endif

   file = NULL;

/*** Modify the BoundingBox ***/
   FixBBox((char *)mem, (int)filelen);

/*** Write the fixed PostScript file ***/
#ifdef AMIGA
   if((file = (FH *)Open(filename, MODE_NEWFILE)) == NULL)
#else
   if((fh = open(filename, O_CREAT|O_TRUNC)) == (-1))
#endif
   {
      ReqMessage3(wind,"Unable to create file",filename,"to update BoundingBox",0);
      goto Cleanup;
   }

#ifdef AMIGA
   if(Write((BPTR)file, (void *)mem, filelen) != filelen)
#else
   if(write(fh, (void *)mem, filelen) != filelen)
#endif
      ReqMessage3(wind,"Error in writing",filename,"to update BoundingBox",0);

Cleanup:
#ifdef AMIGA
   if(file)   Close((BPTR)file);
   if(mem)    FreeMem(mem, filelen);
#else
   if(file)   fclose(file);
   if(fh)     close(fh);
   if(mem)    free(mem);
#endif
   
   return(0);
}

/***************************************************************************/
/*>FixBBox(char *script, int filelen)
   ----------------------------------
   Does the actual work of fixing the BoundingBox information.
   23.06.92 Original
*/
static int FixBBox(char *script,
               int  filelen)
{
   char  *ptr,
         *ptr2,
         buffer[16],
         *bufp;
   int   xmin, xmax,
         ymin, ymax;
   
   /* Set mins and maxes from the globals */
   xmin = (int)floor(PSXMin);
   xmax = (int)ceil(PSXMax);
   ymin = (int)floor(PSYMin);
   ymax = (int)ceil(PSYMax);
   
   /* Search for the %%BoundingBox string */
   for(ptr = script; ptr < script+filelen-14; ptr++)
   {
      if(!strncmp(ptr, "%%BoundingBox:", 14))
      {
         ptr += 14;
         /* Clear up to end of line */
         for(ptr2 = ptr; *ptr2 != '\n'; ptr2++) *ptr2 = ' ';
         
         /* Copy in the mins and maxes */
         ptr++;
         
         sprintf(buffer," %d",xmin);
         for(bufp = buffer; *bufp && ptr<ptr2; bufp++, ptr++) *ptr = *bufp;
         
         sprintf(buffer," %d",ymin);
         for(bufp = buffer; *bufp && ptr<ptr2; bufp++, ptr++) *ptr = *bufp;
         
         sprintf(buffer," %d",xmax);
         for(bufp = buffer; *bufp && ptr<ptr2; bufp++, ptr++) *ptr = *bufp;
         
         sprintf(buffer," %d",ymax);
         for(bufp = buffer; *bufp && ptr<ptr2; bufp++, ptr++) *ptr = *bufp;
         
         break;
      }
   }
   return(0);
}

/***************************************************************************/
/*>UpdateLimits(double X, double Y, double XOff, double YOff)
   ----------------------------------------------------------
   Updates the static global PS plotting coordinate limits.
   23.06.92 Original
*/
static int UpdateLimits(double X,
                    double Y, 
                    double XOff, 
                    double YOff)
{
   /* Scale X and Y */
   X *= PSxpicsize * 72.0;
   Y *= PSypicsize * 72.0;

   /* Update the limits */
   if(X      < PSXMin) PSXMin = X;
   if(X      > PSXMax) PSXMax = X;
   if(Y      < PSYMin) PSYMin = Y;
   if(Y      > PSYMax) PSYMax = Y;
   if(X+XOff < PSXMin) PSXMin = X+XOff;
   if(X+XOff > PSXMax) PSXMax = X+XOff;
   if(Y+YOff < PSYMin) PSYMin = Y+YOff;
   if(Y+YOff > PSYMax) PSYMax = Y+YOff;

   return(0);
}

/***************************************************************************/
/*>PSTriangle(double x, double y, double dpsize)
   ---------------------------------------------
   29.06.92 Original
*/
int PSTriangle(double X, double Y, double size)
{
   size /= 2.0;
   
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits %f sub moveto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits        %7.4f yunits %f add lineto\n",
           X,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits %f sub lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);

   myputs(PSFile,"closepath\n");
   myputs(PSFile,"stroke\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSFillTriangle(double x, double y, double size)
   -----------------------------------------------
   29.06.92 Original
*/
int PSFillTriangle(double X, double Y, double size)
{
   size /= 2.0;
   
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits %f sub moveto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits        %7.4f yunits %f add lineto\n",
           X,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits %f sub lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);

   myputs(PSFile,"closepath\nfill\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSInvTriangle(double x, double y, double size)
   ----------------------------------------------
   29.06.92 Original
*/
int PSInvTriangle(double X, double Y, double size)
{
   size /= 2.0;
   
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits %f add moveto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits        %7.4f yunits %f sub lineto\n",
           X,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits %f add lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);

   myputs(PSFile,"closepath\n");
   myputs(PSFile,"stroke\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSFillInvTriangle(double x, double y, double size)
   --------------------------------------------------
   29.06.92 Original
*/
int PSFillInvTriangle(double X, double Y, double size)
{
   size /= 2.0;
   
   sprintf(PS_pbuf,"%7.4f xunits %f sub %7.4f yunits %f add moveto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits        %7.4f yunits %f sub lineto\n",
           X,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %f add %7.4f yunits %f add lineto\n",
           X,size,Y,size);
   myputs(PSFile,PS_pbuf);

   myputs(PSFile,"closepath\nfill\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSPlus(double x, double y, double size)
   ---------------------------------------
   29.06.92 Original
*/
int PSPlus(double X, double Y, double size)
{
   size /= 2.0;
   
   sprintf(PS_pbuf,"%7.4f xunits %7.4f sub %7.4f yunits moveto\n",
           X,size,Y);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %7.4f add %7.4f yunits lineto\n",
           X,size,Y);
   myputs(PSFile,PS_pbuf);

   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits %7.4f sub moveto\n",
           X,Y,size);
   myputs(PSFile,PS_pbuf);
   sprintf(PS_pbuf,"%7.4f xunits %7.4f yunits %7.4f add lineto\n",
           X,Y,size);
   myputs(PSFile,PS_pbuf);

   myputs(PSFile,"stroke\n");

   UpdateLimits(X,Y, size, size);
   UpdateLimits(X,Y,-size,-size);

   return(0);
}

/***************************************************************************/
/*>PSCTText(double X, double Y, double Offset, char *label)
   --------------------------------------------------------
   Centers a piece of text with X,Y being the Coords of the TOP centre point
   23.06.92 Added UpdateLimits()
   26.06.92 Changed strlen() to use SimplifyText()
   30.06.92 Removed redundant sprintf()'s
   01.07.92 Changed Offset to be in pts rather than a multiplier of 
            font size.
   27.07.92 Changed update limits to account better for descenders.
   15.09.92 Multiply string width by 0.65
*/
int PSCTText(double X,
         double Y,
         double Offset,
         char   *label)
{
   char *buffer;
   
   buffer = label;
   while(*buffer == ' ') buffer++;
   
   /* Put the string on the stack */
   sprintf(PS_pbuf,"(%s)\n",SimplifyText(buffer));
   myputs(PSFile,PS_pbuf);
   /* Define the current line */
   sprintf(PS_pbuf,
           "TextHeight %7.4f yunits exch sub /Line exch def\n",Y);
   myputs(PSFile,PS_pbuf);
   /* We are now left with label on the stack.
      Find half its width */
   sprintf(PS_pbuf,"dup stringwidth pop 2 div\n");
   myputs(PSFile,PS_pbuf);
   /* Put the X position on the stack, subtract width/2 */
   sprintf(PS_pbuf,"%7.4f xunits exch sub\n",X);
   myputs(PSFile,PS_pbuf);
   /* We are left with the label and correct X on the stack
      Put Y on, move there and show the text */
   if (Offset == 0.0)
   {
      myputs(PSFile,"Line moveto pop\n");
   }
   else
   {
      sprintf(PS_pbuf,"Line %7.4f add moveto pop\n",Offset);
      myputs(PSFile,PS_pbuf);
   }
   PSShowText(buffer);

   UpdateLimits(X,Y,-strlen(SimplifyText(buffer))*TextHeight*0.65/2.0,
                Offset-(double)1.5*TextHeight);
   UpdateLimits(X,Y, strlen(SimplifyText(buffer))*TextHeight*0.65/2.0,
                Offset);

   return(0);
}


