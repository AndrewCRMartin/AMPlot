/****************************************************************************

   Program:    AMPlot2
   File:       handlers_amiga.c
   
   Version:    V1.1
   Date:       04.03.92
   Function:   Handle menu functions.
   
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
   V1.1     27.04.92
   Changed all sprintf's to gadgets from %.3lg to %lg

****************************************************************************/
#include "includes.h"

#define ISCHECKED(x) ((x)->Flags & CHECKED)
static char SaveFileName[MAXBUFF];

/***************************************************************************/
/*>EjectSlice(APTR object)
   -----------------------
   04.03.92 Framework
   18.06.92 Original
   22.06.92 Corrected angle by start angle
   23.06.92 Fixed angle correction if angle ends up -ve (add 2PI)
*/
int EjectSlice(APTR object)
{
   int      xscreen,
            yscreen,
            xcent,
            ycent,
            j;
   double   angle = 0.0,
            lastangle,
            clickangle,
            total;
   
   if(graph.style == STYLE_PIE)
   {
      /* Wait for the user to click */
      ClickPointer(MainWind);
      WaitForClick(MainWind, &xscreen, &yscreen);

      /* Find the centre of the circle in screen coords */
      DataToScreen(0.0, 0.0, &xcent, &ycent);
      
      /* Find the angle (using screen coords) */
      clickangle = trueangle((double)(ycent-yscreen),(double)(xscreen-xcent));

      /* Modify by start angle */
      clickangle -= piestyle.start;
      if(clickangle < 0.0) clickangle += (2.0 * PI);

      /* Find total of data in pie */
      for(j=0, total=0.0; j<data->ndata; j++) total += data->x[j];
   
      /* For each data point */
      for(j=0; j<data->ndata; j++)
      {
         /* Find the boundary angles for this slice */
         lastangle = angle;
         angle    += (2.0 * PI * data->x[j] / total);
   
         /* See if click was in this wedge */
         if(clickangle > lastangle && clickangle < angle)
         {
            /* Toggle the eject flag and break */
            if(data->ep[j] == 0.0)  data->ep[j] = 1.0;
            else                    data->ep[j] = 0.0;

            break;
         }
         
      }
   }
   return(0);
}

/***************************************************************************/
/*>OpenFile(APTR object)
   ---------------------
   1. Displays a file requester to get a filename. 
   2. If called as EDIT, finds the name of the editor and edits the file. 
   3. Opens the file. 
   4. Cleans up current plot structures.
   5. Calls ReadData() to read the data file.
   6. Sets scaling and plots to screen.

   04.03.92 Framework
   30.03.92 Original
   02.04.92 No error message if unable to open edit derived filename
   06.04.92 Removed call to DoPlot(). This is now done after every menu
            activity.
   07.04.92 ClearPlots() added when there is a problem. Added SHANGHAI mode.
   27.04.92 Added DIC's on edit filename
   06.07.92 Changed to get directory from default.directory
   07.07.92 File handles set to NULL on close
   10.07.92 Changed to use display.OSVersion rather than finding version
            directly
   22.07.92 Modified to call LoadFileByName()
   24.07.92 Corrected requester totle for edit.
   27.07.92 Blanked SaveFileName[], so Project/Save will bring up a
            file requester.
*/
int OpenFile(APTR object)
{
   static char       DataFileBuff[MAXBUFF],  /* The filename               */
                     DataDirBuff[MAXBUFF],   /* The path                   */
                     DataFileName[MAXBUFF];  /* Complete file and path     */
   static int        FirstCall   = TRUE;     /* Flag for first call        */
   char              *editor,
                     command_line[MAXBUFF],
                     PubScreen[80];          /* Public screen name         */
   UWORD             OldModes;               /* Public screen modes        */
   
   

   /* If this is our first call, get current directory, or, if called as
      EDIT, assume we want to look at a temp file in ram:
   */
   if(FirstCall)
   {
      strcpy(DataDirBuff,Defaults.directory);
      if(object == (APTR)MenuEdit)
      {
         strcpy(DataFileBuff, "AMPlot.dat");
         strcpy(DataDirBuff,  "ram:");
         strcpy(DataFileName, "ram:AMPlot.dat");
      }
      FirstCall = FALSE;
   }
   
   /* Get a filename */
   if(object == (APTR)MenuEdit)
   {
      if(!GetAFile(AslBase,MainWind,MyScreen,"Edit Data File",
                   DataFileBuff,DataDirBuff,DataFileName)) return(0);
   }
   else
   {
      if(!GetAFile(AslBase,MainWind,MyScreen,"Open Data File",
                   DataFileBuff,DataDirBuff,DataFileName)) return(0);
   }
   
   /* If this was called as EDIT, find the editor and run it on the file
      before opening the file.
   */
   if(object == (APTR)MenuEdit)
   {
      /* If can't find environment variable then use the default */
	   if((editor = (char *)getenv("EDITOR")) == (char *)NULL )
		   editor = DEFAULT_EDITOR;


      /* Build the command line */
      strncpy(command_line, editor,       MAXBUFF-1);
      strncat(command_line, " \"",        MAXBUFF-(strlen(command_line)+1));
      strncat(command_line, DataFileName, MAXBUFF-(strlen(command_line)+1));
      strncat(command_line, "\"",         MAXBUFF-(strlen(command_line)+1));

      /* If we've got a public screen, make it the default and
         turn on SHANGHAI, so the editor opens here. Note that this
         refuses to work with lse V2.10A which must do something
         clever (?) to ensure it really opens on the Workbench :-(
      */
      if(display.shanghai && display.OSVersion >= 36)
      {
         GetDefaultPubScreen(PubScreen);
         SetDefaultPubScreen(PUBSCREEN);
         OldModes = SetPubScreenModes(SHANGHAI);
      }
      else
      {
         WaitPointer(MainWind);
         WBenchToFront();
      }

      system(command_line);

      /* Restore the public screens */
      if(display.shanghai && display.OSVersion >= 36)
      {
         if(!strncmp(PubScreen,"Workbench",9))
            SetDefaultPubScreen(NULL);
         else
            SetDefaultPubScreen(PubScreen);

         SetPubScreenModes(OldModes);
      }
      else
      {
         ClearPointer(MainWind);
         ScreenToFront(MyScreen);
      }
   }

   /* Now read and plot this file   */
   LoadFileByName(DataFileName, (object==(APTR)MenuEdit));

   /* Blank the save filename       */
   SaveFileName[0] = '\0';

   return(0);
}

/***************************************************************************/
/*>SaveAsFile(APTR object)
   -----------------------
   1. Displays a file requester.
   2. Opens the file.
   3. Calls SaveGraph() to save the graph.

   04.03.92 Framework
   30.03.92 Original
   31.03.92 Changed to check object against MenuSaveAs rather than NULL
   06.07.92 Changed to get directory from default.directory
   07.07.92 File handles set to NULL on close
   27.07.92 SaveFileName made external so can be reset when a new graph
            is opened.
*/
int SaveAsFile(APTR object)
{
   static char       SaveFileBuff[MAXBUFF],
                     SaveDirBuff[MAXBUFF];
   static int        FirstCall   = TRUE;
   FH *fh         = NULL;
   
   if(FirstCall)
   {
      FirstCall = FALSE;
      strcpy(SaveDirBuff,Defaults.directory);
      SaveFileName[0] = '\0';
   }

   if(object == (APTR)MenuSaveAs || SaveFileName[0] == '\0')
   {
      /* Called as SaveAs, or no filename set, so display requester */
      if(!GetAFile(AslBase,MainWind,MyScreen,"Save Data File",
                   SaveFileBuff,SaveDirBuff,SaveFileName))
         return(0);
   }
   
   if((fh = (FH *)Open(SaveFileName,MODE_NEWFILE))==NULL)
   {
      ReqMessage2(MainWind,"Unable to open save file",SaveFileName,0);
      return(0);
   }
      
   SaveGraph(fh);

   Close((BPTR)fh);
   fh = NULL;

   return(0);
}

/***************************************************************************/
/*>SetPaper(APTR object)
   ---------------------
   Handles requester to set paper dimensions.
   04.03.92 Framework
   30.03.92 Original
*/
int SetPaper(APTR object)
{
   struct Window *ReqWind;
   
   /* Set buffers */
   sprintf(PaperXSizeBuff, "%lg",output.xsize);
   sprintf(PaperYSizeBuff, "%lg",output.ysize);
   sprintf(PaperXOffBuff,  "%lg",output.xoffset);
   sprintf(PaperYOffBuff,  "%lg",output.yoffset);
   if(EPSF) Select(PaperEPSF);
   else     Unselect(PaperEPSF);
   
   /* Display requester and get responses */
   ReqWind = DisplayWindowReq(NWPaper,MainWind);
   if(WaitForEndGadget(ReqWind)  != PaperCancel)
   {
      sscanf(PaperXSizeBuff, "%lf",&output.xsize);
      sscanf(PaperYSizeBuff, "%lf",&output.ysize);
      sscanf(PaperXOffBuff,  "%lf",&output.xoffset);
      sscanf(PaperYOffBuff,  "%lf",&output.yoffset);
      
      EPSF = isSET(PaperEPSF);
   }

   RemoveWindowReq(ReqWind,MainWind);
   
   return(0);
}

/***************************************************************************/
/*>About(APTR object)
   ------------------
   Displays the About requester
   04.03.92 Original
*/
int About(APTR object)
{
   struct Window *ReqWind;
   
   ReqWind = DisplayWindowReq(NWAbout,MainWind);
   WaitForEndGadget(ReqWind);
   RemoveWindowReq(ReqWind,MainWind);
   
   return(0);
}

/***************************************************************************/
/*>QuitProgram(APTR object)
   ------------------------
   Exits the program, asking the user whether to proceed
   04.03.92 Original
   29.04.92 Added forced exit
   28.07.92 Added data==NULL exit
*/
int QuitProgram(APTR object)
{
   if(object == (APTR)1 || data==NULL)
   {
      Cleanup();
      exit(0);
   }
   
   if(ReqMessage(MainWind,"Do you really want to quit?",1))
   {
      Cleanup();
      exit(0);
   }
   return(0);
}

#ifndef DEMO_VERSION
/***************************************************************************/
/*>PlotPS(APTR object)
   -------------------
   Display file requester, open file and call DoPlot() to create PostScript
   plot.
   04.03.92 Framework
   30.03.92 Original
   07.05.92 Changed to use output.filename
   12.06.92 Added support for PS hatching
   19.06.92 Changed to make pie charts square
   23.06.92 Added PSFixBoundingBox() call
   25.06.92 Moved setting of PS globals to AMInitPlot()
   01.07.92 Added message
   06.07.92 Changed to get directory from default.directory
   07.07.92 File handles set to NULL on close
   15.09.92 Added check on output.fixbounds
*/
int PlotPS(APTR object)
{
   static char       PlotFileBuff[MAXBUFF],
                     PlotDirBuff[MAXBUFF];
   static int        FirstCall   = TRUE;
   FH *fh         = NULL;
   
   if(FirstCall)
   {
      FirstCall = FALSE;
      strcpy(PlotDirBuff,Defaults.directory);
      output.filename[0] = '\0';
   }

   if(!GetAFile(AslBase,MainWind,MyScreen,"PostScript Plot File",
                PlotFileBuff,PlotDirBuff,output.filename))
      return(0);
   
   if((fh = (FH *)Open(output.filename,MODE_NEWFILE))==NULL)
   {
      ReqMessage2(MainWind,"Unable to open PostScript file",output.filename,0);
      return(0);
   }

   output.plottype = DEST_PS;
   output.fh       = fh;

   DoPlot(output.plottype,fh);
   Close((BPTR)fh);
   fh = output.fh = NULL;

   /* Fix BoundingBox info */
   if(EPSF && output.fixbounds) PSFixBoundingBox(MainWind,output.filename);

   WriteMessage("PostScript plot complete.");
   
   return(0);
}

