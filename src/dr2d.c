/****************************************************************************

   Program:    
   File:       DR2D.c
   
   Version:    V1.0
   Date:       15.07.92
   Function:   Routines to implement DR2D output
   
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

*****************************************************************************

   Usage:
   ======

*****************************************************************************

   Revision History:
   =================

****************************************************************************/
/* Includes
*/
#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <libraries/iffparse.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/iffparse_protos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef AMIGA
#include "m:macros.h"
#else
#include "bioplib/macros.h"
#endif
#include "DR2D.h" /* Structure and Chunk ID defines */

#ifndef DOPROTOS
#include "dr2d.p"
#include "iffutil.p"
#endif

extern char *SimplifyText(char *);

/***************************************************************************/
/* The IFF Parse library
*/
extern struct Library *IFFParseBase;

/***************************************************************************/
#define MAXDR2DFONTS  10
#define MAXBUFF      240

/***************************************************************************/
static IEEE       *path          = NULL;  /* Store the current path        */
static USHORT     PathLen        = 0,     /* Length of the path so far     */
                  MaxPath        = 0;     /* Size of path array            */
static struct IFFHandle  *iff    = NULL;  /* The IFF context handle        */
static long       IFFERROR       = 0;     /* The IFF error number          */
static char **KnownDR2DFonts = NULL;      /* Array of known font names     */
static int        NumDR2DFonts   = 0,     /* Number of knwon font names    */
                  CurrentDR2DFont= 0,     /* Current font                  */
                  CurDR2DDash    = 1,     /* Current dash pattern          */
                  CurDR2DColour  = 6,     /* Current colour                */
                  CurDR2DFill    = 0,     /* Current fill status           */
                  fonsnum        = 0,     /* Font number for PutFONS()     */
                  AltDR2DFont    = 0;     /* Alternate `Greek' font        */
static IEEE       DR2DFontx      = 0.007, /* Current font dimensions       */
                  DR2DFonty      = 0.014,
                  CurDR2DThick   = 0.014; /* Current line thickness        */

/***************************************************************************/
/*>InitDR2D(char *filename, double xmin, double xmax, 
            double ymin, double ymax, char *AltFontName)
   -----------------------------------------------------
   Initialiase an IFF-DR2D file,
   Returns  0 if OK, 
            1 if unable to start IFF,
            2 if unable to allocate memory.
            
   15.07.92 Original to use iffparse.library
   16.07.92 Added LAYR chunk. Removed nested FORM. Does restart automatically
            if the file is already active.
   17.07.92 Corrected BBOX chunk.
   20.07.92 Added alternate `Greek' font setup. Fixed restart.
*/
int InitDR2D(char     *filename,
         double   xmin,
         double   xmax,
         double   ymin,
         double   ymax,
         char     *AltFontName)
{
   DRHDstruct     header;
   BBOXstruct     bbox;
   BYTE           cmap[21] = {255, 255, 255,
                              213, 213, 213,
                              170, 170, 170,
                              128, 128, 128,
                               86,  86,  86,
                               42,  42,  42,
                                0,   0,   0};
   static double  xmin_l,  ymin_l,
                  xmax_l,  ymax_l;
   int            restart,
                  trycount;
   
   restart = (iff) ? 1 : 0;

   if(restart)
   {
      /* End the IFF file */
      EndDR2D();
      
      /* It seems we need to wait a bit! */
      Delay(50);

      /* We use the bounds stored in a non-restart call */
      xmin = xmin_l;
      ymin = ymin_l;
      xmax = xmax_l;
      ymax = ymax_l;
   }
   else
   {
      /* Store the bounds ready for a restart call */
      xmin_l = xmin;
      ymin_l = ymin;
      xmax_l = xmax;
      ymax_l = ymax;
   }

   /* Start IFF for writing a file. We'll try 10 times */
   for(trycount=0; trycount<10 && iff==NULL; trycount++)
      iff = StartIFF(filename, IFFF_WRITE);
      
   /* Return 1, if we still fail */
   if(iff==NULL)
      return(1);

   /* Reset default current values */
   CurrentDR2DFont= 0;        /* Current font                  */
   CurDR2DDash    = 1;        /* Current dash pattern          */
   CurDR2DColour  = 6;        /* Current colour                */
   CurDR2DFill    = 0;        /* Current fill status           */
   DR2DFontx      = 0.007;    /* Current font dimensions       */
   DR2DFonty      = 0.014;
   CurDR2DThick   = 0.014;    /* Current line thickness        */

   /* Find alternate font number */
   if((AltDR2DFont = FindDR2DFont(AltFontName)) == -1)
      AltDR2DFont = CurrentDR2DFont;

   /* Initialise path for 100 coords */
   if(path) free(path);
   MaxPath = 100;
   PathLen = 0;
   if((path = (IEEE *)malloc(MaxPath * 2 * sizeof(IEEE))) == NULL)
      return(2);

   /* Start the outer DR2D form */
   IFFERROR=PushChunk(iff, ID_DR2D, ID_FORM, IFFSIZE_UNKNOWN);

   /* Write the drawing header */
   if(!IFFERROR)
   {
      header.XLeft   = (IEEE)xmin;
      header.XRight  = (IEEE)xmax;
      header.YTop    = (IEEE)ymax;
      header.YBot    = (IEEE)ymin;
      IFFERROR       = PutChunk(iff, ID_DRHD, sizeof(header), &header);
   }
   
   /* Write a b&w colour map */
   if(!IFFERROR)     IFFERROR = PutChunk(iff, ID_CMAP, 21, cmap);

   /* Define known fonts */
   if(!IFFERROR)     IFFERROR = DefineDR2DFonts();

   /* Define line types */
   if(!IFFERROR)     IFFERROR = DefineDashes();
   
   /* Define a layer */
   if(!IFFERROR)     IFFERROR = DefineLayer(0, "Base");
   
   /* Set the default attributes                  Dash  Fill  col   thick  */
   if(!IFFERROR)     IFFERROR = SetDR2DAttributes(1,    0,    0,    (IEEE)1.0);
   
   /* Set the bounding box. */
   bbox.XMin = xmin;
   bbox.YMin = ymin;
   bbox.XMax = xmax;
   bbox.YMax = ymax;
   if(!IFFERROR)     IFFERROR = PutChunk(iff, ID_BBOX, sizeof(bbox), &bbox);
   
   return(IFFERROR);

}
/***************************************************************************/
/*>double DR2DStrWidth(char *string, int fontnum, double size)
   -----------------------------------------------------------
   This routine is a kludge to return the width of a string in inches.
   The font size is in *inches* not pts.
   This version will only work 100% correctly with non-proportional fonts.
   TODO: Modify to read the IFF-OFNT FORM and calculate the exact width.
   17.07.92 Original kludge version
*/
double DR2DStrWidth(char *string, int fontnum, double size)
{
   int nchar;
   double   strwidth = 0.0;
   
   if(fontnum > -1)
   {
      nchar = strlen(SimplifyText(string));
      /* Halve the size (height) for the width */
      strwidth = (double)nchar * size / 2.0; 
   }

   return(strwidth);
}

