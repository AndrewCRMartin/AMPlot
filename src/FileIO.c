/****************************************************************************

   Program:    AMPlot2
   File:       amplot.h
   
   Version:    V1.0
   Date:       26.03.92
   Function:   Routines to perform file IO
   
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

/***************************************************************************/
/* Defines and macros
*/

/***************************************************************************/
/*>int ReadData(FH *fh)
   -----------------------------------
   Reads the datafile and calls process_command as appropriate. Closes the
   file only if there was a problem causing the routine to Die().
   26.03.92 Original
   02.04.92 Added barlabels allocation
   03.07.92 Added call to UncacheBarLabels() and check on logs
   07.07.92 File handles set to NULL on close
   09.07.92 Correctly initialised iobuff.firstcall
   20.07.92 Added DEMO_VERSION conditional code
*/
int ReadData(FH *fh)
{
   IOBUFF   iobuff;                 /* Buffer for AmigaDOS i/o             */
   char     buffer[160];            /* Buffer for reading file             */

   DATASET  *current_data,          /* The current dataset                 */
            *d;

   int i = 0,                       /* Datapoint counter                   */
       newdata,                     /* Flag for new dataset                */
       gotdata,                     /* Flag to indicate we've got data     */
       ndata;                       /* Number of data points expected      */


   gotdata = FALSE;                 /* To start, we've got no data         */
   ndata   = MAXDATA;               /* Default max datapoints              */

   graph.nplots = 1;

   /* Allocate memory for the first dataset */
   if((current_data = data = AllocDataset(ndata,NULL))==NULL)
   {
#ifdef AMIGA
      Close((BPTR)fh);
#else
      fclose(fh);
#endif
      fh = NULL;
      Die("No memory for dataset");
   }
   
#ifdef REXX_SUPPORT
   /* Initialise current dataset for Rexx RVI to first dataset */
   rexx.current = data;
#endif
   
   /* Initialise the i/o buffer */
   iobuff.firstcall = TRUE;

   /* While there is data in the file */
   while(mygetsbuff(fh,buffer,sizeof(buffer),&iobuff))
   {
      TERMINATE(buffer);
      
      /* If it's not a blank line */
      if((*buffer) && (*buffer != '\n') && (*buffer != '\r'))
      {
         if((*buffer == '!') || isalpha(*buffer))
         {
            /* Treat it as a command */
            newdata = FALSE;
            process_command(buffer,current_data,&newdata,&ndata);
#ifdef DEMO_VERSION
            if(ndata > MAXDATA)
            {
               WriteMessage("Demo version allows only 10 datapoints");
               ndata = MAXDATA;
            }
#endif
            
            /* If we haven't got any data yet and the number of datapoints
               expected has changed, reset the arrays. This only applies
               to the first dataset. In other cases, the NDATA command
               must be specified *before* the NEWDATA command.
            */
            if(!gotdata && ndata!=MAXDATA)
            {
               if(AllocDataset(ndata,current_data)==NULL)
               {
#ifdef AMIGA
                  Close((BPTR)fh);
#else
                  fclose(fh);
#endif
                  fh = NULL;
                  Die("No memory for dataset");
               }
            }

            /* If it was a command to start a new dataset and we've already
               got some data, then record the number of datapoints and
               allocate space for the new dataset.
            */
            if(newdata && gotdata)
            {
               current_data->ndata  = i;
               i                    = 0;

               if((current_data->next = AllocDataset(ndata,NULL))==NULL)
               {
#ifdef AMIGA
                  Close((BPTR)fh);
#else
                  fclose(fh);
#endif
                  fh = NULL;
                  Die("No memory for dataset");
               }

               NEXT(current_data);
#ifdef DEBUG
               printf("Starting new dataset...\n");
#endif
            }
         }
         else
         {
            gotdata = TRUE;

            if(graph.columns == 1)        /* Pie chart                  */
            {
               if(sscanf(buffer,"%lf",&(current_data->x[i])) != 1) continue;
               /* Set up y values, so switching to bar, xy or scatter won't crash */
               current_data->y[i] = i-2*((int)(i/2)); /* Oscillating values of 0 and 1 */
               current_data->ep[i] = 0.0;
               current_data->em[i] = 0.0;
            }
            else if(graph.columns == 3)   /* +/- error bars equal       */
            {
               if(sscanf(buffer,"%lf%lf%lf",&(current_data->x[i]),
                                            &(current_data->y[i]),
                                            &(current_data->ep[i])) != 3) continue;
               current_data->em[i] = current_data->ep[i];
            }
            else if(graph.columns == 4)   /* +/- error bars different   */
            {
               if(sscanf(buffer,"%lf%lf%lf%lf",&(current_data->x[i]),
                                               &(current_data->y[i]),
                                               &(current_data->em[i]),
                                               &(current_data->ep[i])) != 4) continue;
            }
            else
            {
               if(sscanf(buffer,"%lf%lf",&(current_data->x[i]),
                                         &(current_data->y[i])) != 2) continue;
               current_data->ep[i] = 0.0;
               current_data->em[i] = 0.0;
            }

            if(i==0)
            {
               current_data->xmin = current_data->x[0];
               current_data->xmax = current_data->x[0];
               current_data->ymin = current_data->y[0];
               current_data->ymax = current_data->y[0];
            }
            else
            {
               if(current_data->x[i] < current_data->xmin)
               {
                  current_data->xmin = current_data->x[i];
               }
               if(current_data->x[i] > current_data->xmax)
               {
                  current_data->xmax = current_data->x[i];
               }
               if(current_data->y[i] < current_data->ymin)
               {
                  current_data->ymin = current_data->y[i];
               }
               if(current_data->y[i] > current_data->ymax)
               {
                  current_data->ymax = current_data->y[i];
               }
            }

            i++;
            if(i > ndata)
            {

#ifdef AMIGA
               Close((BPTR)fh);
               ClearWindow(MainWind);
#else
               fclose(fh);
#endif
               fh = NULL;

#ifdef DEMO_VERSION
               ReqMessage2(MainWind,"Data array sizes exceeded!",
               "Demo version limited to 10 datapoints.",0);
#else
               ReqMessage(MainWind,"Data array sizes exceeded!",0);
#endif
               return(FALSE);
            }
         }  /* If a datapoint */
      }     /* If not an empty line */
   }        /* While data */

   current_data->ndata  = i;
   
   /* Check logs OK if sepcified */
   for(d=data;d;NEXT(d))
   {
      for(i=0;i<d->ndata;i++)
      {
         if(d->x[i] <= 0.0)
         {
            graph.logx = FALSE;
#ifdef AMIGA
            Uncheck(MenuLogX);
#endif
         }

         if(d->y[i] <= 0.0)
         {
            graph.logy = FALSE;
#ifdef AMIGA
            Uncheck(MenuLogY);
#endif
         }
      }
   }

   /* Allocate space for bar labels */
   if(!AllocBarLabels())
   {
      ReqMessage2(MainWind,"Warning:","No memory for barchart labels.",0);
   }
   
   /* If any bar labels have been cached, place them into the array        */
   UncacheBarLabels();
   
   /* If any text for the key has been cached, place it into the array     */
   UncacheKeytext();
   
   /* Go through any fills and set the datasets                            */
   SetFillDatasets();
   
   return(TRUE);
}