/***************************************************************************/
/*>PlotHPGL(APTR object)
   ---------------------
   Display file requester, open file and call DoPlot() to create HPGL plot.
   04.03.92 Framework
   30.03.92 Original
   01.07.92 Added message
   06.07.92 Changed to get directory from default.directory
   07.07.92 File handles set to NULL on close
*/
int PlotHPGL(APTR object)
{
   static char       PlotFileBuff[MAXBUFF],
                     PlotDirBuff[MAXBUFF],
                     PlotFileName[MAXBUFF];
   static int        FirstCall   = TRUE;
   FH *fh         = NULL;
   
   if(FirstCall)
   {
      FirstCall = FALSE;
      strcpy(PlotDirBuff,Defaults.directory);
      PlotFileName[0] = '\0';
   }

   if(!GetAFile(AslBase,MainWind,MyScreen,"HPGL Plot File",
                PlotFileBuff,PlotDirBuff,PlotFileName))
      return(0);
   
   if((fh = (FH *)Open(PlotFileName,MODE_NEWFILE))==NULL)
   {
      ReqMessage2(MainWind,"Unable to open HPGL file",PlotFileName,0);
      return(0);
   }

   output.plottype = DEST_HPGL;
   output.fh       = fh;

   DoPlot(output.plottype,fh);
   Close((BPTR)fh);
   fh = output.fh = NULL;

   WriteMessage("HPGL plot complete.");

   return(0);
}

/***************************************************************************/
/*>PlotDR2D(APTR object)
   ---------------------
   Display file requester, open file and call DoPlot() to create IFF-DR2D
   plot.
   04.03.92 Framework
   30.03.92 Original
   01.07.92 Added message
   06.07.92 Changed to get directory from default.directory
   07.07.92 File handles set to NULL on close
   16.07.92 Changed so the IFF InitDR2D() does the file opening.
*/
int PlotDR2D(APTR object)
{
   static char       PlotFileBuff[MAXBUFF],
                     PlotDirBuff[MAXBUFF];
   static int        FirstCall   = TRUE;
   
   if(FirstCall)
   {
      FirstCall = FALSE;
      strcpy(PlotDirBuff,Defaults.directory);
      output.filename[0] = '\0';
   }

   if(!GetAFile(AslBase,MainWind,MyScreen,"IFF-DR2D Plot File",
                PlotFileBuff,PlotDirBuff,output.filename))
      return(0);
   
   output.plottype = DEST_DR2D;
   output.fh       = NULL;

   DoPlot(output.plottype,output.fh);
   
   WriteMessage("IFF-DR2D plot complete.");

   return(0);
}

#endif /* !DEMO_VERSION */

/***************************************************************************/
/*>SetPens(APTR object)
   --------------------
   Display and process requester to set pen colours and thicknesses.
   04.03.92 Framework
   30.03.92 Original
*/
int SetPens(APTR object)
{
   struct Window *ReqWind;
   int           pen;
   
   /* Set buffers */
   sprintf(SetPen1ColBuff,"%d",pens[0].colour);
   sprintf(SetPen2ColBuff,"%d",pens[1].colour);
   sprintf(SetPen3ColBuff,"%d",pens[2].colour);
   sprintf(SetPen4ColBuff,"%d",pens[3].colour);
   sprintf(SetPen5ColBuff,"%d",pens[4].colour);
   sprintf(SetPen6ColBuff,"%d",pens[5].colour);

   sprintf(SetPen1ThickBuff,"%lg",pens[0].thickness);
   sprintf(SetPen2ThickBuff,"%lg",pens[1].thickness);
   sprintf(SetPen3ThickBuff,"%lg",pens[2].thickness);
   sprintf(SetPen4ThickBuff,"%lg",pens[3].thickness);
   sprintf(SetPen5ThickBuff,"%lg",pens[4].thickness);
   sprintf(SetPen6ThickBuff,"%lg",pens[5].thickness);



   /* Display requester and get responses */
   ReqWind = DisplayWindowReq(NWSetPens,MainWind);
   if(WaitForEndGadget(ReqWind) != SetPensCancel)
   {
      sscanf(SetPen1ColBuff,  "%d", &pens[0].colour);
      sscanf(SetPen2ColBuff,  "%d", &pens[1].colour);
      sscanf(SetPen3ColBuff,  "%d", &pens[2].colour);
      sscanf(SetPen4ColBuff,  "%d", &pens[3].colour);
      sscanf(SetPen5ColBuff,  "%d", &pens[4].colour);
      sscanf(SetPen6ColBuff,  "%d", &pens[5].colour);

      sscanf(SetPen1ThickBuff,"%lf",&pens[0].thickness);
      sscanf(SetPen2ThickBuff,"%lf",&pens[1].thickness);
      sscanf(SetPen3ThickBuff,"%lf",&pens[2].thickness);
      sscanf(SetPen4ThickBuff,"%lf",&pens[3].thickness);
      sscanf(SetPen5ThickBuff,"%lf",&pens[4].thickness);
      sscanf(SetPen6ThickBuff,"%lf",&pens[5].thickness);

      /* Work out screen colours from these specified colours */
      for(pen=0; pen<MAXPEN; pen++)
      {
         int colour = pens[pen].colour;
         
         pens[pen].screencolour = 
            colour-(int)(colour/display.NColours)*display.NColours;
         if(pens[pen].screencolour == 0)
            pens[pen].screencolour = display.NColours-1;
      }
   }

   RemoveWindowReq(ReqWind,MainWind);
   
   return(0);
}

/***************************************************************************/
/*>StyleXY(APTR object)
   --------------------
   Set plot style to XY
   04.03.92 Framework
   30.03.92 Original
   09.06.92 Added call to SetScaling(FALSE)
   10.07.92 Added OnPieMenus()
*/
int StyleXY(APTR object)
{
   int doscale = FALSE;
   
   if(graph.style == STYLE_PIE ||
     (graph.style == STYLE_BAR && barstyle.style == STYLE_STACKED))
     doscale = TRUE;

   graph.style = STYLE_LINE;

   /* If the barstyle was stacked, reset scaling */
   if(doscale) SetScaling(FALSE);

   /* Switch on menus not required by Pie charts */
   OnPieMenus();
   
   return(0);
}

/***************************************************************************/
/*>StyleScatter(APTR object)
   -------------------------
   Set plot style to scatter plot
   04.03.92 Framework
   30.03.92 Original
   09.06.92 Added call to SetScaling(FALSE)
   10.07.92 Added OnPieMenus()
*/
int StyleScatter(APTR object)
{
   int i;
   int doscale = FALSE;
   
   if(graph.style == STYLE_PIE ||
     (graph.style == STYLE_BAR && barstyle.style == STYLE_STACKED))
     doscale = TRUE;

   graph.style = STYLE_SCATTER;
   for(i=0;i<MAXPEN;i++)
   {
      if(pens[i].mark == 0)
      {
         pens[i].mark = 1;
         switch(i)
         {
            case 0:  HandleMXGadget(NULL,Pen1DP[1]);  break;
            case 1:  HandleMXGadget(NULL,Pen2DP[1]);  break;
            case 2:  HandleMXGadget(NULL,Pen3DP[1]);  break;
            case 3:  HandleMXGadget(NULL,Pen4DP[1]);  break;
            case 4:  HandleMXGadget(NULL,Pen5DP[1]);  break;
            case 5:  HandleMXGadget(NULL,Pen6DP[1]);  break;
            default: break;
         }
      }
   }

   /* If the barstyle was stacked, reset scaling */
   if(doscale) SetScaling(FALSE);

   /* Switch on menus not required by Pie charts */
   OnPieMenus();
   
   return(0);
}

/***************************************************************************/
/*>StyleBar(APTR object)
   ---------------------
   Set plot style to Barchart
   04.03.92 Framework
   30.03.92 Original
   09.06.92 Added switch off LogY and SetScaling(FALSE) for stacked bars.
   10.07.92 Added OnPieMenus()
*/
int StyleBar(APTR object)
{
   int doscale = FALSE;
   
   if(graph.style == STYLE_PIE || barstyle.style == STYLE_STACKED)
      doscale = TRUE;
   
   graph.style = STYLE_BAR;

   graph.logy = FALSE;
   Uncheck(MenuLogY);
   
   if(doscale) SetScaling(FALSE);

   /* Switch on menus not required by Pie charts */
   OnPieMenus();
   
   return(0);
}

/***************************************************************************/
/*>StylePie(APTR object)
   ---------------------
   Set plot style to pie chart
   04.03.92 Framework
   30.03.92 Original
   10.07.92 Added OffPieMenus()
*/
int StylePie(APTR object)
{
   int j;
   
   graph.style = STYLE_PIE;
   SetScaling(TRUE);
   
   /* Reject the error data, since we will use this as a flag for slice
      eject
   */
   for(j=0; j<data->ndata; j++) data->ep[j] = data->em[j] = 0.0;

   /* Switch off menus not required by Pie charts */
   OffPieMenus();
   
   return(0);
}

/***************************************************************************/
/*>ToggleErrors(APTR object)
   -------------------------
   Toggle the display of error bars
   04.03.92 Framework
   31.03.92 Original
   08.04.92 Now correctly toggles rather than just switching on!
   24.07.92 If came from menu, now checks menu CHECK status rather than
            toggling
*/
int ToggleErrors(APTR object)
{
   if(object == NULL)   toggle(graph.errors);
   else                 graph.errors = ISCHECKED(MenuErrors);
   
   return(0);
}

/***************************************************************************/
/*>SetLine(APTR object)
   --------------------
   Display and process requester to handle line style (datapoint marks, etc.)
   04.03.92 Framework
   31.03.92 Original
   07.05.92 Added marks in window and associated AreaInfo and TmpRas stuff
   13.07.92 Apparently *not* OK to share TmpRas with main window, so have
            uncommented the local TmpRas
*/
int SetLine(APTR object)
{
   struct Window     *ReqWind;
   UWORD             AreaBuff_local[200];    /* Buffer for AreaDraw() etc. */
   struct AreaInfo   AInfo_local;            /* Info struc for "           */

   /* Set pen mark gadgets */
   HandleMXGadget(NULL,Pen1DP[pens[0].mark]);
   HandleMXGadget(NULL,Pen2DP[pens[1].mark]);
   HandleMXGadget(NULL,Pen3DP[pens[2].mark]);
   HandleMXGadget(NULL,Pen4DP[pens[3].mark]);
   HandleMXGadget(NULL,Pen5DP[pens[4].mark]);
   HandleMXGadget(NULL,Pen6DP[pens[5].mark]);

   if(pens[0].join)  Select(Pen1Link);
   else              Unselect(Pen1Link);
   if(pens[1].join)  Select(Pen2Link);
   else              Unselect(Pen2Link);
   if(pens[2].join)  Select(Pen3Link);
   else              Unselect(Pen3Link);
   if(pens[3].join)  Select(Pen4Link);
   else              Unselect(Pen4Link);
   if(pens[4].join)  Select(Pen5Link);
   else              Unselect(Pen5Link);
   if(pens[5].join)  Select(Pen6Link);
   else              Unselect(Pen6Link);

   sprintf(LineStyleEBarBuff,   "%lg", graph.ebarwidth);
   sprintf(LineStyleDPSizeBuff, "%lg", graph.dpsize);
   
   /* Display the window requester */
   ReqWind = DisplayWindowReq(NWLineStyle,MainWind);

   /* Set up Area buffer */
   InitArea(&AInfo_local, AreaBuff_local, 80);   /* Separate TmpRas */
   ReqWind->RPort->AreaInfo = &AInfo_local;      /* Separate TmpRas */
//   ReqWind->RPort->AreaInfo = &AInfo;
   ReqWind->RPort->TmpRas = MainWind->RPort->TmpRas;

   /* Draw marks into window */
   SetAPen(ReqWind->RPort, 1);
   ScreenMark(ReqWind->RPort,  59, 24,  1, 16.0);
   ScreenMark(ReqWind->RPort,  79, 24,  2, 16.0);
   ScreenMark(ReqWind->RPort,  99, 24,  3, 16.0);
   ScreenMark(ReqWind->RPort, 119, 24,  4, 16.0);
   ScreenMark(ReqWind->RPort, 139, 24,  5, 16.0);
   ScreenMark(ReqWind->RPort, 159, 24,  6, 16.0);
   ScreenMark(ReqWind->RPort, 179, 24,  7, 16.0);
   ScreenMark(ReqWind->RPort, 199, 24,  8, 16.0);
   ScreenMark(ReqWind->RPort, 219, 24,  9, 16.0);
   ScreenMark(ReqWind->RPort, 239, 24, 10, 16.0);
   ScreenMark(ReqWind->RPort, 259, 24, 11, 16.0);
   ScreenMark(ReqWind->RPort, 279, 24, 12, 16.0);

   if(WaitForEndGadget(ReqWind) != LineStyleCancel)
   {
      pens[0].mark = MXChoice(Pen1DP);
      pens[1].mark = MXChoice(Pen2DP);
      pens[2].mark = MXChoice(Pen3DP);
      pens[3].mark = MXChoice(Pen4DP);
      pens[4].mark = MXChoice(Pen5DP);
      pens[5].mark = MXChoice(Pen6DP);

      pens[0].join = isSET(Pen1Link);
      pens[1].join = isSET(Pen2Link);
      pens[2].join = isSET(Pen3Link);
      pens[3].join = isSET(Pen4Link);
      pens[4].join = isSET(Pen5Link);
      pens[5].join = isSET(Pen6Link);
      
      sscanf(LineStyleEBarBuff,   "%lf", &graph.ebarwidth);
      sscanf(LineStyleDPSizeBuff, "%lf", &graph.dpsize);
   }
   RemoveWindowReq(ReqWind,MainWind);
   
   return(0);
}

