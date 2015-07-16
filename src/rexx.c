/****************************************************************************

   Program:    AMPlot2
   File:       Rexx.c
   
   Version:    V1.0
   Date:       29.04.92
   Function:   ARexx interface routines
   
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
#include "includes.h"
#include "menudefs.h"

/***************************************************************************/
/* Rexx externs
*/
extern char *Rexx_return;

/***************************************************************************/
/* Define macro to create a Rexx command
*/
#define  MAKECOM(x,a,b,c,d,e,f)           \
         {  if(a)                         \
            {                             \
               (x).Name    = (char *)malloc((strlen(a)+2) * sizeof(char)); \
               strcpy((x).Name,a);        \
            }                             \
            else                          \
            {                             \
               (x).Name    = NULL;        \
            }                             \
            (x).Func       = (void *)b;   \
            (x).MinArgs    = c;           \
            (x).MaxArgs    = d;           \
            (x).CaseFlag   = e;           \
            (x).Privilege  = f;           \
         }

/***************************************************************************/
/*>void RexxSave(struct RexxMsg *rexxmsg)
   --------------------------------------
   Save a file from ARexx
   06.07.92 Framework
   13.07.92 Original
*/
void RexxSave(struct RexxMsg *rexxmsg)
{
   FH *fh;
   
   RexxDebug();

   if(rexx.fromrexx)
   {
      WriteMessage("SAVE command not allowed in installed Rexx macro");
   }
   else
   {
      if((fh = (FH *)Open(RXIL_ARGV(1),MODE_NEWFILE))==NULL)
      {
         ReqMessage2(MainWind,"Unable to open save file",RXIL_ARGV(1),0);
         return;
      }

      SaveGraph(fh);
      
      Close((BPTR)fh);
      fh = NULL;
   }
}

/***************************************************************************/
/*>RexxParse(struct RexxMsg *rexxmsg)
   ----------------------------------
   Interface between the Rexx parser and the main parser. Sets the replot
   flag
   29.04.92 Original
   05.05.92 Added debug option
   01.07.92 Changed to use RexxDebug()
   02.07.92 Removed setting of rexx.fromrexx
*/
void RexxParse(struct RexxMsg *rexxmsg)
{
   char buffer[200];
   int  temp;
   
   BuildComLine(buffer);

   RexxDebug();
   
   rexx.replot   = TRUE;
   process_command(buffer, NULL, &temp, &temp);
}

/***************************************************************************/
/*>RexxParseNoR(struct RexxMsg *rexxmsg)
   -------------------------------------
   Interface between the Rexx parser and the main parser. Does not set the
   replot flag.
   07.07.92 Original based on RexxParse()
*/
void RexxParseNoR(struct RexxMsg *rexxmsg)
{
   char buffer[200];
   int  temp;
   
   BuildComLine(buffer);

   RexxDebug();
   
   process_command(buffer, NULL, &temp, &temp);
}

/***************************************************************************/
/*>void RexxDraw(struct RexxMsg *rexxmsg)
   --------------------------------------
   Command received from Rexx to draw a line segment
   29.04.92 Original
   30.04.92 Modified to moveto only, if 5 values given with same x & y
   05.05.92 Added debug option
   01.07.92 Changed to use RexxDebug()
   07.07.92 Added endline for paper plots
   20.07.92 Removed check on rexx.rexxplot. i.e. Always end line. Therefore
            added CurX and CurY.
*/
void RexxDraw(struct RexxMsg *rexxmsg)
{
   double         x,    y,
                  x1,   y1;
   static double  CurX  = 0.0,
                  CurY  = 0.0;

   RexxDebug();

   sscanf(RXIL_ARGV(1),"%lf",&x);
   sscanf(RXIL_ARGV(2),"%lf",&y);
   
   if(RXIL_ARGC >= 5)   /* 4 parameters given, do a move to */
   {
      AMMove(output.plottype,output.fh,x,y);
      sscanf(RXIL_ARGV(3),"%lf",&x1);
      sscanf(RXIL_ARGV(4),"%lf",&y1);
      if(x==x1 && y==y1)
      {
         CurX = x;
         CurY = y;
         return;
      }
      else
      {
         x = x1;
         y = y1;
      }
   }
   else                 /* Move to stored x and y           */
   {
      AMMove(output.plottype,output.fh,CurX,CurY);
   }
   
   AMDraw(output.plottype, output.fh, x, y);
   CurX = x;
   CurY = y;
   AMEndLine(output.plottype, output.fh);
}

/***************************************************************************/
/*>void RexxPen(struct RexxMsg *rexxmsg)
   -------------------------------------
   Command received from Rexx to change pen
   29.04.92 Original
   05.05.92 Added debug option
   01.07.92 Changed to use RexxDebug()
   07.07.92 Added RANGECHECK
*/
void RexxPen(struct RexxMsg *rexxmsg)
{
   int    pen;

   RexxDebug();

   sscanf(RXIL_ARGV(1),"%d",&pen);
   
   AMSetPen(output.plottype, output.fh, RANGECHECK(pen,1,MAXPEN));
}

/***************************************************************************/
/*>void RexxLStyle(struct RexxMsg *rexxmsg)
   ----------------------------------------
   Command received from Rexx to change line style
   07.07.92 Original
*/
void RexxLStyle(struct RexxMsg *rexxmsg)
{
   int    style;

   RexxDebug();

   sscanf(RXIL_ARGV(1),"%d",&style);
   style = RANGECHECK(style,1,MAXSTYLE);
   
   AMSetLineStyle(output.plottype, output.fh, style-1);
}

