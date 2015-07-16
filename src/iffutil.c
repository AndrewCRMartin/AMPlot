/****************************************************************************

   Program:    
   File:       IFFUtil.c
   
   Version:    V1.0
   Date:       14.07.92
   Function:   Routines to ease general IFF output
   
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

#ifndef DOPROTOS
#include "iffutil_protos.h"
#endif

/***************************************************************************/
/*>struct IFFHandle *StartIFF(char *filename, long mode)
   -----------------------------------------------------
   Allocates an IFFHandle, opens an AmigaDOS file which is attached to the
   IFFHandle and starts IFF comms.
   mode is IFFF_WRITE or IFFF_READ.
   Returns NULL (and frees the handle) if there is a problem.

   14.07.92 Original
*/
struct IFFHandle *StartIFF(char *filename,
                           long mode)
{
   struct IFFHandle *iff = NULL;
   
   /* Allocate IFF_File structure. */
   iff = AllocIFF();

   if(iff)
   {
      /* Set up IFF_File for File I/O. */
      if(!(iff->iff_Stream = (BPTR)Open(filename, 
                           ((mode==IFFF_WRITE)?MODE_NEWFILE:MODE_OLDFILE))))
      {
         FreeIFF(iff);
         iff = NULL;
      }

      if(iff)
      {
         InitIFFasDOS(iff);
         /* Start the IFF transaction. */
         if(OpenIFF(iff, mode))
         {
            FreeIFF(iff);
            iff = NULL;
         }
      }
   }
   return(iff);
}

/***************************************************************************/
/*>long PutChunk(struct IFFHandle *iff, long id, long size, void *data)
   --------------------------------------------------------------------
   Puts a complete chunk.
   
   14.07.92 Original
*/
long PutChunk(struct IFFHandle   *iff,
              long               id,
              long               size,
              void               *data)
{
   long error;
   
   if(!(error=PushChunk(iff, 0, id, IFFSIZE_UNKNOWN)))
   {
      /* Now the actual data (the text) */
      if(WriteChunkBytes(iff, (UBYTE *)data, size) != size)
         error = IFFERR_WRITE;
   }

   if(!error) error = PopChunk(iff);
   
   return(error);
}

/***************************************************************************/
/*>void EndIFF(struct IFFHandle *iff)
   ----------------------------------
   Closes up the IFF file
   
   14.07.92 Original
*/
void EndIFF(struct IFFHandle *iff)
{
   if(iff)
   {
      /* Terminate the IFF transaction with the stream and free all 
         associated structures.
      */
      CloseIFF(iff);

      /* Close the file */
      if(iff->iff_Stream) Close((BPTR)iff->iff_Stream);

      /* Free the IFF_File structure itself. */
      FreeIFF(iff);
   }
}

/***************************************************************************/
/*>char *IffError(long error)
   --------------------------
   Returns a pointer to the text associated with an error number
   
   14.07.92 Original
*/
char *IffError(long error)
{
   /*
    * Text error messages for possible IFFERR_#? returns from various
    * IFF routines.  To get the index into this array, take your IFFERR code,
    * negate it, and subtract one.
    * idx = -error - 1;
    */
   static char  *errormsgs[] = 
   {
      "End of file (not an error).",
      "End of context (not an error).",
      "No lexical scope.",
      "Insufficient memory.",
      "Stream read error.",
      "Stream write error.",
      "Stream seek error.",
      "File is corrupt.",
      "IFF syntax error.",
      "Not an IFF file.",
      "Required call-back hook missing.",
      "Return to client.  You should never see this."
   };
   int   idx = -error - 1;
   
   if(idx < 0 && idx > 11)
      return(NULL);
   else
      return(errormsgs[idx]);
}