/***************************************************************************/
/*>int AllocBarLabels(void)
   ------------------------
   Allocates memory for barchart labels and clears the labels. Returns
   TRUE if succesful, FALSE otherwise with barlabels set to NULL. Note that
   failure is not fatal (one just can't use bar labels...).
   07.04.92 Added FREELIST on data if there wasn't any...
   16.04.92 Added free of other stuff in data list
   11.06.92 If number of bars is greater than MAXBARLAB, don't bother with
            the allocation since we won't be able to display all that text.
*/
int AllocBarLabels(void)
{
   DATASET *d;
   int      i;
   
   /* Find the max number of datapoints in any dataset */
   barstyle.nbar = data->ndata;
   for(d=data;d;NEXT(d))
      if(d->ndata > barstyle.nbar) barstyle.nbar = d->ndata;
   
   /* Allocate this many SIMPLETEXT's for storing bar labels */
   if(barlabels) free(barlabels);
   barlabels = NULL;
   
   if(barstyle.nbar < MAXBARLAB)
   {
      if(barstyle.nbar > 0)
      {
         if((barlabels = (SIMPLETEXT *)
                         malloc(barstyle.nbar * sizeof(SIMPLETEXT)))==NULL)
            return(FALSE);
      }
      else
      {
         ReqMessage(MainWind,"No data!",0);
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
      }
      
      /* Now blank the SIMPLETEXT's */
      for(i=0;i<barstyle.nbar;i++)
      {
         barlabels[i].x       = 0.0;
         barlabels[i].y       = 0.0;
         barlabels[i].text[0] = '\0';
      }
   }

   return(TRUE);   
}

