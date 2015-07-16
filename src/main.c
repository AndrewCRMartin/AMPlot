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

#ifdef AMIGA
#include <intuition/intuitionbase.h>
#include <graphics/display.h>
#include <workbench/startup.h>
#include <proto/icon.h>
#else
#include <unistd.h>
#endif

/***************************************************************************/
/* Rexx externs
*/
#ifdef REXX_SUPPORT
extern char *Rexx_return;
#endif

/***************************************************************************/
/* Other externals
*/
#ifdef AMIGA
extern struct WBStartup *WBenchMsg;   /* Used by ProcessToolTypes() to get 
                                         program name                      */
#endif

/***************************************************************************/
/*>int main()
   ------
   AMPlot main program. Opens libraries, sets up parser and defaults, opens
   the display and calls the IDCMP loop.
   12.03.92 Original
   14.04.92 Moved SetDefaults() to after opening display.
   29.04.92 Added Rexx stuff
   05.05.92 Added CLI check
   01.07.92 Changed call to SetDefaults() to use display.FromCLI
   22.07.92 Added ShowFile().
*/
int main(int  argc,
         char **argv)
{
   if(argc > 0)                     /* Flag for running from CLI           */
      display.FromCLI = TRUE;
   else
      display.FromCLI = FALSE;

#ifdef REXX_SUPPORT
   strcpy(rexx.PortName,"AMPlot");  /* Default ARexx port name             */
   rexx.gotrexx     = FALSE;        /* To start, we don't have ARexx       */
   rexx.fromrexx    = FALSE;        /* Commands aren't from Rexx           */
   rexx.quitprogram = FALSE;        /* Flag to exit from Rexx              */
   rexx.empty       = FALSE;        /* Flag for empty graph from Rexx      */
   rexx.debug       = FALSE;        /* Flag for debugging Rexx             */
   rexx.rexxplot    = FALSE;        /* Plot hasn't been called from Rexx   */
   rexx.autoreplot  = TRUE;         /* We'll normally want auto replot     */
#endif
   
   OpenLibraries();                 /* Open and Amiga libraries            */
   InitParser();                    /* Initialise command parser           */
   OpenDisplay();                   /* Open the Amiga display              */
   SetDefaults(display.FromCLI);    /* Setup defaults and read tooltypes   */
#ifdef REXX_SUPPORT
   InitRexx();                      /* Start the ARexx interface           */
#endif

   if(argc==2)                      /* Show initial file                   */
   {
      LoadFileByName(argv[1],0);
#ifdef AMIGA
      DoPlot(DEST_SCREEN, NULL);
#else
      DoPlot(DEST_PS, stdout);
#endif
   }
#ifdef AMIGA
   IDCMPLoop();                     /* Main IDCMP loop                     */
#endif
   Cleanup();                       /* Clean up and close everything       */

   return(0);
}

#ifdef AMIGA
/***************************************************************************/
/*>IDCMPLoop(void)
   ---------------
   The main IDCMP loop which waits for the user to interact with the AMPlot
   window.
   12.03.92 Original
   29.04.92 Added wait on REXX
   12.06.92 Added clear of rexx.replot flag
   02.07.92 Corrected replot destination (not always screen!). Clear
            rexx.fromrexx when a menu event occurs.
   09.07.92 Added check of rexx.autoreplot and rexx.forcereplot flags
*/
int IDCMPLoop(void)
{
   struct IntuiMessage  *message;
   ULONG                Signals;
   ULONG                MIClass;
   APTR                 MIAddress;
   USHORT               MICode;
   int                  MouseX,
                        MouseY;

   for(;;)
   {
      Signals = Wait(1<<MainWind->UserPort->mp_SigBit | RXIL_WAITFLAG);
      
      if(FlagIsSet(Signals, RXIL_WAITFLAG))
      {
         rexx.replot      = FALSE;
         rexx.forcereplot = FALSE;
         RxilCheckPort();

         ProcessReturns();
         
         if(Rexx_return != NULL)
         {
            ReqMessage2(MainWind,"Function macro returned:",Rexx_return,0);

            /* Notify that we've used the return value */
            RexxReturnUsed();
         }

         if(rexx.quitprogram) QuitProgram((APTR) 1);

         if((rexx.replot && rexx.autoreplot) || rexx.forcereplot)
         {
            if(output.plottype == DEST_SCREEN) ClearWindow(MainWind);
            DoPlot(output.plottype,output.fh);
            rexx.replot       = FALSE;
            rexx.forcereplot  = FALSE;
         }
      }
      else
      {
         while((message=(struct IntuiMessage *)GetMsg(MainWind->UserPort))!=NULL)
         {
            MIClass     = message->Class;
            MICode      = message->Code;
            MIAddress   = message->IAddress;
            MouseX      = message->MouseX;
            MouseY      = message->MouseY;
   
            ReplyMsg((struct Message *)message);
   
            rexx.fromrexx = FALSE;

            switch(MIClass)
            {
            case IDCMP_GADGETUP:
               if(MIAddress)
               {
                  HandleSTSGadget(MainWind, (struct Gadget *)MIAddress);
                  HandleGadget((struct Gadget *)MIAddress);
               }
               break;
            case IDCMP_MENUPICK:
               if(MICode) HandleMenu(MICode);
               break;
            case IDCMP_CLOSEWINDOW:
               QuitProgram(NULL);
               break;
            case IDCMP_MOUSEBUTTONS:
               /* We're not interested in these at this point. */
               break;
            default:
#ifdef DEBUG
               printf("Unhandled IDCMP message\n");
#endif
               break;
            }
         }
      }
   }
   return(0);
}
#endif

