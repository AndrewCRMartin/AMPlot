/***************************************************************************

   Program:    
   File:       HPGL.h
   
   Version:    V1.0
   Date:       25.03.91
   Function:   Include file for HPGL support
   
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

   This program is copyright. Any copying without the permission of
   SciTech Software is illegal.

****************************************************************************

   Description:
   ============

****************************************************************************

   Usage:
   ======

****************************************************************************

   Revision History:
   =================

***************************************************************************/
#ifndef DOPROTOS
#include "hpgl.p"
#endif
/**************************************************************************/
#ifdef HPGL_MAIN
FH *HPGLFile;

/**************************************************************************/
#else
extern FH *HPGLFile;

#endif
