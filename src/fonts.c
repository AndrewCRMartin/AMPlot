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
#define FONTS_MAIN 1

#include "includes.h"
#ifdef AMIGA
#include <libraries/diskfont.h>
#include <intuition/intuitionbase.h>

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
static LOADEDFONT    *fontlist       = NULL;    /* The loaded font list    */
static AVAILFONT     *AvailableFonts = NULL;    /* The avail sizes list    */

/***************************************************************************/
/* Statis prototypes
*/
static int *GetFontSizes(char *fontname, int *nsizes);
static struct TextFont *infontlist(char *name, int size);
#endif

/***************************************************************************/
/*>CorrectPSCase(STATICTEXT *stext)
   --------------------------------
   Takes a PostScript font name, appends -Roman if it just says Times and 
   corrects the case to match the standard.
   01.04.92 Framework
   05.05.92 Added actual call commented out
   07.05.92 Removed comment...
*/
int CorrectPSCase(STATICTEXT *stext)
{
   if((stext->PSFont[5] != '-')&&(!strncmp(stext->PSFont,"Times",5)))
         strncpy((stext->PSFont)+5,"-Roman\0",7);
   correctcase(stext->PSFont);

   return(0);
}

/***************************************************************************/
/*>PS2AmigaFont(STATICTEXT *stext)
   -------------------------------
   Takes the PostScript font name and size and works out the best Amiga
   version.
   01.04.92 Framework
   05.05.92 Original
   06.05.92 Added reading of font translation table
   29.06.92 Added HPGL translations
*/
extern char *KillLeadSpaces(char *text);

int PS2AmigaFont(STATICTEXT *stext)
{
   int               i,                   /* Counter                       */
                     done,                /* Flag for having translated    */
                     nvalues,             /* Number of values read         */
                     HPGLFont;            /* HPGL font number              */
   char              *ptr,                /* To use with KillLeadSpaces    */
                     buffer[MAXBUFF];     /* Buffer for reading file       */
   static int        firstcall   = TRUE,  /* Only read table on first call */
                     ntrans      = 0;     /* Number of translations        */
   FH *fh         = NULL;  /* Translation table file        */
   IOBUFF            iobuff;              /* Buffered i/o                  */
   
   if(firstcall)
   {
      firstcall = FALSE;
      
      /* Clear all the pointers, so we can free safely on cleanup */
      for(i=0; i<MAXTRANS; i++)
      {
         FontTable[i].PSFont     = NULL;
         FontTable[i].AmigaFont  = NULL;
      }
      
#ifdef AMIGA
      /* First look in this directory */
      if((fh = (FH *)Open("AMPlot.ftrans",   MODE_OLDFILE))==NULL)
      {
         /* Then in the S: directory */  
         fh  = (FH *)Open("s:AMPlot.ftrans", MODE_OLDFILE);
      }
#else
      fh  = (FH *)fopen("AMPlot.ftrans", "r");
#endif

      /* If we opened the file read and process it */
      if(fh)
      {
         iobuff.firstcall = TRUE;
         while(mygetsbuff(fh, buffer, MAXBUFF, &iobuff) > 0)
         {
            if(((FontTable[ntrans].PSFont    = 
                          (char *)malloc(80 * sizeof(char)))==NULL)
            || ((FontTable[ntrans].AmigaFont = 
                          (char *)malloc(80 * sizeof(char)))==NULL))
            {
               ReqMessage(MainWind,"No memory for font translation",0);
               break;
            }
            nvalues = sscanf(buffer,"%s %s %d",FontTable[ntrans].PSFont,
                                               FontTable[ntrans].AmigaFont,
                                               &(FontTable[ntrans].HPGLFont));

            /* check if no HPGLFont was given */
            if(nvalues == 2)  FontTable[ntrans].HPGLFont = 0; 
            
            if(nvalues == 2 || nvalues == 3)
            {
               if(++ntrans > MAXTRANS)
               {
                  ntrans--;
                  sprintf(buffer,"Maximum = %d",MAXTRANS);
                  ReqMessage2(MainWind,"Too many font translations!",buffer,0);
               }
            }
         }
#ifdef AMIGA
         Close((BPTR)fh);
#else
         fclose(fh);
#endif
      }
   }
   
   
   /* Copy the PS font and downcase it into the Amiga fontname */
   StringToLower(stext->PSFont,stext->AmigaFont);
   ptr = KillLeadSpaces(stext->AmigaFont);
   strcpy(stext->AmigaFont, ptr);
      
   done = FALSE;

   if(ntrans)     /* If we've got any translations, use them   */
   {
      for(i=0; i<ntrans; i++)
      {
         /* Down case the table PS fontname into buffer */
         StringToLower(FontTable[i].PSFont, buffer);
         ptr = KillLeadSpaces(buffer);
         strcpy(buffer, ptr);
         
         /* If they're the same, copy in the Amiga name from the table */
         if(!strcmp(stext->AmigaFont, buffer))
         {
            strcpy(stext->AmigaFont, FontTable[i].AmigaFont);
            HPGLFont = FontTable[i].HPGLFont;
            done = TRUE;
            break;
         }
      }
   }
   
   if(!done)      /* Use our simple default translation scheme */
   {
      /* End the string if there is a space, a . or a - */
      for(i=0; stext->AmigaFont[i] && i<MAXBUFF; i++)
      {
         if(stext->AmigaFont[i] == ' ' || 
            stext->AmigaFont[i] == '.' || 
            stext->AmigaFont[i] == '-')
         {
            stext->AmigaFont[i]='\0';
            break;
         }
      }
      HPGLFont = 0;
   }
   
   /* Append the .font part */
   strncat(stext->AmigaFont, ".font", MAXBUFF-strlen(stext->AmigaFont));
   
   return(HPGLFont);
}