/***************************************************************************/
/*>Cleanup(void)
   -------------
   Remove menu, close window, free memory for menus and gadgets, close
   libraries.
   12.03.92 Original
   05.04.92 Added Close AslBase
   06.04.92 Added Close MyScreen
   08.04.92 Moved STS cleanup out of if(MainWind)
   16.04.92 Explicitly free allocated memory for lists
   05.05.92 Added Close DiskfontBase
   06.05.92 Added free() of FontTable[], call to FreeAmigaFonts(), 
            and FreeRaster()
   23.06.92 Added Close Dos library
   01.07.92 Added Close Icon library. Added remove message window.
   09.07.92 Added Close plot file
   13.07.92 Added casts to CloseLibrary() calls
   16.07.92 Added Close iffparse library.
*/
int Cleanup(void)
{
   DATASET  *d;
   int      j;
   
#ifdef AMIGA
   if(output.fh)     Close((BPTR)output.fh);
#else
   if(output.fh)     fclose(output.fh);
#endif
#ifdef REXX_SUPPORT
   if(rexx.gotrexx)  CloseREXX(MainWind,0);
#endif

#ifdef AMIGA
   RemoveMessageWindow();

   if(MainWind)
   {
      ClearMenuStrip(MainWind);
      CloseWindow(MainWind);
      MainWind = NULL;
   }
   if(MyScreen)
   {
      CloseScreen(MyScreen);
      MyScreen = NULL;
   }

   FreeAllGadgets();
   if(MenuStrip) FreeSTSMenus(MenuStrip);

   FreeAmigaFonts();
   
   if(TBuf) FreeRaster(TBuf,RASX,RASY);

   if(IconBase)
   {
      CloseLibrary((struct Library *)IconBase);
      IconBase = NULL;
   }
   if(DosBase)
   {
      CloseLibrary((struct Library *)DosBase);
      DosBase = NULL;
   }
   if(GfxBase)
   {
      CloseLibrary((struct Library *)GfxBase);
      GfxBase = NULL;
   }
   if(DiskfontBase)
   {
      CloseLibrary((struct Library *)DiskfontBase);
      DiskfontBase = NULL;
   }
   if(IntuitionBase)
   {
      CloseLibrary((struct Library *)IntuitionBase);
      IntuitionBase = NULL;
   }
   if(AslBase)
   {
      CloseLibrary((struct Library *)AslBase);
      AslBase = NULL;
   }
   if(IFFParseBase)
   {
      CloseLibrary((struct Library *)IFFParseBase);
      IFFParseBase = NULL;
   }
#endif
   
   /* Free memory for extra labels */
   FREELIST(labels,TEXTLIST);

   /* Free allocated memory for data */
   for(d=data; d!=NULL; NEXT(d))
   {
      if(d->x)    free(d->x);
      if(d->y)    free(d->y);
      if(d->ep)   free(d->ep);
      if(d->em)   free(d->em);
   }
   FREELIST(data,DATASET);
   data = NULL;
   
   /* Free allocated memory for font translation table */
   for(j=0; j<MAXTRANS; j++)
   {
      if(FontTable[j].PSFont)    free(FontTable[j].PSFont);
      if(FontTable[j].AmigaFont) free(FontTable[j].AmigaFont);
   }

   return(0);
}