/***************************************************************************/
/*>void RexxQuit(struct RexxMsg *rexxmsg)
   --------------------------------------
   Command received from Rexx to quit
   29.04.92 Original
   05.05.92 Added debug option
   01.07.92 Changed to use RexxDebug()
*/
void RexxQuit(struct RexxMsg *rexxmsg)
{
   RexxDebug();

   rexx.quitprogram = TRUE;
}

/***************************************************************************/
/*>void RexxDataset(struct RexxMsg *rexxmsg)
   -----------------------------------------
   Command received from Rexx to change current dataset
   30.04.92 Original
   05.05.92 Added debug option
   12.06.92 Added second not found check
   01.07.92 Changed to use RexxDebug()
*/
void RexxDataset(struct RexxMsg *rexxmsg)
{
   DATASET  *d;
   int      n,
            required;
   
   RexxDebug();

   sscanf(RXIL_ARGV(1),"%d",&required);
   
   /* Count along to the required dataset */
   for(d=data, n=1; d && n<required; NEXT(d), n++);
   
   if(n<required) rexx.current = data;    /* Not found   */
   else           rexx.current = d;       /* Found       */
   
   if(rexx.current == NULL) rexx.current = data; /* Second not found check */
}

/***************************************************************************/
/*>void RexxEmpty(struct RexxMsg *rexxmsg)
   ---------------------------------------
   Command received from Rexx to create an empty graph
   30.04.92 Original
   05.05.92 Added debug option
   01.07.92 Changed to use RexxDebug()
*/
void RexxEmpty(struct RexxMsg *rexxmsg)
{
   double   XSize,
            YSize;

   RexxDebug();

   ClearPlots(TRUE);                      /* Clear any current graphs */

   sscanf(RXIL_ARGV(1),"%lf",&graph.xmin);
   sscanf(RXIL_ARGV(2),"%lf",&graph.xmax);
   sscanf(RXIL_ARGV(3),"%lf",&graph.ymin);
   sscanf(RXIL_ARGV(4),"%lf",&graph.ymax);

   CalcTicks(0, &graph.xtick, &graph.ytick,
                &graph.xsub,  &graph.ysub);

   /* Calculate scaling from these values (can't use SetScaling as this
      finds value for xmin, xmax, etc.)
   */
   if(graph.logx)
      XSize             = log10(graph.xmax)-log10(graph.xmin);
   else
      XSize             = graph.xmax-graph.xmin;
   
   if(graph.logy)
      YSize             = log10(graph.ymax)-log10(graph.ymin);
   else
      YSize             = graph.ymax-graph.ymin;

   graph.XScale   = display.width/XSize;
   graph.YScale   = display.height/YSize;
   graph.XPScale  = 1.0/XSize;
   graph.YPScale  = 1.0/YSize;

   rexx.empty  = TRUE;
   rexx.replot = TRUE;
}  

/***************************************************************************/
/*>void RexxGet(struct RexxMsg *rexxmsg)
   ------------------------------------
   Command received from Rexx to deposit a value in a Rexx variable
   Rexx syntax is:
       GETVAR TYPE VARNAME
   where TYPE     is the value we wish to obtain
   and   VARNAME  is the variable into which it will be deposited.

   30.04.92 Original
   05.05.92 Added debug option
   12.06.92 Corrected value returned by NDSET
   01.07.92 Changed to use RexxDebug()
*/
void RexxGet(struct RexxMsg *rexxmsg)
{
   DATASET  *d;
   char     RetBuff[80];
   int      n;

   RexxDebug();

   strcpy(RetBuff, "0");

   if(data)
   {
      if(!strcmp(RXIL_ARGV(1),"NDATA"))      /* Request for number of points  */
      {
         sprintf(RetBuff,"%d",rexx.current->ndata);
      }
      else if(!strcmp(RXIL_ARGV(1),"NDSET")) /* Request for number of datasets*/
      {
         for(d=data, n=0; d; NEXT(d), n++);
         if(!data) n = 0;
         sprintf(RetBuff,"%d",n);
      }
      else if(!strcmp(RXIL_ARGV(1),"XMIN"))  /* Request for graph xmin        */
      {
         sprintf(RetBuff,"%f",graph.xmin);
      }
      else if(!strcmp(RXIL_ARGV(1),"XMAX"))  /* Request for graph xmax        */
      {
         sprintf(RetBuff,"%f",graph.xmax);
      }
      else if(!strcmp(RXIL_ARGV(1),"YMIN"))  /* Request for graph ymin        */
      {
         sprintf(RetBuff,"%f",graph.ymin);
      }
      else if(!strcmp(RXIL_ARGV(1),"YMAX"))  /* Request for graph ymax        */
      {
         sprintf(RetBuff,"%f",graph.ymax);
      }
      else if(!strcmp(RXIL_ARGV(1),"X"))     /* Request for X data            */
      {
         if(RXIL_ARGC == 4)
         {
            sscanf(RXIL_ARGV(2),"%d",&n);
            sprintf(RetBuff,"%f",rexx.current->x[--n]);
         }
      }
      else if(!strcmp(RXIL_ARGV(1),"Y"))     /* Request for Y data            */
      {
         if(RXIL_ARGC == 4)
         {
            sscanf(RXIL_ARGV(2),"%d",&n);
            sprintf(RetBuff,"%f",rexx.current->y[--n]);
         }
      }
      else if(!strcmp(RXIL_ARGV(1),"EP"))    /* Request for error plus data   */
      {
         if(RXIL_ARGC == 4)
         {
            sscanf(RXIL_ARGV(2),"%d",&n);
            sprintf(RetBuff,"%f",rexx.current->ep[--n]);
         }
      }
      else if(!strcmp(RXIL_ARGV(1),"EM"))    /* Request for error minus data  */
      {
         if(RXIL_ARGC == 4)
         {
            sscanf(RXIL_ARGV(2),"%d",&n);
            sprintf(RetBuff,"%f",rexx.current->em[--n]);
         }
      }
      else                                   /* Unknown parameter             */
      {
         ;
      }
   }

   /* Actually set the Rexx variable */
   SetRexxVar(rexxmsg, RXIL_ARGV(RXIL_ARGC-1), RetBuff, strlen(RetBuff));
}