/***************************************************************************/
/*>SetBar(APTR object)
   -------------------
   Display and process requester to handle barchart setup.
   04.03.92 Framework
   31.03.92 Original
   09.06.92 Added clear of fill list if style changed
*/
int SetBar(APTR object)
{
   struct Window *ReqWind;
   double shrink;
   int    OldStyle;
   
   if(fabs(barstyle.shrink) < 0.00001) shrink = 0.0;
   else                                shrink = 200.0/barstyle.shrink;
   sprintf(BarShrinkBuff,"%lg",shrink);

   if(barstyle.shrinkall)  Select(BarStyleShrink);
   else                    Unselect(BarStyleShrink);

   if(barstyle.fillall)    Select(BarStyleFillAll);
   else                    Unselect(BarStyleFillAll);
   
#ifdef THREE_D
   if(barstyle.D3)         Select(BarStyle3D);
   else                    Unselect(BarStyle3D);
#endif
   
   switch(barstyle.style)
   {
   case STYLE_OVERLAYED:
      HandleMXGadget(NULL,BarStyles[0]);
      break;
   case STYLE_GROUPED:
      HandleMXGadget(NULL,BarStyles[1]);
      break;
   case STYLE_STACKED:
      HandleMXGadget(NULL,BarStyles[2]);
      break;
   case STYLE_OUTLINE:
      HandleMXGadget(NULL,BarStyles[3]);
      break;
   default:
      break;
   }
   
   OldStyle = MXChoice(BarStyles);
   
   ReqWind = DisplayWindowReq(NWBarStyle,MainWind);
   if(WaitForEndGadget(ReqWind) != BarStyleCancel)
   {
      sscanf(BarShrinkBuff,"%lf",&shrink);
      if(fabs(shrink) < 0.00001) barstyle.shrink = 0.0;
      else                       barstyle.shrink = 200.0/shrink;

      barstyle.shrinkall   = isSET(BarStyleShrink);
      barstyle.fillall     = isSET(BarStyleFillAll);
#ifdef THREE_D
      barstyle.D3          = isSET(BarStyle3D);
#endif
      
      switch(MXChoice(BarStyles))
      {
      case 0:  
         barstyle.style = STYLE_OVERLAYED;
         break;
      case 1:  
         barstyle.style = STYLE_GROUPED;
         break;
      case 2:  
         barstyle.style = STYLE_STACKED;
         break;
      case 3:  
         barstyle.style = STYLE_OUTLINE;
         break;
      default:
         break;
      }
      
      /* If the user has changed bar style, clear the fill list */
      if(OldStyle != MXChoice(BarStyles))
      {
         FREELIST(filllist, FILLITEM);
         filllist = NULL;

         if(OldStyle == 2 || MXChoice(BarStyles) == 2)
         {
            /* If either is stacked, redo scaling */
            SetScaling(FALSE);
         }
      }
      
   }
   RemoveWindowReq(ReqWind,MainWind);
   
   return(0);
}

/***************************************************************************/
/*>SetPie(APTR object)
   -------------------
   Display and process requester to set piechart style
   04.03.92 Framework
   31.03.92 Original
   17.06.92 Changed to store piestyle.start in rads.
*/
int SetPie(APTR object)
{
   struct Window *ReqWind;
   double        piestart;
   
   piestart = 180.0 * piestyle.start / PI;
   
   sprintf(PieStartBuff,"%lg",piestart);
   sprintf(PieEjectBuff,"%lg",piestyle.eject);

#ifdef THREE_D
   if(piestyle.D3)   Select(PieStyle3D);
   else              Unselect(PieStyle3D);
#endif

   ReqWind = DisplayWindowReq(NWPieStyle,MainWind);
   if(WaitForEndGadget(ReqWind) != PieStyleCancel)
   {
      sscanf(PieStartBuff,"%lf",&piestart);
      piestyle.start = PI * piestart / 180.0;
      sscanf(PieEjectBuff,"%lf",&piestyle.eject);
#ifdef THREE_D
      piestyle.D3 = isSET(PieStyle3D);
#endif
   }
   RemoveWindowReq(ReqWind,MainWind);
   
   return(0);
}

/***************************************************************************/
/*>ToggleBoxed(APTR object)
   ------------------------
   Toggle boxed axes
   04.03.92 Framework
   31.03.92 Original
   24.07.92 If came from menu, now checks menu CHECK status rather than
            toggling
*/
int ToggleBoxed(APTR object)
{
   if(object == NULL)   toggle(graph.boxed);
   else                 graph.boxed = ISCHECKED(MenuBoxed);
   
   return(0);
}

/***************************************************************************/
/*>ToggleGrid(APTR object)
   -----------------------
   Toggle grid
   04.03.92 Framework
   31.03.92 Original
   07.04.92 Force boxed on.
   24.07.92 If came from menu, now checks menu CHECK status rather than
            toggling
*/
int ToggleGrid(APTR object)
{
   if(object == NULL)   toggle(graph.grid);
   else                 graph.grid = ISCHECKED(MenuGrid);
   
   if(graph.grid)
   {
      /* Force on the boxed option */
      graph.boxed = TRUE;
      Check(MenuBoxed);
   }
   
   return(0);
}

/***************************************************************************/
/*>SetAxes(APTR object)
   --------------------
   Display and process requester to set graph bounds and ticks.
   04.03.92 Framework
   31.03.92 Original
   08.04.92 Added auto-activation of gadgets
   10.04.92 Handles logs, added restart on <=0.0 log axes
   28.04.92 Disabled tick gadgets when using logs and corrected gadget names
   09.06.92 Added setting of graph.userbounds and graph.userticks
   18.06.92 Set auto bounds for piecharts
   22.06.92 Set scaling same in both directions for Piecharts
   23.06.92 Fixed bug in auto bounds for Piecharts
   10.07.92 Added setting of graph.usersub and disabling of gadgets for 
            Piecharts
   24.07.92 Added check that no more than 1000 ticks on either axis
   28.07.92 Swap min/max if they are the wrong way round.
*/
int SetAxes(APTR object)
{
   struct Window        *ReqWind;
   struct IntuiMessage  *message;
   ULONG                Signals,
                        MIClass;
   struct Gadget        *gadget;
   double               XSize,
                        YSize;
   int                  OldUserBounds,
                        OldUserTicks,
                        OldUserSub;
   
   sprintf(BoundsXMinBuff, "%lg",graph.xmin);
   sprintf(BoundsXMaxBuff, "%lg",graph.xmax);
   sprintf(BoundsYMinBuff, "%lg",graph.ymin);
   sprintf(BoundsYMaxBuff, "%lg",graph.ymax);
   sprintf(TickXBuff,      "%lg",graph.xtick);
   sprintf(TickYBuff,      "%lg",graph.ytick);
   sprintf(SubXBuff,       "%lg",graph.xsub);
   sprintf(SubYBuff,       "%lg",graph.ysub);

   if(graph.clip)       Select(BoundsClip);
   else                 Unselect(BoundsClip);
   if(graph.subtick)    Select(TicksSub);
   else                 Unselect(TicksSub);
   
   OldUserBounds = graph.userbounds;
   OldUserTicks  = graph.userticks;
   OldUserSub    = graph.usersub;
   
   if(graph.logx || graph.style == STYLE_PIE)   /* Disable x-tick gadgets  */
   {
      TicksX->Flags     |= GFLG_DISABLED;
      TicksXSub->Flags  |= GFLG_DISABLED;
   }
   else                                         /* Enable x-tick gadgets   */
   {
      TicksX->Flags     &= ~GFLG_DISABLED;
      TicksXSub->Flags  &= ~GFLG_DISABLED;
   }
   
   if(graph.logy || graph.style == STYLE_PIE)   /* Disable y-tick gadgets  */
   {
      TicksY->Flags     |= GFLG_DISABLED;
      TicksYSub->Flags  |= GFLG_DISABLED;
   }
   else                                         /* Enable y-tick gadgets   */
   {
      TicksY->Flags     &= ~GFLG_DISABLED;
      TicksYSub->Flags  &= ~GFLG_DISABLED;
   }
   
   if(graph.style == STYLE_PIE)                 /* Auto, sub & clip gadgs  */
   {
      TicksAuto->Flags  |= GFLG_DISABLED;
      TicksSub->Flags   |= GFLG_DISABLED;
      BoundsClip->Flags |= GFLG_DISABLED;
   }
   else
   {
      TicksAuto->Flags  &= ~GFLG_DISABLED;
      TicksSub->Flags   &= ~GFLG_DISABLED;
      BoundsClip->Flags &= ~GFLG_DISABLED;
   }
   
   
   ReqWind = DisplayWindowReq(NWSetAxes,MainWind);
   ActivateGadget(BoundsXMin,ReqWind,NULL);

restart:
   do
   {
      Signals = Wait(1<<ReqWind->UserPort->mp_SigBit);
      while((message=(struct IntuiMessage *)GetMsg(ReqWind->UserPort))!=NULL)
      {
         gadget = NULL;
         if((MIClass = message->Class) == IDCMP_GADGETUP)
            gadget = (struct Gadget *)message->IAddress;

         ReplyMsg((struct Message *)message);


         if(gadget == BoundsAuto)
         {
            double xmin, xmax,
                   ymin, ymax;
                   
            graph.userbounds = FALSE;

            if(graph.style == STYLE_PIE)
            {
               xmin = -PIEBOUNDS;
               ymin = -PIEBOUNDS;
               xmax =  PIEBOUNDS;
               ymax =  PIEBOUNDS;
            }
            else
            {
               CalcBounds(&xmin,&xmax,&ymin,&ymax);
            }
            sprintf(BoundsXMinBuff, "%lg",xmin);
            sprintf(BoundsXMaxBuff, "%lg",xmax);
            sprintf(BoundsYMinBuff, "%lg",ymin);
            sprintf(BoundsYMaxBuff, "%lg",ymax);
            RefreshGList(BoundsXMin,ReqWind,NULL,4);
         }
         else if(gadget == TicksAuto)
         {
            double xtick, xsub,
                   ytick, ysub;
                   
            graph.userticks = FALSE;
            graph.usersub   = FALSE;

            CalcTicks(1,&xtick,&ytick,&xsub,&ysub);
            sprintf(TickXBuff, "%lg",xtick);
            sprintf(TickYBuff, "%lg",ytick);
            sprintf(SubXBuff,  "%lg",xsub);
            sprintf(SubYBuff,  "%lg",ysub);
            RefreshGList(TicksX,ReqWind,NULL,4);
         }
         else if(gadget == BoundsXMin)
         {
            graph.userbounds = TRUE;
            ActivateGadget(BoundsXMax,ReqWind,NULL);
         }
         else if(gadget == BoundsXMax)
         {
            graph.userbounds = TRUE;
            ActivateGadget(BoundsYMin,ReqWind,NULL);
         }
         else if(gadget == BoundsYMin)
         {
            graph.userbounds = TRUE;
            ActivateGadget(BoundsYMax,ReqWind,NULL);
         }
         else if(gadget == BoundsYMax)
         {
            graph.userbounds = TRUE;

            if((graph.logx && graph.logy) || graph.style == STYLE_PIE)
               ActivateGadget(BoundsXMin,ReqWind,NULL);
            else if(graph.logx)
               ActivateGadget(TicksY,ReqWind,NULL);
            else
               ActivateGadget(TicksX,ReqWind,NULL);
         }
         else if(gadget == TicksX)
         {
            graph.userticks = TRUE;
            ActivateGadget(TicksXSub,ReqWind,NULL);
         }
         else if(gadget == TicksXSub)
         {
            graph.usersub = TRUE;
            if(graph.logy)
               ActivateGadget(BoundsXMin,ReqWind,NULL);
            else
               ActivateGadget(TicksY,ReqWind,NULL);
         }
         else if(gadget == TicksY)
         {
            graph.userticks = TRUE;
            ActivateGadget(TicksYSub,ReqWind,NULL);
         }
         else if(gadget == TicksYSub)
         {
            graph.usersub = TRUE;
            ActivateGadget(BoundsXMin,ReqWind,NULL);
         }
      }
   } while (gadget==NULL || !(gadget->Activation & GACT_ENDGADGET));
      
   if(gadget != SetAxesCancel)
   {
      /* Transfer bounds and ticks from the buffers */
      sscanf(BoundsXMinBuff, "%lf",&graph.xmin);
      sscanf(BoundsXMaxBuff, "%lf",&graph.xmax);
      sscanf(BoundsYMinBuff, "%lf",&graph.ymin);
      sscanf(BoundsYMaxBuff, "%lf",&graph.ymax);
      sscanf(TickXBuff,      "%lf",&graph.xtick);
      sscanf(TickYBuff,      "%lf",&graph.ytick);
      sscanf(SubXBuff,       "%lf",&graph.xsub);
      sscanf(SubYBuff,       "%lf",&graph.ysub);
      
      /* Swap bounds if necessary */
      if(graph.xmin > graph.xmax)
      {
         double temp = graph.xmin;
         graph.xmin  = graph.xmax;
         graph.xmax  = temp;
      }
      if(graph.ymin > graph.ymax)
      {
         double temp = graph.ymin;
         graph.ymin  = graph.ymax;
         graph.ymax  = temp;
      }

      if((graph.logx && graph.xmin <= 0.0) ||
         (graph.logy && graph.ymin <= 0.0))
      {
         ReqMessage2(ReqWind,"Zero or negative bounds invalid",
                             "with log axes",0);
         goto restart;
      }

      /* Calculate scaling from these values */
      if(graph.logx)
         XSize             = log10(graph.xmax)-log10(graph.xmin);
      else
         XSize             = graph.xmax-graph.xmin;
      
      if(graph.logy)
         YSize             = log10(graph.ymax)-log10(graph.ymin);
      else
         YSize             = graph.ymax-graph.ymin;
         
      if(!graph.logx && graph.xtick < XSize / 1000.0)
      {
         ReqMessage(ReqWind,"X tick size is too small!",0);
         goto restart;
      }

      if(!graph.logy && graph.ytick < YSize / 1000.0)
      {
         ReqMessage(ReqWind,"Y tick size is too small!",0);
         goto restart;
      }

      graph.XScale   = display.width/XSize;
      graph.YScale   = display.height/YSize;
      graph.XPScale  = 1.0/XSize;
      graph.YPScale  = 1.0/YSize;
      
      if(graph.style == STYLE_PIE)  /* Set scaling same in both directions */
      {
         if(graph.XScale < graph.YScale)
         {
            graph.YScale   = graph.XScale;
            graph.YPScale  = graph.XPScale;
         }
         else
         {
            graph.XScale   = graph.YScale;
            graph.XPScale  = graph.YPScale;
         }
      }

      graph.clip     = isSET(BoundsClip);
      graph.subtick  = isSET(TicksSub);
      
   }
   else  /* User has canceled; reset flags */
   {
      graph.userbounds = OldUserBounds;
      graph.userticks  = OldUserTicks;
      graph.usersub    = OldUserSub;
   }

   RemoveWindowReq(ReqWind,MainWind);
   
   return(0);
}