/***************************************************************************/
/*>OpenLibraries(void)
   -------------------
   Open Amiga libraries.
   12.03.92 Original
   05.05.92 Added setting of OSVersion variable and font library
   23.06.92 Added Dos library
   01.07.92 Added icon library
   16.07.93 Added iffparse library
*/
int OpenLibraries(void)
{
#ifdef AMIGA
   if ((IntuitionBase=(struct IntuitionBase *)
               OpenLibrary("intuition.library",INTUITION_REV)) == NULL)
               Die("Unable to open Intuition library");

   if ((GfxBase=(struct GfxBase *)
               OpenLibrary("graphics.library",GRAPHICS_REV)) == NULL)
               Die("Unable to open Graphics library");

   if ((DosBase=(struct DosBase *)
               OpenLibrary("dos.library",DOS_REV)) == NULL)
               Die("Unable to open Dos library");

   if ((IconBase=(struct Library *)
               OpenLibrary("icon.library",0)) == NULL)
               Die("Unable to open Icon library");

   if ((DiskfontBase=(struct DiskfontBase *)
               OpenLibrary("diskfont.library",DISKFONT_REV)) == NULL)
               Die("Unable to open Diskfont library");

   AslBase=(struct Library *)OpenLibrary("asl.library",ASL_REV);
   /* We don't make any checks as AslBase isn't essential */

   IFFParseBase=(struct Library *)OpenLibrary("iffparse.library",0L);
   /* We don't make any checks as iffparse isn't essential */


   display.OSVersion = IntuitionBase->LibNode.lib_Version;

#endif

   return(0);
}