/***************************************************************************/
/*>RexxFunc(double x, double *p, int np)
   -------------------------------------
   Function to fit via ARexx
   30.04.92 Original
   01.05.92 Added check on positive rexx.count and on system() return
   02.07.92 Changed to specify directory for rxset
*/
void RexxFunc(double x, 
              double *p, 
              int    np)
{
   char     buffer[80];
   ULONG    Signals;
   
   if(fitting.rexxmacro[0])
   {

#ifdef REXX_FUNCTION                         /* Launch via a function      */
      char     ValBuff[16],
               NPBuff[16];
      sprintf(ValBuff, "%f",x);
      sprintf(NPBuff,  "%d",np);
      
      /* Launch the function macro */
      SetFuncParam(0, fitting.rexxmacro);    /* Function name              */
      SetFuncParam(1, ValBuff);              /* The value for expansion    */
      SetFuncParam(2, NPBuff);               /* Degree of polynomial       */
      LaunchFunc();
#else                                        /* Launch via a macro         */
      sprintf(buffer,"REXXC:rxset AMPLOTX %f",x);
      if(system(buffer))
      {
         if(ReqMessage2(MainWind,"You must have assigned REXXC:.",
                                 "Quit AMPlot?",1))
         {
            QuitProgram((APTR) 1);
         }
         else
         {
            int j;
            
            for(j=0; j<np; j++) p[j] = 0.0;
            return;
         }
      }

      sprintf(buffer,"REXXC:rxset AMPLOTNP %d",np);
      if(system(buffer))
      {
         if(ReqMessage2(MainWind,"You must have assigned REXXC:.",
                                 "Quit AMPlot?",1))
         {
            QuitProgram((APTR) 1);
         }
         else
         {
            int j;
            
            for(j=0; j<np; j++) p[j] = 0.0;
            return;
         }
      }

      LaunchCmd(fitting.rexxmacro);
#endif

      /* To start, we have no values back */
      rexx.count = 0;

      while(rexx.count < np)
      {
         /* Wait for the function to return its data */
         Signals = Wait(RXIL_WAITFLAG);
         
         /* Handle the return */
         if(FlagIsSet(Signals, RXIL_WAITFLAG))
         {
            RxilCheckPort();
   
            ProcessReturns();
            
            if(Rexx_return != NULL)
            {
               ReqMessage2(MainWind,"Function macro returned:",Rexx_return,0);
   
               /* Notify that we've used the return value */
               RexxReturnUsed();
            }

            /* For emergency exit via ARexx */
            if(rexx.quitprogram) QuitProgram((APTR) 1);

         }
         
         /* Set the value (checks rexx.count in case a spurious message is
            received first from Rexx
         */
         if(rexx.count > 0) p[rexx.count-1] = rexx.value;
      }
   }
   return;
}

/***************************************************************************/
/*>void RexxSetVal(struct RexxMsg *rexxmsg)
   ----------------------------------------
   Command received from Rexx to set a value
   30.04.92 Original
   05.05.92 Added debug option
   01.07.92 Changed to use RexxDebug()
*/
void RexxSetVal(struct RexxMsg *rexxmsg)
{
   RexxDebug();

   rexx.count++;
   sscanf(RXIL_ARGV(1), "%lf", &rexx.value);
}