/***************************************************************************/
/*>DefineDR2DFonts(void)
   ---------------------
   Writes FONS chunks for each known font type (defined by UseDR2DFont())

   15.07.92 Original
   17.07.92 Added initialisation of fonsnum
*/
int DefineDR2DFonts(void)
{
   long  iff_error = 0;
   int   j;

   if(KnownDR2DFonts)
   {
      fonsnum = 0;
      
      for(j=0; j<NumDR2DFonts; j++)
      {
         if(!iff_error && KnownDR2DFonts[j])
            iff_error = PutFONS(KnownDR2DFonts[j]);
      }
   }
   
   return(iff_error);
}
   
/***************************************************************************/
/*>UseDR2DFont(char *fontname)
   ---------------------------
   Used by the calling program to build a table of fonts which will be
   written to the DR2D header.
   Returns 0 if OK
           1 if error

   15.07.92 Original
*/
int UseDR2DFont(char *fontname)
{
   int known = FALSE,
       j;
       
   /* Allocate array of pointers if needed */
   if(!KnownDR2DFonts)
      KnownDR2DFonts = (char **)malloc(MAXDR2DFONTS * sizeof(char *));
   if(!KnownDR2DFonts) return(1);
   
   /* See if this font has already been defined */
   for(j=0; j<NumDR2DFonts; j++)
   {
      if(!strcmp(KnownDR2DFonts[j],fontname))
      {
         known = TRUE;
         break;
      }
   }
   
   /* If the font hasn't already been defined */
   if(!known)
   {
      if(NumDR2DFonts >= MAXDR2DFONTS) /* Too many fonts */
         return(1);
      
      /* Allocate space for this fontname */
      KnownDR2DFonts[NumDR2DFonts] =
         (char *)malloc((2+strlen(fontname)) * sizeof(char));
      
      /* Return if no memory, else copy in fontname */
      if(KnownDR2DFonts[NumDR2DFonts] == NULL)
         return(1);
      else
         strcpy(KnownDR2DFonts[NumDR2DFonts++], fontname);
   }
   
   return(0);
}

/***************************************************************************/
/*>PutFONS(char *Font)
   -------------------
   Write a font chunk.
   15.07.92 Original
   16.07.92 Changed to number font chunks from 0
*/
int PutFONS(char *Font)
{
   FONSstruct  fons;
   
   fons.FontID       = fonsnum++;
   fons.Pad1         = 0;
   fons.Proportional = 0;
   fons.Serif        = 0;
   
   IFFERROR=PushChunk(iff, 0, ID_FONS, IFFSIZE_UNKNOWN);
   
   /* Now the FONT header */
   if(!IFFERROR)
   {
      if(WriteChunkBytes(iff, &fons, sizeof(fons)) != sizeof(fons))
         IFFERROR = IFFERR_WRITE;
   }
   /* Now the font name with terminating NULL */
   if(!IFFERROR)
   {
      if(WriteChunkBytes(iff, Font, strlen(Font)+1) != strlen(Font)+1)
         IFFERROR = IFFERR_WRITE;
   }

   /* Now finish the chunk */
   if(!IFFERROR)
      IFFERROR = PopChunk(iff);

   return(IFFERROR);
}

/***************************************************************************/
int SetDR2DAttributes(int dash, int fill, int colour, IEEE thick)
{
   ATTRstruct attr;
   
   attr.FillType     = (UBYTE)  ((fill) ? FT_COLOR : FT_NONE);
   attr.JoinType     = (UBYTE)  JT_MITER;
   attr.DashPattern  = (UBYTE)  dash;
   attr.ArrowHead    = (UBYTE)  0;
   attr.FillValue    = (USHORT) ((fill) ? colour   : 0);
   attr.EdgeValue    = (USHORT) colour;
   attr.WhichLayer   = (USHORT) 0;
   attr.EdgeThick    = (IEEE)   thick;
   
   IFFERROR=PutChunk(iff, ID_ATTR, sizeof(attr), (void *)&attr);

   return(IFFERROR);
}

/***************************************************************************/
int DefineDashes(void)
{
   IEEE        Dashes[8];
   
   /* Solid line  */
   if(!IFFERROR)  IFFERROR = PutDashDef(1, 0, Dashes);

   /* ............ */
   Dashes[0] = 2.0;
   Dashes[1] = 2.0;
   if(!IFFERROR)  IFFERROR = PutDashDef(2, 2, Dashes);
   
   /* -.-.-.-.-.-. */
   Dashes[0] = 2.0;
   Dashes[2] = 4.0;
   Dashes[3] = 8.0;
   Dashes[4] = 4.0;
   if(!IFFERROR)  IFFERROR = PutDashDef(3, 4, Dashes);
   
   /* ------------ */
   Dashes[0] = 8.0;
   Dashes[1] = 4.0;
   if(!IFFERROR)  IFFERROR = PutDashDef(4, 2, Dashes);
   
   /* -..-..-..-.. */
   Dashes[0] = 8.0;
   Dashes[1] = 2.0;
   Dashes[2] = 2.0;
   Dashes[3] = 2.0;
   Dashes[4] = 2.0;
   Dashes[5] = 2.0;
   if(!IFFERROR)  IFFERROR = PutDashDef(5, 6, Dashes);
   
   /* --.--.--.--. */
   Dashes[0] = 8.0;
   Dashes[1] = 2.0;
   Dashes[2] = 8.0;
   Dashes[3] = 2.0;
   Dashes[4] = 2.0;
   Dashes[5] = 2.0;
   if(!IFFERROR)  IFFERROR = PutDashDef(6, 6, Dashes);
   
   return(IFFERROR);
}