#ifdef AMIGA
/***************************************************************************/
/*>int *GetFontSizes(char *fontname, int *nsizes)
   ----------------------------------------------
   Takes a fontname and returns an array of integers representing the
   sizes in which this font is available. Once finished with the array
   the caller should free() it.
   
   05.05.92 Original
   13.07.92 Now keeps available sizes list, so disk access only happens 
            once for each fontname.
*/
static int *GetFontSizes(char *fontname,
                         int  *nsizes)
{
   struct AvailFonts       *af            = NULL;
   struct AvailFontsHeader *afh           = NULL;
   
   int                     afShortage,
                           afSize,
                           j,
                           *fontlist      = NULL;
   AVAILFONT               *f;
   
   if(AvailableFonts)   /* We've already got some info, so check it */
   {
      for(f=AvailableFonts; f; NEXT(f))
      {
         if(!strcmp(fontname, f->name))
         {
            /* We've found this fontname previously. Allocate an array
               to return and copy the previously found data.
            */
            *nsizes = f->nsizes;

            if((fontlist = (int *)malloc(*nsizes * sizeof(int))) == NULL)
            {
               *nsizes = 0;
               return(NULL);
            }
            for(j=0; j<*nsizes; j++) fontlist[j] = f->sizes[j];
                        
            /* Now return the list of sizes */
            return(fontlist);
         }
      }
   }
   
   /* Initialise number of sizes to 0 */
   *nsizes = 0;
      
   /* Get the available fonts list */
   afSize = 400;
   do
   {
      afh = (struct AvailFontsHeader *)AllocMem(afSize, 0);
      if(afh)
      {
         /* AvailFonts() returns the shortfall in size of the buffer */
         afShortage = AvailFonts((STRPTR)afh, (LONG)afSize, AFF_MEMORY|AFF_DISK);
         if(afShortage)
         {
            FreeMem(afh, afSize);
            afSize += afShortage;
         }
      }
      else
      {
         return(NULL);  /* No memory */
      }
   }  while(afShortage);
   
   
   /* If we got the list */
   if(afh)
   {
      /* First of the availfonts */
      af = (struct AvailFonts *) &afh[1];
      
      /* Count number of sizes available */
      for(*nsizes=0, j=0; j<afh->afh_NumEntries; j++, af++)
      {
         if(!(af->af_Attr.ta_Flags & FPF_REMOVED))
         {
            /* If font not removed from system */
            if(!strcmp(af->af_Attr.ta_Name, fontname)) (*nsizes)++;
         }
      }
      
      if(*nsizes)
      {
         /* Allocate memory for font list */
         if((fontlist = (int *)malloc(*nsizes * sizeof(int))) == NULL)
         {
            *nsizes = 0;
            return(NULL);
         }
         
         /* Step through list again, filling in array of sizes */
         af = (struct AvailFonts *) &afh[1];
         for(*nsizes=0, j=0; j<afh->afh_NumEntries; j++, af++)
         {
            if(!(af->af_Attr.ta_Flags & FPF_REMOVED))
            {
               /* If font not removed from system */
               if(!strcmp(af->af_Attr.ta_Name, fontname))
                  fontlist[(*nsizes)++] = af->af_Attr.ta_YSize;
            }
         }
      }
      
      FreeMem(afh, afSize);
   }


   if(*nsizes)
   {
      /* Add this information into our list of fonts already checked */
      if(AvailableFonts != NULL)
      {
         /* Move to end of list and add another entry */
         f = AvailableFonts;
         LAST(f);
         ALLOCNEXTPREV(f, AVAILFONT);
         
         if(f != NULL)
         {
            /* We got some memory, copy in the information */
            f->nsizes = *nsizes;
            strcpy(f->name, fontname);
            if((f->sizes = (int *)malloc(*nsizes * sizeof(int))) == NULL)
            {
               f->prev->next = NULL;
               free(f);
            }
            for(j=0; j<*nsizes; j++) f->sizes[j] = fontlist[j];
         }
      }
      else
      {
         INITPREV(AvailableFonts, AVAILFONT);
         if(AvailableFonts != NULL)
         {
            /* We got some memory, copy in the information */
            AvailableFonts->nsizes = *nsizes;
            strcpy(AvailableFonts->name, fontname);
            if((AvailableFonts->sizes = 
               (int *)malloc(*nsizes * sizeof(int))) == NULL)
            {
               free(AvailableFonts);
               AvailableFonts = NULL;
            }
            else
            {
               for(j=0; j<*nsizes; j++) 
                  AvailableFonts->sizes[j] = fontlist[j];
            }
         }
      }
   }
   
   return(fontlist);
}