/***************************************************************************/
/*>OpenDisplay(void)
   -----------------
   Open the main window, build and attach menus, build the requester 
   NewWindow structures.
   12.03.92 Original
   06.04.92 Added screen opening
   06.05.92 Added init of fill/flood raster
   01.07.92 Added message window, new sizing and centering of display
   17.07.92 Added OffMenu() for DR2D if no iffparse.library
   22.07.92 Added setting of display.shanghai if public screen opened
*/
int OpenDisplay(void)
{
#ifdef AMIGA
   struct   NewWindow      *nw;
   int                     ScrHeight,
                           ScrWidth,
                           ConTop;

   /************* Screen *************/
   struct NewScreen NewScreenStructure = {
   	0,0,	         /* screen XY origin relative to View      */
   	640,512,       /* screen width and height                */
   	2,	            /* screen depth (number of bitplanes)     */
   	0,1,	         /* detail and block pens                  */
   	LACE|HIRES,    /* display modes for this screen          */
   	CUSTOMSCREEN,	/* screen type                            */
   	NULL,	         /* pointer to default screen font         */
   	NULL,	         /* screen title                           */
   	NULL,	         /* first in list of custom screen gadgets */
   	NULL	         /* pointer to custom BitMap structure     */
   };


   /* Get WB display size to modify height */
   GetScreenSizes(&ScrHeight, &ScrWidth);
   NewScreenStructure.Height = ScrHeight;

   /* Set up our display structure to store this info */
   display.WXSize    = 640;
   display.WYSize    = 400;
   display.NColours  = 4;
   display.shanghai  = FALSE;

   /* Open as a public screen if this is V2.0 or above */
   if(display.OSVersion >= 36)
   {
      UWORD PenArray[] = {~0};

      /* Centre the screen */
      NewScreenStructure.LeftEdge = (int)((ScrWidth-display.WXSize)/2);

      if((MyScreen = (struct Screen *)OpenScreenTags(&NewScreenStructure,
                                                     SA_PubName,PUBSCREEN,
                                                     SA_Pens,PenArray,
                                                     TAG_DONE))==NULL)
      {
         /* Try to open the screen as private */
         if((MyScreen = (struct Screen *)OpenScreenTags(&NewScreenStructure,
                                                        SA_Pens,PenArray,
                                                        TAG_DONE))==NULL)

         {
            Die("Unable to open screen");
         }
      }
      else
      {
         /* We managed to open a public type screen, so make it public */
         PubScreenStatus(MyScreen, 0L);
         display.shanghai = TRUE;
      }
   }
   else
   {
      if((MyScreen = (struct Screen *)OpenScreen(&NewScreenStructure))==NULL)
      {
         Die("Unable to open screen");
      }
   }
   
   ShowTitle(MyScreen,FALSE);

   /* Calculate top position of message window */
   ConTop = display.WYSize;
   if(ConTop + 100 > ScrHeight)
      ConTop = ScrHeight-100;

   /* Build and open main window */
#ifdef DEMO_VERSION
   nw = BuildWindow(MyScreen,0,0,display.WXSize,display.WYSize,
                    "AMPlot2 Demo Version ©SciTech Software, 1992",0,1);
#else
   nw = BuildWindow(MyScreen,0,0,display.WXSize,display.WYSize,
                    "AMPlot2 ©SciTech Software, 1992",0,1);
#endif
   nw->IDCMPFlags |= IDCMP_CLOSEWINDOW;
   nw->IDCMPFlags |= IDCMP_MENUPICK;
   nw->IDCMPFlags |= IDCMP_MOUSEBUTTONS;
   /* Stop us from dragging the window */
   nw->Flags &= ~WFLG_DRAGBAR;

   if((MainWind = OpenWindow(nw)) == NULL)
      Die("Unable to open window");

   /* Create message window */
#ifdef DEMO_VERSION
   if(CreateMessageWindow(MyScreen,"AMPlot2 Demo Version: Messages","AMPlot.con",
                          0,ConTop,display.WXSize,100))
      RemoveMessageWindow();
#else
   if(CreateMessageWindow(MyScreen,"AMPlot2: Messages","AMPlot.con",
                          0,ConTop,display.WXSize,100))
      RemoveMessageWindow();
#endif
      
   /* Set up other stuff for main window */
   rp = MainWind->RPort;
   
   /* Set up Area and Temp Raster for Fills */
   InitArea(&AInfo,AreaBuf,(int)(ABUFFSIZE/2.5));
   rp->AreaInfo = &AInfo;
   if((TBuf = (PLANEPTR)AllocRaster(RASX,RASY)) == NULL)
      Die("No memory for fill raster.");

   rp->TmpRas = (struct TmpRas *)InitTmpRas(&TRas,TBuf,RASSIZE(RASX,RASY));
      
   /* Set up menus and attach to window */
   MenuStrip = BuildAllMenus(MyScreen);
   SetMenuStrip(MainWind, MenuStrip);
   
   /* Switch off the IFF-DR2D menu item if we don't have iffparse.library  */
   if(IFFParseBase == NULL) OffMenu(MainWind, MenuNumDR2D);

#ifdef DEMO_VERSION
   OffMenu(MainWind,MenuNumPlot);
#endif
   
   /* Set up requester windows */
   BuildAllWindows(MyScreen);
   
   /* Text, etc in JAM1 mode */
   SetDrMd(rp,JAM1);

#endif /* AMIGA */

   return(0);
}

/***************************************************************************/
/*>HandleMenu(USHORT firstcode)
   ----------------------------
   Handle linked list of menu selections.
   12.03.92 Original
   06.04.92 Calls DoPlot() on screen after each menu activity
   02.07.92 Moved ClearWindow() to AMInitPlot(). Calls RefreshScreen()
            rather than DoPlot() directly
   24.07.92 Added UpdateChecks()
*/
int HandleMenu(USHORT firstcode)
{
#ifdef AMIGA
   struct MenuItem *item;
   USHORT nextcode = firstcode;

   /* If nothing actually selected, just return */
   if(firstcode == MENUNULL) return(0);

   /* Walk the linked list of menu actions */
   while(nextcode != MENUNULL)
   {
      item     = (struct MenuItem *)ItemAddress(MenuStrip, (long)nextcode);
      HandleEvent((APTR)item);
      nextcode = item->NextSelect;
   }
   
   /* Update the check marks in case of multi-selection */
   UpdateChecks();
   
   /* Replot the graph */
   RefreshScreen();
   
#endif
   return(0);
}