/***************************************************************************/
/*>PutDashDef(int id, int ndash, IEEE *dashes)
   -------------------------------------------
   Write a Line dash pattern definitions chunk
   15.07.92 Original
*/
int PutDashDef(int id, int ndash, IEEE *dashes)
{
   DASHstruct  dash;
   
   dash.DashID    = id;
   dash.NumDashes = ndash;
   
   IFFERROR=PushChunk(iff, 0, ID_DASH, IFFSIZE_UNKNOWN);
   
   /* Now the DASH header */
   if(!IFFERROR)
   {
      if(WriteChunkBytes(iff, &dash, sizeof(DASHstruct)) 
         != sizeof(DASHstruct))
         IFFERROR = IFFERR_WRITE;
   }
   
   /* Now the actual dash pattern data */
   if(!IFFERROR && dash.NumDashes)
   {
      if(WriteChunkBytes(iff, (BYTE *)dashes, dash.NumDashes * sizeof(IEEE))
         != dash.NumDashes * sizeof(IEEE))
         IFFERROR = IFFERR_WRITE;
   }

   /* Now finish the chunk */
   if(!IFFERROR)
      IFFERROR = PopChunk(iff);
   
   return(IFFERROR);
}


/***************************************************************************/
/*>EndDR2D(void)
   -------------
   Clean up and flush an IFF-DR2D file
   15.07.92 Original
   16.07.92 Removed nested FORM. Added iff=NULL
*/
void EndDR2D(void)
{
   /* Close the outer DR2D forms */
   if(!IFFERROR)  IFFERROR = PopChunk(iff);

   /* Free memory for the drawing path */
   if(path)
   {
      free(path);
      path = NULL;
      PathLen = 0;
      MaxPath = 0;
   }

   /* End the IFF file */
   EndIFF(iff);
   
   iff = NULL;
}

/***************************************************************************/
/*>DR2DMove(double x, double y)
   ----------------------------
   Do a move to
   04.06.92 Original
*/
int DR2DMove(double            x,
         double            y)
{
   if(PathLen > 0)   /* Flush the current path */
      DR2DStroke();
   
   if(path == NULL)
   {
      IFFERROR = IFFERR_NOMEM;
   }
   else
   {
      path[0] = (IEEE)x;
      path[1] = (IEEE)y;
      PathLen = 1;
   }
   
   return(IFFERROR);   
}

/***************************************************************************/
/*>DR2DDraw(double x, double y)
   ----------------------------
   Do a draw to
   04.06.92 Original
*/
int DR2DDraw(double            x,
         double            y)
{
   if(PathLen > MaxPath)
   {
      /* Lengthen the path array */
      MaxPath += 100;
      if((path = (IEEE *)realloc(path, MaxPath * 2 * sizeof(IEEE))) == NULL)
         IFFERROR = IFFERR_NOMEM;
   }
   
   path[2*PathLen]     = (IEEE)x;
   path[2*PathLen + 1] = (IEEE)y;
   PathLen++;
   
   return(IFFERROR);   
}

/***************************************************************************/
/*>DR2DStroke(void)
   ----------------
   Flush the current path
   04.06.92 Original
   15.07.92 Rewrite for iffparse
*/
int DR2DStroke(void)
{
   IFFERROR=PushChunk(iff, 0, ID_OPLY, IFFSIZE_UNKNOWN);
   
   /* Write the number of points */
   if(!IFFERROR)
   {
      if(WriteChunkBytes(iff, (UBYTE *)&PathLen, sizeof(PathLen))
         != sizeof(PathLen))
         IFFERROR = IFFERR_WRITE;
   }
   /* Now the path array (length: 2(x&y) * 4(bytes) * PathLen) */
   if(!IFFERROR)
   {
      if(WriteChunkBytes(iff, (UBYTE *)path, 2 * PathLen * sizeof(path[0])) 
         != 2 * PathLen * sizeof(path[0]))
         IFFERROR = IFFERR_WRITE;
   }

   /* Now finish the chunk */
   if(!IFFERROR)
      IFFERROR = PopChunk(iff);

   /* Clear the path */
   PathLen = 0;

   return(IFFERROR);
}

/***************************************************************************/
/*>DR2DCloseStroke(void)
   ---------------------
   Close and Flush the current path
   16.07.92 Original
*/
int DR2DCloseStroke(void)
{
   IFFERROR=PushChunk(iff, 0, ID_CPLY, IFFSIZE_UNKNOWN);
   
   /* Write the number of points */
   if(!IFFERROR)
   {
      if(WriteChunkBytes(iff, (UBYTE *)&PathLen, sizeof(PathLen))
         != sizeof(PathLen))
         IFFERROR = IFFERR_WRITE;
   }
   /* Now the path array (length: 2(x&y) * 4(bytes) * PathLen) */
   if(!IFFERROR)
   {
      if(WriteChunkBytes(iff, (UBYTE *)path, 2 * PathLen * sizeof(path[0])) 
         != 2 * PathLen * sizeof(path[0]))
         IFFERROR = IFFERR_WRITE;
   }

   /* Now finish the chunk */
   if(!IFFERROR)
      IFFERROR = PopChunk(iff);

   /* Clear the path */
   PathLen = 0;

   return(IFFERROR);
}

