/****************************************************************************

   Program:    AMPlot2
   File:       smooth.c
   
   Version:    V1.0
   Date:       27.04.92
   Function:   Fourier smoothing routines
   
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

   V1.0  27.04.92
   Tidied up from AMPlot V1.0.
   
****************************************************************************/
#include <math.h>
#include <stdlib.h>
#ifdef FPU
#include <m68881.h>
#endif

#ifndef NULL
#define NULL 0L
#endif

#ifdef AMIGA
#include "mylib:macros.h"
#else
#include "macros.h"
#endif

#ifndef DOPROTOS
#include "smooth.p"
#endif

/*****************************************************************************/
/*>smooft(double *y, int n, int nmax, double pts)
   ----------------------------------------------
   Smooths an array y[] of data length n, with a window whose full width is 
   of order pts neighbouring points, a user supplied value. y[] must be of
   physical length:
      nmax = (power of 2) > (n + pts/2)
   and is modified.
   
   Returns:    int               0: OK
                                 1: Error

   27.04.92 Cleaned up from AMPlot V1.0.
*/
int smooft(double *y,
       int    n,
       int    nmax,
       double pts)
{
   int    m = 2,
          nmin,   mo2,
          j,      k;
   double con,
          y1,     yn,
          rn1,    fac;

   /* Minimum size including buffer against wrap around */
   nmin = n + 2.0 * pts;

   /* Find the next larger power of 2 */
   while(m<nmin) m *= 2;

   /* If array of wrong size, reallocate to larger size. */
   if(m > nmax)
   {
      if((y = (double *)realloc(y, m * sizeof(double))) == NULL)
         return(1);
   }

   /* Useful constants */
   con = (pts/m)*(pts/m);
   y1  = y[0];
   yn  = y[n-1];
   rn1 = 1.0/(n-1.0);

   /* Remove the linear trend */
   for(j=1; j<=n; j++)
      y[j-1] -= rn1*(y1*(n-j)+yn*(j-1));

   /* Zero pad */
   if(n+1 <= m)
      for(j=n+1; j<=m; j++) y[j-1]=0.0;

   mo2=m/2;

   /* Fourier transform */
   realft(y,mo2,1);
   y[0] /= mo2;

   /* Window function */
   fac = 1.0;

   /* Multiply the data by the window function */
   for(j=1; j<=mo2-1; j++)
   {
      k = 2*j+1;
      if(fac != 0.0)
      {
         fac     = MAX(0.0,(1.0-con*j*j)/mo2);
         y[k-1] *= fac;
         y[k]   *= fac;
      }
      else  /* Don't do unnecessary multiplies afer window function is zero */
      {
         y[k-1] = 0.0;
         y[k]   = 0.0;
      }
   }

   /* Last point */
   fac   = MAX(0.0,(1.0 - 0.25*pts*pts)/mo2);
   y[1] *= fac;

   /* Inverse fourier */
   realft(y,mo2,-1);

   /* Restore linear trend */
   for(j=1; j<=n; j++)
      y[j-1] = rn1*(y1*(n-j)+yn*(j-1))+y[j-1];

   return(0);
}