/***************************************************************************/
/*>RefreshScreen(void)
   -------------------
   Calls DoPlot() on the screen
   02.07.92 Original
*/
int RefreshScreen(void)
{
#ifdef AMIGA
   output.plottype = DEST_SCREEN;
   output.fh       = NULL;
   
   DoPlot(DEST_SCREEN,NULL);
#endif
   return(0);
}

/***************************************************************************/
/*>HandleGadget(struct Gadget *gadget)
   -----------------------------------
   Dispatch gadget messages.
   12.03.92 Original
*/
int HandleGadget(struct Gadget *gadget)
{
   return(0);
}

/***************************************************************************/
/*>SetDefaults(int NoTooltypes)
   ----------------------------
   Set default values for everything then parse Tooltypes
   12.03.92 Original
   30.03.92 Various additions
   31.03.92 Various additions
   01.04.92 Various additions, added DoTooltypes parameter
   02.04.92 Various additions
   03.04.92 Various additions, init extra labels list
   07.04.92 Various additions
   29.04.92 Added userprec
   01.05.92 Added rexx fitting defaults
   07.05.92 Added EPSF, changed tick lengths
   08.05.92 Added alternate font
   08.06.92 Added setting fill list
   12.06.92 Added PS hatch support
   17.06.92 Added piestyle init
   01.07.92 Swapped logic of parameter (NoToolTypes rather than DoToolTypes).
            Added call to ProcessToolTypes()
   03.07.92 Added bits found from implementing file commands.
   06.07.92 Added directory.
   09.07.92 Set output.fh = NULL
   22.07.92 Changed default shrink to 20% (as AMPlot V1.0)
   24.07.92 Added graph.fzero and call to UpdateChecks(). Disable Pie menu
            item by default; re-enabled if one data column.
   30.07.92 Added blanking of ARexx macro name.
   04.08.92 Added setting of graph.boxed, graph.grid, graph.(x|y)key,
            graph.(X|Y|XP|YP)Scale, graph.usekey, graph.usertitlepos,
            graph.nplots, graph.maxdata, piestyle.D3, Defaults.Key(X|Y),
            rexx.(various)
   13.08.92 Now switches off pie setup as well as pie
   15.09.92 Added output.fixbounds
*/
int SetDefaults(int NoTooltypes)
{
   int         j;
   STATICTEXT  atext;
   
   /* Graph */
   graph.xmin           = 0.0;                  /* Graph dimensions        */
   graph.xmax           = 0.0;
   graph.ymin           = 0.0;
   graph.ymax           = 0.0;
   graph.xtick          = 0.0;                  /* Tick spacing            */
   graph.ytick          = 0.0;
   graph.xsub           = 0.0;                  /* Subtick spacing         */
   graph.ysub           = 0.0;
   graph.xkey           = 0.0;
   graph.ykey           = 0.0;
   graph.ebarwidth      = 5;                    /* Error bar width (pt)    */
   graph.dpsize         = 12;                   /* Size of datapoint marks */
   graph.XScale         = 1.0;
   graph.YScale         = 1.0;
   graph.XPScale        = 1.0;
   graph.YPScale        = 1.0;
   graph.style          = STYLE_LINE;
   graph.errors         = FALSE;                /* Error bars              */
   graph.columns        = 2;                    /* Number of columns       */
   graph.logx           = FALSE;
   graph.logy           = FALSE;
   graph.boxed          = FALSE;
   graph.grid           = FALSE;
   graph.fzero          = FALSE;
   graph.noxlabels      = FALSE;
   graph.noylabels      = FALSE;
   graph.centrexlabels  = FALSE;
   graph.xprecision     = 0;
   graph.yprecision     = 0;
   graph.usekey         = FALSE;
   graph.userbounds     = FALSE;
   graph.userticks      = FALSE;
   graph.usersub        = FALSE;
   graph.usertitlepos   = FALSE;
   graph.userprec       = FALSE;
   graph.clip           = TRUE;                 /* Clip to bounds          */
   graph.subtick        = FALSE;
   graph.CurrentKey     = 0;                    /* Current dataset for key */
   graph.nplots         = 0;
   graph.nlabels        = 0;                    /* Number of `extra' labels*/
   graph.CurrentLabel   = 0;                    /* Current `extra' label   */
   graph.maxdata        = 0;

   /* Associated menu items */
#ifdef AMIGA
   Check(MenuXY);
   Uncheck(MenuScatter);
   Uncheck(MenuBar);
   Uncheck(MenuPie);
#endif
   
   /* Defaults */
   Defaults.KeyPSSize         = 10.0;              /* Key PostScript size  */
   Defaults.LabelPSSize       = 10.0;              /* Lab PostScript size  */
   Defaults.KeyX              = 0.0;
   Defaults.KeyY              = 0.0;
   Defaults.KeyAmigaSize      = 10;                /* Key Amiga size       */
   Defaults.LabelAmigaSize    = 10;                /* Lab Amiga size       */
   strcpy(Defaults.KeyPSFont,     "Times-Roman");  /* Key PostScript font  */
   strcpy(Defaults.LabelPSFont,   "Times-Roman");  /* Lab PostScript font  */
   strcpy(Defaults.KeyAmigaFont,  "Times");        /* Key Amiga font       */
   strcpy(Defaults.LabelAmigaFont,"Times");        /* Lab Amiga font       */
#ifdef AMIGA
   getcd(0,Defaults.directory);                    /* Default directory    */
#else
   getcwd(Defaults.directory, MAXBUFF);
#endif

   /* Paper */
   output.xsize            = 4.625;
   output.ysize            = 3.0;
   output.xoffset          = 2.5;
   output.yoffset          = 7.4;
   output.ticklen          = 5.0;
   output.subticklen       = 2.0;
   output.hatchthick       = 0.5;
   output.hatchspace       = 2.0;
   output.xmargin          = 0.58;              /* Paper margin for HPGL   */
   output.ymargin          = 0.1465;
   output.fh               = NULL;
   output.plottype         = DEST_SCREEN;
   output.hatching         = FALSE;
   output.filename[0]      = '\0';
   output.fixbounds        = TRUE;
   EPSF                    = FALSE;

   
   /* Pens */
   pens[0].colour       = 1;                    /* Plot colour             */
   pens[1].colour       = 2;
   pens[2].colour       = 3;
   pens[3].colour       = 2;
   pens[4].colour       = 3;
   pens[5].colour       = 1;
   pens[0].screencolour = 1;                    /* Screen colour           */
   pens[1].screencolour = 2;
   pens[2].screencolour = 3;
   pens[3].screencolour = 2;
   pens[4].screencolour = 3;
   pens[5].screencolour = 1;
   pens[0].thickness    = 1.0;                  /* Plot thickness          */
   pens[1].thickness    = 0.5;
   pens[2].thickness    = 1.5;
   pens[3].thickness    = 2.0;
   pens[4].thickness    = 0.25;
   pens[5].thickness    = 1.0;
   pens[0].mark         = 0;                    /* Mark number             */
   pens[1].mark         = 0;
   pens[2].mark         = 0;
   pens[3].mark         = 0;
   pens[4].mark         = 0;
   pens[5].mark         = 0;
   pens[0].join         = FALSE;                /* Join scatter            */
   pens[1].join         = FALSE;
   pens[2].join         = FALSE;
   pens[3].join         = FALSE;
   pens[4].join         = FALSE;
   pens[5].join         = FALSE;
   
   /* Barchart style details */
   barstyle.shrink      = 200.0/20.0;           /* 20% shrink factor       */
   barstyle.style       = STYLE_OVERLAYED;
   barstyle.shrinkall   = FALSE;
   barstyle.fillall     = TRUE;
   barstyle.D3          = FALSE;
   barstyle.nbar        = 0;
   barstyle.CurrentBar  = 0;                    /* Current bar for labels  */
   barstyle.uselabels   = FALSE;                /* Use bar labels          */

   /* Graph title */
   GTitle.text[0]       = '\0';                 /* Times-Roman 14pt        */
   GTitle.x             = 0.0;
   GTitle.y             = 0.0;
   GTitle.PSSize        = 14;
   GTitle.AmigaSize     = 14;
   strcpy(GTitle.PSFont,   "Times-Roman");
   strcpy(GTitle.AmigaFont,"Times");
   
   /* X-axis title */
   XTitle.text[0]       = '\0';                 /* Times-Roman 10pt        */
   XTitle.x             = 0.0;
   XTitle.y             = 0.0;
   XTitle.PSSize        = 10;
   XTitle.AmigaSize     = 10;
   strcpy(XTitle.PSFont,   "Times-Roman");
   strcpy(XTitle.AmigaFont,"Times");
   
   /* Y-axis title */
   YTitle.text[0]       = '\0';                 /* Times-Roman 10pt        */
   YTitle.x             = 0.0;
   YTitle.y             = 0.0;
   YTitle.PSSize        = 10;
   YTitle.AmigaSize     = 10;
   strcpy(YTitle.PSFont,   "Times-Roman");
   strcpy(YTitle.AmigaFont,"Times");
   
   /* X-axis title */
   XLabel.text[0]       = '\0';                 /* Times-Roman 10pt        */
   XLabel.x             = 0.0;
   XLabel.y             = 0.0;
   XLabel.PSSize        = 10;
   XLabel.AmigaSize     = 10;
   strcpy(XLabel.PSFont,   "Times-Roman");
   strcpy(XLabel.AmigaFont,"Times");
   
   /* Y-axis title */
   YLabel.text[0]       = '\0';                 /* Times-Roman 10pt        */
   YLabel.x             = 0.0;
   YLabel.y             = 0.0;
   YLabel.PSSize        = 10;
   YLabel.AmigaSize     = 10;
   strcpy(YLabel.PSFont,   "Times-Roman");
   strcpy(YLabel.AmigaFont,"Times");
   
   /* Fitting */
   fitting.smooth_pts      = 5.0;
   fitting.regress_tol     = 0.00001;
   fitting.rexx_tol        = 0.00001;
   fitting.regression      = FALSE;             /* SVD Linear regression   */
   fitting.robust          = FALSE;             /* Robust straight line    */
   fitting.fourier         = FALSE;             /* Fourier smoothing       */
   fitting.rexx            = FALSE;             /* ARexx fitting routine   */
   fitting.regress_npoly   = 2;
   fitting.rexx_npoly      = 2;
   fitting.regress_write   = FALSE;
   fitting.rexx_write      = FALSE;
   fitting.rexxmacro[0]    = '\0';
   
   display.width           = WIDTH;
   display.height          = HEIGHT;
   display.xoff            = (display.WXSize - display.width)  / 2;
   display.yoff            = (display.WYSize - display.height) / 2;

#ifdef REXX_SUPPORT
   rexx.value              = 0.0;
   rexx.current            = NULL;
   rexx.fromrexx           = FALSE;
   rexx.rexxplot           = FALSE;
   rexx.replot             = FALSE;
   rexx.quitprogram        = FALSE;
   rexx.empty              = FALSE;
   rexx.debug              = FALSE;
   rexx.autoreplot         = TRUE;
   rexx.forcereplot        = FALSE;
   rexx.RexxMacro[0]       = '\0';
#endif

   /* Pie style */
   piestyle.start          = 0.0;
   piestyle.eject          = 25.0;
   piestyle.D3             = FALSE;
   
#ifdef AMIGA
   /* Update check marks to match these flags */
   UpdateChecks();

   /* Disable pie menu items */
   OffMenu(MainWind, MenuNumPie);
   OffMenu(MainWind, MenuNumPieSetup);

   if(!NoTooltypes)  ProcessToolTypes();
#endif

   /* Initialise `extra' label list */
   AllocExtraLabel();
   
   /***
    *** Use PS2AmigaFont to get the Alternate fontname
    ***/
   strcpy(atext.PSFont,"Alternate");
   PS2AmigaFont(&atext);
   /* Remove the .font part */
   for(j=0; j<strlen(atext.AmigaFont); j++)
   {
      if(atext.AmigaFont[j] == '.')
      {
         atext.AmigaFont[j] = '\0';
         break;
      }
   }
   if(!strcmp(atext.AmigaFont,"alternate"))
      strcpy(output.AltFont,"Symbol");
   else
      strcpy(output.AltFont,atext.AmigaFont);

   /*** Fill list ***/
   if(filllist) FREELIST(filllist, FILLITEM);
   filllist = NULL;

   return(0);
}