/***************************************************************************/
/*>ToggleFZero(APTR object)
   ------------------------
   04.03.92 Framework
   01.04.92 Original
   09.04.92 Added check on current graph bounds
   24.07.92 If came from menu, now checks menu CHECK status rather than
            toggling
*/
int ToggleFZero(APTR object)
{
   if(object == NULL)   toggle(graph.fzero);
   else                 graph.fzero = ISCHECKED(MenuFZero);
   
   if(graph.fzero)
   {
      /* Check that zero is represented in the current graph bounds.
         If not, check the user wishes to proceed
      */
      if(graph.xmin > 0.0 || graph.xmax < 0.0 ||
         graph.ymin > 0.0 || graph.ymax < 0.0)
      {
         if(!ReqMessage2(MainWind,"Your graph bounds do not include zero.",
                                  "Proceed?",1))
         {
            graph.fzero = FALSE;
            Uncheck(MenuFZero);
         }
      }
   }
   
   return(0);
}

/***************************************************************************/
/*>SetTitle(APTR object)
   ---------------------
   Displays and processes the graph title requester.
   04.03.92 Framework
   01.04.92 Original
   08.04.92 Added auto-gadget activation
   09.04.92 Responds to mouse click in main window.
   14.04.92 Fixed auto-positioning for logs
   05.05.92 Moved PS->Amiga font translation to SetupFont
   22.06.92 Corrected positioning of pie title
*/
int SetTitle(APTR object)
{
   struct Window        *ReqWind;
   struct IntuiMessage  *message;
   ULONG                Signals,
                        MIClass;
   USHORT               MICode;
   struct Gadget        *gadget;
   int                  screenx,
                        screeny;
   double               x,
                        y;
   
   /* If the graph title has been switched off (by setting zero coords
      for both x and y), then centre the title.
   */
   if(GTitle.x == 0.0 && GTitle.y == 0.0)
   {
      if(graph.logx)
         GTitle.x = alog(log10(graph.xmin)+(log10(graph.xmax)-log10(graph.xmin))/2.0);
      else
         GTitle.x = graph.xmin + (graph.xmax - graph.xmin) / 2.0;

      if(graph.logy)
         GTitle.y = alog(log10(graph.ymin)+(log10(graph.ymax)-log10(graph.ymin))*0.9);
      else
         GTitle.y = graph.ymin + (graph.ymax - graph.ymin) * 0.9;

      if(graph.style == STYLE_PIE)
      {
         GTitle.x =  0.0;
         GTitle.y = 1.1 * PIESIZE;
      }
   }
   
   /* Copy info from GTitle structure to buffers */
   strcpy(TitleTextBuff,  GTitle.text);
   strcpy(TitleFontBuff,  GTitle.PSFont);
   sprintf(TitleSizeBuff, "%lg",GTitle.PSSize);
   sprintf(TitlePosXBuff, "%lg",GTitle.x);
   sprintf(TitlePosYBuff, "%lg",GTitle.y);
   
   /* Display the requester and process gadgets.
      We use OpenWindow, 'cos we don't want to lock the parent */
   ReqWind = (struct Window *)OpenWindow(NWTitle);

   ActivateGadget(TitleText, ReqWind, NULL);

   do
   {
      /* Wait on both the requester window and the main window */
      Signals = Wait(1<<ReqWind->UserPort->mp_SigBit |
                     1<<MainWind->UserPort->mp_SigBit);
      /* Handle messages from requester window */
      while((message=(struct IntuiMessage *)GetMsg(ReqWind->UserPort))!=NULL)
      {
         gadget = NULL;
         if((MIClass = message->Class) == IDCMP_GADGETUP)
            gadget = (struct Gadget *)message->IAddress;

         ReplyMsg((struct Message *)message);

         /* Recalc. the centred coordinates */
         if(gadget == TitleCentre)
         {
            double xtitle,
                   ytitle;
                   
            if(graph.logx)
               xtitle = alog(log10(graph.xmin) + 
                            (log10(graph.xmax) - log10(graph.xmin)) / 2.0);
            else
               xtitle = graph.xmin + (graph.xmax - graph.xmin) / 2.0;

            if(graph.logy)
               ytitle = alog(log10(graph.ymin) +
                            (log10(graph.ymax) - log10(graph.ymin)) * 0.9);
            else
               ytitle = graph.ymin + (graph.ymax - graph.ymin) * 0.9;

            if(graph.style == STYLE_PIE)
            {
               xtitle =  0.0;
               ytitle =  1.1 * PIESIZE;
            }
            sprintf(TitlePosXBuff, "%lg",xtitle);
            sprintf(TitlePosYBuff, "%lg",ytitle);
            RefreshGList(TitlePosX,ReqWind,NULL,2);
            ActivateGadget(TitleText, ReqWind, NULL);
         }
         else if(gadget == TitlePosX)
            ActivateGadget(TitlePosY, ReqWind, NULL);
         else if(gadget == TitlePosY)
            ActivateGadget(TitleText, ReqWind, NULL);
         else if(gadget == TitleText)
            ActivateGadget(TitlePosX, ReqWind, NULL);
      }
      
      /* Handle messages from main window */
      while((message=(struct IntuiMessage *)GetMsg(MainWind->UserPort))!=NULL)
      {
         MIClass = message->Class;
         MICode  = message->Code;
         screenx = message->MouseX;
         screeny = message->MouseY;
         ReplyMsg((struct Message *)message);
         
         gadget = NULL;
   
         if(data && (MIClass == IDCMP_MOUSEBUTTONS) && (MICode == SELECTDOWN))
         {
            ActivateWindow(ReqWind);
            ScreenToData(screenx,screeny,&x,&y);

            sprintf(TitlePosXBuff, "%lg",x);
            sprintf(TitlePosYBuff, "%lg",y);

            RefreshGList(TitlePosX, ReqWind, NULL, 2);
            ActivateGadget(TitleText, ReqWind, NULL);
         }
      }
   } while (gadget==NULL || !(gadget->Activation & GACT_ENDGADGET));
      
   if(gadget != TitleCancel)
   {
      /* Copy info from buffers to GTitle structure */
      strcpy(GTitle.text,   TitleTextBuff);
      strcpy(GTitle.PSFont, TitleFontBuff);
      sscanf(TitleSizeBuff, "%lf",&GTitle.PSSize);
      sscanf(TitlePosXBuff, "%lf",&GTitle.x);
      sscanf(TitlePosYBuff, "%lf",&GTitle.y);
   }
   
   CloseWindow(ReqWind);
   
   return(0);
}

/***************************************************************************/
/*>SetAxTitle(APTR object)
   -----------------------
   Displays and processes the axis title requester.
   04.03.92 Framework
   01.04.92 Original
*/
int SetAxTitle(APTR object)
{
   struct Window *ReqWind;

   strcpy(XTitleTextBuff,  XTitle.text);
   strcpy(XTitleFontBuff,  XTitle.PSFont);
   sprintf(XTitleSizeBuff, "%lg",XTitle.PSSize);

   strcpy(YTitleTextBuff,  YTitle.text);
   strcpy(YTitleFontBuff,  YTitle.PSFont);
   sprintf(YTitleSizeBuff, "%lg",YTitle.PSSize);
   
   ReqWind = DisplayWindowReq(NWAxisTitles,MainWind);
   ActivateGadget(XTitleGadget, ReqWind, NULL);
   
   if(WaitForEndGadget(ReqWind) != AxisTitleCancel)
   {
      strcpy(XTitle.text,    XTitleTextBuff);
      strcpy(XTitle.PSFont,  XTitleFontBuff);
      sscanf(XTitleSizeBuff, "%lf",&XTitle.PSSize);
   
      strcpy(YTitle.text,    YTitleTextBuff);
      strcpy(YTitle.PSFont,  YTitleFontBuff);
      sscanf(YTitleSizeBuff, "%lf",&YTitle.PSSize);
   }
   RemoveWindowReq(ReqWind,MainWind);
   
   return(0);
}