/***************************************************************************/
/*>UncacheBarLabels(void)
   ----------------------
   Get any cached barlabels out of the cache and free the cache memory.
   03.07.92 Original
   07.07.92 Corrected calc of CurrentBar
*/
int UncacheBarLabels(void)
{
   int j;
   
   if(barlabels != NULL && LabCache != NULL && LabCacheSize > 0)
   {
      /* Copy the strings from the cache */
      for(j=0; j<LabCacheSize && j<barstyle.nbar; j++)
         strcpy(barlabels[j].text,LabCache[j]);
      
      /* Free the cache strings  */
      for(j=0; j<LabCacheSize; j++)
         if(LabCache[j]) free(LabCache[j]);

      /* Set current bar to last one */
      barstyle.CurrentBar = MIN(LabCacheSize-1,barstyle.nbar-1);
      
      /* Free the cache array    */
      free(LabCache);

      /* Zero the variables      */
      LabCache     = NULL;
      LabCacheSize = 0;
      
      /* Switch on barlabels and switch off ordinary labels */
      barstyle.uselabels   = TRUE;
      graph.noxlabels      = TRUE;
   }
   return(0);
}

/***************************************************************************/
/*>UncacheKeytext(void)
   --------------------
   Get any cached key text out of the cache and free the cache memory.
   03.07.92 Original
*/
int UncacheKeytext(void)
{
   int      j;
   DATASET  *d;
   
   if(data != NULL && KeyCache != NULL && KeyCacheSize > 0)
   {
      /* Copy the strings from the cache */
      for(j=0, d=data; j<KeyCacheSize && d; j++, NEXT(d))
         strcpy(d->key.text,KeyCache[j]);
      
      /* Free the cache strings  */
      for(j=0; j<KeyCacheSize; j++)
         if(KeyCache[j]) free(KeyCache[j]);

      /* Free the cache array    */
      free(KeyCache);

      /* Zero the variables      */
      KeyCache     = NULL;
      KeyCacheSize = 0;
      
      /* Set the flag to use the key */
      graph.usekey = TRUE;
   }
   return(0);
}

/***************************************************************************/
/*>SetFillDatasets(void)
   ---------------------
   Goes through any fills which have been specified in the datafile and
   puts in the dataset information.
   03.07.92 Original
*/
int SetFillDatasets(void)
{
   FILLITEM *f;

   if(graph.style != STYLE_PIE)
      for(f=filllist; f; NEXT(f))
         f->dataset = FindBarDataset(f->x, f->y);

   return(0);
}


/***************************************************************************/
/*>int ClearPlots(int force)
   -------------------------
   Clears all current plot information, freeing datasets. Resets defaults.
   If not called with force TRUE, asks whether the user wants to proceed.
   Returns TRUE if we proceeded, FALSE otherwise.
   26.03.92 Original
   01.04.92 Added SetDefaults() call. Data set to NULL once freed.
   03.04.92 Added clear of the labels list
   07.04.92 Corrected bug in use of FREELIST
   01.07.92 Changed call to SetDefaults() to use display.FromCLI
   09.07.92 Check on data and labels before FREELIST()
*/
int ClearPlots(int force)     /* Input flag for prompt */
{
   DATASET     *p;

   if(!force)
   {
      if(!ReqMessage(MainWind,"Overwrite current plots?",1))
         return(FALSE);
   }
   
   graph.logx = FALSE;
   graph.logy = FALSE;

#ifdef AMIGA
   Uncheck(MenuLogX);
   Uncheck(MenuLogY);
#endif

   /* Free allocated memory for data */
   for(p=data; p!=NULL; NEXT(p))    /* Datasets    */
   {
      if(p->x)    free(p->x);
      if(p->y)    free(p->y);
      if(p->ep)   free(p->ep);
      if(p->em)   free(p->em);
   }
   if(data) FREELIST(data,DATASET);
   data = NULL;
   
   /* Free allocated memory for labels */
   if(labels) FREELIST(labels,TEXTLIST);
   labels             = NULL;
   graph.CurrentLabel = 0;
   graph.nlabels      = 0;
   
   SetDefaults(display.FromCLI);
   
   return(TRUE);
}

