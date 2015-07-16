/****************************************************************************

   Program:    AMPlot2
   File:       Fonts.c
   
   Version:    V1.0
   Date:       01.04.92
   Function:   Font handling functions
   
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
#ifdef AMIGA
#include <libraries/diskfont.h>
#include <intuition/intuitionbase.h>
#endif

/***************************************************************************/
/* Local structure definitions
*/

/* LOADEDFONT is used to store info on fonts we've already loaded          */
typedef struct FontEntry
{
   struct FontEntry        *next,
                           *prev;
   struct TextFont         *font;
   int                     Size,
                           ReqSize;
   char                    name[MAXBUFF];
} LOADEDFONT;

/* AVAILFONT is used to store available sizes for each font                */
typedef struct FontAvailSizes
{
   struct FontAvailSizes   *next,
                           *prev;
   int                     *sizes,
                           nsizes;
   char                    name[MAXBUFF];
}  AVAILFONT;

/***************************************************************************/
/* Local external definitions.
*/
extern static LOADEDFONT    *fontlist;          /* The loaded font list    */
extern static AVAILFONT     *AvailableFonts;    /* The avail sizes list    */

/***************************************************************************/
/*>SetAmigaFont(struct Window *wind, char *AmigaFont, int  ReqSize,
                char *DefFont)
   ----------------------------------------------------------------
   Sets an Amiga screen font, first opening it, if necessary. Maintains
   a list of fonts which have already been opened.

   05.05.92 Original
   06.05.92 Clear pointer on failed return
   10.07.92 Removed calls to WaitPointer()
   13.07.92 Added the DefFont parameter and recursion with the default
            font if the first requested font not found. Now returns 1
            if failed, rather than Die()'ing. (Still Die()'s if no
            memory.) Now reads os version from IntuitionBase rather than
            from display.osversion for portability to other s/w
*/
int SetAmigaFont(struct Window *wind,      /* Window in which to set font      */
             char          *AmigaFont, /* Requested font                   */
             int           ReqSize,    /* Requested size                   */
             char          *DefFont)   /* Fall back default font           */
{

   int                     *AvailSizes = NULL,  /* Sizes available         */
                           i,                   /* Counter                 */
                           nsizes,              /* Number of sizes         */
                           diff,                /* Avail vs. requested size*/
                           mindiff     = 999,   /* Lowest difference       */
                           BestSize;            /* Best font size          */
   struct TextFont         *FontPtr;            /* Font pointer            */
   static struct TextAttr  ThisFont;            /* Text attribute          */
   LOADEDFONT              *p;                  /* Font pointer            */
   
   static int              nfonts      = 0;     /* Number of fonts loaded  */
   struct RastPort         *RasP;               /* RastPort for the window */
   
   /* Find window's rastport */
   RasP = wind->RPort;

   if(nfonts)   /* If we've got any fonts in our fontlist */
   {
      /* Check the list */
      if((FontPtr = (struct TextFont *)infontlist(AmigaFont,ReqSize))==NULL)
      {
         /*** We haven't got this font ***/
         
         /* If we haven't got scalable fonts, get the nearest available 
            font size
         */
         BestSize = ReqSize;
         if(IntuitionBase->LibNode.lib_Version < 36)
         {
            /* Get list of available sizes for this font */
            if((AvailSizes = GetFontSizes(AmigaFont, &nsizes)) == NULL)
            {
               if(!strcmp(DefFont,AmigaFont))
               {
                  char buffer[80];
                  sprintf(buffer,"You must install the %s font!",DefFont);
                  ReqMessage(wind,buffer,0);
                  return(1);
               }
               else
               {
                  /* We didn't find this font, so recurse asking for
                     the default font
                  */
                  SetAmigaFont(wind,DefFont,ReqSize,DefFont);
                  return(0);
               }
            }
      
            for(i=0; i<nsizes; i++)
            {
               diff = abs((ReqSize-AvailSizes[i]));
               if(diff<mindiff)
               {
                  mindiff  = diff;
                  BestSize = AvailSizes[i];
               }
            }
            
            free(AvailSizes);
         }

         /* Set up the text attribute structure */
         strcpy(ThisFont.ta_Name, AmigaFont);
         ThisFont.ta_YSize = BestSize;
         ThisFont.ta_Style = 0;
         ThisFont.ta_Flags = 0;

         /* Attempt to open this font from disk */
         if((FontPtr = (struct TextFont *)OpenDiskFont(&ThisFont)) == NULL)
         {
            if(!strcmp(DefFont,AmigaFont))
            {
               char buffer[80];
               sprintf(buffer,"You must install the %s font!",DefFont);
               ReqMessage(wind,buffer,0);
               return(1);
            }
            else
            {
               /* We didn't find this font, so recurse asking for
                  the default font
               */
               SetAmigaFont(wind,DefFont,ReqSize,DefFont);
               return(0);
            }
         }

         /* Add this font to our list */
         for(p=fontlist;p->next;NEXT(p));          /* Move to end of list  */
         ALLOCNEXTPREV(p,LOADEDFONT);              /* Add another item     */
         if(p==NULL) Die("No memory for fonts");

         /* Setup this item in the list */
         strcpy(p->name,AmigaFont);
         p->ReqSize  = ReqSize;
         p->Size     = BestSize;
         p->font     = FontPtr;
         
         nfonts++;
      }
   }
   else  /* We haven't got _any_ fonts in the list */
   {
      /* If we haven't got scalable fonts, get the nearest available 
         font size
      */
      BestSize = ReqSize;
      if(IntuitionBase->LibNode.lib_Version < 36)
      {
         /* Get list of available sizes for this font */
         if((AvailSizes = GetFontSizes(AmigaFont, &nsizes)) == NULL)
         {
            if(!strcmp(DefFont,AmigaFont))
            {
               char buffer[80];
               sprintf(buffer,"You must install the %s font!",DefFont);
               ReqMessage(wind,buffer,0);
               return(1);
            }
            else
            {
               /* We didn't find this font, so recurse asking for
                  the default font
               */
               SetAmigaFont(wind,DefFont,ReqSize,DefFont);
               return(0);
            }
         }
   
         for(i=0; i<nsizes; i++)
         {
            diff = abs((ReqSize-AvailSizes[i]));
            if(diff<mindiff)
            {
               mindiff  = diff;
               BestSize = AvailSizes[i];
            }
         }
         
         free(AvailSizes);
      }

      /* Set up the text attribute structure */
      ThisFont.ta_Name  = AmigaFont;
      ThisFont.ta_YSize = BestSize;
      ThisFont.ta_Style = 0;
      ThisFont.ta_Flags = 0;

      /* Attempt to open this font from disk */
      if((FontPtr = (struct TextFont *)OpenDiskFont(&ThisFont)) == NULL)
      {
         if(!strcmp(DefFont,AmigaFont))
         {
            char buffer[80];
            sprintf(buffer,"You must install the %s font!",DefFont);
            ReqMessage(wind,buffer,0);
            return(1);
         }
         else
         {
            /* We didn't find this font, so recurse asking for
               the default font
            */
            SetAmigaFont(wind,DefFont,ReqSize,DefFont);
            return(0);
         }
      }

      /* Initialise our list */
      INITPREV(fontlist,LOADEDFONT);

      /* Copy in the data */
      strcpy(fontlist->name,AmigaFont);
      fontlist->ReqSize  = ReqSize;
      fontlist->Size     = BestSize;
      fontlist->font     = FontPtr;
         
      nfonts++;
   }
      
   /* Now we can actually set up the font */
   SetFont(RasP,FontPtr);

   return(0);
}