/***************************************************************************/
/*>SetAxLabel(APTR object)
   -----------------------
   Displays and processes the axis labels requester.
   04.03.92 Framework
   01.04.92 Original
   08.04.92 Added auto-gadget activation
   06.07.92 Disable precision gadgets for log axes. Changed gadget
            activation logic to match.
   10.07.92 Added setting of graph.userprec. Disable gadgets for Piecharts.
   21.07.92 Stopped disabling precision gadgets for log axes. N.B. with
            log axes we only use the *sign* of the precision value.
            Consequently changed activation chain.
   24.07.92 Corrected first gadget to be activated.
*/
int SetAxLabel(APTR object)
{
   struct Window        *ReqWind;
   struct IntuiMessage  *message;
   ULONG                Signals,
                        MIClass;
   struct Gadget        *gadget;
   int                  SaveNoXLab,
                        SaveUseLabels,
                        SaveUserPrec;

   strcpy(AxLabXFontBuff,          XLabel.PSFont);
   strcpy(AxLabYFontBuff,          YLabel.PSFont);
   sprintf(AxLabXSizeBuff,"%lg",   XLabel.PSSize);
   sprintf(AxLabYSizeBuff,"%lg",   YLabel.PSSize);
   sprintf(AxLabXPrecBuff,"%d",    graph.xprecision);
   sprintf(AxLabYPrecBuff,"%d",    graph.yprecision);

   SaveNoXLab              = graph.noxlabels;
   SaveUseLabels           = barstyle.uselabels;
   SaveUserPrec            = graph.userprec;

   if(graph.noxlabels)     Select(AxLabNoX);
   else                    Unselect(AxLabNoX);

   if(graph.noylabels)     Select(AxLabNoY);
   else                    Unselect(AxLabNoY);

   if(graph.centrexlabels) Select(AxLabCentreX);
   else                    Unselect(AxLabCentreX);
   
   if(graph.style == STYLE_PIE)
      AxLabXPrec->Flags |= GFLG_DISABLED;
   else
      AxLabXPrec->Flags &= ~GFLG_DISABLED;
   
   if(graph.style == STYLE_PIE)
      AxLabYPrec->Flags |= GFLG_DISABLED;
   else                    
      AxLabYPrec->Flags &= ~GFLG_DISABLED;

   if(graph.style == STYLE_PIE)
   {
      AxLabNoX->Flags      |= GFLG_DISABLED;
      AxLabNoY->Flags      |= GFLG_DISABLED;
      AxLabCentreX->Flags  |= GFLG_DISABLED;
   }
   else
   {
      AxLabNoX->Flags      &= ~GFLG_DISABLED;
      AxLabNoY->Flags      &= ~GFLG_DISABLED;
      AxLabCentreX->Flags  &= ~GFLG_DISABLED;
   }




   
   ReqWind = DisplayWindowReq(NWAxisLabels,MainWind);

   if(graph.style != STYLE_PIE)
      ActivateGadget(AxLabXPrec, ReqWind, NULL);

   do
   {
      Signals = Wait(1<<ReqWind->UserPort->mp_SigBit);
      while((message=(struct IntuiMessage *)GetMsg(ReqWind->UserPort))!=NULL)
      {
         gadget = NULL;
         if((MIClass = message->Class) == IDCMP_GADGETUP)
            gadget = (struct Gadget *)message->IAddress;

         ReplyMsg((struct Message *)message);

         /* Display the bar labels requester */
         if(gadget == AxLabBarLabels)
         {
            BarLabels(ReqWind);

            /* TODO: gadget should be removed and replaced... */
            if(graph.noxlabels)     Select(AxLabNoX);
            else                    Unselect(AxLabNoX);
            RefreshGList(AxLabNoX,ReqWind,NULL,1);
         }
         else if(gadget == AxLabXPrec)
         {
            graph.userprec = TRUE;
            ActivateGadget(AxLabYPrec, ReqWind, NULL);
         }
         else if(gadget == AxLabYPrec)
         {
            graph.userprec = TRUE;
            ActivateGadget(AxLabXPrec, ReqWind, NULL);
         }
      }
   } while (gadget==NULL || !(gadget->Activation & GACT_ENDGADGET));

   if(gadget != AxisLabelsCancel)
   {
      strcpy(XLabel.PSFont,AxLabXFontBuff);
      strcpy(YLabel.PSFont,AxLabYFontBuff);
      sscanf(AxLabXSizeBuff,"%lf", &XLabel.PSSize);
      sscanf(AxLabYSizeBuff,"%lf", &YLabel.PSSize);
      sscanf(AxLabXPrecBuff,"%d",  &graph.xprecision);
      sscanf(AxLabYPrecBuff,"%d",  &graph.yprecision);
   
      graph.noxlabels     = isSET(AxLabNoX);
      graph.noylabels     = isSET(AxLabNoY);
      graph.centrexlabels = isSET(AxLabCentreX);
   }
   else if(gadget == AxisLabelsCancel)
   {
      /* Needed in case BarLabels() has changed these */
      graph.noxlabels      = SaveNoXLab;
      barstyle.uselabels   = SaveUseLabels;
      graph.userprec       = SaveUserPrec;
   }

   RemoveWindowReq(ReqWind,MainWind);
   
   return(0);
}

/***************************************************************************/
/*>SetKey(APTR object)
   -------------------
   Displays and processes the key requester.
   04.03.92 Original
   08.04.92 Added auto-gadget activation and treating text as next
   09.04.92 Responds to mouse clicks in main window for position
   23.06.92 Added setting of graph.usekey for Piecharts. Added disable/
            enable of string gadgets for Piecharts.
   24.06.92 Re-enabled position gadgets for Piecharts and added change to
            activation chain for Piecharts. Set default position for
            Pie key.
   10.07.92 No longer activate text gadget after clicking on main window
            for Pie charts. Always activate Xpos initially.
*/
int SetKey(APTR object)
{
   struct Window        *ReqWind;
   struct IntuiMessage  *message;
   ULONG                Signals,
                        MIClass;
   USHORT               MICode;
   struct Gadget        *gadget;
   char                 *text;      /* Pointer to key text being handled */
   DATASET              *p;
   int                  i,
                        screenx,
                        screeny;
   double               x,
                        y;
   
   if(data != NULL)
   {
      /* If it's a Piechart, set default key position, if nescessary */
      if(graph.style == STYLE_PIE && 
         data->key.x == 0.0       && 
         data->key.y == 0.0)
      {
         data->key.x = PIESIZE;
         data->key.y = PIESIZE;
      }
      
      /* Step along to the appropriate dataset */
      for(i=0, p=data; i<graph.CurrentKey && p; NEXT(p), i++);
      text = p->key.text;
      
      strcpy(KeyTextBuff,text);
      strcpy(KeyFontBuff,data->key.PSFont);
      sprintf(KeySizeBuff,"%lg",data->key.PSSize);
      sprintf(KeyPosXBuff,"%lg",data->key.x);
      sprintf(KeyPosYBuff,"%lg",data->key.y);
      sprintf(KeyDatasetBuff,"%d",graph.CurrentKey+1);
   
      if(graph.style == STYLE_PIE)           /* Disable text               */
      {
         KeyText->Flags    |=  GFLG_DISABLED;
         KeyPrev->Flags    |=  GFLG_DISABLED;
         KeyNext->Flags    |=  GFLG_DISABLED;
         KeyDataset->Flags |=  GFLG_DISABLED;
      }
      else                                   /* Enable text                */
      {
         KeyText->Flags    &= ~GFLG_DISABLED;
         KeyPrev->Flags    &= ~GFLG_DISABLED;
         KeyNext->Flags    &= ~GFLG_DISABLED;
         KeyDataset->Flags &= ~GFLG_DISABLED;
      }

      /* Use OpenWindow() since we don't want to lock the parent */
      ReqWind = (struct Window *)OpenWindow(NWKey);
   
      ActivateGadget(KeyXPos,ReqWind,NULL);

      do
      {
         /* Wait on requester and main window */
         Signals = Wait(1<<ReqWind->UserPort->mp_SigBit |
                        1<<MainWind->UserPort->mp_SigBit);
         /* Handle messages from requester */
         while((message=(struct IntuiMessage *)GetMsg(ReqWind->UserPort))!=NULL)
         {
            gadget = NULL;
            if((MIClass = message->Class) == IDCMP_GADGETUP)
               gadget = (struct Gadget *)message->IAddress;
   
            ReplyMsg((struct Message *)message);
   
            /* Handle the gadgets */
            if(gadget == KeyPrev ||
               gadget == KeyNext ||
               gadget == KeyDataset)
            {
               text = ChangeKey(ReqWind,gadget);
               strcpy(KeyTextBuff,text);
               RefreshGList(KeyText,ReqWind,NULL,1);
               ActivateGadget(KeyText,ReqWind,NULL);
            }
            else if(gadget == KeyXPos)
            {
               ActivateGadget(KeyYPos,ReqWind,NULL);
            }
            else if(gadget == KeyYPos)
            {
               if(graph.style == STYLE_PIE)
                  ActivateGadget(KeyXPos,ReqWind,NULL);
               else
                  ActivateGadget(KeyText,ReqWind,NULL);
            }
            else if(gadget == KeyText)
            {
               /* Hit return in the y-position, treat as next */
               text = ChangeKey(ReqWind,KeyNext);
               strcpy(KeyTextBuff,text);
               RefreshGList(KeyText,ReqWind,NULL,1);
               ActivateGadget(KeyText,ReqWind,NULL);
            }
         }
         /* Handle messages from main window */
         while((message=(struct IntuiMessage *)GetMsg(MainWind->UserPort))!=NULL)
         {
            MIClass = message->Class;
            MICode  = message->Code;
            screenx = message->MouseX;
            screeny = message->MouseY;
            ReplyMsg((struct Message *)message);
         
            gadget = NULL;
   
            if((MIClass == IDCMP_MOUSEBUTTONS) && (MICode == SELECTDOWN))
            {
               ActivateWindow(ReqWind);
               ScreenToData(screenx,screeny,&x,&y);
               sprintf(KeyPosXBuff,"%lg",x);
               sprintf(KeyPosYBuff,"%lg",y);
               RefreshGList(KeyXPos, ReqWind, NULL, 2);
               if(graph.style != STYLE_PIE) ActivateGadget(KeyText, ReqWind, NULL);
            }
         }
      } while (gadget==NULL || !(gadget->Activation & GACT_ENDGADGET));
   
      strcpy(text,KeyTextBuff);

      if(gadget == KeyRemove)
      {
         ClearAllKeys();
         graph.usekey = FALSE;
      }
      else
      {
         strcpy(data->key.PSFont,KeyFontBuff);
         sscanf(KeySizeBuff,"%lf",&data->key.PSSize);
         sscanf(KeyPosXBuff,"%lf",&data->key.x);
         sscanf(KeyPosYBuff,"%lf",&data->key.y);
         graph.usekey = TRUE;
      }
   
      CloseWindow(ReqWind);
   }
   else
   {
      ReqMessage(MainWind,"Data not yet read",0);
   }
   
   return(0);
}

/***************************************************************************/
/*>ClearAllKeys(void)
   ------------------
   Clears all the current keys
   01.04.92 Original
*/
int ClearAllKeys(void)
{
   DATASET *p;
   
   for(p=data;p;NEXT(p))
   {
      p->key.PSSize      = Defaults.KeyPSSize;
      p->key.AmigaSize   = Defaults.KeyAmigaSize;
      p->key.x           = 0.0;
      p->key.y           = 0.0;
      p->key.text[0]     = '\0';
      strcpy(p->key.PSFont,   Defaults.KeyPSFont);
      strcpy(p->key.AmigaFont,Defaults.KeyAmigaFont);
   }
   
   return(0);
}

/***************************************************************************/
/*>char *ChangeKey(struct Window *ReqWind, struct Gadget *gadget)
   --------------------------------------------------------------
   Changes which key text we're playing with.

   Input:   struct Window     *ReqWind    Pointer to requester window
            struct Gadget     *gadget     Pointer to change key gadget hit
   Returns: char *                        Pointer to current key text.

   01.04.92 Original
*/
char *ChangeKey(struct Window *ReqWind,
                struct Gadget *gadget)
{
   char        *text;
   DATASET     *p;
   int         i;
   
   /* Store the string in the correct key */
   /* Step along to the appropriate dataset */
   for(i=0, p=data; i<graph.CurrentKey && p; NEXT(p), i++);
   strcpy(p->key.text,KeyTextBuff);

   if(gadget == KeyNext)
   {
      /* try to increment key number */
      if(++graph.CurrentKey >= graph.nplots) graph.CurrentKey--;
   }
   else if(gadget == KeyPrev)
   {
      /* try to decrement key number */
      if(--graph.CurrentKey < 0) graph.CurrentKey++;
   }
   else if(gadget == KeyDataset)
   {
      /* get the key number from the buffer */
      sscanf(KeyDatasetBuff,"%d",&graph.CurrentKey);
      graph.CurrentKey = RANGECHECK(graph.CurrentKey,1,graph.nplots);
      graph.CurrentKey--;
   }

