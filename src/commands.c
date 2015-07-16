/****************************************************************************

   Program:    AMPlot2
   File:       commands.c
   
   Version:    V1.0
   Date:       27.03.92
   Function:   AMPlot command handling
   
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
/* Includes
*/
#include "includes.h"
#include "menudefs.h"

/***************************************************************************/
/*>process_command(char *buffer, DATASET *current_data,
                   int *newdata, int *ndata)
   ----------------------------------------------------
   Input:   buffer         Command line buffer
            current_data   Pointer to current dataset
   Output:  newdata        Flag to show NEWSET found
            ndata          Result of NDATA keyword
            
   This is the main command parser dispatcher. Takes a command line buffer
   and takes appropriate actions.
   27.03.92 Original (some bits missing...)
   07.04.92 Some fixes to axis titles
   08.04.92 COLUMNS no longer switches on errors
   28.04.92 Corrected PEN command.
   29.04.92 Various fixes for REXX
   03.07.92 Added lots of additional commands
   06.07.92 Stopped REXX specific toggles. Now separate commands.
            Removed unnecessary gadget selections
   07.07.92 GROUPED was setting shrink to 0.0 --- No longer does!
   10.07.92 Corrected setting of graph.usersub
   13.07.92 Added OffPieMenus() for appropriate styles.
   24.07.92 COLUMNS now OnMenu()'s the Pie menu if just one column.
   13.08.92 Now also OnMenu()'s Pie setup.
*/
int process_command(char    *buffer,          /* Command line buffer           */
                DATASET *current_data,    /* Pointer to current dataset    */
                int     *newdata,         /* Flag to show NEWSET found     */
                int     *ndata)           /* Result of NDATA keyword       */
{
   int      i,                   /* Counter                                */
            pen,                 /* pen number                             */
            R,G,B,
            iparm;
   TEXTLIST *p;
   FILLITEM *f;

   /* Variables for the command parser */
   int   key;                    /* Return value from parse()              */
   REAL  numparam[MAXNUMPARAM];  /* Array for returned numbers             */

   *newdata = FALSE;
   
   if(*buffer != '!')
   {
      /* Pass this to the command parser  */

      key = parse(buffer,NCOMM,keywd,numparam,strparam);

      /* Carry out appropriate action for each keyword. This
         could be made more readable by #define'ing a label for
         each key number which matches the command name */
      switch(key)
      {
      case PARSE_ERRC:
         WriteMessage("Unknown or ambiguous keyword in datafile:");
         WriteMessage(buffer);
         break;
      case PARSE_ERRP:
         WriteMessage("Error in datafile keyword parameters:");
         WriteMessage(buffer);
         break;
      case PARSE_COMMENT:
         break;
      case 0:  /* SETPEN pen colour thickness                              */
         pen = RANGECHECK((int)numparam[0],1,MAXPEN);
         pen--;
         pens[pen].colour = (int)numparam[1];
         if(numparam[1] == 0)
         {
            pens[pen].screencolour = 1;
         }
         else
         {
            pens[pen].screencolour = 
               numparam[1]-(int)(numparam[1]/display.NColours)*display.NColours;
            if(pens[pen].screencolour == 0)
               pens[pen].screencolour = display.NColours-1;
         }
         pens[pen].thickness = numparam[2];
         break;
      case 1:  /* XY                                                       */
         graph.style = STYLE_LINE;
#ifdef AMIGA
         /* Switch on menus not required by Pie charts */
         if(MainWind) OnPieMenus();
         Check(MenuXY);
         Uncheck(MenuScatter);
         Uncheck(MenuBar);
         Uncheck(MenuPie);
#endif
         break;
      case 2:  /* SCATTER                                                  */
         graph.style = STYLE_SCATTER;
#ifdef AMIGA
         /* Switch on menus not required by Pie charts */
         if(MainWind) OnPieMenus();
         Uncheck(MenuXY);
         Check(MenuScatter);
         Uncheck(MenuBar);
         Uncheck(MenuPie);
#endif
         for(i=0;i<MAXPEN;i++)
         {
            if(pens[i].mark == 0)
               pens[i].mark = 1;
         }
         break;
      case 3:  /* BAR                                                      */
         graph.style = STYLE_BAR;
         /* Switch on menus not required by Pie charts */
#ifdef AMIGA
         if(MainWind) OnPieMenus();
         Uncheck(MenuXY);
         Uncheck(MenuScatter);
         Check(MenuBar);
         Uncheck(MenuPie);
#endif
         break;
      case 4:  /* PIE                                                      */
         graph.style = STYLE_PIE;
         /* If we have opened a window, switch of non-relevant menus       */
         SetScaling(TRUE);
#ifdef AMIGA
         if(MainWind) OffPieMenus();  
         OnMenu(MainWind, MenuNumPie);
         OnMenu(MainWind, MenuNumPieSetup);
         Uncheck(MenuXY);
         Uncheck(MenuScatter);
         Uncheck(MenuBar);
         Check(MenuPie);
#endif
         break;
      case 5:  /* DATAPOINT pen mark join                                  */
         pen = RANGECHECK((int)numparam[0],1,MAXPEN);
         pen--;
         pens[pen].mark = RANGECHECK((int)numparam[1],0,MAXMARKS-1);
         pens[pen].join = numparam[2];
         break;
      case 6:  /* ERRORS                                                   */
         graph.errors = TRUE;
#ifdef AMIGA
         Check(MenuErrors);
#endif
         break;
      case 7:  /* NOERRORS                                                 */
         graph.errors = FALSE;
#ifdef AMIGA
         Uncheck(MenuErrors);
#endif
         break;
      case 8:  /* SHRINK percent                                           */
         if(fabs(numparam[0]) < 0.00001)  barstyle.shrink = 0.0;
         else                             barstyle.shrink = 200.0/numparam[0];
         break;
      case 9: /* SHRFIRST                                                  */
         barstyle.shrinkall = TRUE;
         break;
      case 10: /* OUTLINE                                                  */
         barstyle.style = STYLE_OUTLINE;
         break;
      case 11: /* NOXLAB                                                   */
         graph.noxlabels = TRUE;
         break;
      case 12: /* BOXED                                                    */
         graph.boxed = TRUE;
#ifdef AMIGA
         Check(MenuBoxed);
#endif
         break;
      case 13: /* LOGX                                                     */
         ToggleLogX(NULL);                            /* 29.04.92 */
#ifdef AMIGA
         if(graph.logx) Check(MenuLogX);
         else           Uncheck(MenuLogX);
#endif
         break;
      case 14: /* LOGY                                                     */
         ToggleLogY(NULL);                            /* 29.04.92 */
#ifdef AMIGA
         if(graph.logy) Check(MenuLogY);
         else           Uncheck(MenuLogY);
#endif
         break;
      case 15: /* BOUNDS xmin xmax ymin ymax                               */
         graph.xmin        = numparam[0];
         graph.xmax        = numparam[1];
         graph.ymin        = numparam[2];
         graph.ymax        = numparam[3];
         graph.XScale      = display.width/(graph.xmax-graph.xmin);
         graph.YScale      = display.height/(graph.ymax-graph.ymin);
         graph.XPScale     = 1.0/(graph.xmax-graph.xmin);
         graph.YPScale     = 1.0/(graph.ymax-graph.ymin);
         graph.userbounds  = TRUE;
         break;
      case 16: /* TICKS x y                                                */
         graph.xtick       = numparam[0];
         graph.ytick       = numparam[1];
         graph.userticks   = TRUE;
         break;
      case 17: /* XLABEL label                                             */
         strcpy(XTitle.text,strparam[0]);
         break;
      case 18: /* YLABEL label                                             */
         strcpy(YTitle.text,strparam[0]);
         break;
      case 19: /* LFONT font size                                          */
         strcpy(XTitle.PSFont,strparam[0]);
         strcpy(YTitle.PSFont,strparam[0]);
         strcpy(XLabel.PSFont,strparam[0]);
         strcpy(YLabel.PSFont,strparam[0]);
         sscanf(strparam[1],"%lf",&XTitle.PSSize);
         XLabel.PSSize = YLabel.PSSize = YTitle.PSSize = XTitle.PSSize;
         break;
      case 20: /* TITLE label x y                                          */
         strcpy(GTitle.text,strparam[0]);
         sscanf(strparam[1],"%lf",&GTitle.x);
         sscanf(strparam[2],"%lf",&GTitle.y);
         if((GTitle.x==0.0)&&(GTitle.y==0.0))
            graph.usertitlepos = FALSE;
         else
            graph.usertitlepos = TRUE;
         break;
      case 21: /* TFONT font size                                          */
         strcpy(GTitle.PSFont,strparam[0]);
         sscanf(strparam[1],"%lf",&GTitle.PSSize);
         break;
      case 22: /* GRID                                                     */
         graph.grid = TRUE;
#ifdef AMIGA
         Check(MenuGrid);
#endif
         break;
      case 23: /* FZERO                                                    */
         graph.fzero = TRUE;
#ifdef AMIGA
         Check(MenuFZero);
#endif
         break;
      case 24: /* NEWSET                                                   */
         graph.nplots++;
         *newdata = TRUE;
         break;
      case 25: /* PEN pen                                                  */
         current_data->pen = RANGECHECK((int)numparam[0],1,MAXPEN);
         break;
      case 26: /* STYLE style                                              */
         current_data->linestyle = RANGECHECK((int)numparam[0],1,MAXSTYLE);
         current_data->linestyle--;
         break;
      case 27: /* LABEL number string x y font size                      */
         /* Get the label number from the command line                   */
         sscanf(strparam[0],"%d",&graph.CurrentLabel);
         /* Allocate space as necessary                         */
         while(graph.nlabels < graph.CurrentLabel)
            AllocExtraLabel();

         /* Move to the label in question                       */
         for(p=labels,i=0;
             p && i<graph.CurrentLabel;
             NEXT(p), i++);
         if(p)
         {
            /* Now copy the stuff from the command line into the list */
            strcpy(p->stext.text,strparam[1]);
            sscanf(strparam[2],"%lf",&p->stext.x);
            sscanf(strparam[3],"%lf",&p->stext.y);
            strcpy(p->stext.PSFont,strparam[4]);
            sscanf(strparam[5],"%lf",&p->stext.PSSize);
         }

         break;
      case 29: /* NOFILLALL                                                */
         barstyle.fillall = FALSE;
         break;
      case 30: /* REGRESS n                                                */
         fitting.regression = TRUE;
         fitting.regress_npoly = RANGECHECK((int)numparam[0],2,20);
         break;
      case 31: /* ROBUST                                                   */
         fitting.robust = TRUE;
#ifdef AMIGA
         Check(MenuRobust);
#endif
         break;
      case 32: /* FOURIER n                                                */
         fitting.fourier      = TRUE;
         fitting.smooth_pts   = numparam[0];
         break;
      case 33: /* SIZE x y                                                 */
         
         output.xsize = (float)numparam[0];
         output.ysize = (float)numparam[1];
         break;
      case 34: /* OFFSET x y                                               */
         output.xoffset = (float)numparam[0];
         output.yoffset = (float)numparam[1];
         break;
      case 35: /* COLUMNS n                                                */
         graph.columns = RANGECHECK((int)numparam[0],1,4);
#ifdef AMIGA
         if(graph.columns == 1)
         {
            OnMenu(MainWind, MenuNumPie);
            OnMenu(MainWind, MenuNumPieSetup);
         }
#endif
         break;
      case 37: /* GROUPED                                                  */
         barstyle.style       = STYLE_GROUPED;
         barstyle.shrinkall   = TRUE;
         break;
      case 38: /* EPSF                                                     */
         EPSF = TRUE;
         break;         
      case 39: /* XLFONT                                                   */
         strcpy(XLabel.PSFont,strparam[0]);
         sscanf(strparam[1],"%lf",&XLabel.PSSize);
         break;
      case 40: /* YLFONT                                                   */
         strcpy(YLabel.PSFont,strparam[0]);
         sscanf(strparam[1],"%lf",&YLabel.PSSize);
         break;
      case 41: /* NDATA n                                                  */
         *ndata = (int)numparam[0];
         break;

/***                                     ***
 ***  Commands from here added 03.07.92  ***
 ***                                     ***/

      case 42: /* PALETTE scrpen r g b                                     */
         pen = RANGECHECK((int)numparam[0], 0, 3);
         R   = RANGECHECK((int)numparam[1], 0,15);
         G   = RANGECHECK((int)numparam[2], 0,15);
         B   = RANGECHECK((int)numparam[3], 0,15);
#ifdef AMIGA
         SetRGB4(&MyScreen->ViewPort,pen,(UBYTE)R,(UBYTE)G,(UBYTE)B);
#endif
         break;
      case 43: /* EWIDTH width                                             */
         graph.ebarwidth = numparam[0];
         break;
      case 44: /* MARKSIZE size                                            */
         graph.dpsize    = numparam[0];
         break;
      case 45: /* OVERLAYED                                                */
         barstyle.style  = STYLE_OVERLAYED;
         break;
      case 46: /* STACKED                                                  */
         barstyle.style  = STYLE_STACKED;
         break;
      case 47: /* EJECT slice                                              */
         if(data==NULL)
         {
            WriteMessage("The EJECT command must be specified after the data");
         }
         else
         {
            iparm = (int)numparam[0];
            if(graph.style == STYLE_PIE && iparm>0 && iparm <= data->maxdata)
               data->ep[iparm-1] = 1.0;
         }
         break;
      case 48: /* PSTART angle                                             */
         piestyle.start = PI * numparam[0] / 180.0;
         break;
      case 49: /* EJBY percent                                             */
         piestyle.eject = RANGECHECK(numparam[0], 0.0, 100.0);
         break;
      case 50: /* HATCH                                                    */
         output.hatching = TRUE;
         break;
      case 51: /* SHADE                                                    */
         output.hatching = FALSE;
         break;
      case 52: /* HTHICK thick                                             */
         output.hatchthick = numparam[0];
         break;
      case 53: /* HSPACE spacing                                           */
         output.hatchspace = numparam[0];
         break;
      case 54: /* CLIP                                                     */
         graph.clip = TRUE;
         break;
      case 55: /* NOCLIP                                                   */
         graph.clip = FALSE;
         break;
      case 56: /* SUBTICKS x y                                             */
         graph.xsub     = numparam[0];
         graph.ysub     = numparam[1];
         graph.usersub  = TRUE;
         break;
      case 57: /* SUBON                                                    */
         graph.subtick  = TRUE;
         break;
      case 58: /* SUBOFF                                                   */
         graph.subtick  = FALSE;
         break;
      case 59: /* XTFONT font size                                         */
         strcpy(XTitle.PSFont,strparam[0]);
         sscanf(strparam[1],"%lf",&XTitle.PSSize);
         break;
      case 60: /* YTFONT font size                                         */
         strcpy(YTitle.PSFont,strparam[0]);
         sscanf(strparam[1],"%lf",&YTitle.PSSize);
         break;
      case 61: /* NOYLAB                                                   */
         graph.noylabels = TRUE;
         break;
      case 62: /* CENTXLAB                                                 */
         graph.centrexlabels = TRUE;
         break;
      case 63: /* PRECISION xprec yprec                                    */
         graph.xprecision = (int)numparam[0];
         graph.yprecision = (int)numparam[1];
         graph.userprec = TRUE;
         break;
      case 64: /* BPLAB                                                    */
         sscanf(strparam[0],"%d",&iparm);
         CacheBarlabel(iparm,strparam[1]);
         break;
      case 65: /* KEYPOS x y                                               */
         if(data)
         {
            data->key.x   = numparam[0];
            data->key.y   = numparam[1];
         }
         else
         {
            Defaults.KeyX = numparam[0];
            Defaults.KeyY = numparam[1];
         }
         break;
      case 66: /* KEYFONT font size                                        */
         strcpy(Defaults.KeyPSFont,strparam[0]);
         sscanf(strparam[1],"%lf",&(Defaults.KeyPSSize));
         break;
      case 67: /* KEYTEXT n text                                           */
         sscanf(strparam[0],"%d",&iparm);
         CacheKeytext(iparm,strparam[1]);
         break;
      case 68: /* TOL tol                                                  */
         fitting.regress_tol = numparam[0];
         break;
      case 69: /* REGWRITE                                                 */
         fitting.regress_write = TRUE;
         break;
      case 70: /* REXXFIT degree macro                                     */
         sscanf(strparam[0],"%d",&fitting.rexx_npoly);
         strcpy(fitting.rexxmacro,strparam[1]);

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
         break;
      case 71: /* RTOL tol                                                 */
         fitting.rexx_tol = numparam[0];
         break;
#ifdef REXX_SUPPORT
      case 72: /* REXXWRITE                                                */
         fitting.rexx_write = TRUE;
         break;
      case 73: /* MACRO macro                                              */
         strcpy(rexx.RexxMacro,strparam[0]);
         break;
      case 74: /* REXXDEBUG                                                */
         rexx.debug = TRUE;
#ifdef AMIGA
         Check(MenuDebRexx);
#endif
         break;
#endif
      case 28: /* FILL number x y                                           */
         if(graph.style == STYLE_PIE)
         {
            WriteMessage("Datafile FILL command ignored! Use PFILL for Pie Charts.");
         }
         else
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
               f->pattern  = RANGECHECK((int)numparam[0], 0, 6);
               f->x        = numparam[1];
               f->y        = numparam[2];
               f->dataset  = 0;             /* 29.05.97 Was NULL            */
               f->angle    = 0.0;
            }
         }
         break;
      case 36: /* PFILL number angle                                        */
         if(graph.style == STYLE_PIE)        /* 29.05.97 Corrected from =   */
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
               f->pattern  = RANGECHECK((int)numparam[0], 0, 6);
               f->x        = 0.0;
               f->y        = 0.0;
               f->dataset  = 0;        /* 29.05.96   Was NULL              */
               f->angle    = PI * numparam[1] / 180.0;
            }
         }
         else
         {
            WriteMessage("PFILL ignored! Only applicable to Pie Charts");
         }
         break;
      default:
         break;
      }
   }
   return(0);
}




