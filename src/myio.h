#define MYIO_MAXBUFF 80
#ifdef AMIGA
#include <dos/dos.h>
#endif

#include "port.h"

typedef struct
{
   int   firstcall,
         ngot,
         bufptr;
   char  buffer[MYIO_MAXBUFF];
}  IOBUFF;

/* Prototypes
*/
int mygets(FH *file,
       char              *string,
       int               maxl)
;
int mygetsbuff(FH *file,
           char                *string,
           int                 maxl,
           IOBUFF              *iobuff)
;
int myputs(FH *file,
       char              *string)
;
