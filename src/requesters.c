/***************************************************************************

   Program:    AMPlot2
   File:       requesters.c
   
   Version:    V0.1
   Date:       04.03.92
   Function:   Build window style requesters
   
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

****************************************************************************

   Usage:
   ======

****************************************************************************

   Revision History:
   =================

***************************************************************************/
#include "includes.h"

/**************************************************************************/
/*>struct NewWindow *BuildAboutReq(struct Screen *scrn)
   ----------------------------------------------------
   Builds the About requester
   04.03.92 Original
   21.07.92 Added DEMO_VERSION conditional code
*/
struct NewWindow *BuildAboutReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,200,150," About",0,0);
   
   AboutOK     = BuildBoolGadget(nw,"OK",    0,-1,10,1,1,tlpen,brpen,0,0);
   MakeEndGadget(AboutOK);
   
   BuildText(nw,"AMPlot",              1,100, 30,2);
   BuildText(nw,"© SciTech Software",  1,100, 50,2);
   BuildText(nw,"1992",                1,100, 65,2);
#ifdef DEMO_VERSION
   BuildText(nw,"V2.01 Demo Version",  1,100, 80,2);
#else
   BuildText(nw,"V2.01",               1,100, 80,2);
#endif

   return(nw);   
}

/**************************************************************************/
/*>struct NewWindow *BuildPenReq(struct Screen *scrn)
   --------------------------------------------------
   Builds the Change pen requester
   04.03.92 Original
   09.04.92 Added return for PenNumGadget
*/
struct NewWindow *BuildPenReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,300,120," Change Pen",0,0);
   
   PenOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   PenCancel = BuildBoolGadget(nw,"Cancel",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(PenOK);
   MakeEndGadget(PenCancel);

   BuildText(nw,"Having entered the pen number,",1,150,30,2);
   BuildText(nw,"click `OK' and click on a",     1,150,45,2);
   BuildText(nw,"datapoint.",                    1,150,60,2);

   PenNumGadget =
   BuildStringGadget(nw,PenNumberBuff,3, 130, 80,3,tlpen,brpen,2,0);
   MakeEndGadget(PenNumGadget);
   
   return(nw);
}   