/***************************************************************************/
/*>CacheBarlabel(int labnum, char *text)
   -------------------------------------
   Caches the bar labels specified in the datafile.
   03.07.92 Original
*/
int CacheBarlabel(int labnum, char *text)
{
   int         j,
               k;
   
   /* Expand the cache if necessary */
   if(LabCacheSize < labnum)
   {
      if(LabCache == NULL)    /* No cache yet      */
      {
         LabCacheSize = 10;
         LabCache     = (char **)malloc(LabCacheSize * sizeof(char **));
      }
      else                    /* Expand the cache  */
      {
         LabCacheSize += 10;
         LabCache     = (char **)realloc(LabCache,
                                         LabCacheSize*sizeof(char **));
      }

      /* Initialise each of the extra cache positions to store a string */
      for(j=LabCacheSize-10; j<LabCacheSize; j++)
      {
         if((LabCache[j] = (char *)malloc(MAXBUFF * sizeof(char))) == NULL)
         {
            /* If no mem, free the ones we've just allocated and reduce
               the cache size
            */
            for(k=LabCacheSize-10; k<j; k++)
               if(LabCache[k]) free(LabCache[j]);
            LabCacheSize -= 10;
            return(1);
         }
         LabCache[j][0] = '\0';
      }
   }
   
   /* Copy the text into the cache */
   if(LabCacheSize > --labnum && labnum>=0)  /* Check we have space in cache  */
      strcpy(LabCache[labnum], text);

   return(0);
}