/***************************************************************************/
/*>DR2DThick(double thick)
   -----------------------
   Set linethickness
   15.07.92 Original
   16.07.92 Dimension is inches so /72.0
*/
int DR2DThick(double thick)
{
   thick /= 72.0;
   
   CurDR2DThick = (IEEE)thick;
   if(!IFFERROR)
      SetDR2DAttributes(CurDR2DDash, CurDR2DFill, CurDR2DColour, CurDR2DThick);
   return(0);
}

/***************************************************************************/
/*>DR2DSetDash(int pattern)
   ------------------------
   Set line pattern
   15.07.92 Original
   17.07.92 Corrected to bump pattern number so solid line is 0
*/
int DR2DSetDash(int pattern)
{
   CurDR2DDash = pattern + 1;
   if(!IFFERROR)
      SetDR2DAttributes(CurDR2DDash, CurDR2DFill, CurDR2DColour, CurDR2DThick);
   return(0);
}

/***************************************************************************/
/*>DR2DFillRect(double x1, double y1, double x2, double y2,
                int pattern, int hatch, double spacing, double hatchthick)
   -----------------------------------------------------------------------
   Draw a filled rectangle
   15.07.92 Original
   16.07.92 Removed nested FORMs for fills. Changed to DR2DCloseStroke().
            Added hatchthick parameter.
   28.07.92 Now fill in white before hatching. Also check IFFERROR for
            shade fills
*/
int DR2DFillRect(double x1, 
             double y1, 
             double x2, 
             double y2,
             int    pattern, 
             int    hatch, 
             double spacing,
             double hatchthick)

{
   double   step;
   int      x1p, y1p,
            x2p, y2p;
   
   if(x2<x1)
   {
      double temp = x1;

      x1 = x2;
      x2 = temp;
   }
   
   if(y2<y1)
   {
      double temp = y1;

      y1 = y2;
      y2 = temp;
   }
   
   if(hatch)
   {
      spacing    /= 72.0;     /* Convert to inches */
      hatchthick /= 72.0;

      /* Set the fill pattern to clear */
      if(!IFFERROR)
         SetDR2DAttributes(CurDR2DDash, 1, 0, CurDR2DThick);
   
      /* Draw the box and fill it in white */
      DR2DMove(x1,y1);
      DR2DDraw(x2,y1);
      DR2DDraw(x2,y2);
      DR2DDraw(x1,y2);
      DR2DCloseStroke();

      if(!IFFERROR)
         SetDR2DAttributes(CurDR2DDash, CurDR2DFill, 
                           CurDR2DColour, (IEEE)hatchthick);

      switch(pattern)
      {
      case 0:
         break;
      case 1:
         for(step = y1 - 2*(x2-x1); step < y2; step += spacing)
         {
            x1p = (int)(10000 * x1);
            x2p = (int)(10000 * x2);
            y1p = (int)(10000 * step);
            y2p = y1p + (int)(10000 * (x2-x1));
            
            if(!CSClip(&x1p,&y1p,&x2p,&y2p,
                       (int)(10000 * x1), (int)(10000 * y1),
                       (int)(10000 * x2), (int)(10000 * y2)))
            {
               DR2DMove((double)x1p/10000.0, (double)y1p/10000.0);
               DR2DDraw((double)x2p/10000.0, (double)y2p/10000.0);
               DR2DStroke();
            }
         }
         break;
      case 2:
         for(step = y1; step < y2 + 2*(x2-x1); step += spacing)
         {
            x1p = (int)(10000 * x1);
            x2p = (int)(10000 * x2);
            y1p = (int)(10000 * step);
            y2p = y1p - (int)(10000 * (x2-x1));
            
            if(!CSClip(&x1p,&y1p,&x2p,&y2p,
                       (int)(10000 * x1), (int)(10000 * y1),
                       (int)(10000 * x2), (int)(10000 * y2)))
            {
               DR2DMove((double)x1p/10000.0, (double)y1p/10000.0);
               DR2DDraw((double)x2p/10000.0, (double)y2p/10000.0);
               DR2DStroke();
            }
         }
         break;
      case 3:
         for(step = y1; step < y2; step += spacing)
         {
            DR2DMove(x1,step);
            DR2DDraw(x2,step);
            DR2DStroke();
         }
         break;
      case 4:
         for(step = x1; step < x2; step += spacing)
         {
            DR2DMove(step,y1);
            DR2DDraw(step,y2);
            DR2DStroke();
         }
         break;
      case 5:
         for(step = y1; step < y2; step += spacing)
         {
            DR2DMove(x1,step);
            DR2DDraw(x2,step);
            DR2DStroke();
         }
         for(step = x1; step < x2; step += spacing)
         {
            DR2DMove(step,y1);
            DR2DDraw(step,y2);
            DR2DStroke();
         }
         break;
      case 6:
         for(step = y1 - 2*(x2-x1); step < y2; step += spacing)
         {
            x1p = (int)(10000 * x1);
            x2p = (int)(10000 * x2);
            y1p = (int)(10000 * step);
            y2p = y1p + (int)(10000 * (x2-x1));
            
            if(!CSClip(&x1p,&y1p,&x2p,&y2p,
                       (int)(10000 * x1), (int)(10000 * y1),
                       (int)(10000 * x2), (int)(10000 * y2)))
            {
               DR2DMove((double)x1p/10000.0, (double)y1p/10000.0);
               DR2DDraw((double)x2p/10000.0, (double)y2p/10000.0);
               DR2DStroke();
            }
         }
         for(step = y1; step < y2 + 2*(x2-x1); step += spacing)
         {
            x1p = (int)(10000 * x1);
            x2p = (int)(10000 * x2);
            y1p = (int)(10000 * step);
            y2p = y1p - (int)(10000 * (x2-x1));
            
            if(!CSClip(&x1p,&y1p,&x2p,&y2p,
                       (int)(10000 * x1), (int)(10000 * y1),
                       (int)(10000 * x2), (int)(10000 * y2)))
            {
               DR2DMove((double)x1p/10000.0, (double)y1p/10000.0);
               DR2DDraw((double)x2p/10000.0, (double)y2p/10000.0);
               DR2DStroke();
            }
         }
         break;
      default:
         break;
      }

      if(!IFFERROR)
         SetDR2DAttributes(CurDR2DDash, CurDR2DFill, 
                           CurDR2DColour, CurDR2DThick);

   }
   else
   {
      /* Set the fill pattern */
      if(!IFFERROR)
         SetDR2DAttributes(CurDR2DDash, 1, RANGECHECK(pattern,0,6), 
                           CurDR2DThick);
   
      /* Draw the box */
      DR2DMove(x1,y1);
      DR2DDraw(x2,y1);
      DR2DDraw(x2,y2);
      DR2DDraw(x1,y2);
      DR2DCloseStroke();

      /* Reset fill pattern */
      if(!IFFERROR)
         SetDR2DAttributes(CurDR2DDash, CurDR2DFill, CurDR2DColour, 
                           CurDR2DThick);
   }

   return(0);
}