/***************************************************************************/
/*>void RexxCBounds(struct RexxMsg *rexxmsg)
   -----------------------------------------
   Command received from Rexx to set clean bounds
   05.05.92 Original
   12.06.92 Added scale setting (bug fix!) and rexx.replot
   01.07.92 Changed to use RexxDebug()
   02.07.92 Removed setting of rexx.fromrexx
*/
void RexxCBounds(struct RexxMsg *rexxmsg)
{
   double   xmin, xmax,
            ymin, ymax,
            size;

   RexxDebug();

   /* Get user specified values */
   sscanf(RXIL_ARGV(1), "%lf", &xmin);
   sscanf(RXIL_ARGV(2), "%lf", &xmax);
   sscanf(RXIL_ARGV(3), "%lf", &ymin);
   sscanf(RXIL_ARGV(4), "%lf", &ymax);
   
   if(graph.logx)
   {
      xmin = alog(floor(log10(xmin)));
      xmax = alog(ceil(log10(xmax)));
   }
   else
   {
      size = xmax - xmin;
      RoundDown(&xmin,size);
      RoundUp(&xmax,size);
   }
   if(graph.logy)
   {
      ymin = alog(floor(log10(ymin)));
      ymax = alog(ceil(log10(ymax)));
   }
   else
   {
      size = ymax - ymin;
      RoundDown(&ymin,size);
      RoundUp(&ymax,size);
   }
   
   graph.xmin        = xmin;
   graph.xmax        = xmax;
   graph.ymin        = ymin;
   graph.ymax        = ymax;

   graph.XScale      = display.width/(graph.xmax-graph.xmin);
   graph.YScale      = display.height/(graph.ymax-graph.ymin);
   graph.XPScale     = 1.0/(graph.xmax-graph.xmin);
   graph.YPScale     = 1.0/(graph.ymax-graph.ymin);
   graph.userbounds  = TRUE;

   rexx.replot       = TRUE;
}

/***************************************************************************/
/*>RexxDebug()
   -----------
   Checks the rexx.debug flag and, if set outputs rexx debugging information.
   Always outputs to the message window and, if program started from CLI,
   also outputs to CLI.

   01.07.92 Original
*/
RexxDebug()
{
   char  buffer[200];

   if(rexx.debug)
   {
      BuildComLine(buffer);
      WriteMessage(buffer);
      if(display.FromCLI) printf("%s\n",buffer);
   }

   return(0);
}

/***************************************************************************/
/*>void RexxEnd(struct RexxMsg *rexxmsg)
   -------------------------------------
   Clears the rexx.fromrexx flag and ends any line being drawn.
   02.07.92 Original
   07.07.92 No longer ends the line. This is now done for each draw.
*/
void RexxEnd(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   rexx.fromrexx = FALSE;
}

/***************************************************************************/
/*>void RexxEndPlot(struct RexxMsg *rexxmsg)
   -------------------------------------
   Ends a Rexx initiated plot. Closes the file and fixes bbox after PSPLOT
   07.07.92 Original
   10.07.92 Reset plottype to screen when finished.
   15.09.92 Added check on output.fixbounds
*/
void RexxEndPlot(struct RexxMsg *rexxmsg)
{
   RexxDebug();

   if(output.plottype != DEST_SCREEN   && 
      !rexx.fromrexx                   && 
      output.fh != NULL                &&
      rexx.rexxplot)
   {
      AMEndLine(output.plottype, output.fh);

      rexx.rexxplot = FALSE;
      
      /* End the plot */
      AMEndPlot(output.plottype, output.fh);
      
      Close((BPTR)output.fh);
      output.fh = NULL;

      /* Fix BoundingBox info */
      if(output.plottype == DEST_PS && EPSF && output.fixbounds)
         PSFixBoundingBox(MainWind,output.filename);
         
      /* Reset the plot destination to the screen */
      output.plottype = DEST_SCREEN;

      WriteMessage("Plot file completed and closed.");
   }

   rexx.fromrexx = FALSE;
}

