/***************************************************************************

   Program:    
   File:       MyIO.c
   
   Version:    V1.3
   Date:       06.02.91
   Function:   New versions of gets and puts to use AmigaDOS IO
   
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
   These routines to replace the Lattice gets and puts so direct AmigaDOS i/o
   routines may be used. Note that the string returned by mygets() has
   a '\n' before the '\0' (as with gets()) so it is necessary to call
   terminate() before you can do anything useful.

****************************************************************************

   Usage:
   ======
   mygets(file,string,maxl)
   Input:   struct FileHandle    *file       An AmigaDOS file handle
            int                  maxl        Max length of string
   Output:  char                 *string     A pointer to a string
   Return:  int                              Length of string (including '\0')
   

   myputs(file,string)
   Input:   struct FileHandle    *file       An AmigaDOS file handle
            char                 *string     A pointer to a string
   Return:  int                              Number of characters written


   mygetsbuff(file,string,maxl,iobuff)
   Input:   struct FileHandle    *file       An AmigaDOS file handle
            int                  maxl        Max length of string
   Output:  char                 *string     A pointer to a string
   I/O:     IOBUFF               *iobuff     Buffer for i/o (defined in myio.h)
   Return:  int                              Length of string (including '\0')
   
   iobuff->firstcall should be set to TRUE before the first call.
   
****************************************************************************

   Revision History:
   =================
   V1.1  13.09.91
   Added mygetsbuff()

   V1.2  07.11.91
   Fixed mygetsbuff() to work if there's no return on the last line
   
   V1.3  28.05.92
   ANSIed

***************************************************************************/
#include <stdlib.h>
#include <string.h>

#ifdef AMIGA
#include <dos.h>
#include <exec/types.h>
#include <libraries/dosextens.h>
#include <proto/dos.h>
#include "mylib:myio.h"
#else
#include "myio.h"
#endif

#include "port.h"

/*------------------------------------------------------------------------*/
int mygets(FH *file,
       char              *string,
       int               maxl)
{
   int j,
       nret,
       first;
       
   first = 1;
   
   for(j=0;j<maxl-1;j++)
   {
#ifdef AMIGA
      nret=Read((BPTR)file, &string[j], 1);
#else
      if((nret = fgetc(file)) == EOF)
      {
         nret = 0;
      }
      else
      {
         string[j] = (char)nret;
         nret = 1;
      }
#endif
      if(nret<=0)
      {
         if(first)
         {
            j = -1;  /* So the routine returns 0 */
            break;
         }
         else
         {
            string[j] = '\0';
            j--;
            break;
         }
      }
      first = 0;
      if(string[j]=='\n')
      {
         string[j+1] = '\0';
         break;
      }
   }
   return(j+1);
}

/*------------------------------------------------------------------------*/
int mygetsbuff(FH *file,
           char   *string,
           int     maxl,
           IOBUFF *iobuff)
{
   int j,
       nret;
#ifndef AMIGA
   int i;
#endif
               
   /* Initialise stuff if this is the first call */
   if(iobuff->firstcall)
   {
      iobuff->firstcall = 0;
      iobuff->ngot      = 0;
      iobuff->bufptr    = 0;
   }
   
   for(j=0;j<maxl-1;j++)
   {
      /* If we haven't got any characters, read characters into the buffer */
      if(!iobuff->ngot)
      {
#ifdef AMIGA
         nret = Read((BPTR)file, iobuff->buffer, MYIO_MAXBUFF);
#else
         for(i=0; i<MYIO_MAXBUFF; i++)
         {
            if((nret = fgetc(file)) == EOF)
            {
               nret = i;
               break;
            }
            else
            {
               iobuff->buffer[i] = nret;
               nret = i+1;
            }
         }
#endif

/* V1.2 */
         if(nret == 0)           /* EOF */
         {
            string[j] = '\0';
            return(j);           /* This will be 0 if this is the string start*/
         }
         else if (nret < 0)      /* A real error                              */
            return(nret);        /* Return error                              */
         else
         {
            iobuff->ngot = nret; /* Set to number of chars in our buffer      */
            iobuff->bufptr = 0;  /* Reset pointer to start of buffer          */
         }
      }
      
      /* Copy the character out of the buffer */
      string[j] = iobuff->buffer[(iobuff->bufptr)++];
      iobuff->ngot--;
      
      /* Check for newline character */
      if(string[j] == '\n')
      {
         j++;
         break;
      }
   }
   
   string[j] = '\0';
   
   return(j);
}

/*------------------------------------------------------------------------*/
int myputs(FH   *file,
           char *string)
{
   int i;
#ifdef AMIGA
   i=Write((BPTR)file,string,strlen(string));
#else
   i=fputs(string, file);
#endif
   return(i);
}




/***************************************************************************
*                                                                          *
*                 DEMO CODE --- Define DEMO to compile                     *
*                 Assumes a file called "test.dat" exists                  *
*                                                                          *
***************************************************************************/

#ifdef DEMO
#include <stdio.h>

main()
{
   char string[80];
   FH *in,*out;
   
#ifdef AMIGA
   out=(FH *)Output();
#else
   out = stdout;
#endif

#ifdef AMIGA
   if((in=(FH *)Open("test.dat",MODE_OLDFILE))==NULL)
#else
   if((in=(FH *)fopen("test.dat","r"))==NULL)
#endif
   {
      printf("Unable to open test.dat\n");
      exit(1);
   }

   while(mygets(in,string,80))
      myputs(out,string);
      
#ifdef AMIGA
   Close(in);
#else
   fclose(in);
#endif
}
#endif
