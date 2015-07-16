/****************************************************************************

   Program:    AMPlot2
   File:       robust.c
   
   Version:    V1.0
   Date:       27.04.92
   Function:   Robust straight line fitting routines
   
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
   medfit(X,Y,ndata,a,b,abdev) double *X,*Y,*a,*b,*abdev; int ndata;
      Fits y = a+bx by the criterion of least absolute deviations.
      X and Y are input data of length ndata.
      Parimeters a and b are output with abdev, the mean absolute
      deviation (in y) of the input points from the fitted line.
      Communicates with rofunc() via global variables.
   
   double rofunc(b) double b;
      Evaluates the rhs of the equation:
      $$ 0=\sum_{i=1}^N x_i mbox{sgn}(y_i - a - bx_i) $$
      Communicates with medfit() via global variables.
   
   double sign(a,b) double a,b;
      Returns the absolute value of a with the sign of b.
   
   sort(n,arrin) int n; double *arrin;
      Sorts array arrin of length n by Heapsort.

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

#ifndef DOPROTOS
#include "robust.p"
#endif

/***************************************************************************/
/* Max number of iterations during bracketing before medfit() exits
*/
#define MAXITER 1000

/***************************************************************************/
/* Global variables for communication between medfit() and rofunc()
*/

static int    ndatat;
static double *xt,
              *yt,
              *arr,
              aa,
              abdevt;

/***************************************************************************/
/* Static prototypes
*/
static double sign(double a, double b);
static double rofunc(double b);

/***************************************************************************/
/*>sign(double a, double b)
   ------------------------
   Returns the absolute value of a with the sign of b.
   
   27.04.92 Tidied from AMPlot V1.0
*/
static double sign(double a,
                   double b)
{
   double retval;
   
   if(b<0.0) retval = -fabs(a);
   else      retval =  fabs(a);
   
   return(retval);
}

/***************************************************************************/
/*>sort(int n, double *arrin)
   --------------------------
   Sorts array arrin of length n by Heapsort.

   27.04.92 Tidied from AMPlot V1.0
*/
int sort(int    n,
     double *arrin)
{  
   int      l,    ir,
            i,    j;
   double   q;
   
   l  = n/2+1;
   ir = n;
   
   for(;;)
   {
      if(l>1)
      {
         q = arrin[--l - 1];
      }
      else
      {
         q = arrin[ir-1];
         arrin[ir-1] = arrin[0];
         if(--ir == 1)
         {
            arrin[0] = q;
            return(0);
         }
      }
      i = l;
      j = l+l;
      
      while(j<=ir)
      {
         if(j<ir)
         {
            if(arrin[j-1] < arrin[j]) j++;
         }
         if(q < arrin[j-1])
         {
            arrin[i-1] = arrin[j-1];
            i  = j;
            j += j;
         }
         else
         {
            j = ir+1;
         }
      }
      arrin[i-1] = q;
   }
   return(0);
}

/***************************************************************************/
/*>double rofunc(double b)
   -----------------------
   Evaluates the rhs of the equation:
   $$ 0=\sum_{i=1}^N x_i mbox{sgn}(y_i - a - bx_i) $$
   Communicates with medfit() via global variables.

   27.04.92 Tidied from AMPlot V1.0
*/
static double rofunc(double b)
{
   int    n1,
          nml,
          nmh,
          j;
   double sum,
          d;
   
   n1     = ndatat+1;
   nml    = n1/2;
   nmh    = n1-nml;
   for(j=1; j<=ndatat; j++)
      arr[j-1] = yt[j-1]-b*xt[j-1];
   sort(ndatat,arr);
   aa     = 0.5*(arr[nml-1]+arr[nmh-1]);
   sum    = 0.0;
   abdevt = 0.0;
   for(j=1; j<=ndatat; j++)
   {
      d      = yt[j-1]-(b*xt[j-1]+aa);
      abdevt = abdevt+abs(d);
      sum   += xt[j-1]*sign(1.0,d);
   }
   return(sum);
}

/***************************************************************************/
/*>medfit(double *X, double *Y, int ndata, 
          double *a, double *b, double *abdev)
   -------------------------------------------
   Fits y = a+bx by the criterion of least absolute deviations.
   X and Y are input data of length ndata
   Parameters a and b are output with abdev, the mean absolute
   deviation (in y) of the input points from the fitted line.
   
   Returns:    int            0: OK
                              1: Problem
                             -1: No memory

   27.04.92 Tidied from AMPlot V1.0
   28.04.92 Makes iter count check during bracketing
*/
int medfit(double *X,
       double *Y,
       int    ndata,
       double *a,
       double *b,
       double *abdev)
{
   double sx,    sy,
          sxy,   sxx,
          del,   bb,
          chisq, sigb,
          b1,    b2,
          f,
          f1,    f2;
   int    j,
          niter;
   
   /* Allocate space for communication arrays */
   if((xt = (double *)malloc(ndata * sizeof(double))) == NULL)
      return(-1);
   if((yt = (double *)malloc(ndata * sizeof(double))) == NULL)
   {
      free(xt);
      return(-1);
   }
   if((arr = (double *)malloc(ndata * sizeof(double))) == NULL)
   {
      free(xt);
      free(yt);
      return(-1);
   }
   
   sx  = 0.0;  sy  = 0.0;
   sxy = 0.0;  sxx = 0.0;

   for(j=0; j<ndata; j++)
   {
      xt[j] = X[j];
      yt[j] = Y[j];
      sx   += X[j];
      sy   += Y[j];
      sxy  += X[j] * Y[j];
      sxx  += X[j] * X[j];
   }
   ndatat = ndata;
   
   /* Least-squares solution */
   del = ndata*sxx  - sx*sx;
   aa  = (sxx*sy    - sx*sxy)/del;
   bb  = (ndata*sxy - sx*sy)/del;
   
   chisq=0.0;
   
   for(j=0; j<ndata; j++)
      chisq += ((Y[j]-(aa+bb*X[j]))*(Y[j]-(aa+bb*X[j])));
   sigb = sqrt(chisq/del);
   b1   = bb;
   f1   = rofunc(b1);
   b2   = bb+sign((3.0*sigb),f1);
   f2   = rofunc(b2);
   
   niter = 0;
   while(f1*f2 > 0.0)
   {
      bb = 2.0 * b2 - b1;
      b1 = b2;
      f1 = f2;
      b2 = bb;
      f2 = rofunc(b2);
      if(++niter > MAXITER) return(1);
   }
   sigb *= 0.01;
   while(abs(b2-b1) > sigb)
   {
      bb = 0.5*(b1+b2);
      if(bb==b1 || bb==b2) break;
      f = rofunc(bb);
      if(f*f1 >= 0.0)
      {
         f1 = f;
         b1 = bb;
      }
      else
      {
         f2 = f;
         b2 = bb;
      }
   }
   *a     = aa;
   *b     = bb;
   *abdev = abdevt/ndata;

   free(xt);
   free(yt);
   free(arr);
   
   return(0);
}