/***************************************************************************/
/*>DATASET *AllocDataset(int ndata, DATASET *reset)
   ------------------------------------------------
   Allocates memory for a dataset.
   ndata: Size of data arrays to allocate
   reset: If NULL, creates new dataset, otherwise change array sizes

   26.03.92 Original
   15.05.92 Corrected linestyle to default to 0
   03.07.92 Key position set from Defaults rather than just set to zero.
            Added data->maxdata.
*/
DATASET *AllocDataset(int     ndata,
                      DATASET *reset)
{
   DATASET *ptr;
   
   if(reset != NULL)    /* Reset the data array sizes       */
   {
      ptr = reset;

      if((ptr->x  = (double *)realloc(ptr->x,  ndata*sizeof(double)))==NULL)
         return(NULL);
      if((ptr->y  = (double *)realloc(ptr->y,  ndata*sizeof(double)))==NULL)
         return(NULL);
      if((ptr->ep = (double *)realloc(ptr->ep, ndata*sizeof(double)))==NULL)
         return(NULL);
      if((ptr->em = (double *)realloc(ptr->em, ndata*sizeof(double)))==NULL)
         return(NULL);
      ptr->maxdata = ndata;
   }
   else                 /* Initialise all data arrays, etc. */
   {
      if((ptr = (DATASET *)malloc(sizeof(DATASET)))==NULL)
         return(NULL);
      
      if((ptr->x  = (double *)malloc(ndata*sizeof(double)))==NULL)
         return(NULL);
      if((ptr->y  = (double *)malloc(ndata*sizeof(double)))==NULL)
         return(NULL);
      if((ptr->ep = (double *)malloc(ndata*sizeof(double)))==NULL)
         return(NULL);
      if((ptr->em = (double *)malloc(ndata*sizeof(double)))==NULL)
         return(NULL);
   
      ptr->next            = NULL;
      ptr->xmin            = 0.0;
      ptr->xmax            = 0.0;
      ptr->ymin            = 0.0;
      ptr->ymax            = 0.0;
      ptr->pen             = 1+((graph.nplots-1)%MAXPEN);
      ptr->linestyle       = 0;
      ptr->ndata           = 0;
      
      ptr->key.PSSize      = Defaults.KeyPSSize;
      ptr->key.AmigaSize   = Defaults.KeyAmigaSize;
      ptr->key.x           = Defaults.KeyX;
      ptr->key.y           = Defaults.KeyY;
      ptr->key.text[0]     = '\0';
      strcpy(ptr->key.PSFont,   Defaults.KeyPSFont);
      strcpy(ptr->key.AmigaFont,Defaults.KeyAmigaFont);
      ptr->maxdata = ndata;
   }

   return(ptr);
}