/***************************************************************************/
/*>LoadFileByName(char *filename, int edit)
   ----------------------------------------
   Load and plot a file given its name.
   22.07.92 Extracted from OpenFile()
*/
int LoadFileByName(char *filename,
               int  edit)
{
   FH *fh         = NULL;     /* File handle                */
   static int        FirstCall   = TRUE,     /* Flag for first call        */
                     Force       = TRUE;     /* Force plot clearing        */

   /* Open the file */
#ifdef AMIGA
   if((fh = (FH *)Open(filename,MODE_OLDFILE))==NULL)
#else
   if((fh = fopen(filename,"r"))==NULL)
#endif
   {
      /* Give a message only if we didn't enter this from Edit where the
         user might have cancelled the edit
      */
      if(!edit)
#ifdef AMIGA
         ReqMessage2(MainWind,"Unable to open data file:",filename,0);
#else
         fprintf(stderr,"Unable to open data file: %s\n",filename);
#endif
      return(0);
   }

#ifdef AMIGA
   WaitPointer(MainWind);
#endif

   /* Clear all current plot data */
   if(FirstCall)
   {
      /* Clear any current data (with force option on) */
      ClearPlots(Force);
      FirstCall   = FALSE;
      Force       = FALSE;
   }
   else
   {
      /* Clear current data, checking with user */
      if(!ClearPlots(Force))
      {
#ifdef AMIGA
         Close((BPTR)fh);
#else
         fclose(fh);
#endif
         fh = NULL;

#ifdef AMIGA
         ClearPointer(MainWind);
#endif
         return(0);
      }
   }

   /* Read the dataset. Returns TRUE if OK. */
   if(ReadData(fh))
   {
      /* Everything OK, go ahead and plot */
#ifdef AMIGA
      Close((BPTR)fh);
#else
      fclose(fh);
#endif
      fh = NULL;
      Force = FALSE;       /* Next time, we'll want to check */
   
      SetScaling(FALSE);
   }
   else
   {
      /* A problem, but the user has already decided to overwrite the current
         graph, so we don't want to ask again. Free the current dataset, 
         basically so data is set to NULL.
      */
      Force = TRUE;
      ClearPlots(TRUE);
   }

#ifdef AMIGA
   ClearPointer(MainWind);
#endif

   return(0);
}

