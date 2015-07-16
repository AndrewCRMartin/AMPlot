/****************************************************************************

   Program:    AMPlot2
   File:       main.c
   
   Version:    V1.0
   Date:       12.03.92
   Function:   AMPlot graph plotting V2.0
   
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
#define MAIN 1

#include "includes.h"
#include "menudefs.h"

#include <intuition/intuitionbase.h>
#include <graphics/display.h>
#include <workbench/startup.h>
#include <proto/icon.h>

/***************************************************************************/
/* Rexx externs
*/
#ifdef REXX_SUPPORT
extern char *Rexx_return;
#endif

/***************************************************************************/
/* Other externals
*/
extern struct WBStartup *WBenchMsg;   /* Used by ProcessToolTypes() to get 
                                         program name                      */

/***************************************************************************/
/***************************************************************************/
/*>HandleEvent(APTR object)
   ------------------------
   Dispatch menu events.
   12.03.92 Original.
   30.03.92 Modified MenuEdit to call OpenFile() directly.
   31.03.92 Modified MenuSave to call SaveAsFile() directly.
   05.05.92 Added Rexx menu handling
   12.06.92 Removed fill grey/hatch items and replaced with requester
   02.07.92 Added MenuPalette
   13.07.92 Added MenuScrBack
   20.07.92 Added DEMO_VERSION conditional
   07.08.92 Added RemoveFills()
*/
HandleEvent(APTR object)
{
   if(object == (APTR)MenuOpen)     { OpenFile(object);        return(0); }
   if(object == (APTR)MenuSave)     { SaveAsFile(object);      return(0); }
   if(object == (APTR)MenuSaveAs)   { SaveAsFile(object);      return(0); }
   if(object == (APTR)MenuEdit)     { OpenFile(object);        return(0); }
   if(object == (APTR)MenuPaper)    { SetPaper(object);        return(0); }
   if(object == (APTR)MenuSetPens)  { SetPens(object);         return(0); }
   if(object == (APTR)MenuPalette)  { SetPalette(object);      return(0); }
   if(object == (APTR)MenuScrBack)  { ScreenToBack(MyScreen);  return(0); }
   if(object == (APTR)MenuAbout)    { About(object);           return(0); }
   if(object == (APTR)MenuQuit)     { QuitProgram(object);     return(0); }
#ifndef DEMO_VERSION
   if(object == (APTR)MenuPS)       { PlotPS(object);          return(0); }
   if(object == (APTR)MenuHPGL)     { PlotHPGL(object);        return(0); }
   if(object == (APTR)MenuDR2D)     { PlotDR2D(object);        return(0); }
#endif
   if(object == (APTR)MenuXY)       { StyleXY(object);         return(0); }
   if(object == (APTR)MenuScatter)  { StyleScatter(object);    return(0); }
   if(object == (APTR)MenuBar)      { StyleBar(object);        return(0); }
   if(object == (APTR)MenuPie)      { StylePie(object);        return(0); }
   if(object == (APTR)MenuErrors)   { ToggleErrors(object);    return(0); }
   if(object == (APTR)MenuEject)    { EjectSlice(object);      return(0); }
   if(object == (APTR)MenuLineSet)  { SetLine(object);         return(0); }
   if(object == (APTR)MenuBarSet)   { SetBar(object);          return(0); }
   if(object == (APTR)MenuPieSet)   { SetPie(object);          return(0); }
   if(object == (APTR)MenuLogX)     { ToggleLogX(object);      return(0); }
   if(object == (APTR)MenuLogY)     { ToggleLogY(object);      return(0); }
   if(object == (APTR)MenuBoxed)    { ToggleBoxed(object);     return(0); }
   if(object == (APTR)MenuGrid)     { ToggleGrid(object);      return(0); }
   if(object == (APTR)MenuAxes)     { SetAxes(object);         return(0); }
   if(object == (APTR)MenuFZero)    { ToggleFZero(object);     return(0); }
   if(object == (APTR)MenuTitle)    { SetTitle(object);        return(0); }
   if(object == (APTR)MenuAxTitle)  { SetAxTitle(object);      return(0); }
   if(object == (APTR)MenuAxLabel)  { SetAxLabel(object);      return(0); }
   if(object == (APTR)MenuKey)      { SetKey(object);          return(0); }
   if(object == (APTR)MenuLabel)    { SetLabel(object);        return(0); }
   if(object == (APTR)MenuPen)      { ChangePen(object);       return(0); }
   if(object == (APTR)MenuFillType) { FillControl(object);     return(0); }
   if(object == (APTR)MenuRegress)  { SetRegress(object);      return(0); }
   if(object == (APTR)MenuRobust)   { ToggleRobust(object);    return(0); }
   if(object == (APTR)MenuFourier)  { SetFourier(object);      return(0); }
   if(object == (APTR)MenuRexxFit)  { RexxFit(object);         return(0); }
   if(object == (APTR)MenuRexx)     { InstallMacro(object);    return(0); }
   if(object == (APTR)MenuRunRexx)  { RunMacro(object);        return(0); }
   if(object == (APTR)MenuDebRexx)  { ToggleRexxDeb(object);   return(0); }
   if(object == (APTR)MenuLS1)      { SetLineDash(object);     return(0); }
   if(object == (APTR)MenuLS2)      { SetLineDash(object);     return(0); }
   if(object == (APTR)MenuLS3)      { SetLineDash(object);     return(0); }
   if(object == (APTR)MenuLS4)      { SetLineDash(object);     return(0); }
   if(object == (APTR)MenuLS5)      { SetLineDash(object);     return(0); }
   if(object == (APTR)MenuLS6)      { SetLineDash(object);     return(0); }
   if(object == (APTR)MenuFT1)      { SetFillType(object);     return(0); }
   if(object == (APTR)MenuFT2)      { SetFillType(object);     return(0); }
   if(object == (APTR)MenuFT3)      { SetFillType(object);     return(0); }
   if(object == (APTR)MenuFT4)      { SetFillType(object);     return(0); }
   if(object == (APTR)MenuFT5)      { SetFillType(object);     return(0); }
   if(object == (APTR)MenuFT6)      { SetFillType(object);     return(0); }
   if(object == (APTR)MenuNoFills)  { RemoveFills(object);     return(0); }

   return(0);
}

