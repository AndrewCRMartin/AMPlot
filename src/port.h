#ifndef __port_h
#define __port_h

/***************************************************************************/
/* Defines and macros
*/
#define TERMINATE(x) do {  int _terminate_macro_j;                    \
                        for(_terminate_macro_j=0;                     \
                            (x)[_terminate_macro_j];                  \
                            _terminate_macro_j++)                     \
                        {  if((x)[_terminate_macro_j] == '\n')        \
                           {  (x)[_terminate_macro_j] = '\0';         \
                              break;                                  \
                     }  }  }  while(0)


/***************************************************************************/
#ifdef AMIGA

typedef struct FileHandle FH;
#define WORDCHIP WORD __chip
#define USHORTCHIP USHORT __chip

#else

#include <unistd.h>
#include <stdio.h>
#include "port.p"

typedef short int WORD;
typedef unsigned char      UBYTE;
typedef unsigned short int USHORT;
typedef unsigned long  int ULONG;
typedef FILE FH;
typedef double IEEE;
#define WORDCHIP WORD
#define USHORTCHIP USHORT
#define APTR void *
#define BPTR void *

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


#endif

#endif