/***************************************************************************/
/*>SaveGraph(FH *fh)
   --------------------------------
   Save the current graph to a file.
   26.03.92 Framework
   10.07.92 Original
   17.07.92 Corrected writing of KEYTEXT (was just writing first keyword)
   04.08.92 Corrected pie slice eject (should be numbered from 1)
*/
int SaveGraph(FH *fh)
{
   int      i;
   char     buffer[MAXBUFF];
   TEXTLIST *l;
   DATASET  *d;
   FILLITEM *f;

/***                                                                     ***
 ***  Keywords applicable to all graph styles                            ***
 ***                                                                     ***/
 
/* Graph style */
   switch(graph.style)
   {
   case STYLE_LINE:
      myputs(fh,"XY\n");
      break;
   case STYLE_SCATTER:
      myputs(fh,"SCATTER\n");
      break;
   case STYLE_BAR:
      myputs(fh,"BAR\n");
      break;
   case STYLE_PIE:
      myputs(fh,"PIE\n");
      break;
   }
   
/* X-axis title font */
   if(XTitle.text[0] != '\0')
   {
      sprintf(buffer,"XTFONT \"%s\" %f\n",XTitle.PSFont,XTitle.PSSize);
      myputs(fh,buffer);
   }

/* Y-axis title font */
   if(YTitle.text[0] != '\0')
   {
      sprintf(buffer,"YTFONT \"%s\" %f\n",YTitle.PSFont,YTitle.PSSize);
      myputs(fh,buffer);
   }
   
/* X-Label font */
   sprintf(buffer,"XLFONT \"%s\" %f\n",XLabel.PSFont,XLabel.PSSize);
   myputs(fh, buffer);

/* Y-Label font */
   sprintf(buffer,"YLFONT \"%s\" %f\n",YLabel.PSFont,YLabel.PSSize);
   myputs(fh, buffer);

/* Graph title and font */
   if(GTitle.text[0] != '\0')
   {
      sprintf(buffer,"TITLE \"%s\" %g %g\n",GTitle.text,GTitle.x,GTitle.y);
      myputs(fh,buffer);

      sprintf(buffer,"TFONT \"%s\" %f\n",GTitle.PSFont,GTitle.PSSize);
      myputs(fh,buffer);
   }
   
/* Extra labels */
   for(l=labels, i=1; l!=NULL; NEXT(l), i++)
   {
      if(l->stext.text[0] != '\0')
      {
         sprintf(buffer,"LABEL %d \"%s\" %g %g \"%s\" %f\n", 
                 i, l->stext.text, l->stext.x, l->stext.y, 
                 l->stext.PSFont, l->stext.PSSize);
         myputs(fh, buffer);
      }
   }

/* PEN types */
   for(i=0;i<MAXPEN;i++)
   {
      sprintf(buffer,"SETPEN %d %d %f\n", 
              i+1, pens[i].colour, pens[i].thickness);
      myputs(fh, buffer);
   }
   
/* Boxed */
   if(graph.boxed)      myputs(fh,"BOXED\n");
   
/* Graph boundaries */
   if(graph.userbounds)
   {
      sprintf(buffer,"BOUNDS %g %g %g %g\n", graph.xmin, graph.xmax,
                                             graph.ymin, graph.ymax);
      myputs(fh,buffer);
   }
   
/* Paper size */
   sprintf(buffer,"SIZE %f %f\n",output.xsize, output.ysize);
   myputs(fh, buffer);
   
/* Paper offset */
   sprintf(buffer,"OFFSET %f %f\n",output.xoffset, output.yoffset);
   myputs(fh, buffer);
   
/* EPSF flag */
   if(EPSF) myputs(fh,"EPSF\n");
   
#ifdef AMIGA
/* Palette */
   for(i=0; i<4; i++)
   {
      int   R, G, B;
      UWORD rgb;
      
      rgb = GetRGB4((&MyScreen->ViewPort)->ColorMap,i);
   
      /* Get RGB and write them to the file */
      R   = (rgb >> 8) & 0xF;
      G   = (rgb >> 4) & 0xF;
      B   = (rgb >> 0) & 0xF;
      sprintf(buffer,"PALETTE %3d %3d %3d %3d\n",i,R,G,B);
      myputs(fh,buffer);
   }
#endif
   
#ifdef REXX_SUPPORT
/* ARexx macro */   
   if(rexx.RexxMacro[0] != '\0')
   {
      sprintf(buffer,"MACRO \"%s\"\n",rexx.RexxMacro);
      myputs(fh,buffer);
   }
   
/* ARexx debugging */
   if(rexx.debug) myputs(fh,"REXXDEBUG\n");
#endif

/***                                                                     ***
 ***  Keywords applicable to line and scatter graphs                     ***
 ***                                                                     ***/

   if(graph.style == STYLE_LINE || graph.style == STYLE_SCATTER)
   {
/* Datapoint mark styles */
      for(i=0; i<MAXPEN; i++)
      {
         sprintf(buffer,"DATAPOINT %d %d %d\n",
                 i+1,pens[i].mark,pens[i].join);
         myputs(fh, buffer);
      }
   }

/***                                                                     ***
 ***  Keywords applicable to barcharts only                              ***
 ***                                                                     ***/
   if(graph.style == STYLE_BAR)
   {
/* Bar shrink */
      if(barstyle.shrink < 0.00001)
         sprintf(buffer,"SHRINK 0.0\n");
      else
         sprintf(buffer,"SHRINK %f\n",200.0/barstyle.shrink);
      myputs(fh,buffer);
   
/* Shrink first bar */
      if(barstyle.shrinkall)  myputs(fh,"SHRFIRST\n");
   
/* Bar style */
      switch(barstyle.style)
      {
      case STYLE_OVERLAYED:
         myputs(fh,"OVERLAYED\n");
         break;
      case STYLE_OUTLINE:
         myputs(fh,"OUTLINE\n");
         break;
      case STYLE_STACKED:
         myputs(fh,"STACKED\n");
         break;
      case STYLE_GROUPED:
         myputs(fh,"GROUPED\n");
         break;
      }

/* No Fill all flag */
      if(!barstyle.fillall) myputs(fh,"NOFILLALL\n");
   }


/***                                                                     ***
 ***  Keywords applicable to piecharts only                              ***
 ***                                                                     ***/
   if(graph.style == STYLE_PIE)
   {
/* Pie start angle */
      sprintf(buffer,"PSTART %f\n",piestyle.start*180.0/PI);
      myputs(fh,buffer);
   
/* Pie eject % */
      sprintf(buffer,"EJBY %f\n",piestyle.eject);
      myputs(fh,buffer);
   }


/***                                                                     ***
 ***  Keywords applicable to piecharts and barcharts                     ***
 ***                                                                     ***/
   if(graph.style == STYLE_PIE || graph.style == STYLE_BAR)
   {
/* Hatching or shading */
      if(output.hatching)
      {
         myputs(fh,"HATCH\n");
         sprintf(buffer,"HSPACE %f\n",output.hatchspace);
         myputs(fh,buffer);
         sprintf(buffer,"HTHICK %f\n",output.hatchthick);
         myputs(fh,buffer);
      }
      else
      {
         myputs(fh,"SHADE\n");
      }
   }

/***                                                                     ***
 ***  Keywords applicable to everything but piecharts                    ***
 ***                                                                     ***/
   if(graph.style != STYLE_PIE)
   {
/* Error switch */
      if(graph.errors)  myputs(fh,"ERRORS\n");
      else              myputs(fh,"NOERRORS\n");
   
/* No x-labels */
      if(graph.noxlabels)  myputs(fh,"NOXLAB\n");
   
/* No y-labels */
      if(graph.noylabels)  myputs(fh,"NOYLAB\n");
   
/* Centre x-labels */
      if(graph.centrexlabels)  myputs(fh,"CENTXLAB\n");
   
/* Log axes */
      if(graph.logx)       myputs(fh,"LOGX\n");
      if(graph.logy)       myputs(fh,"LOGY\n");
   
/* Tick spacing */
      if(graph.userticks)
      {
         sprintf(buffer,"TICKS %g %g\n", graph.xtick, graph.ytick);
         myputs(fh,buffer);
      }

/* Subticks */
      if(graph.subtick && graph.usersub)
      {
         sprintf(buffer,"SUBTICKS %g %g\n",graph.xsub,graph.ysub);
         myputs(fh,buffer);
      }
      if(graph.subtick) myputs(fh,"SUBON\n");
      else              myputs(fh,"SUBOFF\n");
   
/* X-axis label */
      if(XTitle.text[0] != '\0')
      {
         sprintf(buffer,"XLABEL \"%s\"\n",XTitle.text);
         myputs(fh,buffer);
      }

/* Y-axis label */
      if(YTitle.text[0] != '\0')
      {
         sprintf(buffer,"YLABEL \"%s\"\n",YTitle.text);
         myputs(fh,buffer);
      }
   
/* Grid flag */
      if(graph.grid)    myputs(fh,"GRID\n");
   
/* Follow zero flag */
      if(graph.fzero)   myputs(fh,"FZERO\n");
   
/* Regression */
      if(fitting.regression)
      {
         sprintf(buffer,"REGRESS %d\n",fitting.regress_npoly);
         myputs(fh, buffer);
         sprintf(buffer,"TOL %g\n",fitting.regress_tol);
         myputs(fh, buffer);
         if(fitting.regress_write) myputs(fh,"REGWRITE\n");
      }
   
/* Rexx fitting */
      if(fitting.rexx)
      {
         sprintf(buffer,"REXXFIT %d \"%s\"\n",fitting.rexx_npoly,
                                              fitting.rexxmacro);
         myputs(fh, buffer);
         sprintf(buffer,"RTOL %g\n",fitting.rexx_tol);
         myputs(fh, buffer);
         if(fitting.regress_write) myputs(fh,"REXXWRITE\n");
      }
   
/* Robust fitting */
      if(fitting.robust) myputs(fh, "ROBUST\n");
   
/* Fourier smoothing */
      if(fitting.fourier)
      {
         sprintf(buffer,"FOURIER %f\n",fitting.smooth_pts);
         myputs(fh, buffer);
      }
   
/* Error bar width */
      sprintf(buffer,"EWIDTH %f\n",graph.ebarwidth);
      myputs(fh,buffer);
   
/* Datapoint mark size */
      sprintf(buffer,"MARKSIZE %f\n",graph.dpsize);
      myputs(fh,buffer);
   
/* Clipping */
      if(graph.clip)    myputs(fh,"CLIP\n");
      else              myputs(fh,"NOCLIP\n");
   
/* Precision of axis labels */
      if(graph.userprec)
      {
         sprintf(buffer,"PRECISION %d %d\n",
                 graph.xprecision,graph.yprecision);
         myputs(fh,buffer);
      }
   }


/***                                                                     ***
 ***  Now the actual data and its keywords                               ***
 ***                                                                     ***/
   sprintf(buffer,"NDATA %d\n",graph.maxdata+2);
   myputs(fh, buffer);
   sprintf(buffer,"COLUMNS %d\n",graph.columns);
   myputs(fh, buffer);
   
   for(d=data; d; NEXT(d))
   {
      if(d != data) myputs(fh,"NEWSET\n");
      sprintf(buffer,"PEN %d\n",d->pen);
      myputs(fh, buffer);
      sprintf(buffer,"STYLE %d\n",d->linestyle+1);
      myputs(fh, buffer);
      for(i=0; i<d->ndata; i++)
      {
         switch(graph.columns)
         {
         case 1:
            sprintf(buffer,"%g\n",d->x[i]);
            break;
         case 3:
            sprintf(buffer,"%g %g %g\n",d->x[i], d->y[i], d->ep[i]);
            break;
         case 4:
            sprintf(buffer,"%g %g %g %g\n",
                    d->x[i], d->y[i], d->ep[i], d->em[i]);
            break;
         default:
            sprintf(buffer,"%g %g\n",d->x[i], d->y[i]);
            break;
         }
         myputs(fh, buffer);
      }
         
   }
   
/***                                                                     ***
 ***  Post data keywords for piecharts only                              ***
 ***                                                                     ***/
   if(graph.style == STYLE_PIE)
   {
/* Fills */
      if(filllist)
      {
         for(f=filllist; f; NEXT(f))
         {
            sprintf(buffer,"PFILL %d %f\n",f->pattern,f->angle);
            myputs(fh, buffer);
         }
      }
/* Slice ejections */
      for(i=0; i<data->ndata; i++)
      {
         if(data->ep[i] != 0.0)
         {
            sprintf(buffer,"EJECT %d\n",i+1);
            myputs(fh, buffer);
         }
      }
   }

/***                                                                     ***
 ***  Post data keywords for barcharts only                              ***
 ***                                                                     ***/
   if(graph.style == STYLE_BAR)
   {
      if(filllist)
      {
         for(f=filllist; f; NEXT(f))
         {
            sprintf(buffer,"FILL %d %g %g\n",f->pattern,f->x,f->y);
            myputs(fh, buffer);
         }
      }
   }

/***                                                                     ***
 ***  Post data keywords for the key                                     ***
 ***                                                                     ***/
   for(d=data; d; NEXT(d))
   {
      /* Do the font and position only if there is some text */
      if(d->key.text[0] != '\0')
      {
         sprintf(buffer,"KEYFONT \"%s\" %f\n",data->key.PSFont,
                                              data->key.PSSize);
         myputs(fh,buffer);
         sprintf(buffer,"KEYPOS %g %g\n",data->key.x,data->key.y);
         myputs(fh,buffer);
         break;
      }
   }
   /* Do the actual text items */
   for(d=data,i=1; d; NEXT(d),i++)
   {
      if(d->key.text[0] != '\0')
      {
         sprintf(buffer,"KEYTEXT %d \"%s\"\n",i,d->key.text);
         myputs(fh,buffer);
      }
   }
   
/***                                                                     ***
 ***  Post data keywords for bar and pie labels                          ***
 ***                                                                     ***/
   if(barlabels != NULL && barstyle.uselabels)
   {
      for(i=0; i<barstyle.nbar; i++)
      {
         if(barlabels[i].text[0] != '\0')
         {
            sprintf(buffer,"BPLAB %d \"%s\"\n",i+1,barlabels[i].text);
            myputs(fh,buffer);
         }
      }
   }
   
   return(0);
}