/***************************************************************************/
/*>DR2DSetFont(char *Font, double size)
   ------------------------------------
   Set the font.

   15.07.92 Original
   17.07.92 Changed to use FindDR2DFont()
*/
int DR2DSetFont(char     *Font,
            double   size)
{
   int fontnum;
   
   fontnum = FindDR2DFont(Font);
   
   if(fontnum > -1)
   {
      CurrentDR2DFont = fontnum;
      DR2DFonty       = (IEEE)size/72.0;
      DR2DFontx       = (IEEE)size/144.0;
   }
   
   return(IFFERROR);
}

/***************************************************************************/
/*>FindDR2DFont(char *font)
   ------------------------
   Finds a font (by name) in the font list and returns the font table number.
   Returns -1 if font not found.
   17.07.92 Original
*/
int FindDR2DFont(char *font)
{
   int j,
       known = FALSE;

   if(KnownDR2DFonts)
   {
      for(j=0; j<NumDR2DFonts; j++)
      {
         if(!strcmp(KnownDR2DFonts[j],font))
         {
            known = TRUE;
            break;
         }
      }
   }
   
   if(known)   return(j);
   else        return(-1);
}

/***************************************************************************/
/*>DefineLayer(int id, char *name)
   -------------------------------
   Write a layer chunk
   16.07.92 Original
*/
int DefineLayer(int id, char *name)
{
   LAYRstruct  layer;
   int         j;
   
   layer.LayerID  = id;                                  /* Layer ID       */
   strcpy(layer.LayerName, name);                        /* Padded name    */
   for(j=strlen(name); j<16; j++)
      layer.LayerName[j] = '\0';
   layer.Flags    = LF_ACTIVE | LF_DISPLAYED;            /* Flags          */
   layer.Pad0     = (UBYTE)0;                            /* Pad byte       */
   
   IFFERROR=PushChunk(iff, 0, ID_LAYR, IFFSIZE_UNKNOWN);
   
   /* Now the LAYR structure */
   if(!IFFERROR)
   {
      if(WriteChunkBytes(iff, &layer, sizeof(LAYRstruct)) 
         != sizeof(LAYRstruct))
         IFFERROR = IFFERR_WRITE;
   }
   
   /* Now finish the chunk */
   if(!IFFERROR)
      IFFERROR = PopChunk(iff);
   
   return(IFFERROR);
}

/***************************************************************************/
/*>DR2DLText(double x, double y, char *string)
   -------------------------------------------
   Left bottom justified text
   16.07.92 Original
   20.07.92 Changed to use DR2DShowText()
*/
int DR2DLText(double x,
          double y, 
          char   *string)
{
   DR2DShowText(string, 0, x, y);

   return(IFFERROR);
}
/***************************************************************************/
/*>DR2DCTText(double x, double y, double offset, char *text)
   ---------------------------------------------------------
   Center Top justify text at x,y with y-offset in pts.
   16.07.92 Original
   17.07.92 Changed to use DR2DStrWidth()
   20.07.92 Changed to use DR2DShowText()
*/
int DR2DCTText(double x, 
           double y, 
           double offset, 
           char   *text)
{
   offset/=72.0;
   
   x -= DR2DStrWidth(text, CurrentDR2DFont, DR2DFonty) / 2.0;
   y -= DR2DFonty - offset;
   
   DR2DShowText(text, 0, x, y);
      
   return(IFFERROR);
}

/***************************************************************************/
/*>DR2DCBText(double x, double y, char *string)
   --------------------------------------------
   Centre bottom justified text
   16.07.92 Original
   17.07.92 Changed to use DR2DStrWidth()
   20.07.92 Changed to use DR2DShowText()
*/
int DR2DCBText(double x, 
           double y, 
           char   *string)
{
   x -= DR2DStrWidth(string, CurrentDR2DFont, DR2DFonty) / 2.0;
   DR2DShowText(string, 0, x, y);

   return(IFFERROR);

}
/***************************************************************************/
/*>DR2DLCText(double x, double y, char *string)
   --------------------------------------------
   Left centre height justified text
   16.07.92 Original
   20.07.92 Changed to use DR2DShowText()
*/
int DR2DLCText(double x, 
           double y, 
           char   *string)
{
   y -= DR2DFonty/3.0;
   DR2DShowText(string, 0, x, y);

   return(IFFERROR);
}
/***************************************************************************/
/*>DR2DVText(double x, double y, double xoff, char *text, 
             char *TitleFont, double TitleSize, 
             char *label, char *LabelFont, double LabelSize)
   ---------------------------------------------------------
   Vertical text centred on x,y offset along x by xoff and the size of label
   16.07.92 Original
   17.07.92 Corrected label offset calc. Changed to use DR2DStrWidth()
   20.07.92 Changed to use DR2DShowText()
*/
int DR2DVText(double x, 
          double y, 
          double xoff, 
          char   *text, 
          char   *TitleFont, 
          double TitleSize, 
          char   *label, 
          char   *LabelFont, 
          double LabelSize)
{
   xoff      /= 72.0;
   LabelSize /= 72.0;
   
   x += xoff - DR2DStrWidth(label, FindDR2DFont(LabelFont), LabelSize);
   y -= strlen(SimplifyText(text)) * DR2DFontx / 2.0;
   DR2DShowText(text, 1, x, y);
   
   return(IFFERROR);
}