/***************************************************************************/
/*>void RexxNoFZero(struct RexxMsg *rexxmsg)
   -----------------------------------------
   06.07.92 Original
*/
void RexxNoFZero(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   graph.fzero = FALSE;
   Uncheck(MenuFZero);
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxNoEPSF(struct RexxMsg *rexxmsg)
   ----------------------------------------
   06.07.92 Original
*/
void RexxNoEPSF(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   EPSF = FALSE;
}

/***************************************************************************/
/*>void RexxNoShr(struct RexxMsg *rexxmsg)
   ---------------------------------------
   06.07.92 Original
*/
void RexxNoShr(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   barstyle.shrinkall = FALSE;
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxFillAll(struct RexxMsg *rexxmsg)
   -----------------------------------------
   06.07.92 Original
*/
void RexxFillAll(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   barstyle.fillall = TRUE;
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxXLab(struct RexxMsg *rexxmsg)
   --------------------------------------
   06.07.92 Original
*/
void RexxXLab(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   graph.noxlabels = FALSE;
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxYLab(struct RexxMsg *rexxmsg)
   --------------------------------------
   06.07.92 Original
*/
void RexxYLab(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   graph.noylabels = FALSE;
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxNoRegW(struct RexxMsg *rexxmsg)
   ----------------------------------------
   06.07.92 Original
*/
void RexxNoRegW(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   fitting.regress_write = FALSE;
}

/***************************************************************************/
/*>void RexxNoReg(struct RexxMsg *rexxmsg)
   ---------------------------------------
   06.07.92 Original
*/
void RexxNoReg(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   fitting.regression = FALSE;
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxNoRxW(struct RexxMsg *rexxmsg)
   ---------------------------------------
   06.07.92 Original
*/
void RexxNoRxW(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   fitting.rexx_write = FALSE;
}

/***************************************************************************/
/*>void RexxNoRx(struct RexxMsg *rexxmsg)
   --------------------------------------
   06.07.92 Original
*/
void RexxNoRx(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   fitting.rexx = FALSE;
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxNoRob(struct RexxMsg *rexxmsg)
   ---------------------------------------
   06.07.92 Original
*/
void RexxNoRob(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   fitting.robust = FALSE;
   Uncheck(MenuRobust);
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxNoFour(struct RexxMsg *rexxmsg)
   ----------------------------------------
   06.07.92 Original
*/
void RexxNoFour(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   fitting.fourier = FALSE;
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxNoBox(struct RexxMsg *rexxmsg)
   ---------------------------------------
   07.07.92 Original
*/
void RexxNoBox(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   graph.boxed = FALSE;
   Uncheck(MenuBoxed);
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxNoGrid(struct RexxMsg *rexxmsg)
   ----------------------------------------
   07.07.92 Original
*/
void RexxNoGrid(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   graph.grid = FALSE;
   Uncheck(MenuGrid);
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxUneject(struct RexxMsg *rexxmsg)
   -----------------------------------------
   07.07.92 Original
*/
void RexxUneject(struct RexxMsg *rexxmsg)
{
   int iparm;
   
   RexxDebug();
   if(data==NULL)
   {
      WriteMessage("The UNEJECT command must be specified after data has been read.");
   }
   else
   {
      sscanf(RXIL_ARGV(1), "%d", &iparm);
      if(graph.style == STYLE_PIE && iparm>0 && iparm <= data->maxdata)
         data->ep[iparm-1] = 0.0;
   }
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxStacked(struct RexxMsg *rexxmsg)
   -----------------------------------------
   07.07.92 Original
*/
void RexxStacked(struct RexxMsg *rexxmsg)
{
   RexxDebug();

   barstyle.style = STYLE_STACKED;
   SetScaling(TRUE);

   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxCalcBounds(struct RexxMsg *rexxmsg)
   --------------------------------------------
   06.07.92 Framework
   07.07.92 Original
*/
void RexxCalcBounds(struct RexxMsg *rexxmsg)
{
   double XSize,
          YSize;
          
   RexxDebug();
   CalcBounds(  &graph.xmin,  &graph.xmax,  &graph.ymin, &graph.ymax);

   /* Calculate scaling from these values */
   if(graph.logx)
      XSize             = log10(graph.xmax)-log10(graph.xmin);
   else
      XSize             = graph.xmax-graph.xmin;
   
   if(graph.logy)
      YSize             = log10(graph.ymax)-log10(graph.ymin);
   else
      YSize             = graph.ymax-graph.ymin;
   
   graph.XScale   = display.width/XSize;
   graph.YScale   = display.height/YSize;
   graph.XPScale  = 1.0/XSize;
   graph.YPScale  = 1.0/YSize;
   
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxCalcTicks(struct RexxMsg *rexxmsg)
   -------------------------------------------
   06.07.92 Framework
   07.07.92 Original
*/
void RexxCalcTicks(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   CalcTicks(0, &graph.xtick, &graph.ytick, &graph.xsub, &graph.ysub);
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxMark(struct RexxMsg *rexxmsg)
   --------------------------------------
   06.07.92 Framework
   07.07.92 Original
*/
void RexxMark(struct RexxMsg *rexxmsg)
{
   double   x,
            y;
   int      mark;

   RexxDebug();

   sscanf(RXIL_ARGV(1), "%lf", &x);
   sscanf(RXIL_ARGV(2), "%lf", &y);
   sscanf(RXIL_ARGV(3), "%d",  &mark);

   AMDPMark(output.plottype,output.fh,x,y,RANGECHECK(mark,0,12));
}

/***************************************************************************/
/*>void RexxOpen(struct RexxMsg *rexxmsg)
   --------------------------------------
   06.07.92 Framework
   07.07.92 Original
*/
void RexxOpen(struct RexxMsg *rexxmsg)
{
   FH *fh;
   
   RexxDebug();

   if(rexx.fromrexx)
   {
      WriteMessage("OPEN command not allowed in installed Rexx macro");
   }
   else
   {
      /* Open the file */
      if((fh = (FH *)Open(RXIL_ARGV(1),MODE_OLDFILE))==NULL)
      {
         /* Give a message if unable to open file */
         ReqMessage2(MainWind,"Unable to open data file:",RXIL_ARGV(1),0);
         return;
      }

      WaitPointer(MainWind);
   
      /* Clear any current data (with force option on) */
      ClearPlots(TRUE);

      /* Read the dataset. Returns TRUE if OK. */
      if(ReadData(fh))
      {
         /* Everything OK, go ahead and plot */
         Close((BPTR)fh);
         fh = NULL;
         SetScaling(FALSE);
         rexx.replot = TRUE;
      }
      else
      {
         /* Free the current dataset, basically so data is set to NULL     */
         ClearPlots(TRUE);
      }

      ClearPointer(MainWind);
   }
}

/***************************************************************************/
/*>void RexxNoCXLab(struct RexxMsg *rexxmsg)
   -----------------------------------------
   07.07.92 Original
*/
void RexxNoCXLab(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   
   graph.centrexlabels = FALSE;   
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxBPLOff(struct RexxMsg *rexxmsg)
   -----------------------------------------
   07.07.92 Original
*/
void RexxBPLOff(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   
   barstyle.uselabels = FALSE;
   graph.noxlabels    = FALSE;
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxBPLOn(struct RexxMsg *rexxmsg)
   -----------------------------------------
   07.07.92 Original
*/
void RexxBPLOn(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   
   barstyle.uselabels = TRUE;
   graph.noxlabels    = TRUE;
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxBPLab(struct RexxMsg *rexxmsg)
   ---------------------------------------
   07.07.92 Original
*/
void RexxBPLab(struct RexxMsg *rexxmsg)
{
   int labnum;
   
   RexxDebug();
   
   sscanf(RXIL_ARGV(1),"%d",&labnum);
   
   labnum--;
   if(barlabels != NULL && labnum >= 0 && labnum < barstyle.nbar)
   {
      strcpy(barlabels[labnum].text,RXIL_ARGV(2));
      barstyle.CurrentBar = labnum;

      /* Switch on barlabels and switch off ordinary labels */
      barstyle.uselabels   = TRUE;
      graph.noxlabels      = TRUE;
   }
   else
   {
      if(barlabels == NULL)
         WriteMessage("Bar/pie labels not available");
      else
         WriteMessage("Bar/pie label number invalid");
   }
   
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxKeyText(struct RexxMsg *rexxmsg)
   -----------------------------------------
   07.07.92 Original
*/
void RexxKeyText(struct RexxMsg *rexxmsg)
{
   int      labnum,
            j;
   DATASET  *d;
   
   RexxDebug();
   
   sscanf(RXIL_ARGV(1),"%d",&labnum);
   labnum--;
   
   if(data != NULL)
   {
      /* Move to appropriate dataset */
      for(d=data, j=0; d && j<labnum; j++, NEXT(d)) ;
      
      if(d != NULL)
         strcpy(d->key.text,RXIL_ARGV(2));
      else
         WriteMessage("Invalid dataset number from ARexx KEYTEXT command.");
   }
   else
   {
      WriteMessage("ARexx KEYTEXT command must be give after data has been specified.");
   }
   
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxFill(struct RexxMsg *rexxmsg)
   --------------------------------------
   07.07.92 Original
*/
void RexxFill(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   
   RexxParse(rexxmsg);
   SetFillDatasets();

   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxKeyFont(struct RexxMsg *rexxmsg)
   -----------------------------------------
   07.07.92 Original
*/
void RexxKeyFont(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   
   strcpy(Defaults.KeyPSFont,RXIL_ARGV(1));
   sscanf(RXIL_ARGV(2),"%lf",&(Defaults.KeyPSSize));

   if(data)
   {
      strcpy(data->key.PSFont, Defaults.KeyPSFont);
      data->key.PSSize = Defaults.KeyPSSize;
   }

   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxPSPlot(struct RexxMsg *rexxmsg)
   ----------------------------------------
   06.07.92 Framework
   07.07.92 Original
   20.07.92 Added DEMO_VERSION conditional code
*/
void RexxPSPlot(struct RexxMsg *rexxmsg)
{
   FH *fh;
   
   RexxDebug();

   if(rexx.fromrexx)
   {
      WriteMessage("ARexx PSPLOT command not allowed within installed macro");
   }
   else
   {
#ifdef DEMO_VERSION
      WriteMessage("Plotting disabled in demo version!");
#else
      WriteMessage("All plotting commands now directed to PostScript file.");
      strcpy(output.filename, RXIL_ARGV(1));
      
      if((fh = (FH *)Open(output.filename,MODE_NEWFILE))==NULL)
      {
         ReqMessage2(MainWind,"Unable to open PostScript file",output.filename,0);
         return;
      }

      output.plottype = DEST_PS;
      output.fh       = fh;
      rexx.rexxplot   = TRUE;

      DoPlot(output.plottype,fh);
#endif
   }
}

/***************************************************************************/
/*>void RexxHPGLPlot(struct RexxMsg *rexxmsg)
   ------------------------------------------
   06.07.92 Framework
   07.07.92 Original
   20.07.92 Added DEMO_VERSION conditional code
*/
void RexxHPGLPlot(struct RexxMsg *rexxmsg)
{
   FH *fh;
   
   RexxDebug();

   if(rexx.fromrexx)
   {
      WriteMessage("ARexx HPGLPLOT command not allowed within installed macro");
   }
   else
   {
#ifdef DEMO_VERSION
      WriteMessage("Plotting disabled in demo version!");
#else
      WriteMessage("All plotting commands now directed to HPGL file.");
      strcpy(output.filename, RXIL_ARGV(1));
      
      if((fh = (FH *)Open(output.filename,MODE_NEWFILE))==NULL)
      {
         ReqMessage2(MainWind,"Unable to open HPGL file",output.filename,0);
         return;
      }

      output.plottype = DEST_HPGL;
      output.fh       = fh;
      rexx.rexxplot   = TRUE;

      DoPlot(output.plottype,fh);
#endif
   }
}

/***************************************************************************/
/*>void RexxDR2DPlot(struct RexxMsg *rexxmsg)
   ------------------------------------------
   06.07.92 Framework
   07.07.92 Original
   20.07.92 Added DEMO_VERSION conditional code. Corrected to work properly!
*/
void RexxDR2DPlot(struct RexxMsg *rexxmsg)
{
   RexxDebug();

   if(rexx.fromrexx)
   {
      WriteMessage("ARexx DR2DPLOT command not allowed within installed macro");
   }
   else
   {
#ifdef DEMO_VERSION
      WriteMessage("Plotting disabled in demo version!");
#else
      WriteMessage("All plotting commands now directed to DR2D file.");
      strcpy(output.filename, RXIL_ARGV(1));
      
      output.plottype = DEST_DR2D;
      output.fh       = NULL;
      rexx.rexxplot   = TRUE;

      DoPlot(output.plottype,output.fh);
#endif
   }
}

/***************************************************************************/
/*>void RexxInstall(struct RexxMsg *rexxmsg)
   ------------------------------------------
   07.07.92 Original
*/
void RexxInstall(struct RexxMsg *rexxmsg)
{
   RexxDebug();

   if(rexx.fromrexx)
   {
      WriteMessage("ARexx MACRO command not allowed within installed macro");
   }
   else
   {
      /* Check full path specified */
      if((strchr(RXIL_ARGV(1),'/') == NULL) &&
         (strchr(RXIL_ARGV(1),':') == NULL))
      {
         WriteMessage("Path for ARexx macro must be specified.");
      }
      else
      {
         WriteMessage("ARexx macro installed.");
         strcpy(rexx.RexxMacro,RXIL_ARGV(1));
         rexx.replot = TRUE;
      }
   }
}

/***************************************************************************/
/*>void RexxStyle(struct RexxMsg *rexxmsg)
   ------------------------------------------
   Dispatches commands for changing the main graph style.
   07.07.92 Original
   24.07.92 OnMenu()'s Pie if given Rexx pie command
   13.08.92 Also OnMenu()'s Pie Setup
*/
void RexxStyle(struct RexxMsg *rexxmsg)
{
   RexxDebug();

   Uncheck(MenuXY);
   Uncheck(MenuScatter);
   Uncheck(MenuBar);
   Uncheck(MenuPie);

   if(!strcmp(RXIL_ARGV(0), "XY"))
   {
      StyleXY(NULL);
      Check(MenuXY);
   }
   else if(!strcmp(RXIL_ARGV(0), "SCATTER"))
   {
      StyleScatter(NULL);
      Check(MenuScatter);
   }
   else if(!strcmp(RXIL_ARGV(0), "BAR"))
   {
      StyleBar(NULL);
      Check(MenuBar);
   }
   else if(!strcmp(RXIL_ARGV(0), "PIE"))
   {
      StylePie(NULL);
      OnMenu(MainWind, MenuNumPie);
      OnMenu(MainWind, MenuNumPieSetup);
      Check(MenuPie);
   }
   
   rexx.replot = TRUE;
}

/***************************************************************************/
/*>void RexxAutoOn(struct RexxMsg *rexxmsg)
   ----------------------------------------
   Switches on auto replotting after ARexx commands which change the graph
   format.
   07.07.92 Original
*/
void RexxAutoOn(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   rexx.autoreplot = TRUE;
}

/***************************************************************************/
/*>void RexxAutoOff(struct RexxMsg *rexxmsg)
   -----------------------------------------
   Switches off auto replotting after ARexx commands which change the graph
   format.
   07.07.92 Original
*/
void RexxAutoOff(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   rexx.autoreplot = FALSE;
}

/***************************************************************************/
/*>void RexxReplot(struct RexxMsg *rexxmsg)
   ----------------------------------------
   Causes the graph to be replotted. Used when ARexx auto replotting
   switched off.
   07.07.92 Original
*/
void RexxReplot(struct RexxMsg *rexxmsg)
{
   RexxDebug();
   rexx.forcereplot = TRUE;
}

/***************************************************************************/
/*>InitRexx(void)
   --------------
   Initialise the ARexx port.
   29.04.92 Original
   30.04.92 Added information in About requester
   02.07.92 Added ENDPLOT command.
   06.07.92 Added lots of extra commands.
   07.07.92 Added extra commands.
   09.07.92 Added extra commands.
   13.07.92 Added OffMenu() of ARexx menu, if Rexx not available
*/
InitRexx(void)
{
   MAKECOM(rexx_cmd_table[0],  "XY",         &RexxStyle,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[1],  "SCATTER",    &RexxStyle,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[2],  "BAR",        &RexxStyle,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[3],  "PIE",        &RexxStyle,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[4],  "ERRORS",     &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[5],  "NOERRORS",   &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[6],  "BOXED",      &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[7],  "LOGX",       &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[8],  "LOGY",       &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[9],  "BOUNDS",     &RexxParse,   4, 4,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[10], "TICKS",      &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[11], "GRID",       &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[12], "FZERO",      &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[13], "ROBUST",     &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[14], "DRAW",       &RexxDraw,    2, 4,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[15], "QUIT",       &RexxQuit,    0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[16], "PEN",        &RexxPen,     1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[17], "GETVAR",     &RexxGet,     2, 3,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[18], "DATASET",    &RexxDataset, 1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[19], "EMPTY",      &RexxEmpty,   4, 4,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[20], "SETVAR",     &RexxSetVal,  1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[21], "CBOUNDS",    &RexxCBounds, 4, 4,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[22], "ENDMACRO",   &RexxEnd,     0, 0,  FALSE, RXIL_PUBLIC);
   /* From here added 06.07.92 */
   MAKECOM(rexx_cmd_table[23], "SIZE",       &RexxParseNoR,2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[24], "OFFSET",     &RexxParseNoR,2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[25], "EPSF",       &RexxParseNoR,0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[26], "NOEPSF",     &RexxNoEPSF,  0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[27], "STYLE",      &RexxLStyle,  1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[28], "SETPEN",     &RexxParse,   3, 3,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[29], "PALETTE",    &RexxParseNoR,4, 4,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[30], "DATAPOINT",  &RexxParse,   3, 3,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[31], "EWIDTH",     &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[32], "MARKSIZE",   &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[33], "OVERLAYED",  &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[34], "GROUPED",    &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[35], "STACKED",    &RexxStacked, 0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[36], "OUTLINE",    &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[37], "EJECT",      &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[38], "SHRINK",     &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[39], "SHRFIRST",   &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[40], "NOSHRFIRST", &RexxNoShr,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[41], "PSTART",     &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[42], "EJBY",       &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[43], "HATCH",      &RexxParseNoR,0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[44], "SHADE",      &RexxParseNoR,0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[45], "HTHICK",     &RexxParseNoR,1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[46], "HSPACE",     &RexxParseNoR,1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[47], "FALL",       &RexxFillAll, 0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[48], "NOFILLALL",  &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[49], "FILL",       &RexxFill,    3, 3,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[50], "PFILL",      &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[51], "CLIP",       &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[52], "NOCLIP",     &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[53], "SUBTICKS",   &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[54], "SUBON",      &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[55], "SUBOFF",     &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[56], "CALCBOUNDS", &RexxCalcBounds, 0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[57], "CALCTICKS",  &RexxCalcTicks,  0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[58], "NOFZERO",    &RexxNoFZero, 0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[59], "TITLE",      &RexxParse,   3, 3,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[60], "TFONT",      &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[61], "XLABEL",     &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[62], "YLABEL",     &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[63], "LFONT",      &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[64], "XTFONT",     &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[65], "YTFONT",     &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[66], "XLFONT",     &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[67], "YLFONT",     &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[68], "XLAB",       &RexxXLab,    0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[69], "YLAB",       &RexxYLab,    0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[70], "NOXLAB",     &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[71], "NOYLAB",     &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[72], "CENTXLAB",   &RexxParse,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[73], "PRECISION",  &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[74], "BPLAB",      &RexxBPLab,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[75], "KEYPOS",     &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[76], "KEYFONT",    &RexxKeyFont, 2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[77], "KEYTEXT",    &RexxKeyText, 2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[78], "LABEL",      &RexxParse,   6, 6,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[79], "REGRESS",    &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[80], "TOL",        &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[81], "REGWRITE",   &RexxParseNoR,0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[82], "NOREGWRITE", &RexxNoRegW,  0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[83], "NOREGRESS",  &RexxNoReg,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[84], "REXXFIT",    &RexxParse,   2, 2,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[85], "RTOL",       &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[86], "REXXWRITE",  &RexxParseNoR,0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[87], "NOREXXWRITE",&RexxNoRxW,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[88], "NOREXXFIT",  &RexxNoRx,    0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[89], "NOROBUST",   &RexxNoRob,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[90], "FOURIER",    &RexxParse,   1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[91], "NOFOURIER",  &RexxNoFour,  0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[92], "MARK",       &RexxMark,    3, 3,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[93], "OPEN",       &RexxOpen,    1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[94], "SAVE",       &RexxSave,    1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[95], "PSPLOT",     &RexxPSPlot,  1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[96], "HPGLPLOT",   &RexxHPGLPlot,1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[97], "DR2DPLOT",   &RexxDR2DPlot,1, 1,  FALSE, RXIL_PUBLIC);
   /* From here added 07.07.92 */
   MAKECOM(rexx_cmd_table[98], "NOBOXED",    &RexxNoBox,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[99], "NOGRID",     &RexxNoGrid,  0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[100],"UNEJECT",    &RexxUneject, 1, 1,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[101],"NOCENTXLAB", &RexxNoCXLab, 0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[102],"BPLOFF",     &RexxBPLOff,  0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[103],"BPLON",      &RexxBPLOn,   0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[104],"ENDPLOT",    &RexxEndPlot, 0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[105],"MACRO",      &RexxInstall, 1, 1,  FALSE, RXIL_PUBLIC);
   /* From here added 09.07.92 */
   MAKECOM(rexx_cmd_table[106],"AUTOON",     &RexxAutoOn,  0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[107],"AUTOOFF",    &RexxAutoOff, 0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[108],"REPLOT",     &RexxReplot,  0, 0,  FALSE, RXIL_PUBLIC);
   MAKECOM(rexx_cmd_table[109],NULL,         NULL,         0, 0,  FALSE, 0          );

   rexx.gotrexx = TRUE;

   if(SetupREXX(rexx.PortName,NULL,NULL,1,rexx_cmd_table,NULL))
   {
      /* Build the port name into the About requester */
      sprintf(rexx.PortName,"ARexx port: %s",PortName());
      BuildText(NWAbout,rexx.PortName,1,100,100,2);
   }
   else     /* No Rexx available */
   {
      OffMenu(MainWind,MenuNumRexxFit);
      OffMenu(MainWind,MenuNumARexx);
      rexx.gotrexx = FALSE;
   }
   return(0);
}

/***************************************************************************/
/*>WaitForRexx(void)
   -----------------
   02.07.92 Original
*/
WaitForRexx(void)
{
   while(rexx.fromrexx)
   {
      Wait(global_rdef->SigBit);
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
         DoPlot(output.plottype,output.fh);
         rexx.replot      = FALSE;
         rexx.forcereplot = FALSE;
      }
   }
   return(0);
}





























