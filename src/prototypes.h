/****************************************************************************

   Program:    AMPlot V2.0
   File:       prototypes.h
   
   Version:    V0.1
   Date:       13.03.92
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
   Includes prototype files.

*****************************************************************************

   Usage:
   ======

*****************************************************************************

   Revision History:
   =================

****************************************************************************/

#ifndef DOPROTOS

#include "main.p"
#include "FileIO.p"
#include "plots.p"
#include "axes.p"
#include "commands.p"
#include "fonts.p"
#include "plotting.p"
#include "svdfit.p"
#include "smooth.p"
#include "robust.p"
#include "barchart.p"
#include "piechart.p"
#include "hpgl.p"
#include "dr2d.p"
#include "handlers.p"
#include "myio.p"

#ifdef AMIGA
#include "requesters.p"
#include "rexx.p"
#include "messages.p"
#include "menus.p"
#include "main_amiga.p"
#include "fonts_amiga.p"
#include "plots_amiga.p"
#include "plotting_amiga.p"
#include "handlers_amiga.p"
#endif


#endif /* DOPROTOS */