   /* Place the key number back into the buffer */
   sprintf(KeyDatasetBuff,"%d",graph.CurrentKey+1);
   RefreshGList(KeyDataset,ReqWind,NULL,1);

   /* Step along to the appropriate dataset */
   for(i=0, p=data; i<graph.CurrentKey && p; NEXT(p), i++);
   /* Set text pointer to the text for this dataset's key */
   text = p->key.text;
   
   return(text);
}

/***************************************************************************/
/*>SetRegress(APTR object)
   -----------------------
   04.03.92 Framework
   02.04.92 Original
   01.05.92 Removed reading of values on Off exit
   24.07.92 Added check mark handling
*/
int SetRegress(APTR object)
{
   struct Window *ReqWind;
   struct Gadget *gadget;

   sprintf(RegressDegreeBuff, "%d",    fitting.regress_npoly);
   sprintf(RegressTolBuff,    "%lg",   fitting.regress_tol);
   if(fitting.regress_write)  Select(RegressionWrite);
   else                       Unselect(RegressionWrite);

   ReqWind = DisplayWindowReq(NWRegression,MainWind);
   gadget = WaitForEndGadget(ReqWind);
   
   if(gadget == RegressionOK)
   {
      fitting.regression    = TRUE;
      fitting.regress_write = isSET(RegressionWrite);
      sscanf(RegressDegreeBuff, "%d",  &fitting.regress_npoly);
      fitting.regress_npoly = RANGECHECK(fitting.regress_npoly, 2, 20);
      sscanf(RegressTolBuff,    "%lf", &fitting.regress_tol);
   }
   else if(gadget == RegressionOff)
   {
      fitting.regression   = FALSE;
   }

   RemoveWindowReq(ReqWind,MainWind);

   if(fitting.regression)  Check(MenuRegress);
   else                    Uncheck(MenuRegress);
   
   return(0);
}

/***************************************************************************/
/*>ToggleRobust(APTR object)
   -------------------------
   04.03.92 Framework
   02.04.92 Original
   24.07.92 If came from menu, now checks menu CHECK status rather than
            toggling
*/
int ToggleRobust(APTR object)
{
   if(object == NULL)   toggle(fitting.robust);
   else                 fitting.robust = ISCHECKED(MenuRobust);
   
   return(0);
}

/***************************************************************************/
/*>SetFourier(APTR object)
   -----------------------
   04.03.92 Framework
   02.04.92 Original
   24.07.92 Added check mark handling
*/
int SetFourier(APTR object)
{
   struct Window *ReqWind;
   struct Gadget *gadget;
   
   sprintf(FourierBuff,"%lg",fitting.smooth_pts);

   ReqWind = DisplayWindowReq(NWFourier,MainWind);
   gadget = WaitForEndGadget(ReqWind);
   
   if(gadget == FourierOK)
   {
      fitting.fourier = TRUE;
      sscanf(FourierBuff,"%lf",&fitting.smooth_pts);
   }
   else if(gadget == FourierOff)
   {
      fitting.fourier = FALSE;
      sscanf(FourierBuff,"%lf",&fitting.smooth_pts);
   }
   
   RemoveWindowReq(ReqWind,MainWind);

   if(fitting.fourier)  Check(MenuFourier);
   else                 Uncheck(MenuFourier);
   
   return(0);
}

/***************************************************************************/
/*>FillControl(APTR object)
   ------------------------
   12.06.92 Original
   Display and process fill control requester. Replaces the MX Grey and Hatch
   menu subitems.
*/
int FillControl(APTR object)
{
   struct Window        *ReqWind;
   struct Gadget        *gadget;
   
   sprintf(FillHatchThickBuff, "%lg",output.hatchthick);
   sprintf(FillHatchSpaceBuff, "%lg",output.hatchspace);

   if(output.hatching)
      HandleMXGadget(NULL, FillStyle[1]);
   else
      HandleMXGadget(NULL, FillStyle[0]);
   
   ReqWind = DisplayWindowReq(NWFill,MainWind);
   gadget  = WaitForEndGadget(ReqWind);
   
   if(gadget == FillOK)
   {
      sscanf(FillHatchThickBuff, "%lf",&output.hatchthick);
      sscanf(FillHatchSpaceBuff, "%lf",&output.hatchspace);
      output.hatching = ((MXChoice(FillStyle) == 1) ? TRUE : FALSE);
   }
   
   RemoveWindowReq(ReqWind,MainWind);

   return(0);
}

/***************************************************************************/
/*>BarLabels(struct Window *ParentWind)
   ------------------------------------
   Displays and handles the Bar Labels `requester'. The parent window is
   locked while the requester is displayed.

   Input:   struct Window     *ParentWind    Pointer to parent window.

   01.04.92 Framework
   02.04.92 Original
   11.06.92 Changed message if no bar label memory.
   19.06.92 Added string copy when OK clicked.
*/
int BarLabels(struct Window *ParentWind)
{
   struct Window        *ReqWind;
   struct IntuiMessage  *message;
   ULONG                Signals,
                        MIClass;
   struct Gadget        *gadget;
   char                 *text;
   
   if(barlabels)
   {
      strcpy(BarLabTextBuff,barlabels[barstyle.CurrentBar].text);
      sprintf(BarLabNumBuff,"%d",barstyle.CurrentBar+1);
      
      ReqWind = DisplayWindowReq(NWBarLabels,ParentWind);
   
      ActivateGadget(BarLabText,ReqWind,NULL);
      do
      {
         Signals = Wait(1<<ReqWind->UserPort->mp_SigBit);
         while((message=(struct IntuiMessage *)GetMsg(ReqWind->UserPort))!=NULL)
         {
            gadget = NULL;
            if((MIClass = message->Class) == IDCMP_GADGETUP)
               gadget = (struct Gadget *)message->IAddress;
   
            ReplyMsg((struct Message *)message);
   
            /* Handle the gadgets */
            if(gadget == BarLabPrev ||
               gadget == BarLabNext ||
               gadget == BarLabNum)
            {
               /* A change current label gadget */
               text = ChangeBarLab(ReqWind,gadget);
               strcpy(BarLabTextBuff,text);
               RefreshGList(BarLabText,ReqWind,NULL,1);
               ActivateGadget(BarLabText,ReqWind,NULL);
            }
            else if(gadget == BarLabText)
            {
               /* The main text gadget - treat as NEXT */
               text = ChangeBarLab(ReqWind,BarLabNext);
               strcpy(BarLabTextBuff,text);
               RefreshGList(BarLabText,ReqWind,NULL,1);
               ActivateGadget(BarLabText,ReqWind,NULL);
            }
         }
      } while (gadget==NULL || !(gadget->Activation & GACT_ENDGADGET));
   
      RemoveWindowReq(ReqWind,ParentWind);

      if(gadget==BarLabOK)
      {
         /* Store current string in the correct barlabel (in case user
            hasn't clicked NEXT or hit return before OK
         */
         strcpy(barlabels[barstyle.CurrentBar].text,BarLabTextBuff);
         barstyle.uselabels   = TRUE;
         graph.noxlabels      = TRUE;
      }
      else if(gadget==BarLabRemove)
      {
         barstyle.uselabels   = FALSE;
         graph.noxlabels      = FALSE;
      }
   }
   else
   {
      ReqMessage(MainWind,"Bar labels not available",0);
   }
   
   return(0);
}

/***************************************************************************/
/*>char *ChangeBarLab(struct Window *ReqWind, struct Gadget *gadget)
   -----------------------------------------------------------------
   Processes clicks on the change current bar label gadgets.

   Input:   struct Window     *ReqWind    Window containing gadgets
            struct Gadget     *gadget     The gadget clicked
   Returns: char *                        Pointer to current bar label text
   
   02.04.92 Original
*/
char *ChangeBarLab(struct Window *ReqWind, 
                   struct Gadget *gadget)
{
   char        *text;
   
   /* Store the current string in the correct barlabel */
   strcpy(barlabels[barstyle.CurrentBar].text,BarLabTextBuff);

   if(gadget == BarLabNext)
   {
      /* try to increment key number */
      if(++barstyle.CurrentBar >= barstyle.nbar) barstyle.CurrentBar--;
   }
   else if(gadget == BarLabPrev)
   {
      /* try to decrement key number */
      if(--barstyle.CurrentBar < 0) barstyle.CurrentBar++;
   }
   else if(gadget == BarLabNum)
   {
      /* get the key number from the buffer */
      sscanf(BarLabNumBuff,"%d",&barstyle.CurrentBar);
      barstyle.CurrentBar = RANGECHECK(barstyle.CurrentBar,1,barstyle.nbar);
      barstyle.CurrentBar--;
   }

   /* Place the key number back into the buffer */
   sprintf(BarLabNumBuff,"%d",barstyle.CurrentBar+1);
   RefreshGList(BarLabNum,ReqWind,NULL,1);

   /* Set text pointer to the text for this dataset's key */
   text = barlabels[barstyle.CurrentBar].text;
   
   return(text);
}

/***************************************************************************/
/*>TEXTLIST *ChangeExtraLabel(struct Window *ReqWind, struct Gadget *gadget)
   -------------------------------------------------------------------------
   Processes the Next, Prev and Kill label gadgets. Depending on the gadget,
   changes the current label, allocating extra memory, if necessary, or 
   freeing memory if a label is being killed.

   Input:   struct Window     *ReqWind    Window containing gadgets
            struct Gadget     *gadget     The gadget clicked
   Returns: TEXTLIST *                    Pointer to current label structure

   03.04.92 Original
   16.06.92 Added handling for Pie charts
   19.06.92 Few corrections to Pie handling
   22.06.92 Placed piechart handling into #ifdef SEPPIEREQ's. Currently
            NOT defined.
*/
TEXTLIST *ChangeExtraLabel(struct Window *ReqWind,
                           struct Gadget *gadget)
{
   TEXTLIST *p,
            *q;
   int      i;
   
   /* Move to current label */
   for(p=labels, i=0; p && i<graph.CurrentLabel; NEXT(p), i++);
   /* Copy buffers into this label */
   if(p)
   {
#ifdef SEPPIEREQ
      if(graph.style == STYLE_PIE)
      {
         double angle;
         
         strcpy(p->stext.text,PieLabTextBuff);
         strcpy(p->stext.PSFont,PieLabFontBuff);
         sscanf(PieLabSizeBuff,"%lf",&p->stext.PSSize);
         sscanf(PieLabAngleBuff,"%lf",&angle);
         sscanf(PieLabDistBuff,"%lf",&p->stext.y);
         p->stext.x = PI * angle/180.0;
      }
      else
      {
#endif
         strcpy(p->stext.text,LabelTextBuff);
         strcpy(p->stext.PSFont,LabelFontBuff);
         sscanf(LabelSizeBuff,"%lf",&p->stext.PSSize);
         sscanf(LabelPosXBuff,"%lf",&p->stext.x);
         sscanf(LabelPosYBuff,"%lf",&p->stext.y);
#ifdef SEPPIEREQ
      }
#endif
   }

   if(gadget == LabelNext || gadget == PieLabNext)
   {
      /* Increment the label number */
      if(++graph.CurrentLabel > graph.nlabels)
      {
         if(!AllocExtraLabel())  return(p);
      }
      /* Move on to next label */
      NEXT(p);
   }
   else if(gadget == LabelPrev || gadget == PieLabPrev)
   {
      /* try to decrement key number and move back one */
      if(--graph.CurrentLabel < 0) graph.CurrentLabel++;
      else                         PREV(p);
   }
   else if(gadget == LabelKill || gadget == PieLabKill)
   {
      /* Check the label list exists */
      if(p)
      {
         /* Check there's not just one label left */
         if(p->prev != NULL || p->next != NULL)
         {
            q = p->prev;                  /* Record previous label         */
            if(q)                         /* Not the very first label      */
            {
               q->next       = p->next;   /* Unlink current label          */
               if(p->next) p->next->prev = q;
               free(p);                   /* Free current label            */
               if(q->next) p = q->next;   /* Step p on if possible         */
               else        p = q;
            }
            else                          /* Is the very first label       */
            {
               p->next->prev = NULL;      /* Unlink current label          */
               q = p->next;               /* Record next label             */
               free(p);                   /* Free current label            */
               labels = p = q;            /* Set p to next label           */
            }
            graph.nlabels--;              /* Decrement number of labels    */

            /* Try to decrement key number */
            if(--graph.CurrentLabel < 0) graph.CurrentLabel++;
         }
      }
   }