/***************************************************************************/
/*>CacheKeytext(int labnum, char *text)
   ------------------------------------
   Caches text for the key specified in the datafile.
   03.07.92 Original
*/
int CacheKeytext(int labnum, char *text)
{
   int         j,
               k;
   
   /* Expand the cache if necessary */
   if(KeyCacheSize < labnum)
   {
      if(KeyCache == NULL)    /* No cache yet      */
      {
         KeyCacheSize = 5;
         KeyCache  = (char **)malloc(KeyCacheSize * sizeof(char **));
      }
      else                    /* Expand the cache  */
      {
         KeyCacheSize += 5;
         KeyCache  = (char **)realloc(KeyCache,
                                      KeyCacheSize*sizeof(char **));
      }

      /* Initialise each of the extra cache positions to store a string */
      for(j=KeyCacheSize-5; j<KeyCacheSize; j++)
      {
         if((KeyCache[j] = (char *)malloc(MAXBUFF * sizeof(char))) == NULL)
         {
            /* If no mem, free the ones we've just allocated and reduce
               the cache size
            */
            for(k=KeyCacheSize-5; k<j; k++)
               if(KeyCache[k]) free(KeyCache[j]);
            KeyCacheSize -= 5;
            return(1);
         }
         KeyCache[j][0] = '\0';
      }
   }
   
   /* Copy the text into the cache */
   if(KeyCacheSize > --labnum && labnum>=0)  /* Check we have space in cache  */
      strcpy(KeyCache[labnum], text);

   return(0);
}