/***************************************************************************/
/*>OffPieMenus(void)
   -----------------
   Switch off menus not applicable to pie charts
   12.03.92 Framework
   10.07.92 Original
*/
OffPieMenus(void)
{
   OffMenu(MainWind, MenuNumErrors);
   OffMenu(MainWind, MenuNumLogX);
   OffMenu(MainWind, MenuNumLogY);
   OffMenu(MainWind, MenuNumGrid);
   OffMenu(MainWind, MenuNumFZero);
   OffMenu(MainWind, MenuNumAxTitle);
   OffMenu(MainWind, MenuNumPen);
   OffMenu(MainWind, MenuNumLStyle);
   OffMenu(MainWind, MenuNumFit);

   return(0);
}

/***************************************************************************/
/*>OnPieMenus(void)
   ----------------
   Switch on menus not applicable to pie charts
   12.03.92 Framework
   10.07.92 Original
*/
OnPieMenus(void)
{
   OnMenu(MainWind, MenuNumErrors);
   OnMenu(MainWind, MenuNumLogX);
   OnMenu(MainWind, MenuNumLogY);
   OnMenu(MainWind, MenuNumGrid);
   OnMenu(MainWind, MenuNumFZero);
   OnMenu(MainWind, MenuNumAxTitle);
   OnMenu(MainWind, MenuNumPen);
   OnMenu(MainWind, MenuNumLStyle);
   OnMenu(MainWind, MenuNumFit);

   return(0);
}

/***************************************************************************/
/*>ClearWindow(struct Window *wind)
   --------------------------------
   Clears the specified RastPort with a rectfill
   06.04.92 Original
*/
ClearWindow(struct Window *wind)
{
   SetAPen(wind->RPort,0);
   RectFill(wind->RPort,
            wind->BorderLeft,
            wind->BorderTop,
            wind->Width  - wind->BorderRight,
            wind->Height - wind->BorderBottom);
   return(0);
}