/***************************************************************************/
/*>AllocExtraLabel(void)
   ---------------------
   Allocate memory for the extra labels linked list.
   If the list has not been initialised (labels == NULL), this routine will 
   do so; if the list has been initialised, an additional item will be 
   allocated. If the routine fails, it will give an error message.

   Returns: int         TRUE        Success
                        FALSE       Failure

   03.04.92 Original
*/
int AllocExtraLabel(void)
{
   TEXTLIST *p;
   
   if(labels != NULL)
   {
      /* Walk to last label */
      for(p=labels;p->next;NEXT(p));
      
      ALLOCNEXTPREV(p,TEXTLIST);
      
      if(p==NULL)
      {
         ReqMessage(MainWind,"No memory for label",0);
         return(FALSE);
      }
      
      p->stext.text[0]           = '\0';
      p->stext.x                 = 0.0;
      p->stext.y                 = 0.0;
      p->stext.PSSize            = Defaults.LabelPSSize;
      p->stext.AmigaSize         = Defaults.LabelAmigaSize;
      strcpy(p->stext.PSFont,      Defaults.LabelPSFont);
      strcpy(p->stext.AmigaFont,   Defaults.LabelAmigaFont);
      
      graph.nlabels++;
   }
   else
   {
      /* Initialise the `extra' label list */
      if((labels = (TEXTLIST *)malloc(sizeof(TEXTLIST)))==NULL)
      {
         ReqMessage(MainWind,"No memory for labels",0);
         return(FALSE);
      }

      /* Initialise this label */
      labels->next            = NULL;
      labels->prev            = NULL;
      labels->stext.text[0]   = '\0';
      labels->stext.x         = 0.0;
      labels->stext.y         = 0.0;
      labels->stext.PSSize    = Defaults.LabelPSSize;
      labels->stext.AmigaSize = Defaults.LabelAmigaSize;
      strcpy(labels->stext.PSFont,     Defaults.LabelPSFont);
      strcpy(labels->stext.AmigaFont,  Defaults.LabelAmigaFont);

      graph.CurrentLabel      = 0;
      graph.nlabels           = 0;
   }

   return(TRUE);
}
   
