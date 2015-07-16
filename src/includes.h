/****************************************************************************

   Program:    AMPlot V2.0
   File:       includes.h
   
   Version:    V0.1
   Date:       04.03.92
   Function:   Amiga system and C includes
   
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
   Does any C and Amiga specific includes

*****************************************************************************

   Usage:
   ======

*****************************************************************************

   Revision History:
   =================

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "port.h"             /* System portability                        */

#ifdef AMIGA
#include "mylib:MathType.h"
#else
#include "MathType.h"
#endif

#ifdef AMIGA
#include <proto/intuition.h>  /* Amiga Intuition interface                 */
#include <proto/graphics.h>   /* Amiga Graphics library                    */
#include <proto/dos.h>        /* Amiga DOS routines                        */
#include <sts/stslib.h>       /* STSLib menu and gadget library            */

#include "mylib:rexx/rxil.h"  /* Rxil Rexx shell                           */

#include "mylib:myio.h"       /* My version of gets(), etc.                */
#include "mylib:macros.h"     /* Useful macros                             */
#include "mylib:parse.h"      /* Commands parser                           */
#else /* Unix */
#include "myio.h"
#include "macros.h"           /* Useful macros                             */
#include "parse.h"            /* Command parser                            */
#endif

#include "ps.h"               /* PostScript routines                       */

#include "amplot.h"           /* Misc defines and externs                  */
#include "interface.h"        /* Interface menus and gadgets               */


#include "prototypes.h"       /* Prototypes                                */

