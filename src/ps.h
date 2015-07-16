/***************************************************************************

   Program:    
   File:       ps.h
   
   Version:    V1.10AMIGA
   Date:       06.02.91
   Function:   Include file for PostScript
   
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
   Include file for using with PSRoutines.c
   Variables are defined only from the main program. Otherwise they
   are referenced as external.

****************************************************************************

   Usage:
   ======
   Expects MAIN to be defined in the main program file
   Expects PSMAIN to be defined in PSRoutines.c

****************************************************************************

   Revision History:
   =================
   V1.7AMIGA   25.02.91
   Fixed prototypes and definition of PSFile.
   
   V1.10AMIGA  07.05.92
   Changed all prototypes to doubles

***************************************************************************/

#ifdef AMIGA
#include <exec/types.h>
#include <intuition/intuition.h>
#include <dos/dosextens.h>
#endif

#ifdef PSMAIN
   double PSxpicsize    = 5.0,
          PSypicsize    = 5.0,
          PSxoffset     = 1.0,
          PSyoffset     = 2.0,
          PSHatchSpace  = 2.0,
          PSHatchThick  = 0.5;
   int    EPSF          = 0;
   FH                   *PSFile;

#else /* Not PSMAIN */
   extern double  PSxpicsize,
                  PSypicsize,
                  PSxoffset,
                  PSyoffset,
                  PSHatchSpace,
                  PSHatchThick;
   extern int     EPSF;
   extern FH      *PSFile;
#endif /* !PSMAIN */

/* Prototypes for functions defined in psroutines.c */

int PSInit(char *FName,
           char *creator,
           double EPSFxoff,
           double EPSFyoff,
           char *AltFont);
int PSCross(double X,
            double Y,
            double size);
int PSBox(double X,
          double Y,
          double size);
int PSFillBox(double X,
              double Y,
              double size);
int PSCircle(double X,
             double Y,
             double size);
int PSFillCircle(double X,
                 double Y,
                 double size);
int PSDiamond(double X,
              double Y,
              double size);
int PSFillDiamond(double X,
                  double Y,
                  double size);
int PSMove(double X,
           double Y);
int PSDraw(double X,
           double Y);
int PSDrawRel(double X,
              double Y);
int PSStroke(void);
int PSEnd(void);
int PSRText(double X,
            double Y,
            char *label);
int PSRTText(double X,
             double Y,
             char *label);
int PSLText(double X,
            double Y,
            char *label);
int PSLTText(double X,
             double Y,
             char *label);
int PSRText2(double X1,
             double X,
             double Y1,
             double Y,
             char *scale,
             char *label);
int PSRTText2(double X1,
              double X,
              double Y1,
              double Y,
              char *scale,
              char *label);
int PSLText2(double X1,
             double X,
             double Y1,
             double Y,
             char *scale,
             char *label);
int PSLTText2(double X1,
              double X,
              double Y1,
              double Y,
              char *scale,
              char *label);
int PSCTText(double X,
             double Y,
             double Offset,
             char *label);
int PSCBText(double X,
             double Y,
             double Offset,
             char *label);
char *correctcase(char *font);
int PSFont(char *fontname,
           double size);
int PSSetDash(char *linepatt);
int PSClearDash(void);
int PSThick(double thickness);
int PSROffText(double X,
               double Y,
               double offset,
               char   *label);
int PSShowText(char *text);
int PSHatchBox(double x1, 
               double y1, 
               double x2, 
               double y2, 
               int pattern);
int PSLCText(double X,
             double Y,
             char   *label);
int PSVText(double   x, 
            double   y, 
            double   xoff,
            char     *text,  
            char     *font,  
            double   size,
            char     *label, 
            char     *lfont, 
            double   lsize);
int PSTriangle(double X, double Y, double size);
int PSFillTriangle(double X, double Y, double size);
int PSInvTriangle(double X, double Y, double size);
int PSFillInvTriangle(double X, double Y, double size);
int PSPlus(double X, double Y, double size);
int PSArc(double xc, double yc, double rad, double start, double end);

