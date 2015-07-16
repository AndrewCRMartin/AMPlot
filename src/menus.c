/***************************************************************************

   Program:    AMPlot V2.0
   File:       menus.c
   
   Version:    V0.3
   Date:       04.03.92
   Function:   Build menus for AMPlot using STSLib
   
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
               
****************************************************************************

   This program is copyright. Any copying without the permission of
   SciTech Software is illegal.

****************************************************************************

   Description:
   ============
   Builds all the textual menu and menuitem structures for AMPlot

****************************************************************************

   Usage:
   ======

****************************************************************************

   Revision History:
   =================
   V0.1     27.03.92
   Added MX to Grey-scale and hatch fill types
   
   V0.2     05.05.92
   Added separate Rexx menu
   
   V0.3     12.06.92
   Removed menu items for fill shade and hatch, to be replaced by requester

***************************************************************************/
/* Includes
*/

#include "includes.h"   /* Amiga system and C includes   */
#include "ImageData.h"  /* Images for menu items         */

/**************************************************************************/
/*>struct Menu *BuildAllMenus(struct Screen *scrn)
   -----------------------------------------------
   Calls STSLib routine to build the menus
   04.03.92 Original
   27.03.92 Added MX to Grey-scale and hatch fill types
   05.05.92 Added separate Rexx menu
   14.05.92 Added image menus for line and fill styles
   22.06.92 Changed menu text `Axis Labels' to `Axis/Pie Labels'
   01.07.92 Removed requirement for rexx debug to have CLI
   02.07.92 Added Palette menu item
   13.07.92 Added Screen to back menu item
   16.07.92 Added conditional check on iff-dr2d output menu item
   17.07.92 Removed this check. Item is now OffMenu()'d
   24.07.92 Made all fitting items check-able
   07.08.92 Added No Fills menu item. Changed Axis/Pie to Axis & Pie
*/
struct Menu *BuildAllMenus(struct Screen *scrn)
{
/*** Main menus ***/
   ProjectMenu =  BuildMenu(scrn, NULL,         "Project");
   StyleMenu   =  BuildMenu(scrn, ProjectMenu,  "Style");
   AxesMenu    =  BuildMenu(scrn, StyleMenu,    "Axes");
   TextMenu    =  BuildMenu(scrn, AxesMenu,     "Text");
   LinesMenu   =  BuildMenu(scrn, TextMenu,     "Lines");
   FitMenu     =  BuildMenu(scrn, LinesMenu,    "Fit");
   RexxMenu    =  BuildMenu(scrn, FitMenu,      "Rexx");
   
/*** Project MenuItems ***/
   MenuOpen =     BuildMenuItem(scrn,0,(APTR)ProjectMenu,"Open...",        'O', 0,0,0,1);
   MenuSave =     BuildMenuItem(scrn,0,(APTR)ProjectMenu,"Save",           'S', 0,0,0,1);
   MenuSaveAs =   BuildMenuItem(scrn,0,(APTR)ProjectMenu,"Save As...",     '\0',0,0,0,1);
   MenuEdit =     BuildMenuItem(scrn,0,(APTR)ProjectMenu,"Edit",           '\0',0,0,0,1);
   MenuPlot =     BuildMenuItem(scrn,0,(APTR)ProjectMenu,"Plot          »",'\0',0,0,0,1);
   MenuPaper =    BuildMenuItem(scrn,0,(APTR)ProjectMenu,"Paper...",       '\0',0,0,0,1);
   MenuSetPens =  BuildMenuItem(scrn,0,(APTR)ProjectMenu,"Set Pens...",    '\0',0,0,0,1);
   MenuPalette =  BuildMenuItem(scrn,0,(APTR)ProjectMenu,"Screen Palette...",'\0',0,0,0,1);
   MenuScrBack =  BuildMenuItem(scrn,0,(APTR)ProjectMenu,"Screen To Back",   '\0',0,0,0,1);
   MenuAbout =    BuildMenuItem(scrn,0,(APTR)ProjectMenu,"About...",       '\0',0,0,0,1);
   MenuQuit =     BuildMenuItem(scrn,0,(APTR)ProjectMenu,"Quit...",        'Q', 0,0,0,1);
   /* Plot subitems */
   MenuPS =       BuildMenuItem(scrn,1,(APTR)MenuPlot,"PostScript...", 'P' ,0,0,0,1);
   MenuHPGL =     BuildMenuItem(scrn,1,(APTR)MenuPlot,"HPGL...",       'H' ,0,0,0,1);
   MenuDR2D =     BuildMenuItem(scrn,1,(APTR)MenuPlot,"IFF-DR2D...",   'I' ,0,0,0,1);
   
/*** Style MenuItems ***/
   MenuXY =       BuildMenuItem(scrn,0,(APTR)StyleMenu,"XY",               '\0',0,0,3,1);
   MenuScatter =  BuildMenuItem(scrn,0,(APTR)StyleMenu,"Scatter",          '\0',0,0,2,1);
   MenuBar =      BuildMenuItem(scrn,0,(APTR)StyleMenu,"Bar",              '\0',0,0,2,1);
   MenuPie =      BuildMenuItem(scrn,0,(APTR)StyleMenu,"Pie",              '\0',0,0,2,1);
   MenuErrors =   BuildMenuItem(scrn,0,(APTR)StyleMenu,"Errors",           'E', 0,0,2,1);
   MenuEject =    BuildMenuItem(scrn,0,(APTR)StyleMenu,"Eject Slice",      '\0',0,0,1,1);
   MenuLineSet =  BuildMenuItem(scrn,0,(APTR)StyleMenu,"Line Setup...",    'L', 0,0,1,1);
   MenuBarSet =   BuildMenuItem(scrn,0,(APTR)StyleMenu,"Bar Setup...",     '\0',0,0,1,1);
   MenuPieSet =   BuildMenuItem(scrn,0,(APTR)StyleMenu,"Pie Setup...",     '\0',0,0,1,1);
   /* Mutual excludes */
   MxMenu(StyleMenu,MenuXY,     MenuScatter);
   MxMenu(StyleMenu,MenuXY,     MenuBar);
   MxMenu(StyleMenu,MenuXY,     MenuPie);
   MxMenu(StyleMenu,MenuScatter,MenuBar);
   MxMenu(StyleMenu,MenuScatter,MenuPie);
   MxMenu(StyleMenu,MenuBar,    MenuPie);

/*** Axes MenuItems ***/
   MenuLogX =     BuildMenuItem(scrn,0,(APTR)AxesMenu,"LogX",              '\0',0,0,2,1);
   MenuLogY =     BuildMenuItem(scrn,0,(APTR)AxesMenu,"LogY",              '\0',0,0,2,1);
   MenuBoxed =    BuildMenuItem(scrn,0,(APTR)AxesMenu,"Boxed",             'B', 0,0,2,1);
   MenuGrid =     BuildMenuItem(scrn,0,(APTR)AxesMenu,"Grid",              'G', 0,0,2,1);
   MenuAxes =     BuildMenuItem(scrn,0,(APTR)AxesMenu,"Set Axes...",       'A', 0,0,1,1);
   MenuFZero =    BuildMenuItem(scrn,0,(APTR)AxesMenu,"Follow Zero",       '\0',0,0,2,1);

/*** Text MenuItems ***/
   MenuTitle =    BuildMenuItem(scrn,0,(APTR)TextMenu,"Title...",          '\0',0,0,0,1);
   MenuAxTitle =  BuildMenuItem(scrn,0,(APTR)TextMenu,"Axis Titles...",    '\0',0,0,0,1);
   MenuAxLabel =  BuildMenuItem(scrn,0,(APTR)TextMenu,"Axis & Pie Labels...",'\0',0,0,0,1);
   MenuKey =      BuildMenuItem(scrn,0,(APTR)TextMenu,"Key...",            '\0',0,0,0,1);
   MenuLabel =    BuildMenuItem(scrn,0,(APTR)TextMenu,"Extra Labels...",   '\0',0,0,0,1);

/*** Lines MenuItems ***/
   MenuPen =      BuildMenuItem(scrn,0,(APTR)LinesMenu,"Pen...",           '\0',0,0,0,1);
   MenuStyle =    BuildMenuItem(scrn,0,(APTR)LinesMenu,"Style        »",   '\0',0,0,0,1);
   MenuFills =    BuildMenuItem(scrn,0,(APTR)LinesMenu,"Fills        »",   '\0',0,0,0,1);
   MenuFillType = BuildMenuItem(scrn,0,(APTR)LinesMenu,"Fill Control...",  '\0',0,0,0,1);
   /* Style subitems */
   MenuLS1 =      BuildImageMenu(1,(APTR)MenuStyle,ImageLine1,50,8,1,0);
   MenuLS2 =      BuildImageMenu(1,(APTR)MenuStyle,ImageLine2,50,8,1,0);
   MenuLS3 =      BuildImageMenu(1,(APTR)MenuStyle,ImageLine3,50,8,1,0);
   MenuLS4 =      BuildImageMenu(1,(APTR)MenuStyle,ImageLine4,50,8,1,0);
   MenuLS5 =      BuildImageMenu(1,(APTR)MenuStyle,ImageLine5,50,8,1,0);
   MenuLS6 =      BuildImageMenu(1,(APTR)MenuStyle,ImageLine6,50,8,1,0);
   /* Fill subitems */
   MenuFT1 =      BuildImageMenu(1,(APTR)MenuFills,ImageFill1,32,32,1,0);
   MenuFT2 =      BuildImageMenu(1,(APTR)MenuFills,ImageFill2,32,32,1,1);
   MenuFT3 =      BuildImageMenu(1,(APTR)MenuFills,ImageFill3,32,32,1,2);
   MenuFT4 =      BuildImageMenu(1,(APTR)MenuFills,ImageFill4,32,32,1,0);
   MenuFT5 =      BuildImageMenu(1,(APTR)MenuFills,ImageFill5,32,32,1,1);
   MenuFT6 =      BuildImageMenu(1,(APTR)MenuFills,ImageFill6,32,32,1,2);
   MenuNoFills =  BuildMenuItem(scrn,0,(APTR)LinesMenu,"No Fills",         '\0',0,0,0,1);

/*** Fit MenuItems ***/
   MenuRegress =  BuildMenuItem(scrn,0,(APTR)FitMenu,"Regression...",      '\0',0,0,2,1);
   MenuRobust =   BuildMenuItem(scrn,0,(APTR)FitMenu,"Robust",             '\0',0,0,2,1);
   MenuFourier =  BuildMenuItem(scrn,0,(APTR)FitMenu,"Fourier...",         '\0',0,0,2,1);
   MenuRexxFit =  BuildMenuItem(scrn,0,(APTR)FitMenu,"Rexx fit...",        '\0',0,0,2,1);

/*** Rexx MenuItems ***/
   MenuRunRexx =  BuildMenuItem(scrn,0,(APTR)RexxMenu,"Execute macro...",  'R', 0,0,1,1);
   MenuRexx =     BuildMenuItem(scrn,0,(APTR)RexxMenu,"Install macro...",  '\0',0,0,1,1);
   MenuDebRexx =  BuildMenuItem(scrn,0,(APTR)RexxMenu,"Debug",             '\0',0,0,2,1);
   
   return(ProjectMenu);
}