   /* Copy data for the current label into gadget buffers */      
   if(p)
   {
#ifdef SEPPIEREQ
      if(graph.style == STYLE_PIE)
      {
         strcpy(PieLabTextBuff, p->stext.text);
         strcpy(PieLabFontBuff, p->stext.PSFont);
         sprintf(PieLabSizeBuff,"%lg",p->stext.PSSize);
         sprintf(PieLabAngleBuff,"%lg",180.0 * p->stext.x / PI);
         sprintf(PieLabDistBuff,"%lg",p->stext.y);
      }
      else
      {
#endif
         strcpy(LabelTextBuff, p->stext.text);
         strcpy(LabelFontBuff, p->stext.PSFont);
         sprintf(LabelSizeBuff,"%lg",p->stext.PSSize);
         sprintf(LabelPosXBuff,"%lg",p->stext.x);
         sprintf(LabelPosYBuff,"%lg",p->stext.y);
#ifdef SEPPIEREQ
      }
#endif
   }

   /* Refresh gadgets */
#ifdef SEPPIEREQ
   if(graph.style == STYLE_PIE)
      RefreshGadgets(PieLabText,ReqWind,NULL);
   else
#endif
      RefreshGadgets(LabelText,ReqWind,NULL);

   return(p);
}

/***************************************************************************/
/*>ChangePen(APTR object)
   ----------------------
   04.03.92 Framework
   02.04.92 Extended Framework
   09.04.92 Original
   15.05.92 Added check on presence of data
*/
int ChangePen(APTR object)
{
   struct Window  *ReqWind;
   static int     FirstCall = TRUE;
   int            pen,
                  xscreen,
                  yscreen;
   DATASET        *d;

   if(FirstCall)
   {
      FirstCall = FALSE;
      strcpy(PenNumberBuff,"1");    /* Default to pen 1 */
   }
   
   ReqWind = DisplayWindowReq(NWPen,MainWind);
   pen = 0;
   ActivateGadget(PenNumGadget,ReqWind,NULL);
   
   if(WaitForEndGadget(ReqWind) != PenCancel)
   {
      sscanf(PenNumberBuff,"%d",&pen);
      pen = RANGECHECK(pen,1,MAXPEN);
   }
   RemoveWindowReq(ReqWind,MainWind);
      
   if(pen && data!=NULL)
   {
      /* Wait for the user to click */
      if(WaitForClick(MainWind,&xscreen, &yscreen))
      {
         /* Find data set with a point nearest to this point */
         d = FindDataSet(xscreen,yscreen);
         /* Set the pen for this dataset */
         d->pen = pen;
      }
   }
   
   return(0);
}

/***************************************************************************/
/*>WaitForClick(struct Window *wind, int *x, int *y)
   -------------------------------------------------
   Waits for the user to click in a window and returns the screen coordinates
   09.04.92 Original
*/

int WaitForClick(struct Window *wind,
             int           *x,
             int           *y)
{
   struct IntuiMessage  *message;
   ULONG                Signals,
                        MIClass;
   USHORT               MICode;
   int                  clicked = FALSE,
                        retval;

   ClickPointer(MainWind);
   
   do
   {
      Signals = Wait(1<<wind->UserPort->mp_SigBit);
      while(message=(struct IntuiMessage *)GetMsg(wind->UserPort))
      {
         MIClass = message->Class;
         MICode  = message->Code;
         *x      = message->MouseX;
         *y      = message->MouseY;
         ReplyMsg((struct Message *)message);
   
         if((MIClass == IDCMP_MOUSEBUTTONS) && (MICode == SELECTDOWN))
         {
            clicked = TRUE;
            retval  = TRUE;
         }
         else if((MIClass == IDCMP_MOUSEBUTTONS) && (MICode == MENUDOWN))
         {
            clicked = TRUE;
            retval  = FALSE;
         }
      }
   } while(!clicked);
   
   ClearPointer(MainWind);

   return(retval);
}

/***************************************************************************/
/*>ScreenToData(int xscreen, int yscreen, double *x, double *y)
   ------------------------------------------------------------
   Converts screen coordinates to data coordinates
   09.04.92 Original
   10.04.92 Added log support
   27.04.92 Fixed log support (log10() not log()!)
*/
int ScreenToData(int    xscreen, 
             int    yscreen, 
             double *x, 
             double *y)
{
   if(graph.logx)
      *x = alog(log10(graph.xmin) + (xscreen - display.xoff)/graph.XScale);
   else
      *x = graph.xmin + (xscreen - display.xoff)/graph.XScale;

   if(graph.logy)
      *y = alog(log10(graph.ymin)+(display.WYSize-display.yoff-yscreen)/graph.YScale);
   else
      *y = graph.ymin + (display.WYSize - display.yoff - yscreen)/graph.YScale;
   
   return(0);
}

/***************************************************************************/
/*>DATASET *FindDataSet(int x, int y)
   ----------------------------------
   Finds the dataset which contains the nearest coordinate.
   09.04.92 Original
   15.05.92 Changed to take screen coordinate parameters rather than
            data coordinates.
*/

DATASET *FindDataSet(int x,
                     int y)
{
   DATASET *d,
           *dnear;
   int     dist,
           ndist,
           i,
           xs,
           ys;
   
   dnear = data;
   
   DataToScreen(data->x[0], data->y[0], &xs, &ys);

   ndist = (xs - x)*(xs - x) + (ys - y)*(ys - y);
   
   for(d=data; d; NEXT(d))
   {
      for(i=0; i<d->ndata; i++)
      {
         DataToScreen(d->x[i], d->y[i], &xs, &ys);
         dist = (xs - x)*(xs - x) + (ys - y)*(ys - y);

         if(dist < ndist)
         {
            ndist = dist;
            dnear = d;
         }
      }
   }
   return(dnear);
}

/***************************************************************************/
/*>InstallMacro(APTR object)
   -----------------------
   Installs a Rexx macro to run on every plot
   29.04.92 Original
   06.07.92 Changed to get directory from default.directory
*/
int InstallMacro(APTR object)
{
   static char       FileBuff[MAXBUFF],      /* The filename               */
                     DirBuff[MAXBUFF];       /* The path                   */
   static int        FirstCall   = TRUE;     /* Flag for first call        */

   /* If this is our first call, get current directory */
   if(FirstCall)
   {
      strcpy(DirBuff,Defaults.directory);
      FirstCall = FALSE;
   }
   
   /* Get a filename */
   if(!GetAFile(AslBase,MainWind,MyScreen,"Rexx Macro",
                FileBuff,DirBuff,rexx.RexxMacro))
   {
      rexx.RexxMacro[0] = '\0';
   }
   
   return(0);
}

/***************************************************************************/
/*>RexxFit(APTR object)
   --------------------
   Sets up fitting by an ARexx function
   04.03.92 Framework
   02.04.92 Original
   30.04.92 Added Cancel condition code
   01.05.92 Rewritten to display and handle requester before file req.
   06.07.92 Changed to get directory from default.directory
   24.07.92 Added check mark handling
*/
int RexxFit(APTR object)
{
   struct Window        *ReqWind;
   struct IntuiMessage  *message;
   ULONG                Signals,
                        MIClass;
   struct Gadget        *gadget;
   static char          RexxFileBuff[MAXBUFF],  /* The filename            */
                        RexxDirBuff[MAXBUFF];   /* The path                */
   static int           FirstCall = TRUE;
   char                 CurrentMacro[MAXBUFF];  /* The current macro name  */

   /* If this is our first call, get current directory. */
   if(FirstCall)
   {
      FirstCall = FALSE;
      strcpy(RexxDirBuff,Defaults.directory);
   }
   

   sprintf(RexxFitDegreeBuff, "%d",    fitting.rexx_npoly);
   sprintf(RexxFitTolBuff,    "%lg",   fitting.rexx_tol);
   if(fitting.rexx_write)     Select(RexxFitWrite);
   else                       Unselect(RexxFitWrite);

   strcpy(CurrentMacro, fitting.rexxmacro);  /* For CANCEL */

   /* Display requester */
   ReqWind = DisplayWindowReq(NWRexxFit,MainWind);

   do
   {
      Signals = Wait(1<<ReqWind->UserPort->mp_SigBit);
      while((message=(struct IntuiMessage *)GetMsg(ReqWind->UserPort))!=NULL)
      {
         gadget = NULL;
         if((MIClass = message->Class) == IDCMP_GADGETUP)
            gadget = (struct Gadget *)message->IAddress;

         ReplyMsg((struct Message *)message);

         /* Display the bar labels requester */
         if(gadget == RexxFitFiles)
         {
            /* Get a filename */
            if(GetAFile(AslBase,MainWind,MyScreen,"Rexx Fitting Macro",
                        RexxFileBuff,RexxDirBuff,fitting.rexxmacro))
            {
               fitting.rexx = TRUE;
               RefreshGList(RexxFitFName,ReqWind,NULL,1);
            }
            else
            {
               fitting.rexx = FALSE;
               fitting.rexxmacro[0] = '\0';
            }
         }
      }
   } while (gadget==NULL || !(gadget->Activation & GACT_ENDGADGET));


   /* Handle exit conditions */
   if(gadget == RexxFitOK)
   {
      if(fitting.rexxmacro[0])
      {
         fitting.rexx       = TRUE;
         sscanf(RexxFitDegreeBuff, "%d",  &fitting.rexx_npoly);
         fitting.rexx_npoly = RANGECHECK(fitting.rexx_npoly, 2, 20);
         sscanf(RexxFitTolBuff,    "%lf", &fitting.rexx_tol);
         fitting.rexx_write = isSET(RexxFitWrite);

         /* Check full path specified */
         if((strchr(fitting.rexxmacro,'/') == NULL) &&
            (strchr(fitting.rexxmacro,':') == NULL))
         {
            fitting.rexx = FALSE;
            WriteMessage("Path for ARexx fitting macro must be specified.");
         }
         else
         {
            fitting.rexx = TRUE;
         }
      }
      else
      {
         fitting.rexx       = FALSE;
      }
   }
   else if(gadget == RexxFitOff)
   {
      fitting.rexx         = FALSE;
   }
   else if(gadget == RexxFitCancel)
   {
      strcpy(fitting.rexxmacro, CurrentMacro);
   }

   RemoveWindowReq(ReqWind,MainWind);

   if(fitting.rexx)  Check(MenuRexxFit);
   else              Uncheck(MenuRexxFit);

   return(0);
}

/***************************************************************************/
/*>RunMacro(APTR object)
   ---------------------
   Does a one-off run of an ARexx macro
   05.05.92 Original
   06.07.92 Changed to get directory from default.directory
*/
int RunMacro(APTR object)
{
   static char          FileBuff[MAXBUFF],      /* The filename            */
                        DirBuff[MAXBUFF];       /* The path                */
   char                 MacroName[MAXBUFF];     /* The macro name          */
   static int           FirstCall = TRUE;

   /* If this is our first call, get current directory. */
   if(FirstCall)
   {
      FirstCall = FALSE;
      strcpy(DirBuff,Defaults.directory);
   }
   
   /* Get a filename */
   if(GetAFile(AslBase,MainWind,MyScreen,"Run Rexx Macro",
               FileBuff,DirBuff,MacroName))
   {
      LaunchCmd(MacroName);
   }
   
   return(0);
}

/***************************************************************************/
/*>ToggleRexxDeb(APTR object)
   --------------------------
   Toggles the Rexx debug display
   05.05.92 Original
   01.07.92 No longer require CLI. Added message.
   24.07.92 If came from menu, now checks menu CHECK status rather than
            toggling
*/
int ToggleRexxDeb(APTR object)
{
   if(object == NULL)   toggle(rexx.debug);
   else                 rexx.debug = ISCHECKED(MenuDebRexx);

   if(rexx.debug)    WriteMessage("ARexx debugging started.");
   else              WriteMessage("ARexx debugging stopped.");

   return(0);
}