/*****************************************************************************/
/*>realft(double *data, int n, int isign)
   --------------------------------------
   Performs real fourier transform

   27.04.92 Cleaned up from AMPlot V1.0.
*/
int realft(double *data,
       int    n,
       int    isign)
{
   double wr,     wi,
          wpr,    wpi,
          wtemp,  theta,
          c1,     c2,
          h1r,    h1i,
          h2r,    h2i,
          wrs,    wis;
   int    n2p3,   i,
          i1,     i2,
          i3,     i4;

   /* Initialise the recurrence */
   theta = PI/(double)n;
   c1    = 0.5;

   if(isign==1)
   {
      c2 = -0.5;
      /* Forward transform */
      four1(data,n,1);
   }
   else
   {
      /* Set up for inverse transform */
      c2    = 0.5;
      theta = -theta;
   }

   wpr  = (double)-2.0 * sin(0.5*theta)*sin(0.5*theta);
   wpi  = sin(theta);
   wr   = 1.0 + wpr;
   wi   = wpi;
   n2p3 = 2*n + 3;

   /* Case i==1 done separately, below */
   for(i=2; i<=n/2+1; i++)
   {
      i1  = 2*i-1;
      i2  = i1+1;
      i3  = n2p3-i2;
      i4  = i3+1;
      wrs = wr;
      wis = wi;

      /* The 2 separate transforms are pulled out of z */
      h1r =  c1*(data[i1-1]+data[i3-1]);
      h1i =  c1*(data[i2-1]-data[i4-1]);
      h2r = -c2*(data[i2-1]+data[i4-1]);
      h2i =  c2*(data[i1-1]-data[i3-1]);

      /* and recombined to form the real transform */
      data[i1-1] =  h1r+wrs*h2r-wis*h2i;
      data[i2-1] =  h1i+wrs*h2i+wis*h2r;
      data[i3-1] =  h1r-wrs*h2r+wis*h2i;
      data[i4-1] = -h1i+wrs*h2i+wis*h2r;

      /* The recurrence */
      wtemp  = wr;
      wr    += (wr*wpr - wi*wpi);
      wi    += (wi*wpr + wtemp*wpi);
   }

   if(isign==1)
   {
      /* Squeeze the first and last data together */
      h1r     = data[0];
      data[0] = h1r+data[1];
      data[1] = h1r-data[1];
   }
   else
   {
      h1r     = data[0];
      data[0] = c1*(h1r+data[1]);
      data[1] = c1*(h1r-data[1]);
      /* Inverse transform */
      four1(data,n,-1);
   }
   return(0);
}

/*****************************************************************************/
/*>four1(double *data, int nn, int isign)
   --------------------------------------
   Performs fourier transform

   27.04.92 Cleaned up from AMPlot V1.0.
*/

int four1(double *data,
      int    nn,
      int    isign)
{
   double wr,    wi,
          wpr,   wpi,
          wtemp, theta,
          tempr, tempi;
   int    istep, mmax,
          m,     n,
          i,     j;
   
   n = 2*nn;
   j = 1;
   
   /* Bit reversal */
   for(i=1; i<=n; i+=2)
   {
      if(j>i)
      {
         /* Exchange two complex numbers */
         tempr     = data[j-1];
         tempi     = data[j];
         data[j-1] = data[i-1];
         data[j]   = data[i];
         data[i-1] = tempr;
         data[i]   = tempi;
      }
      
      m = n/2;
      while((m>=2)&&(j>m))
      {
         j -= m;
         m /= 2;
      }
      j += m;
   }
   
   /* Danielson-Lanczos routine;
      Outer loop executed log2(NN) times
   */
   mmax = 2;
   while(n>mmax)
   {
      istep = 2*mmax;

      /* Initialise for trig recurrence */
      theta =  2.0 * PI/(isign*mmax);
      wpr   = -2.0 * sin(0.5*theta) * sin(0.5*theta);
      wpi   =  sin(theta);
      wr    =  1.0;
      wi    =  0.0;

      /* 2 nested inner loops */
      for(m=1; m<=mmax; m+=2)
      {
         for(i=m; i<=n; i+=istep)
         {
            /* Danielson-Lanczos formula */
            j          = i+mmax;
            tempr      = wr*data[j-1] - wi*data[j];
            tempi      = wr*data[j]   + wi*data[j-1];
            data[j-1]  = data[i-1]    - tempr;
            data[j]    = data[i]      - tempi;
            data[i-1] += tempr;
            data[i]   += tempi;
         }

         /* Trig recurrence */
         wtemp  = wr;
         wr    += (wr*wpr - wi*wpi);
         wi    += (wi*wpr + wtemp*wpi);
      }
      mmax = istep;
   }
   return(0);
}
         