/**************************************************************************/
/*>struct NewWindow *BuildLineStyleReq(struct Screen *scrn)
   --------------------------------------------------------
   Builds the Line Style requester
   04.03.92 Original
   08.04.92 Added DP size
*/
struct NewWindow *BuildLineStyleReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,365,215," Line Style",0,0);
   
   LineStyleOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   LineStyleCancel = BuildBoolGadget(nw,"Cancel",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(LineStyleOK);
   MakeEndGadget(LineStyleCancel);
   
   BuildText(nw,"Pen",1,10, 18,0);
   BuildText(nw," 1 ",1,10, 38,0);
   BuildText(nw," 2 ",1,10, 58,0);
   BuildText(nw," 3 ",1,10, 78,0);
   BuildText(nw," 4 ",1,10, 98,0);
   BuildText(nw," 5 ",1,10,118,0);
   BuildText(nw," 6 ",1,10,138,0);

   Pen1DP[0]  = BuildRadioButton(nw,NULL, 30, 40, 1,tlpen,brpen,1,0,Pen1DP);
   Pen1DP[1]  = BuildRadioButton(nw,NULL, 50, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[2]  = BuildRadioButton(nw,NULL, 70, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[3]  = BuildRadioButton(nw,NULL, 90, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[4]  = BuildRadioButton(nw,NULL,110, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[5]  = BuildRadioButton(nw,NULL,130, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[6]  = BuildRadioButton(nw,NULL,150, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[7]  = BuildRadioButton(nw,NULL,170, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[8]  = BuildRadioButton(nw,NULL,190, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[9]  = BuildRadioButton(nw,NULL,210, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[10] = BuildRadioButton(nw,NULL,230, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[11] = BuildRadioButton(nw,NULL,250, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[12] = BuildRadioButton(nw,NULL,270, 40, 1,tlpen,brpen,0,0,Pen1DP);
   Pen1DP[13] = NULL;

   Pen2DP[0]  = BuildRadioButton(nw,NULL, 30, 60, 1,tlpen,brpen,1,0,Pen2DP);
   Pen2DP[1]  = BuildRadioButton(nw,NULL, 50, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[2]  = BuildRadioButton(nw,NULL, 70, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[3]  = BuildRadioButton(nw,NULL, 90, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[4]  = BuildRadioButton(nw,NULL,110, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[5]  = BuildRadioButton(nw,NULL,130, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[6]  = BuildRadioButton(nw,NULL,150, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[7]  = BuildRadioButton(nw,NULL,170, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[8]  = BuildRadioButton(nw,NULL,190, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[9]  = BuildRadioButton(nw,NULL,210, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[10] = BuildRadioButton(nw,NULL,230, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[11] = BuildRadioButton(nw,NULL,250, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[12] = BuildRadioButton(nw,NULL,270, 60, 1,tlpen,brpen,0,0,Pen2DP);
   Pen2DP[13] = NULL;

   Pen3DP[0]  = BuildRadioButton(nw,NULL, 30, 80, 1,tlpen,brpen,1,0,Pen3DP);
   Pen3DP[1]  = BuildRadioButton(nw,NULL, 50, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[2]  = BuildRadioButton(nw,NULL, 70, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[3]  = BuildRadioButton(nw,NULL, 90, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[4]  = BuildRadioButton(nw,NULL,110, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[5]  = BuildRadioButton(nw,NULL,130, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[6]  = BuildRadioButton(nw,NULL,150, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[7]  = BuildRadioButton(nw,NULL,170, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[8]  = BuildRadioButton(nw,NULL,190, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[9]  = BuildRadioButton(nw,NULL,210, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[10] = BuildRadioButton(nw,NULL,230, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[11] = BuildRadioButton(nw,NULL,250, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[12] = BuildRadioButton(nw,NULL,270, 80, 1,tlpen,brpen,0,0,Pen3DP);
   Pen3DP[13] = NULL;

   Pen4DP[0]  = BuildRadioButton(nw,NULL, 30,100, 1,tlpen,brpen,1,0,Pen4DP);
   Pen4DP[1]  = BuildRadioButton(nw,NULL, 50,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[2]  = BuildRadioButton(nw,NULL, 70,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[3]  = BuildRadioButton(nw,NULL, 90,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[4]  = BuildRadioButton(nw,NULL,110,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[5]  = BuildRadioButton(nw,NULL,130,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[6]  = BuildRadioButton(nw,NULL,150,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[7]  = BuildRadioButton(nw,NULL,170,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[8]  = BuildRadioButton(nw,NULL,190,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[9]  = BuildRadioButton(nw,NULL,210,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[10] = BuildRadioButton(nw,NULL,230,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[11] = BuildRadioButton(nw,NULL,250,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[12] = BuildRadioButton(nw,NULL,270,100, 1,tlpen,brpen,0,0,Pen4DP);
   Pen4DP[13] = NULL;

   Pen5DP[0]  = BuildRadioButton(nw,NULL, 30,120, 1,tlpen,brpen,1,0,Pen5DP);
   Pen5DP[1]  = BuildRadioButton(nw,NULL, 50,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[2]  = BuildRadioButton(nw,NULL, 70,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[3]  = BuildRadioButton(nw,NULL, 90,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[4]  = BuildRadioButton(nw,NULL,110,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[5]  = BuildRadioButton(nw,NULL,130,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[6]  = BuildRadioButton(nw,NULL,150,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[7]  = BuildRadioButton(nw,NULL,170,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[8]  = BuildRadioButton(nw,NULL,190,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[9]  = BuildRadioButton(nw,NULL,210,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[10] = BuildRadioButton(nw,NULL,230,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[11] = BuildRadioButton(nw,NULL,250,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[12] = BuildRadioButton(nw,NULL,270,120, 1,tlpen,brpen,0,0,Pen5DP);
   Pen5DP[13] = NULL;

   Pen6DP[0]  = BuildRadioButton(nw,NULL, 30,140, 1,tlpen,brpen,1,0,Pen6DP);
   Pen6DP[1]  = BuildRadioButton(nw,NULL, 50,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[2]  = BuildRadioButton(nw,NULL, 70,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[3]  = BuildRadioButton(nw,NULL, 90,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[4]  = BuildRadioButton(nw,NULL,110,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[5]  = BuildRadioButton(nw,NULL,130,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[6]  = BuildRadioButton(nw,NULL,150,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[7]  = BuildRadioButton(nw,NULL,170,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[8]  = BuildRadioButton(nw,NULL,190,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[9]  = BuildRadioButton(nw,NULL,210,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[10] = BuildRadioButton(nw,NULL,230,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[11] = BuildRadioButton(nw,NULL,250,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[12] = BuildRadioButton(nw,NULL,270,140, 1,tlpen,brpen,0,0,Pen6DP);
   Pen6DP[13] = NULL;

   Pen1Link = BuildBoolGadget(nw,"Link",290, 38,0,1,1,tlpen,brpen,1,0);
   Pen2Link = BuildBoolGadget(nw,"Link",290, 58,0,1,1,tlpen,brpen,1,0);
   Pen3Link = BuildBoolGadget(nw,"Link",290, 78,0,1,1,tlpen,brpen,1,0);
   Pen4Link = BuildBoolGadget(nw,"Link",290, 98,0,1,1,tlpen,brpen,1,0);
   Pen5Link = BuildBoolGadget(nw,"Link",290,118,0,1,1,tlpen,brpen,1,0);
   Pen6Link = BuildBoolGadget(nw,"Link",290,138,0,1,1,tlpen,brpen,1,0);

   BuildStringGadget(nw,LineStyleEBarBuff,5,230,156,5,tlpen,brpen,2,0);
   BuildText(nw,"Error bar cross width",1,230,156,1);
   BuildText(nw,"pt",                   1,287,156,0);

   BuildStringGadget(nw,LineStyleDPSizeBuff,5,230,171,5,tlpen,brpen,2,0);
   BuildText(nw,"Data point mark size", 1,230,171,1);
   BuildText(nw,"pt",                   1,287,171,0);

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildBarLabelsReq(struct Screen *scrn)
   --------------------------------------------------------
   Builds the Bar Labels requester
   04.03.92 Original
   02.04.92 Added return values BarLabText and BarLabNum
   10.08.92 BuildFrame(), Prev & Next now have pens specified explicitly
*/
struct NewWindow *BuildBarLabelsReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,365,120," Bar/Pie Labels",0,0);
   
   BarLabOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   BarLabRemove = BuildBoolGadget(nw,"Remove",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(BarLabOK);
   MakeEndGadget(BarLabRemove);

   BarLabText =
   BuildStringGadget(nw,BarLabTextBuff,MAXBUFF, 60, 20,35,tlpen,brpen,2,0);
   BuildText(nw,"Text", 1, 60,20,1);
   
   BarLabPrev = BuildBoolGadget(nw,"Prev", 129,72,0,1,1,tlpen,brpen,0,0);
   BarLabNext = BuildBoolGadget(nw,"Next", 182,72,0,1,1,tlpen,brpen,0,0);
   BarLabNum = 
   BuildStringGadget(nw,BarLabNumBuff,3,   198,56,3,tlpen,brpen,2,1);
   BuildText(nw,"Bar No.", 1, 188,56,1);
   
   BuildFrame(nw,122,49,120,44,tlpen,brpen,1);

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildFourierReq(struct Screen *scrn)
   ------------------------------------------------------
   Builds the Fourier requester
   04.03.92 Original
*/
struct NewWindow *BuildFourierReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,240,100," Fourier Smoothing",0,0);
   
   FourierOK     = BuildBoolGadget(nw,"OK",    -1,-1, 6,1,1,tlpen,brpen,0,0);
   FourierCancel = BuildBoolGadget(nw,"Cancel",-2,-1, 6,1,1,tlpen,brpen,0,0);
   FourierOff    = BuildBoolGadget(nw,"Off",    0,50,10,1,1,tlpen,brpen,0,0);
   MakeEndGadget(FourierOK);
   MakeEndGadget(FourierCancel);
   MakeEndGadget(FourierOff);

   BuildStringGadget(nw,FourierBuff,NUMBUFF, 150, 20,8,tlpen,brpen,2,0);
   BuildText(nw,"Smoothing Factor",1,150,20,1);

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildKeyReq(struct Screen *scrn)
   --------------------------------------------------
   Builds the Key requester
   04.03.92 Original
   01.04.92 Got rid of CANCEL and changed it to REMOVE
   08.04.92 Added gadget return values
   10.08.92 BuildFrame(), Prev & Next now have pens specified explicitly
*/
struct NewWindow *BuildKeyReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,365,164," Key",0,0);
   
   KeyOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   KeyRemove = BuildBoolGadget(nw,"Remove",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(KeyOK);
   MakeEndGadget(KeyRemove);

   KeyText =
   BuildStringGadget(nw,KeyTextBuff,MAXBUFF, 60, 20,35,tlpen,brpen,2,0);
   BuildStringGadget(nw,KeyFontBuff,MAXBUFF, 60, 40,20,tlpen,brpen,2,0);
   BuildStringGadget(nw,KeySizeBuff,NUMBUFF,276, 40, 8,tlpen,brpen,2,0);
   BuildText(nw,"Text", 1, 60,20,1);
   BuildText(nw,"Font", 1, 60,40,1);
   BuildText(nw,"Size", 1,276,40,1);
   
   KeyXPos =
   BuildStringGadget(nw,KeyPosXBuff,NUMBUFF,156, 65, 8,tlpen,brpen,2,0);
   KeyYPos =
   BuildStringGadget(nw,KeyPosYBuff,NUMBUFF,276, 65, 8,tlpen,brpen,2,0);
   BuildText(nw,"X", 1,156,65,1);
   BuildText(nw,"Y", 1,276,65,1);
   BuildText(nw,"Position of", 1, 135, 60, 1);
   BuildText(nw,"First Key",   1, 135, 70, 1);

   KeyPrev = BuildBoolGadget(nw,"Prev", 129,112,0,1,1,tlpen,brpen,0,0);
   KeyNext = BuildBoolGadget(nw,"Next", 182,112,0,1,1,tlpen,brpen,0,0);
   KeyDataset =
   BuildStringGadget(nw,KeyDatasetBuff,4, 198, 96, 3,tlpen,brpen,2,1);
   BuildText(nw,"Dataset", 1, 188,96,1);
   
   BuildFrame(nw,122,89,120,44,tlpen,brpen,1);

/*   KeyRemove = BuildBoolGadget(nw,"Remove",0,-1,10,1,1,tlpen,brpen,0,0);
*/

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildRegressionReq(struct Screen *scrn)
   ---------------------------------------------------------
   Builds the Regression requester
   04.03.92 Original
*/
struct NewWindow *BuildRegressionReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,200,140," Linear Regression",0,0);
   
   RegressionOK     = BuildBoolGadget(nw,"OK",    -1,-1, 6,1,1,tlpen,brpen,0,0);
   RegressionCancel = BuildBoolGadget(nw,"Cancel",-2,-1, 6,1,1,tlpen,brpen,0,0);
   RegressionOff    = BuildBoolGadget(nw,"Off",    0,90,10,1,1,tlpen,brpen,0,0);
   MakeEndGadget(RegressionOK);
   MakeEndGadget(RegressionCancel);
   MakeEndGadget(RegressionOff);

   BuildStringGadget(nw,RegressDegreeBuff,NUMBUFF,110, 30, 3,tlpen,brpen,2,0);
   BuildText(nw,"Degree",   1,110,30,1);
   BuildStringGadget(nw,RegressTolBuff,   NUMBUFF,110, 50, 8,tlpen,brpen,2,0);
   BuildText(nw,"Tolerence",1,110,50,1);
   RegressionWrite = 
   BuildCheckBox(nw,"Write Data",110,70,1,tlpen,brpen,0,1);
   
   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildExtraLabelsReq(struct Screen *scrn)
   ----------------------------------------------------------
   Builds the Extra Labels requester
   04.03.92 Original
   03.04.92 Added LabelText return value.
            Changed gadget order for efficient refresh.
            Changed CANCEL to KillAll.
   10.08.92 BuildFrame(), Prev, Next & Kill now have pens specified explicitly
*/
struct NewWindow *BuildExtraLabelsReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,365,160," Extra Labels",0,0);
   
   ExtraLabelsOK      = BuildBoolGadget(nw,"OK",      -1,-1,8,1,1,tlpen,brpen,0,0);
   ExtraLabelsKillAll = BuildBoolGadget(nw,"Kill All",-2,-1,8,1,1,tlpen,brpen,0,0);
   MakeEndGadget(ExtraLabelsOK);
   MakeEndGadget(ExtraLabelsKillAll);

   LabelPrev = BuildBoolGadget(nw,"Prev", 129,100,0,1,1,tlpen,brpen,0,0);
   LabelNext = BuildBoolGadget(nw,"Next", 182,100,0,1,1,tlpen,brpen,0,0);
   LabelKill = BuildBoolGadget(nw,"Kill", 157,116,0,1,1,tlpen,brpen,0,0);
   BuildFrame(nw,122,93,120,44,tlpen,brpen,1);

   LabelText =
   BuildStringGadget(nw,LabelTextBuff,MAXBUFF, 60, 20,35,tlpen,brpen,2,0);
   BuildStringGadget(nw,LabelFontBuff,MAXBUFF, 60, 40,20,tlpen,brpen,2,0);
   BuildStringGadget(nw,LabelSizeBuff,NUMBUFF,276, 40, 8,tlpen,brpen,2,0);
   BuildText(nw,"Text", 1, 60,20,1);
   BuildText(nw,"Font", 1, 60,40,1);
   BuildText(nw,"Size", 1,276,40,1);
   
   LabelPosX =
   BuildStringGadget(nw,LabelPosXBuff,NUMBUFF, 60, 60, 8,tlpen,brpen,2,0);
   LabelPosY =
   BuildStringGadget(nw,LabelPosYBuff,NUMBUFF,172, 60, 8,tlpen,brpen,2,0);
   BuildText(nw,"X", 1, 60,60,1);
   BuildText(nw,"Y", 1,172,60,1);

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildAxisLabelsReq(struct Screen *scrn)
   ---------------------------------------------------------
   Builds the Axis Labels requester
   04.03.92 Original
*/
struct NewWindow *BuildAxisLabelsReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,365,150," Axis Labels",0,0);
   
   AxisLabelsOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   AxisLabelsCancel = BuildBoolGadget(nw,"Cancel",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(AxisLabelsOK);
   MakeEndGadget(AxisLabelsCancel);
   
   BuildStringGadget(nw,AxLabXFontBuff,MAXBUFF, 60, 30,20,tlpen,brpen,2,0);
   BuildStringGadget(nw,AxLabXSizeBuff,NUMBUFF,276, 30, 8,tlpen,brpen,2,0);
   BuildText(nw,"XFont",     1, 60,30,1);
   BuildText(nw,"Size",      1,276,30,1);
   
   BuildStringGadget(nw,AxLabYFontBuff,MAXBUFF, 60, 50,20,tlpen,brpen,2,0);
   BuildStringGadget(nw,AxLabYSizeBuff,NUMBUFF,276, 50, 8,tlpen,brpen,2,0);
   BuildText(nw,"YFont",     1, 60,50,1);
   BuildText(nw,"Size",      1,276,50,1);

   AxLabXPrec =
   BuildStringGadget(nw,AxLabXPrecBuff,NUMBUFF,276, 70, 8,tlpen,brpen,2,1);
   AxLabYPrec =
   BuildStringGadget(nw,AxLabYPrecBuff,NUMBUFF,276, 90, 8,tlpen,brpen,2,1);
   BuildText(nw,"X Precision",               1,276, 70,1);
   BuildText(nw,"Y Precision",               1,276, 90,1);
   BuildText(nw,"(Number of digits)",        1,276,105,2);
   
   AxLabNoX = 
   BuildCheckBox(nw,"No X-labels",    20, 70,1,tlpen,brpen,0,0);
   AxLabNoY = 
   BuildCheckBox(nw,"No Y-labels",    20, 83,1,tlpen,brpen,0,0);
   AxLabCentreX = 
   BuildCheckBox(nw,"Centre X-labels",20, 96,1,tlpen,brpen,0,0);
   
   AxLabBarLabels = BuildBoolGadget(nw,"Bar/Pie labels", 0,-1,15,1,1,tlpen,brpen,0,0);
   
   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildTitleReq(struct Screen *scrn)
   ----------------------------------------------------
   Builds the Title requester
   04.03.92 Original
   01.04.92 PosX gadget return value kept.
   08.04.92 Keep return vales for text gadgets.
*/
struct NewWindow *BuildTitleReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,365,120," Graph Title",0,0);
   
   TitleOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   TitleCancel = BuildBoolGadget(nw,"Cancel",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(TitleOK);
   MakeEndGadget(TitleCancel);
   
   TitleText =
   BuildStringGadget(nw,TitleTextBuff,MAXBUFF, 60, 20,35,tlpen,brpen,2,0);
   BuildStringGadget(nw,TitleFontBuff,MAXBUFF, 60, 40,20,tlpen,brpen,2,0);
   BuildStringGadget(nw,TitleSizeBuff,NUMBUFF,276, 40, 8,tlpen,brpen,2,0);
   BuildText(nw,"Text", 1, 60,20,1);
   BuildText(nw,"Font", 1, 60,40,1);
   BuildText(nw,"Size", 1,276,40,1);
   
   TitlePosX =
   BuildStringGadget(nw,TitlePosXBuff,NUMBUFF, 60, 60, 8,tlpen,brpen,2,0);
   TitlePosY =
   BuildStringGadget(nw,TitlePosYBuff,NUMBUFF,172, 60, 8,tlpen,brpen,2,0);
   BuildText(nw,"X", 1, 60,60,1);
   BuildText(nw,"Y", 1,172,60,1);

   TitleCentre  = BuildBoolGadget(nw,"Centre",284, 60, 0,1,1,tlpen,brpen,0,0);

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildAxisTitlesReq(struct Screen *scrn)
   ---------------------------------------------------------
   Builds the Axis Titles requester
   04.03.92 Original
*/
struct NewWindow *BuildAxisTitlesReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,370,140," Axis Titles",0,0);
   
   AxisTitleOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   AxisTitleCancel = BuildBoolGadget(nw,"Cancel",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(AxisTitleOK);
   MakeEndGadget(AxisTitleCancel);
   
   XTitleGadget =
   BuildStringGadget(nw,XTitleTextBuff,MAXBUFF, 60, 20,35,tlpen,brpen,2,0);
   BuildStringGadget(nw,XTitleFontBuff,MAXBUFF, 60, 40,20,tlpen,brpen,2,0);
   BuildStringGadget(nw,XTitleSizeBuff,NUMBUFF,276, 40, 8,tlpen,brpen,2,0);
   BuildText(nw,"XTitle",1, 60,20,1);
   BuildText(nw,"Font",  1, 60,40,1);
   BuildText(nw,"Size",  1,276,40,1);
   
   BuildStringGadget(nw,YTitleTextBuff,MAXBUFF, 60, 60,35,tlpen,brpen,2,0);
   BuildStringGadget(nw,YTitleFontBuff,MAXBUFF, 60, 80,20,tlpen,brpen,2,0);
   BuildStringGadget(nw,YTitleSizeBuff,NUMBUFF,276, 80, 8,tlpen,brpen,2,0);
   BuildText(nw,"YTitle",1, 60,60,1);
   BuildText(nw,"Font",  1, 60,80,1);
   BuildText(nw,"Size",  1,276,80,1);

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildPieStyleReq(struct Screen *scrn)
   -------------------------------------------------------
   Builds the Pie Style requester
   04.03.92 Original
   18.06.92 Changed eject to % rather than pt
*/
struct NewWindow *BuildPieStyleReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,280,110," Pie Style",0,0);
   
   PieStyleOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   PieStyleCancel = BuildBoolGadget(nw,"Cancel",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(PieStyleOK);
   MakeEndGadget(PieStyleCancel);

   BuildStringGadget(nw,PieStartBuff,NUMBUFF,100, 20,8,tlpen,brpen,2,0);
   BuildText(nw,"Start at",1,100,20,1);
   BuildText(nw,"degrees", 1,180,20,0);

   BuildStringGadget(nw,PieEjectBuff,NUMBUFF,100, 40,8,tlpen,brpen,2,0);
   BuildText(nw,"Eject by",1,100,40,1);
   BuildText(nw,"%",       1,180,40,0);

#ifdef THREE_D
   PieStyle3D = 
   BuildCheckBox(nw,"3D",100,60,1,tlpen,brpen,0,0);
#endif

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildSetAxesReq(struct Screen *scrn)
   ------------------------------------------------------
   Builds the Set Axes requester
   04.03.92 Original
   08.04.92 String gadget return values kept
   28.04.92 Corrected names of tick gadgets
*/
struct NewWindow *BuildSetAxesReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,380,180," Set Axes",0,0);

   SetAxesOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   SetAxesCancel = BuildBoolGadget(nw,"Cancel",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(SetAxesOK);
   MakeEndGadget(SetAxesCancel);

   BoundsXMin = 
   BuildStringGadget(nw,BoundsXMinBuff,NUMBUFF, 50, 40,8,tlpen,brpen,2,0);
   BoundsXMax = 
   BuildStringGadget(nw,BoundsXMaxBuff,NUMBUFF,200, 40,8,tlpen,brpen,2,0);
   BoundsYMin = 
   BuildStringGadget(nw,BoundsYMinBuff,NUMBUFF, 50, 60,8,tlpen,brpen,2,0);
   BoundsYMax = 
   BuildStringGadget(nw,BoundsYMaxBuff,NUMBUFF,200, 60,8,tlpen,brpen,2,0);

   BuildText(nw,"XMin",1, 50,40, 1);
   BuildText(nw,"XMax",1,200,40, 1);
   BuildText(nw,"YMin",1, 50,60, 1);
   BuildText(nw,"YMax",1,200,60, 1);
   
   BoundsAuto = BuildBoolGadget(nw,"Auto",300, 40,0,1,1,tlpen,brpen,0,0);
   BoundsClip = 
   BuildCheckBox(nw,"Clip",300,60,1,tlpen,brpen,0,0);

   TicksX =
   BuildStringGadget(nw,TickXBuff,     NUMBUFF, 50,110,8,tlpen,brpen,2,0);
   TicksXSub =
   BuildStringGadget(nw,SubXBuff,      NUMBUFF,200,110,8,tlpen,brpen,2,0);
   TicksY =
   BuildStringGadget(nw,TickYBuff,     NUMBUFF, 50,130,8,tlpen,brpen,2,0);
   TicksYSub =
   BuildStringGadget(nw,SubYBuff,      NUMBUFF,200,130,8,tlpen,brpen,2,0);
   
   BuildText(nw,"X",    1, 50,110, 1);
   BuildText(nw,"XSub", 1,200,110, 1);
   BuildText(nw,"Y",    1, 50,130, 1);
   BuildText(nw,"YSub", 1,200,130, 1);

   TicksAuto  = BuildBoolGadget(nw,"Auto",300,110,0,1,1,tlpen,brpen,0,0);
   TicksSub   = 
   BuildCheckBox(nw,"Sub",300,130,1,tlpen,brpen,0,0);

   BuildText(nw,"Bounds", 1,190, 20, 2);
   BuildText(nw,"Ticks",  1,190, 90, 2);

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildPaperReq(struct Screen *scrn)
   ----------------------------------------------------
   Builds the Paper requester
   04.03.92 Original
*/
struct NewWindow *BuildPaperReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,300,100," Set Paper Dimensions",0,0);
   
   PaperOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   PaperCancel = BuildBoolGadget(nw,"Cancel",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(PaperOK);
   MakeEndGadget(PaperCancel);

   PaperEPSF   = BuildCheckBox(nw,"EPSF",110,60,1,tlpen,brpen,0,0);

   BuildStringGadget(nw,PaperXSizeBuff,NUMBUFF,110,20,8,tlpen,brpen,2,0);
   BuildStringGadget(nw,PaperYSizeBuff,NUMBUFF,210,20,8,tlpen,brpen,2,0);
   BuildStringGadget(nw,PaperXOffBuff, NUMBUFF,110,40,8,tlpen,brpen,2,0);
   BuildStringGadget(nw,PaperYOffBuff, NUMBUFF,210,40,8,tlpen,brpen,2,0);

   BuildText(nw,"Size    X:",1,110,20,1);
   BuildText(nw,"Offset  X:",1,110,40,1);
   BuildText(nw,"Y:",        1,210,20,1);
   BuildText(nw,"Y:",        1,210,40,1);
   
   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildSetPensReq(struct Screen *scrn)
   ------------------------------------------------------
   Builds the Set Pens requester
   04.03.92 Original
*/
struct NewWindow *BuildSetPensReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,200,165," Define Pens",0,0);

   SetPensOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   SetPensCancel = BuildBoolGadget(nw,"Cancel",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(SetPensOK);
   MakeEndGadget(SetPensCancel);

   BuildText(nw,"Colour",   1,35, 20,0);
   BuildText(nw,"Thickness",1,102,20,0);

   BuildStringGadget(nw,SetPen1ColBuff,        3, 40, 35, 3,tlpen,brpen,2,1);
   BuildStringGadget(nw,SetPen2ColBuff,        3, 40, 50, 3,tlpen,brpen,2,1);
   BuildStringGadget(nw,SetPen3ColBuff,        3, 40, 65, 3,tlpen,brpen,2,1);
   BuildStringGadget(nw,SetPen4ColBuff,        3, 40, 80, 3,tlpen,brpen,2,1);
   BuildStringGadget(nw,SetPen5ColBuff,        3, 40, 95, 3,tlpen,brpen,2,1);
   BuildStringGadget(nw,SetPen6ColBuff,        3, 40,110, 3,tlpen,brpen,2,1);

   BuildStringGadget(nw,SetPen1ThickBuff,NUMBUFF,100,35, 8,tlpen,brpen,2,0);
   BuildStringGadget(nw,SetPen2ThickBuff,NUMBUFF,100,50, 8,tlpen,brpen,2,0);
   BuildStringGadget(nw,SetPen3ThickBuff,NUMBUFF,100,65, 8,tlpen,brpen,2,0);
   BuildStringGadget(nw,SetPen4ThickBuff,NUMBUFF,100,80, 8,tlpen,brpen,2,0);
   BuildStringGadget(nw,SetPen5ThickBuff,NUMBUFF,100,95, 8,tlpen,brpen,2,0);
   BuildStringGadget(nw,SetPen6ThickBuff,NUMBUFF,100,110,8,tlpen,brpen,2,0);

   BuildText(nw,"1",1,30,35, 1);
   BuildText(nw,"2",1,30,50, 1);
   BuildText(nw,"3",1,30,65, 1);
   BuildText(nw,"4",1,30,80, 1);
   BuildText(nw,"5",1,30,95, 1);
   BuildText(nw,"6",1,30,110,1);
   
   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildBarStyleReq(struct Screen *scrn)
   -------------------------------------------------------
   Builds the Bar Style requester
   04.03.92 Original
*/
struct NewWindow *BuildBarStyleReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,280,175," Set Barchart Style",0,0);
   
   BarStyleOK     = BuildBoolGadget(nw,"OK",    -1,-1,6,1,1,tlpen,brpen,0,0);
   BarStyleCancel = BuildBoolGadget(nw,"Cancel",-2,-1,6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(BarStyleOK);
   MakeEndGadget(BarStyleCancel);

   BuildStringGadget(nw,BarShrinkBuff,NUMBUFF,190,20,3,tlpen,brpen,2,0);
   BuildText(nw,"Bar shrink factor:",1,190,20,1);
   BuildText(nw,"%",                 1,230,20,0);
   
   BarStyleShrink = 
   BuildCheckBox(nw,"Shrink first data set",40,40,1,tlpen,brpen,0,0);
   BarStyleFillAll = 
   BuildCheckBox(nw,"Fill all bars",40,55,1,tlpen,brpen,1,0);
#ifdef THREE_D
   BarStyle3D = 
   BuildCheckBox(nw,"3D",40,70,1,tlpen,brpen,0,0);
#endif
   
   BarStyles[0] = 
   BuildRadioButton(nw,"Overlayed",45,90, 1,tlpen,brpen,1,0,BarStyles);
   BarStyles[1] = 
   BuildRadioButton(nw,"Grouped",  45,103,1,tlpen,brpen,0,0,BarStyles);
   BarStyles[2] = 
   BuildRadioButton(nw,"Stacked",  45,116,1,tlpen,brpen,0,0,BarStyles);
   BarStyles[3] = 
   BuildRadioButton(nw,"Outline",  45,129,1,tlpen,brpen,0,0,BarStyles);
   BarStyles[4] = NULL;
   
   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildRexxFitReq(struct Screen *scrn)
   ------------------------------------------------------
   Builds the Rexx regression fitting requester
   01.05.92 Original
*/
struct NewWindow *BuildRexxFitReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,250,160," Rexx Linear Regression",0,0);
   
   RexxFitOK     = BuildBoolGadget(nw,"OK",    -1,-1, 6,1,1,tlpen,brpen,0,0);
   RexxFitCancel = BuildBoolGadget(nw,"Cancel",-2,-1, 6,1,1,tlpen,brpen,0,0);
   RexxFitOff    = BuildBoolGadget(nw,"Off",    0,115,10,1,1,tlpen,brpen,0,0);
   MakeEndGadget(RexxFitOK);
   MakeEndGadget(RexxFitCancel);
   MakeEndGadget(RexxFitOff);

   BuildStringGadget(nw,RexxFitDegreeBuff,NUMBUFF,180, 30, 3,tlpen,brpen,2,0);
   BuildText(nw,"Degree",   1,76,30,0);
   BuildStringGadget(nw,RexxFitTolBuff,   NUMBUFF,160, 50, 8,tlpen,brpen,2,0);
   BuildText(nw,"Tolerence",1,76,50,0);

   RexxFitWrite = 
   BuildCheckBox(nw,"Write Data",160,70,1,tlpen,brpen,0,1);
   
   RexxFitFName =
   BuildStringGadget(nw,fitting.rexxmacro,MAXBUFF, 72, 90,19,tlpen,brpen,0,0);

   RexxFitFiles = 
   BuildBoolGadget(nw,"Macro",-1,90,0,1,1,tlpen,brpen,0,0);

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildFillReq(struct Screen *scrn)
   ---------------------------------------------------
   Builds the fill control requester
   12.06.92 Original
*/
struct NewWindow *BuildFillReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,220,135," Fill Control",0,0);

   FillOK     = BuildBoolGadget(nw,"OK",    -1,-1, 6,1,1,tlpen,brpen,0,0);
   FillCancel = BuildBoolGadget(nw,"Cancel",-2,-1, 6,1,1,tlpen,brpen,0,0);
   MakeEndGadget(FillOK);
   MakeEndGadget(FillCancel);

   BuildText(nw,"Style", 1,95,30,1);
   FillStyle[0] = 
   BuildRadioButton(nw,"Shaded",  95,30,1,tlpen,brpen,1,0,FillStyle);
   FillStyle[1] = 
   BuildRadioButton(nw,"Hatched", 95,43,1,tlpen,brpen,0,0,FillStyle);
   FillStyle[2] = NULL;
   
   BuildStringGadget(nw,FillHatchThickBuff,NUMBUFF,150, 65, 3,tlpen,brpen,2,0);
   BuildText(nw,"Hatch Thickness", 1,150,65,1);
   BuildStringGadget(nw,FillHatchSpaceBuff,NUMBUFF,150, 80, 3,tlpen,brpen,2,0);
   BuildText(nw,"Hatch Spacing",   1,150,80,1);

   return(nw);
}

/**************************************************************************/
/*>struct NewWindow *BuildPieLabReq(struct Screen *scrn)
   -----------------------------------------------------
   Builds the equivalent of the Extra labels requester for Pie charts
   16.06.92 Original
   22.06.92 Put NWPieLab into #ifdef SEPPIEREQ's. Currently NOT defined.
   10.08.92 BuildFrame(), Prev, Next & Kill now have pens specified explicitly
*/
#ifdef SEPPIEREQ
struct NewWindow *BuildPieLabReq(struct Screen *scrn)
{
   struct NewWindow *nw;
   nw = BuildWindow(scrn,10,10,365,160," Extra Pie Labels",0,0);
   
   PieLabelsOK      = BuildBoolGadget(nw,"OK",      -1,-1,8,1,1,tlpen,brpen,0,0);
   PieLabelsKillAll = BuildBoolGadget(nw,"Kill All",-2,-1,8,1,1,tlpen,brpen,0,0);
   MakeEndGadget(PieLabelsOK);
   MakeEndGadget(PieLabelsKillAll);

   PieLabPrev = BuildBoolGadget(nw,"Prev", 129,100,0,1,1,tlpen,brpen,0,0);
   PieLabNext = BuildBoolGadget(nw,"Next", 182,100,0,1,1,tlpen,brpen,0,0);
   PieLabKill = BuildBoolGadget(nw,"Kill", 157,116,0,1,1,tlpen,brpen,0,0);
   BuildFrame(nw,122,93,120,44,tlpen,brpen,1);

   PieLabText =
   BuildStringGadget(nw,PieLabTextBuff,MAXBUFF, 60, 20,35,tlpen,brpen,2,0);
   BuildStringGadget(nw,PieLabFontBuff,MAXBUFF, 60, 40,20,tlpen,brpen,2,0);
   BuildStringGadget(nw,PieLabSizeBuff,NUMBUFF,276, 40, 8,tlpen,brpen,2,0);
   BuildText(nw,"Text", 1, 60,20,1);
   BuildText(nw,"Font", 1, 60,40,1);
   BuildText(nw,"Size", 1,276,40,1);
   
   PieLabAngle =
   BuildStringGadget(nw,PieLabAngleBuff,NUMBUFF, 60, 60, 8,tlpen,brpen,2,0);
   PieLabDist =
   BuildStringGadget(nw,PieLabDistBuff, NUMBUFF,276, 60, 8,tlpen,brpen,2,0);
   BuildText(nw,"Angle",    1, 60,60,1);
   BuildText(nw,"Distance", 1,276,60,1);

   return(nw);
}
#endif

/**************************************************************************/
/*>BuildAllWindows(struct Screen *scrn)
   ------------------------------------
   Calls all the requester building routines.
   04.03.92 Original
   01.05.92 Added NWRexxFit
   12.06.92 Added NWFill
   16.06.92 Added NWPieLab
   22.06.92 Put NWPieLab into #ifdef SEPPIEREQ's. Currently NOT defined.
   04.08.92 Removed SelectPens. Just assume the user has defined sensible
            (V2-like) pen colours
*/
BuildAllWindows(struct Screen *scrn)
{
/*
//   SelectPens(scrn, &tlpen, &brpen);
*/
   tlpen = 2;
   brpen = 1;
   
   NWPaper        = BuildPaperReq(scrn);
   NWSetPens      = BuildSetPensReq(scrn);
   NWLineStyle    = BuildLineStyleReq(scrn);
   NWBarStyle     = BuildBarStyleReq (scrn);
   NWPieStyle     = BuildPieStyleReq(scrn);
   NWSetAxes      = BuildSetAxesReq(scrn);
   NWAxisTitles   = BuildAxisTitlesReq(scrn);
   NWTitle        = BuildTitleReq(scrn);
   NWAxisLabels   = BuildAxisLabelsReq(scrn);
   NWExtraLabels  = BuildExtraLabelsReq(scrn);
   NWKey          = BuildKeyReq(scrn);
   NWRegression   = BuildRegressionReq(scrn);
   NWRexxFit      = BuildRexxFitReq(scrn);
   NWFourier      = BuildFourierReq(scrn);
   NWBarLabels    = BuildBarLabelsReq(scrn);
   NWAbout        = BuildAboutReq(scrn);
   NWPen          = BuildPenReq(scrn);
   NWFill         = BuildFillReq(scrn);
#ifdef SEPPIEREQ
   NWPieLab       = BuildPieLabReq(scrn);
#endif

   return(0);
}

/**************************************************************************/
/*>FreeAllGadgets(void)
   --------------------
   Frees all STSLib gadgets.
   04.03.92 Original
   08.04.92 Added if()'s
   01.05.92 Added NWRexxFit
   12.06.92 Added NWFill
   16.06.92 Added NWPieLab
   22.06.92 Put NWPieLab into #ifdef SEPPIEREQ's. Currently NOT defined.
   02.07.92 Now frees the NewWindow structures as well.
*/
FreeAllGadgets(void)
{
   if(NWPaper)       { FreeSTSGadgets(NWPaper->FirstGadget);       free(NWPaper); }
   if(NWSetPens)     { FreeSTSGadgets(NWSetPens->FirstGadget);     free(NWPaper); }
   if(NWLineStyle)   { FreeSTSGadgets(NWLineStyle->FirstGadget);   free(NWPaper); }
   if(NWBarStyle)    { FreeSTSGadgets(NWBarStyle->FirstGadget);    free(NWPaper); }
   if(NWPieStyle)    { FreeSTSGadgets(NWPieStyle->FirstGadget);    free(NWPaper); }
   if(NWSetAxes)     { FreeSTSGadgets(NWSetAxes->FirstGadget);     free(NWPaper); }
   if(NWAxisTitles)  { FreeSTSGadgets(NWAxisTitles->FirstGadget);  free(NWPaper); }
   if(NWTitle)       { FreeSTSGadgets(NWTitle->FirstGadget);       free(NWPaper); }
   if(NWAxisLabels)  { FreeSTSGadgets(NWAxisLabels->FirstGadget);  free(NWPaper); }
   if(NWExtraLabels) { FreeSTSGadgets(NWExtraLabels->FirstGadget); free(NWPaper); }
   if(NWKey)         { FreeSTSGadgets(NWKey->FirstGadget);         free(NWPaper); }
   if(NWRegression)  { FreeSTSGadgets(NWRegression->FirstGadget);  free(NWPaper); }
   if(NWRexxFit)     { FreeSTSGadgets(NWRexxFit->FirstGadget);     free(NWPaper); }
   if(NWFourier)     { FreeSTSGadgets(NWFourier->FirstGadget);     free(NWPaper); }
   if(NWBarLabels)   { FreeSTSGadgets(NWBarLabels->FirstGadget);   free(NWPaper); }
   if(NWAbout)       { FreeSTSGadgets(NWAbout->FirstGadget);       free(NWPaper); }
   if(NWPen)         { FreeSTSGadgets(NWPen->FirstGadget);         free(NWPaper); }
   if(NWFill)        { FreeSTSGadgets(NWFill->FirstGadget);        free(NWPaper); }
#ifdef SEPPIEREQ
   if(NWPieLab)      { FreeSTSGadgets(NWPieLab->FirstGadget);      free(NWPaper); }
#endif
   return(0);
}