/***************************************************************************/
/*>SetLineDash(APTR object)
   ------------------------
   15.05.92 Original
*/
int SetLineDash(APTR object)
{
   int      style = 0;
   int      xscreen,
            yscreen;
   DATASET  *d;
   
   if     (object == (APTR)MenuLS1) style = 0;
   else if(object == (APTR)MenuLS2) style = 1;
   else if(object == (APTR)MenuLS3) style = 2;
   else if(object == (APTR)MenuLS4) style = 3;
   else if(object == (APTR)MenuLS5) style = 4;
   else if(object == (APTR)MenuLS6) style = 5;
   
   if(data)
   {
      /* Wait for the user to click */
      if(WaitForClick(MainWind,&xscreen, &yscreen))
      {
         /* Find data set with a point nearest to this point */
         d = FindDataSet(xscreen,yscreen);
         /* Set the linestyle for this dataset */
         d->linestyle = style;
      }
   }
   
   return(0);
}

/***************************************************************************/
/*>SetLabel(APTR object)
   ---------------------
   Displays and processes the Extra labels requester
   04.03.92 Framework
   03.04.92 Original
   09.04.92 Responds to mouse click in main window.
   16.06.92 Added handling of alternative requester for Pie charts.
   19.06.92 Corrected gadget refresh for Pie requester
   22.06.92 Placed piechart handling into #ifdef SEPPIEREQ's. Currently
            NOT defined.
   02.07.92 Calls RefreshScreen() rather than DoPlot() directly.
*/
int SetLabel(APTR object)
{
   struct Window        *ReqWind;
   struct IntuiMessage  *message;
   ULONG                Signals,
                        MIClass;
   USHORT               MICode;
   struct Gadget        *gadget;
   TEXTLIST             *p;
   int                  i,
                        screenx,
                        screeny;
   double               x,
                        y;

   /* Move to current label and copy into buffers */
   for(p=labels, i=0; p && i<graph.CurrentLabel; NEXT(p), i++);
   if(p)
   {
#ifdef SEPPIEREQ
      if(graph.style == STYLE_PIE)
      {
         strcpy(PieLabTextBuff,   p->stext.text);
         strcpy(PieLabFontBuff,   p->stext.PSFont);
         sprintf(PieLabSizeBuff,  "%lg",p->stext.PSSize);
         sprintf(PieLabAngleBuff, "%lg",180.0 * p->stext.x / PI);
         sprintf(PieLabDistBuff,  "%lg",p->stext.y);
      }
      else
      {
#endif
         strcpy(LabelTextBuff, p->stext.text);
         strcpy(LabelFontBuff, p->stext.PSFont);
         sprintf(LabelSizeBuff,"%lg",p->stext.PSSize);
         sprintf(LabelPosXBuff,"%lg",p->stext.x);
         sprintf(LabelPosYBuff,"%lg",p->stext.y);
#ifdef SEPPIEREQ
      }
#endif
   }

   /* Display the requester as window (don't lock parent) */   
#ifdef SEPPIEREQ
   if(graph.style == STYLE_PIE)
   {
      ReqWind = OpenWindow(NWPieLab);
      ActivateGadget(PieLabText,ReqWind,NULL);
   }
   else
   {
#endif
      ReqWind = OpenWindow(NWExtraLabels);
      ActivateGadget(LabelText,ReqWind,NULL);
#ifdef SEPPIEREQ
   }
#endif

   do
   {
      Signals = Wait(1<<ReqWind->UserPort->mp_SigBit |
                     1<<MainWind->UserPort->mp_SigBit);

      /* Handle messages from requester window */
      while((message=(struct IntuiMessage *)GetMsg(ReqWind->UserPort))!=NULL)
      {
         gadget = NULL;
         if((MIClass = message->Class) == IDCMP_GADGETUP)
            gadget = (struct Gadget *)message->IAddress;
   
         ReplyMsg((struct Message *)message);
   
#ifdef SEPPIEREQ
         if(graph.style == STYLE_PIE)  /* Handle gadgets for Pie-chart     */
         {
            
            if(gadget == PieLabPrev ||
               gadget == PieLabNext ||
               gadget == PieLabKill)
            {
               /* A change current label gadget */
               p = ChangeExtraLabel(ReqWind,gadget);
               ActivateGadget(PieLabText,ReqWind,NULL);
               /* Replot */
               RefreshScreen();

            }
            else if(gadget == PieLabText)
            {
               /* The main text gadget - move to angle */
               ActivateGadget(PieLabAngle,ReqWind,NULL);
            }
            else if(gadget == PieLabAngle)
            {
               /* The angle gadget - move to dist */
               ActivateGadget(PieLabDist,ReqWind,NULL);
            }
            else if(gadget == PieLabDist)
            {
               /* The dist gadget - treat as NEXT */
               p = ChangeExtraLabel(ReqWind,PieLabNext);
               ActivateGadget(PieLabText,ReqWind,NULL);
            }
         }
         else                          /* Handle gadgets for non Pie-chart */
         {
#endif            
            if(gadget == LabelPrev ||
               gadget == LabelNext ||
               gadget == LabelKill)
            {
               /* A change current label gadget */
               p = ChangeExtraLabel(ReqWind,gadget);
               ActivateGadget(LabelText,ReqWind,NULL);
               /* Replot */
               RefreshScreen();

            }
            else if(gadget == LabelText)
            {
               /* The main text gadget - move to X */
               ActivateGadget(LabelPosX,ReqWind,NULL);
            }
            else if(gadget == LabelPosX)
            {
               /* The X-position gadget - move to Y */
               ActivateGadget(LabelPosY,ReqWind,NULL);
            }
            else if(gadget == LabelPosY)
            {
               /* The Y-position gadget - treat as NEXT */
               p = ChangeExtraLabel(ReqWind,LabelNext);
               ActivateGadget(LabelText,ReqWind,NULL);
            }
#ifdef SEPPIEREQ
         }
#endif
      }

      /* Handle messages from main window */
      while((message=(struct IntuiMessage *)GetMsg(MainWind->UserPort))!=NULL)
      {
         MIClass = message->Class;
         MICode  = message->Code;
         screenx = message->MouseX;
         screeny = message->MouseY;
         ReplyMsg((struct Message *)message);
         
         gadget = NULL;
   
         if(data && (MIClass == IDCMP_MOUSEBUTTONS) && (MICode == SELECTDOWN))
         {
            ActivateWindow(ReqWind);
#ifdef SEPPIEREQ
            if(graph.style == STYLE_PIE)
            {
               ScreenToPieData(screenx,screeny,&x,&y);
               sprintf(PieLabAngleBuff,   "%lg", 180.0 * x / PI);
               sprintf(PieLabDistBuff,    "%lg",y);
               RefreshGList(PieLabAngle,   ReqWind, NULL, 2);
               ActivateGadget(PieLabText, ReqWind, NULL);
            }
            else
            {
#endif
               ScreenToData(screenx,screeny,&x,&y);
               sprintf(LabelPosXBuff,    "%lg",x);
               sprintf(LabelPosYBuff,    "%lg",y);
               RefreshGList(LabelPosX,   ReqWind, NULL, 2);
               ActivateGadget(LabelText, ReqWind, NULL);
#ifdef SEPPIEREQ
            }
#endif
         }
      }
   } while (gadget==NULL || !(gadget->Activation & GACT_ENDGADGET));

   CloseWindow(ReqWind);

   if(gadget == ExtraLabelsKillAll || gadget == PieLabelsKillAll)
   {
      /* Free allocated memory for labels */
      FREELIST(labels,TEXTLIST);
      labels = NULL;

      /* Initialise `extra' label list */
      AllocExtraLabel();
   }
   else
   {
      /* Fill the current label from the buffers */
      if(p)
      {
#ifdef SEPPIEREQ
         double angle;
         
         if(graph.style == STYLE_PIE)
         {
            strcpy(p->stext.text,   PieLabTextBuff);
            strcpy(p->stext.PSFont, PieLabFontBuff);
            sscanf(PieLabSizeBuff,  "%lf",&p->stext.PSSize);
            sscanf(PieLabAngleBuff, "%lf",&angle);
            sscanf(PieLabDistBuff,  "%lf",&p->stext.y);
            p->stext.x = PI * angle / 180.0;
         }
         else
         {
#endif
            strcpy(p->stext.text,LabelTextBuff);
            strcpy(p->stext.PSFont,LabelFontBuff);
            sscanf(LabelSizeBuff,"%lf",&p->stext.PSSize);
            sscanf(LabelPosXBuff,"%lf",&p->stext.x);
            sscanf(LabelPosYBuff,"%lf",&p->stext.y);
#ifdef SEPPIEREQ
         }
#endif
      }
   }
   
   return(0);
}

/***************************************************************************/
/*>SetFillType(APTR object)
   ------------------------
   Waits for user to click in a bar and sets the fill type for this bar.
   
   15.05.92 Framework
   09.06.92 Original
   16.06.92 Added set of f->angle
   17.06.92 Added piechart support
   18.06.92 Corrected use of scales in pie charts
   22.06.92 Corrected angle by pie start angle
   23.06.92 Added 2PI if angle < 0.0 (for Piecharts)
*/
int SetFillType(APTR object)
{
   int         fill = 0,
               xscreen,
               yscreen,
               xcent,
               ycent,
               dataset;
   FILLITEM    *f;
   double      x,
               y,
               angle;
   
   if(graph.style == STYLE_BAR || graph.style == STYLE_PIE)
   {
      if     (object == (APTR)MenuFT1) fill = 1;
      else if(object == (APTR)MenuFT2) fill = 2;
      else if(object == (APTR)MenuFT3) fill = 3;
      else if(object == (APTR)MenuFT4) fill = 4;
      else if(object == (APTR)MenuFT5) fill = 5;
      else if(object == (APTR)MenuFT6) fill = 6;

      /* Wait for the user to click */
      ClickPointer(MainWind);
      WaitForClick(MainWind, &xscreen, &yscreen);
         
      if(graph.style == STYLE_PIE)
      {
         /* Find the centre of the circle in screen coords */
         DataToScreen(0.0, 0.0, &xcent, &ycent);
         
         /* Find the angle (using screen coords) */
         angle = trueangle((double)(ycent-yscreen),(double)(xscreen-xcent));
         
         /* Modify by start angle */
         angle -= piestyle.start;
         if(angle < 0.0) angle += (2.0 * PI);

         if(filllist == NULL)
         {
            INIT(filllist, FILLITEM);
            f = filllist;
         }
         else
         {
            f = filllist;
            LAST(f);
            ALLOCNEXT(f, FILLITEM);
         }
         
         if(f==NULL)
         {
            ReqMessage(MainWind, "No Memory for fill.",0);
         }
         else
         {
            f->next     = NULL;
            f->pattern  = fill;
            f->x        = 0.0;
            f->y        = 0.0;
            f->dataset  = 0;
            f->angle    = angle;
         }
      }
      else
      {
         ScreenToData(xscreen,yscreen,&x,&y);

         /* Find the first dataset which contains this coord in a box */
         dataset = FindBarDataset(x, y);
   
         if(dataset)
         {
            if(filllist == NULL)
            {
               INIT(filllist, FILLITEM);
               f = filllist;
            }
            else
            {
               f = filllist;
               LAST(f);
               ALLOCNEXT(f, FILLITEM);
            }
         
            if(f==NULL)
            {
               ReqMessage(MainWind, "No Memory for fill.",0);
            }
            else
            {
               f->next     = NULL;
               f->pattern  = fill;
               f->x        = x;
               f->y        = y;
               f->dataset  = dataset;
               f->angle    = 0.0;
            }
         }
      }
   }
   return(0);
}

/***************************************************************************/
/*>SetPalette(APTR object)
   -----------------------
   Puts up the palette window. 
   02.07.92 Original
*/
int SetPalette(APTR object)
{
   DoColorWindow(MyScreen,10,10,1,TRUE);

/*
//   {
//      FreeAllGadgets();
//      BuildAllWindows();
//   }
*/

   return(0);
}

/***************************************************************************/
/*>RemoveFills(APTR object)
   ------------------------
   Puts up the palette window. 
   07.08.92 Original
*/
int RemoveFills(APTR object)
{
   if(filllist != NULL)
   {
      FREELIST(filllist, FILLITEM);
      filllist = NULL;
   }

   return(0);
}