/***************************************************************************/
/*>InitParser(void)
   ----------------
   Initialises the command parser.
   27.03.92 Original
   03.07.92 Added lots of extra commands
   28.07.92 Changed BAR to BARCHART
*/
int InitParser(void)
{
   int   i;                         /* Counter           */
   
   for(i=0; i<MAXSTRPARAM; i++)
      strparam[i] = (char *)malloc(MAXSTRLEN * sizeof(char));

/* Keywords in AMPlot V1.0 */
   MAKEKEY(keywd[0], "SETPEN",   NUMBER,3);
   MAKEKEY(keywd[1], "XY",       NUMBER,0);
   MAKEKEY(keywd[2], "SCATTER",  NUMBER,0);
   MAKEKEY(keywd[3], "BARCHART", NUMBER,0);
   MAKEKEY(keywd[4], "PIE",      NUMBER,0);
   MAKEKEY(keywd[5], "DATAPOINT",NUMBER,3);
   MAKEKEY(keywd[6], "ERRORS",   NUMBER,0);
   MAKEKEY(keywd[7], "NOERRORS", NUMBER,0);
   MAKEKEY(keywd[8], "SHRINK",   NUMBER,1);
   MAKEKEY(keywd[9], "SHRFIRST", NUMBER,0);
   MAKEKEY(keywd[10],"OUTLINE",  NUMBER,0);
   MAKEKEY(keywd[11],"NOXLAB",   NUMBER,0);
   MAKEKEY(keywd[12],"BOXED",    NUMBER,0);
   MAKEKEY(keywd[13],"LOGX",     NUMBER,0);
   MAKEKEY(keywd[14],"LOGY",     NUMBER,0);
   MAKEKEY(keywd[15],"BOUNDS",   NUMBER,4);
   MAKEKEY(keywd[16],"TICKS",    NUMBER,2);
   MAKEKEY(keywd[17],"XLABEL",   STRING,1);
   MAKEKEY(keywd[18],"YLABEL",   STRING,1);
   MAKEKEY(keywd[19],"LFONT",    STRING,2);
   MAKEKEY(keywd[20],"TITLE",    STRING,3);
   MAKEKEY(keywd[21],"TFONT",    STRING,2);
   MAKEKEY(keywd[22],"GRID",     NUMBER,0);
   MAKEKEY(keywd[23],"FZERO",    NUMBER,0);
   MAKEKEY(keywd[24],"NEWSET",   NUMBER,0);
   MAKEKEY(keywd[25],"PEN",      NUMBER,1);
   MAKEKEY(keywd[26],"STYLE",    NUMBER,1);
   MAKEKEY(keywd[27],"LABEL",    STRING,6);
   MAKEKEY(keywd[28],"FILL",     NUMBER,3);
   MAKEKEY(keywd[29],"NOFILLALL",NUMBER,0);
   MAKEKEY(keywd[30],"REGRESS",  NUMBER,1);
   MAKEKEY(keywd[31],"ROBUST",   NUMBER,0);
   MAKEKEY(keywd[32],"FOURIER",  NUMBER,1);
   MAKEKEY(keywd[33],"SIZE",     NUMBER,2);
   MAKEKEY(keywd[34],"OFFSET",   NUMBER,2);
   MAKEKEY(keywd[35],"COLUMNS",  NUMBER,1);
   MAKEKEY(keywd[36],"PFILL",    NUMBER,2);
   MAKEKEY(keywd[37],"GROUPED",  NUMBER,0);
   MAKEKEY(keywd[38],"EPSF",     NUMBER,0);

/***                                 ***
 ***  Extra keywords added for V2.0  ***
 ***                                 ***/
   MAKEKEY(keywd[39],"XLFONT",   STRING,2);
   MAKEKEY(keywd[40],"YLFONT",   STRING,2);
   MAKEKEY(keywd[41],"NDATA",    NUMBER,1);
   /* Added 03.07.92                   */
   MAKEKEY(keywd[42],"PALETTE",  NUMBER,4);
   MAKEKEY(keywd[43],"EWIDTH",   NUMBER,1);
   MAKEKEY(keywd[44],"MARKSIZE", NUMBER,1);
   MAKEKEY(keywd[45],"OVERLAYED",NUMBER,0);
   MAKEKEY(keywd[46],"STACKED",  NUMBER,0);
   MAKEKEY(keywd[47],"EJECT",    NUMBER,1);
   MAKEKEY(keywd[48],"PSTART",   NUMBER,1);
   MAKEKEY(keywd[49],"EJBY",     NUMBER,1);
   MAKEKEY(keywd[50],"HATCH",    NUMBER,0);
   MAKEKEY(keywd[51],"SHADE",    NUMBER,0);
   MAKEKEY(keywd[52],"HTHICK",   NUMBER,1);
   MAKEKEY(keywd[53],"HSPACE",   NUMBER,1);
   MAKEKEY(keywd[54],"CLIP",     NUMBER,0);
   MAKEKEY(keywd[55],"NOCLIP",   NUMBER,0);
   MAKEKEY(keywd[56],"SUBTICKS", NUMBER,2);
   MAKEKEY(keywd[57],"SUBON",    NUMBER,0);
   MAKEKEY(keywd[58],"SUBOFF",   NUMBER,0);
   MAKEKEY(keywd[59],"XTFONT",   STRING,2);
   MAKEKEY(keywd[60],"YTFONT",   STRING,2);
   MAKEKEY(keywd[61],"NOYLAB",   NUMBER,0);
   MAKEKEY(keywd[62],"CENTXLAB", NUMBER,0);
   MAKEKEY(keywd[63],"PRECISION",NUMBER,2);
   MAKEKEY(keywd[64],"BPLAB",    STRING,2);
   MAKEKEY(keywd[65],"KEYPOS",   NUMBER,2);
   MAKEKEY(keywd[66],"KEYFONT",  STRING,2);
   MAKEKEY(keywd[67],"KEYTEXT",  STRING,2);
   MAKEKEY(keywd[68],"TOL",      NUMBER,1);
   MAKEKEY(keywd[69],"REGWRITE", NUMBER,0);
   MAKEKEY(keywd[70],"REXXFIT",  STRING,2);
   MAKEKEY(keywd[71],"RTOL",     NUMBER,1);
   MAKEKEY(keywd[72],"REXXWRITE",NUMBER,0);
   MAKEKEY(keywd[73],"MACRO",    STRING,1);
   MAKEKEY(keywd[74],"REXXDEBUG",NUMBER,0);

   return(0);
}