/***************************************************************************/
/*>ProcessToolTypes(void)
   ----------------------
   Alters defaults based on icon tool types. Handles the following
   tooltypes:
   
      Size=<x> <y>                  Output dimensions
      Offset=<x> <y>                Output offset
      HPGLMargin=<x> <y>            HPGL paper margins
      Title=<Font> <Size>           Title font and size
      XTitle=<Font> <Size>          X-axis title font and size 
      YTitle=<Font> <Size>          Y-axis title font and size 
      XLabel=<Font> <Size>          X-axis label font and size 
      YLabel=<Font> <Size>          Y-axis label font and size
      Key=<Font> <Size>             Key font and size
      Extralabel=<Font> <Size>      Extra label font and size
      Hatching=ON|OFF               Use hatching or shading
      HatchControl=<thick> <space>  Thickness and spacing of hatching
      Pen0=<R> <G> <B>              Pen colours
      Pen1=<R> <G> <B>
      Pen2=<R> <G> <B>
      Pen3=<R> <G> <B>
      DIR=<directory>               Default directory
      NOSHANGHAI                    Stops editor SHANGHAI under V2.0
      AXISBOUNDS                    EPSF bounding box set to size given by paper
   
   01.07.92 Original
   02.07.92 Added pen colours. Also rebuilds windows if colours have been
            set.
   06.07.92 Added default directory.
   04.08.92 Added NOSHANGHAI
   15.09.92 Added AXISBOUNDS
*/
ProcessToolTypes(void)
{
   struct DiskObject *DiskObj = NULL;
   char   *valptr,
          string[80];
   double value,
          value2;
   int    ival1,
          ival2,
          ival3,
          redowindows = FALSE;
   
   
   if((DiskObj = (struct DiskObject *)
                 GetDiskObject(WBenchMsg->sm_ArgList->wa_Name)) != NULL)
   {
      /* Tooltype: Size=<x> <y>                       */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"Size")) != NULL)
         if(sscanf(valptr,"%lf %lf",&value,&value2) == 2)
         {
            output.xsize   = value;
            output.ysize   = value2;
         }
      
      /* Tooltype: Offset=<x> <y>                     */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"Offset")) != NULL)
         if(sscanf(valptr,"%lf %lf",&value,&value2) == 2)
         {
            output.xoffset = value;
            output.yoffset = value2;
         }
      
      /* Tooltype: HPGLMargin=<x> <y>                 */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"HPGLMargin")) != NULL)
         if(sscanf(valptr,"%lf %lf",&value,&value2) == 2)
         {
            output.xmargin = value;
            output.ymargin = value2;
         }

      /* Tooltype: Title=<font> <size>                */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"Title")) != NULL)
         if(sscanf(valptr,"%s %lf",string,&value) == 2) 
         {
            strcpy(GTitle.PSFont,string);
            GTitle.PSSize = value;
         }

      /* Tooltype: XAxTitle=<font> <size>             */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"XTitle")) != NULL)
         if(sscanf(valptr,"%s %lf",string,&value) == 2) 
         {
            strcpy(XTitle.PSFont,string);
            XTitle.PSSize = value;
         }

      /* Tooltype: YAxTitle=<font> <size>             */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"YTitle")) != NULL)
         if(sscanf(valptr,"%s %lf",string,&value) == 2) 
         {
            strcpy(YTitle.PSFont,string);
            YTitle.PSSize = value;
         }

      /* Tooltype: XLabel=<font> <size>               */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"XLabel")) != NULL)
         if(sscanf(valptr,"%s %lf",string,&value) == 2) 
         {
            strcpy(XLabel.PSFont,string);
            XLabel.PSSize = value;
         }

      /* Tooltype: YLabel=<font> <size>               */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"YLabel")) != NULL)
         if(sscanf(valptr,"%s %lf",string,&value) == 2) 
         {
            strcpy(YLabel.PSFont,string);
            YLabel.PSSize = value;
         }

      /* Tooltype: Key=<font> <size>                  */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"Key")) != NULL)
         if(sscanf(valptr,"%s %lf",string,&value) == 2) 
         {
            strcpy(Defaults.KeyPSFont,string);
            Defaults.KeyPSSize = value;
         }

      /* Tooltype: ExtraLabel=<font> <size>           */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"ExtraLabel")) != NULL)
         if(sscanf(valptr,"%s %lf",string,&value) == 2) 
         {
            strcpy(Defaults.LabelPSFont,string);
            Defaults.LabelPSSize = value;
         }

      /* Tooltype: Hatching=ON|OFF                    */
      if(MatchToolValue(FindToolType(DiskObj->do_ToolTypes,"Hatching"),"ON"))
         output.hatching = TRUE;
      if(MatchToolValue(FindToolType(DiskObj->do_ToolTypes,"Hatching"),"OFF"))
         output.hatching = FALSE;

      /* Tooltype: HatchControl=<thickness> <spacing> */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"HatchControl")) != NULL)
         if(sscanf(valptr,"%lf %lf",&value,&value2) == 2) 
         {
            output.hatchthick = value;
            output.hatchspace = value2;
         }

      /* Tooltype: Pen0=<R> <G> <B>                   */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"Pen0")) != NULL)
         if(sscanf(valptr,"%d %d %d",&ival1,&ival2,&ival3) == 3) 
         {
            ival1 = RANGECHECK(ival1,0,15);
            ival2 = RANGECHECK(ival2,0,15);
            ival3 = RANGECHECK(ival3,0,15);
            SetRGB4(&MyScreen->ViewPort,0,(UBYTE)ival1,(UBYTE)ival2,(UBYTE)ival3);
            redowindows = TRUE;
         }

      /* Tooltype: Pen1=<R> <G> <B>                   */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"Pen1")) != NULL)
         if(sscanf(valptr,"%d %d %d",&ival1,&ival2,&ival3) == 3) 
         {
            ival1 = RANGECHECK(ival1,0,15);
            ival2 = RANGECHECK(ival2,0,15);
            ival3 = RANGECHECK(ival3,0,15);
            SetRGB4(&MyScreen->ViewPort,1,(UBYTE)ival1,(UBYTE)ival2,(UBYTE)ival3);
            redowindows = TRUE;
         }

      /* Tooltype: Pen2=<R> <G> <B>                   */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"Pen2")) != NULL)
         if(sscanf(valptr,"%d %d %d",&ival1,&ival2,&ival3) == 3) 
         {
            ival1 = RANGECHECK(ival1,0,15);
            ival2 = RANGECHECK(ival2,0,15);
            ival3 = RANGECHECK(ival3,0,15);
            SetRGB4(&MyScreen->ViewPort,2,(UBYTE)ival1,(UBYTE)ival2,(UBYTE)ival3);
            redowindows = TRUE;
         }

      /* Tooltype: Pen3=<R> <G> <B>                   */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"Pen3")) != NULL)
         if(sscanf(valptr,"%d %d %d",&ival1,&ival2,&ival3) == 3) 
         {
            ival1 = RANGECHECK(ival1,0,15);
            ival2 = RANGECHECK(ival2,0,15);
            ival3 = RANGECHECK(ival3,0,15);
            SetRGB4(&MyScreen->ViewPort,3,(UBYTE)ival1,(UBYTE)ival2,(UBYTE)ival3);
            redowindows = TRUE;
         }

      /* Tooltype: DIR=<directory>                    */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"DIR")) != NULL)
         strcpy(Defaults.directory,valptr);

      /* Tooltype: NOSHANGHAI                         */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"NOSHANGHAI")) != NULL)
         display.shanghai = FALSE;

      /* Tooltype: AXISBOUNDS                         */
      if((valptr = FindToolType(DiskObj->do_ToolTypes,"AXISBOUNDS")) != NULL)
         output.fixbounds = FALSE;

      FreeDiskObject(DiskObj);