/***************************************************************************/
/*>DR2DArc(double xc, double yc, double dlength, double startangle, 
           double endangle, int pattern, int hatch, double hatchstep,
           double hatchthick)
   ------------------------------------------------------------------
   Draws a (filled) arc.
   16.07.92 Original (based on HPGL version)
*/
int DR2DArc(double xc, 
        double yc, 
        double dlength, 
        double startangle, 
        double endangle, 
        int    pattern,
        int    hatch,
        double hatchstep,
        double hatchthick)
{
   double x,
          y,
          angle,
          yoff,
          x1,  y1,
          x2,  y2;
   
   if(hatch)
   {
      hatchstep   /= 72.0;
      hatchthick  /= 72.0;

      SetDR2DAttributes(CurDR2DDash, CurDR2DFill, CurDR2DColour, (IEEE)hatchthick);

      switch(pattern)
      {
      case 1:
         for(yoff = -2.0 * dlength; yoff < 2.0 * dlength; yoff += hatchstep)
         {
            if(WedgeClip(xc,yc,dlength,1.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
            {
               DR2DMove(x1,y1);
               DR2DDraw(x2,y2);
               DR2DStroke();
            }
         }
         break;
      case 2:
         for(yoff = -2.0 * dlength; yoff < 2.0 * dlength; yoff += hatchstep)
         {
            if(WedgeClip(xc,yc,dlength,-1.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
            {
               DR2DMove(x1,y1);
               DR2DDraw(x2,y2);
               DR2DStroke();
            }
         }
         break;
      case 3:
         for(yoff = -2.0 * dlength; yoff < 2.0 * dlength; yoff += hatchstep)
         {
            if(WedgeClip(xc,yc,dlength,0.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
            {
               DR2DMove(x1,y1);
               DR2DDraw(x2,y2);
               DR2DStroke();
            }
         }
         break;
      case 4:
         for(yoff = -2.0 * dlength; yoff < 2.0 * dlength; yoff += hatchstep)
         {
            if(WedgeClip(xc,yc,dlength,-9999.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
            {
               DR2DMove(x1,y1);
               DR2DDraw(x2,y2);
               DR2DStroke();
            }
         }
         break;
      case 5:
         for(yoff = -2.0 * dlength; yoff < 2.0 * dlength; yoff += hatchstep)
         {
            if(WedgeClip(xc,yc,dlength,1.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
            {
               DR2DMove(x1,y1);
               DR2DDraw(x2,y2);
               DR2DStroke();
            }
         }
         for(yoff = -2.0 * dlength; yoff < 2.0 * dlength; yoff += hatchstep)
         {
            if(WedgeClip(xc,yc,dlength,-1.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
            {
               DR2DMove(x1,y1);
               DR2DDraw(x2,y2);
               DR2DStroke();
            }
         }
         break;
      case 6:
         for(yoff = -2.0 * dlength; yoff < 2.0 * dlength; yoff += hatchstep)
         {
            if(WedgeClip(xc,yc,dlength,0.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
            {
               DR2DMove(x1,y1);
               DR2DDraw(x2,y2);
               DR2DStroke();
            }
         }
         for(yoff = -2.0 * dlength; yoff < 2.0 * dlength; yoff += hatchstep)
         {
            if(WedgeClip(xc,yc,dlength,-9999.0,yoff,startangle,endangle,&x1,&y1,&x2,&y2))
            {
               DR2DMove(x1,y1);
               DR2DDraw(x2,y2);
               DR2DStroke();
            }
         }
         break;
      default:
         break;
      }
   }
   else
   {
      if(pattern)
         SetDR2DAttributes(CurDR2DDash, 1, pattern, CurDR2DThick);
   }


/***                                ***
 ***  Now draw the wedge itself.    ***
 ***                                ***/

   /* Move to centre and draw out to start of arc */
   DR2DMove(xc,yc);
   
   if(startangle < endangle)
   {
      for(angle = startangle; angle < endangle; angle += (5.0 * PI / 180.0))
      {
         /* Calculate the point in the arc */
         x = dlength * cos(angle);
         y = dlength * sin(angle);
         
         /* Draw to this position */
         DR2DDraw(x+xc, y+yc);
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
         DR2DDraw(x+xc, y+yc);
      }
      
      for(angle -= 2*PI; angle < endangle; angle += (5.0 * PI / 180.0))
      {
         /* Calculate the start point of the arc */
         x = dlength * cos(angle);
         y = dlength * sin(angle);
         
         /* Draw to this position */
         DR2DDraw(x+xc, y+yc);
      }
   }
   
   /* Draw to end angle */
   x = dlength * cos(endangle);
   y = dlength * sin(endangle);
   
   DR2DDraw(x+xc, y+yc);
   
   /* Close the wedge */
   DR2DCloseStroke();
   
   SetDR2DAttributes(CurDR2DDash, CurDR2DFill, CurDR2DColour, CurDR2DThick);

   return(0);
}

/***************************************************************************/
/*>DR2DMark(double x, double y, int mark, double size)
   ---------------------------------------------------
   Draw a mark
   15.07.92 Original
   16.07.92 Removed nested FORMs for fills. Changed to DR2DCloseStroke().
            Added circle support.
   17.07.92 Added casting of calls to AMMove() and AMDraw()
*/
int DR2DMark(double x, 
         double y, 
         int    mark, 
         double size)
{
   double   ang;

   size /= (72.0 * 4);
   
   switch(mark)
   {
   case 0:                             /* No mark                          */
      break;
   case 1:                             /* Cross                            */
      DR2DMove((double)(x-size), (double)(y-size));
      DR2DDraw((double)(x+size), (double)(y+size));
      DR2DMove((double)(x+size), (double)(y-size));
      DR2DDraw((double)(x-size), (double)(y+size));
      DR2DStroke();
      break;
   case 2:                             /* Open square                      */
      DR2DMove((double)(x-size), (double)(y-size));
      DR2DDraw((double)(x+size), (double)(y-size));
      DR2DDraw((double)(x+size), (double)(y+size));
      DR2DDraw((double)(x-size), (double)(y+size));
      DR2DCloseStroke();
      break;
   case 3:                             /* Filled square                    */
      /* Set the fill pattern */
      SetDR2DAttributes(CurDR2DDash, 1, 6, CurDR2DThick);
   
      /* Draw the actual box */
      DR2DMove((double)(x-size), (double)(y-size));
      DR2DDraw((double)(x+size), (double)(y-size));
      DR2DDraw((double)(x+size), (double)(y+size));
      DR2DDraw((double)(x-size), (double)(y+size));
      DR2DCloseStroke();

      /* Reset fill pattern */
      SetDR2DAttributes(CurDR2DDash, CurDR2DFill, CurDR2DColour, CurDR2DThick);
      break;
   case 4:                             /* Open circle                      */
      DR2DMove(x+size, y);
      for(ang=(2*PI)/180.0; ang <= 2*PI; ang += ((2*PI)/18.0))
      {
         double   xr,
                  yr;
         xr = size * cos(ang);
         yr = size * sin(ang);
         DR2DDraw(x+xr, y+yr);
      }
      DR2DCloseStroke();
      break;
   case 5:                             /* Filled circle                    */
      /* Set the fill pattern */
      SetDR2DAttributes(CurDR2DDash, 1, 6, CurDR2DThick);
   
      DR2DMove(x+size, y);
      for(ang=(2*PI)/180.0; ang <= 2*PI; ang += ((2*PI)/18.0))
      {
         double   xr,
                  yr;
         xr = size * cos(ang);
         yr = size * sin(ang);
         DR2DDraw(x+xr, y+yr);
      }
      DR2DCloseStroke();

      /* Reset fill pattern */
      SetDR2DAttributes(CurDR2DDash, CurDR2DFill, CurDR2DColour, CurDR2DThick);
      break;
   case 6:                             /* Open diamond                     */
      DR2DMove((double)(x-size), (double)(y));
      DR2DDraw((double)(x),      (double)(y+size));
      DR2DDraw((double)(x+size), (double)(y));
      DR2DDraw((double)(x),      (double)(y-size));
      DR2DCloseStroke();
      break;
   case 7:                             /* Filled diamond                   */
      /* Set the fill pattern */
      SetDR2DAttributes(CurDR2DDash, 1, 6, CurDR2DThick);
   
      /* Draw the actual box */
      DR2DMove((double)(x-size), (double)(y));
      DR2DDraw((double)(x),      (double)(y+size));
      DR2DDraw((double)(x+size), (double)(y));
      DR2DDraw((double)(x),      (double)(y-size));
      DR2DCloseStroke();

      /* Reset fill pattern */
      SetDR2DAttributes(CurDR2DDash, CurDR2DFill, CurDR2DColour, CurDR2DThick);
      break;
   case 8:                             /* Open triangle                    */
      DR2DMove((double)(x-size), (double)(y-size));
      DR2DDraw((double)(x),      (double)(y+size));
      DR2DDraw((double)(x+size), (double)(y-size));
      DR2DCloseStroke();
      break;
   case 9:                             /* Filled triangle                  */
      /* Set the fill pattern */
      SetDR2DAttributes(CurDR2DDash, 1, 6, CurDR2DThick);
   
      /* Draw the actual box */
      DR2DMove((double)(x-size), (double)(y-size));
      DR2DDraw((double)(x),      (double)(y+size));
      DR2DDraw((double)(x+size), (double)(y-size));
      DR2DCloseStroke();

      /* Reset fill pattern */
      SetDR2DAttributes(CurDR2DDash, CurDR2DFill, CurDR2DColour, CurDR2DThick);
      break;
   case 10:                            /* Open inverted triangle           */
      DR2DMove((double)(x-size), (double)(y+size));
      DR2DDraw((double)(x),      (double)(y-size));
      DR2DDraw((double)(x+size), (double)(y+size));
      DR2DCloseStroke();
      break;
   case 11:                            /* Filled inverted triangle         */
      /* Set the fill pattern */
      SetDR2DAttributes(CurDR2DDash, 1, 6, CurDR2DThick);
   
      /* Draw the actual box */
      DR2DMove((double)(x-size), (double)(y+size));
      DR2DDraw((double)(x),      (double)(y-size));
      DR2DDraw((double)(x+size), (double)(y+size));
      DR2DCloseStroke();

      /* Reset fill pattern */
      SetDR2DAttributes(CurDR2DDash, CurDR2DFill, CurDR2DColour, CurDR2DThick);
      break;
   case 12:                            /* Plus sign                        */
      DR2DMove((double)(x-size), (double)(y));
      DR2DDraw((double)(x+size), (double)(y));
      DR2DMove((double)(x),      (double)(y+size));
      DR2DDraw((double)(x),      (double)(y-size));
      DR2DStroke();
      break;
   default:
      break;
   }
   return(0);
}
/***************************************************************************/
/*>DR2DROffText(double x, double y, double offset, char *string)
   -------------------------------------------------------------
   Do rigt justified text, centred on y and offet along x by offset pts.
   16.07.92 Original
   17.07.92 Changed to use DR2DStrWidth(). Moved left 1 pt.
   20.07.92 Changed to use DR2DShowText()
*/
int DR2DROffText(double x, 
             double y, 
             double offset,
             char   *string)
{
   offset -= 1;
   offset /= 72.0;
   
   x -= DR2DStrWidth(string, CurrentDR2DFont, DR2DFonty) - offset;
   y -= DR2DFonty/3.0;
   
   DR2DShowText(string, 0, x, y);

   return(IFFERROR);
}

/***************************************************************************/
/*>DR2DShowText(char *text, int orientation, double XBase, double YBase)
   ---------------------------------------------------------------------
   Write text raising and lowering text and selecting alternate font as
   appropriate.
   
   20.07.92 Original based on HPGL version.
*/
int DR2DShowText(char    *text, 
             int     orientation,
             double  XBase,
             double  YBase)
{
   char        buffer[MAXBUFF];
   int         i, j,
               mainfont;
   double      mainwidth,
               mainheight;


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
            DR2DOutputString(buffer, XBase, YBase, orientation);

            /* Update current position */
            if(orientation)
               YBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);
            else
               XBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);

            j = 0;
         }
         /* Switch to Greek font */
         mainfont        = CurrentDR2DFont;
         CurrentDR2DFont = AltDR2DFont;

         /* Output character */
         buffer[0] = text[++i];
         buffer[1] = '\0';
         DR2DOutputString(buffer, XBase, YBase, orientation);

         /* Update current position */
         if(orientation)
            YBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);
         else
            XBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);

         /* Switch font back */
         CurrentDR2DFont = mainfont;
         break;
      case '^':            /* Should raise next character                  */
         /* Finish the current string */
         if(j)
         {
            buffer[j] = '\0';
            DR2DOutputString(buffer, XBase, YBase, orientation);

            /* Update current position */
            if(orientation)
               YBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);
            else
               XBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);

            j = 0;
         }
         
         /* If necessary build string */
         if(text[++i] == '{')
         {
            while(text[++i] != '}' && text[i] != '\0' && j<MAXBUFF-1)
               buffer[j++] = text[i];
         }
         else
         {
            buffer[j++] = text[i];
         }
         
         /* Output raised string */
         if(j)
         {
            buffer[j]  = '\0';

            /* Modify base position */
            if(orientation)   XBase -= DR2DFonty/2.0;
            else              YBase += DR2DFonty/2.0;
            
            /* Store current font size */
            mainwidth  = DR2DFontx;
            mainheight = DR2DFonty;
            
            /* Switch to smaller font  */
            DR2DFontx  /= 2.0;
            DR2DFonty  /= 2.0;
            
            /* Output string */
            DR2DOutputString(buffer, XBase, YBase, orientation);

            /* Update current position */
            if(orientation)
               YBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);
            else
               XBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);

            /* Switch back to main font */
            DR2DFontx = mainwidth;
            DR2DFonty = mainheight;

            /* Reset base position */
            if(orientation)   XBase += DR2DFonty/2.0;
            else              YBase -= DR2DFonty/2.0;
            
            j = 0;
         }
         break;

      case '_':            /* Should lower next character                  */
         /* Finish the current string */
         if(j)
         {
            buffer[j] = '\0';
            DR2DOutputString(buffer, XBase, YBase, orientation);

            /* Update current position */
            if(orientation)
               YBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);
            else
               XBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);

            j = 0;
         }
         
         /* If necessary build string */
         if(text[++i] == '{')
         {
            while(text[++i] != '}' && text[i] != '\0' && j<MAXBUFF-1)
               buffer[j++] = text[i];
         }
         else
         {
            buffer[j++] = text[i];
         }
         
         /* Output lowered string */
         if(j)
         {
            buffer[j]  = '\0';

            /* Modify base position */
            if(orientation)   XBase += DR2DFonty/4.0;
            else              YBase -= DR2DFonty/4.0;
            
            /* Store current font size */
            mainwidth  = DR2DFontx;
            mainheight = DR2DFonty;
            
            /* Switch to smaller font  */
            DR2DFontx  /= 2.0;
            DR2DFonty  /= 2.0;
            
            /* Output string */
            DR2DOutputString(buffer, XBase, YBase, orientation);

            /* Update position */
            if(orientation)
               YBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);
            else
               XBase += DR2DStrWidth(buffer,CurrentDR2DFont, DR2DFonty);

            /* Switch back to main font */
            DR2DFontx = mainwidth;
            DR2DFonty = mainheight;

            /* Reset base position */
            if(orientation)   XBase -= DR2DFonty/4.0;
            else              YBase += DR2DFonty/4.0;
            
            j = 0;
         }
         break;

      default:             /* An ordinary character                        */
         buffer[j++] = text[i];
         break;
      }
   }
   
   if(j)
   {
      buffer[j] = '\0';
      DR2DOutputString(buffer, XBase, YBase, orientation);
      j = 0;
   }

   return(0);
}