/***************************************************************************/
/*>FreeAmigaFonts(void)
   --------------------
   Closes all the fonts opened by calls to SetAmigaFont and frees allocated
   memory.
   06.05.92 Original
   13.07.92 Now clears the available size list as well
*/
int FreeAmigaFonts(void)
{
   LOADEDFONT  *p;                              /* Font pointer            */
   AVAILFONT   *f;                              /* Avail size list pointer */

   /* Close all the fonts in the list        */
   for(p=fontlist; p; NEXT(p))
      if(p->font) CloseFont(p->font);

   /* Free the linked list                   */
   FREELIST(fontlist, LOADEDFONT);
   fontlist = NULL;

   /* Free the size list for each fontname   */
   for(f=AvailableFonts; f; NEXT(f))
      free(f->sizes);
   FREELIST(AvailableFonts, AVAILFONT);
   AvailableFonts = NULL;

   return(0);
}
   
/***************************************************************************/
/*>struct TextFont *infontlist(char *name, int size)
   --------------------------------------------------------------------
   Searches through the loaded font list for the requested font and size.
   If found, returns the pointer to the TextFont, otherwise returns NULL.
   05.05.92 Original
   13.07.92 Removed flist parameter and made static. This routine is only
            used locally and the fontlist is a static global so is
            accessible to this routine.
   
*/
static struct TextFont *infontlist(char       *name,  /* Requested font    */
                                   int        size)   /* Requested size    */
{
   LOADEDFONT *p;
   
   for(p=fontlist; p; NEXT(p))
   {
      if(!strcmp(p->name, name) && p->ReqSize == size)
         return(p->font);
   }
   
   return(NULL);
}
#endif