/*
//      if(redowindows)
//      {
//         FreeAllGadgets();
//         BuildAllWindows();
//      }
*/
   }
   return(0);
}


/**************************************************************************/
/*>GetScreenSizes(int *ScrHeight, int *ScrWidth)
   ---------------------------------------------
   This routine gets the screen sizes from the WorkBench and returns
   interlaced value if the WB was not interlaced.
   01.07.92 Original based on code from MoG.
*/
GetScreenSizes(int *ScrHeight,
               int *ScrWidth)
{
   struct Screen ScreenBuffer;
   
   GetScreenData(&ScreenBuffer,sizeof(struct Screen), WBENCHSCREEN, NULL);

   /* Get the height of the WB screen */
   *ScrHeight     = ScreenBuffer.Height;

   /* If it wasn't interlace, make it so */
   if(*ScrHeight < 300) (*ScrHeight) *= 2;
   
   /* Now get the width; V1.3 returns this as 0 */
   *ScrWidth = ScreenBuffer.Width;
   if(*ScrWidth == 0)   (*ScrWidth)   = 640;
   
   return(0);
}

/**************************************************************************/
/*>UpdateChecks(void)
   ------------------
   Ensures that the menu check marks match the actual status of the
   appropriate flags.
   24.07.92 Original
   13.08.92 Added correct style ticking (if style reselected).
   09.09.92 Added regression, fourier and rexxfit
*/
UpdateChecks(void)
{
   if(graph.logx)          Check(MenuLogX);
   else                    Uncheck(MenuLogX);

   if(graph.logy)          Check(MenuLogY);
   else                    Uncheck(MenuLogY);
   
   if(graph.errors)        Check(MenuErrors);
   else                    Uncheck(MenuErrors);
   
   if(graph.boxed)         Check(MenuBoxed);
   else                    Uncheck(MenuBoxed);
   
   if(graph.grid)          Check(MenuGrid);
   else                    Uncheck(MenuGrid);

   if(graph.fzero)         Check(MenuFZero);
   else                    Uncheck(MenuFZero);
   
   if(rexx.debug)          Check(MenuDebRexx);
   else                    Uncheck(MenuDebRexx);

   if(fitting.robust)      Check(MenuRobust);
   else                    Uncheck(MenuRobust);
   
   if(fitting.regression)  Check(MenuRegress);
   else                    Uncheck(MenuRegress);
   
   if(fitting.fourier)     Check(MenuFourier);
   else                    Uncheck(MenuFourier);
   
   if(fitting.rexx)        Check(MenuRexxFit);
   else                    Uncheck(MenuRexxFit);

   Uncheck(MenuXY);
   Uncheck(MenuScatter);
   Uncheck(MenuBar);
   Uncheck(MenuPie);
   
   switch(graph.style)
   {
      case STYLE_LINE:     Check(MenuXY);       break;
      case STYLE_SCATTER:  Check(MenuScatter);  break;
      case STYLE_BAR:      Check(MenuBar);      break;
      case STYLE_PIE:      Check(MenuPie);      break;
      default:                                  break;
   }

   return(0);
}