/***************************************************************************/
/*>DR2DOutputString(char *string, double x, double y, int orientation)
   -------------------------------------------------------------------
   Actually send a string to the DR2D file. Specify string and position.
   Orientation is 1 for vertical text, 0 otherwise.
   
   20.07.92 Original
*/
int DR2DOutputString(char *string, double x, double y, int orientation)
{
   STXTstruct  stxt;

   stxt.Pad0      = (UBYTE)0;
   stxt.WhichFont = CurrentDR2DFont;
   stxt.CharW     = DR2DFontx;
   stxt.CharH     = DR2DFonty;
   stxt.BaseX     = x;
   stxt.BaseY     = y;
   stxt.Rotation  = (orientation ? 90.0 : 0.0);
   stxt.NumChars  = strlen(string);

   IFFERROR=PushChunk(iff, 0, ID_STXT, IFFSIZE_UNKNOWN);
   
   /* Write the STXT header */
   if(!IFFERROR)
   {
      if(WriteChunkBytes(iff, (UBYTE *)&stxt, sizeof(stxt))
         != sizeof(stxt))
         IFFERROR = IFFERR_WRITE;
   }
   /* Now the string */
   if(!IFFERROR)
   {
      if(WriteChunkBytes(iff, (UBYTE *)string, strlen(string))
         != strlen(string))
         IFFERROR = IFFERR_WRITE;
   }

   /* Now finish the chunk */
   if(!IFFERROR)
      IFFERROR = PopChunk(iff);

   return(IFFERROR);
}
