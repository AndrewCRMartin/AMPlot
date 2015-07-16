/****************************************************************************

   Program:    AMPlot2
   File:       SVDFit.c
   
   Version:    V1.2
   Date:       14.04.92
   Function:   SVD Fitting routines
   
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

   V1.0  14.04.92
   Tidied up from AMPlot V1.0.
   
   V1.1  27.04.92
   Various fixes for dynamic memory allocation for big arrays.
   
   V1.2  01.05.92
   Added routines to calculate variance and covariance matrices

****************************************************************************/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef FPU
#include <m68881.h>
#endif

#ifdef AMIGA
#include "mylib:macros.h"
#else
#include "macros.h"
#endif

#ifndef NULL
#define NULL 0L
#endif

#ifndef DOPROTOS
#include "svdfit.p"
#endif

/***************************************************************************/
/* Static prototypes
*/
static int svdcmp(double *a[], int m, int n, int mp, int np, double *w, 
                  double *v[]);
static int svbksb(double *u[], double *w, double *v[], int m, int n, int mp,
                  int np, double *b, double *x);
static double sign(double a, double b);

/***************************************************************************/
/*>funcs(double x, double *p, int np)
   ----------------------------------
   This is the function being fitted.

   14.04.92 Cleaned up from AMPlot V1.0.
*/
void funcs(double x,
           double *p,
           int    np)
{
   int j;
   
   p[0]=1.0;
   for(j=1;j<np;j++)
      p[j]=p[j-1]*x;
}

/***************************************************************************/
/*>expand(double *p, double *a, int n, double *z)
   ----------------------------------------------
   Expand a function

   14.04.92 Cleaned up from AMPlot V1.0.
   30.04.92 Changed to return value rather than output it
*/
double expand(double *p, 
              double *a, 
              int    n)
{
   int      j;
   double   z;
   
   z=0;
   for(j=0;j<n;j++)
      z += p[j] * a[j];

   return(z);
}

/***************************************************************************/
/*>svdfit(double *x, double *y, double *sig, int    ndata, double tol,
          double *a, int    ma, double **u,  double **v,   double *w,
          int    mp, int    np, double *chisq,
          void (*Func)(double xf, double *pf, int nf))
   -------------------------------------------------------------------
   Returns:    int                   0: OK
                                     1: Error
                                    -1: Memory error

   Perform SVD linear regression fitting
   
   14.04.92 Cleaned up from AMPlot V1.0. Now takes tol as a parameter
   27.04.92 Local arrays allocated dynamically
   30.04.92 Added function parameter
*/
int svdfit(double *x,             /* x-data array                              */
       double *y,             /* y-data array                              */
       double *sig,           /* standard deviation array                  */
       int    ndata,          /* number of data points                     */
       double tol,            /* fitting tolerence                         */
       double *a,             /* Returned coefficients                     */
       int    ma,             /* Number of polynomials                     */
       double *u[],           /* U-maxtrix                                 */
       double *v[],           /* V-matrix                                  */
       double *w,             /* W-vector                                  */
       int    mp,             /* Max number of datapoints (array size)     */
       int    np,             /* Max number of polynomials (array size)    */
       double *chisq,         /* Returned value of chi^2                   */
       void   (*Func)(double xf, double *pf, int nf))  /* Function to fit  */
{
   double            *svd_b,
                     *svd_afunc,
                     tmp,
                     wmax,
                     thresh,
                     sum;
   int               i,j,retval;
   
   /* Allocate space for svd_b and svd_afunc */
   if((svd_b     = (double *)malloc(mp * sizeof(double))) == NULL)
      return(-1);
   if((svd_afunc = (double *)malloc(np * sizeof(double))) == NULL)
   {
      free(svd_b);
      return(-1);
   }
   
   for(i=0;i<mp;i++) for(j=0;j<np;j++) u[i][j]=0.0;

   for(i=1;i<=ndata;i++)
   {
      Func(x[i-1],svd_afunc,ma);
      tmp=1.0/sig[i-1];
      for(j=1;j<=ma;j++)
         u[i-1][j-1]=svd_afunc[j-1]*tmp;
      svd_b[i-1]=y[i-1]*tmp;
   }
   
   /* Returns 1 if there was an error, -1 if no memory */
   retval = svdcmp(u,ndata,ma,mp,np,w,v);
   if(retval)
   {
      free(svd_b);
      free(svd_afunc);
      return(retval);
   }

   wmax=0.0;
   for(i=1;i<=ma;i++)
      if(w[i-1]>wmax) wmax = w[i-1];
   thresh=tol*wmax;

   for(i=1;i<=ma;i++)
      if(w[i-1]<thresh) w[i-1] = 0.0;
   if(svbksb(u,w,v,ndata,ma,mp,np,svd_b,a) == -1)
   {
      free(svd_b);
      free(svd_afunc);
      return(-1);
   }

   *chisq=0.0;
   for(i=1;i<=ndata;i++)
   {
      Func(x[i-1],svd_afunc,ma);
      sum=0.0;
      for(j=1;j<=ma;j++)
         sum += (a[j-1]*svd_afunc[j-1]);
      *chisq += (((y[i-1]-sum)/sig[i-1])*((y[i-1]-sum)/sig[i-1]));
   }

   free(svd_b);
   free(svd_afunc);
   return(0);
}

/***************************************************************************/
/*>svdcmp(double **a, int m, int n, int mp, int np, double *w, double **v)
   -----------------------------------------------------------------------
   Singular value decomposition.

   14.04.92 Cleaned up from AMPlot V1.0.
   27.04.92 Local array allocated dynamically
*/
static int svdcmp(double *a[],
                  int    m,
                  int    n,
                  int    mp,
                  int    np,
                  double *w,
                  double *v[])
{
   double            *svd_rv1,g,scale,anorm,s,f,h,c,x,y,z;
   int               i,j,l,k,its,nm,jj;
   
   /* Return error condition */
   if(m<n) return(1);

   if((svd_rv1 = (double *)malloc(np * sizeof(double))) == NULL)
      return(-1);

   g     = 0.0;
   scale = 0.0;
   anorm = 0.0;

   for(i=1;i<=n;i++)
   {
      l              = i+1;
      svd_rv1[i-1]   = scale * g;
      g              = 0.0;
      s              = 0.0;
      scale          = 0.0;
      
      if(i <= m)
      {
         for(k=i; k<=m; k++)
            scale+=abs(a[k-1][i-1]);

         if(scale!=0.0)
         {
            for(k=i;k<=m;k++)
            {
               a[k-1][i-1] /= scale;
               s += (a[k-1][i-1] * a[k-1][i-1]);
            }
            f=a[i-1][i-1];
            g= -sign(sqrt(s),f);
            h=f * g - s;
            a[i-1][i-1]=f-g;
            if(i!=n)
            {
               for(j=l;j<=n;j++)
               {
                  s=0.0;
                  for(k=i;k<=m;k++)
                     s += (a[k-1][i-1] * a[k-1][j-1]);
                  f=s/h;
                  for(k=i;k<=m;k++)
                     a[k-1][j-1] += (f * a[k-1][i-1]);
               }
            }
            for(k=i;k<=m;k++)
               a[k-1][i-1] *= scale;
         }
      }

      w[i-1]   = scale * g;
      g        = 0.0;
      s        = 0.0;
      scale    = 0.0;
      
      if((i<=m)&&(i!=n))
      {
         for(k=l;k<=n;k++)
            scale += abs(a[i-1][k-1]);

         if(scale != 0.0)
         {
            for(k=l;k<=n;k++)
            {
               a[i-1][k-1] /= scale;
               s += (a[i-1][k-1] * a[i-1][k-1]);
            }

            f           =  a[i-1][l-1];
            g           = -sign(sqrt(s),f);
            h           =  f * g - s;
            a[i-1][l-1] =  f-g;

            for(k=l;k<=n;k++)
               svd_rv1[k-1] = a[i-1][k-1]/h;

            if(i!=m)
            {
               for(j=l;j<=m;j++)
               {
                  s=0.0;
                  for(k=l;k<=n;k++)
                     s += (a[j-1][k-1] * a[i-1][k-1]);
                  for(k=l;k<=n;k++)
                     a[j-1][k-1] += (s * svd_rv1[k-1]);
               }
            }
            for(k=l;k<=n;k++)
               a[i-1][k-1] *= scale;
         }
      }
      anorm = MAX(anorm,(abs(w[i-1])+abs(svd_rv1[i-1])));
   }

   for(i=n;i>=1;i--)
   {
      if(i<n)
      {
         if(g!=0.0)
         {
            for(j=l;j<=n;j++)
               v[j-1][i-1]=(a[i-1][j-1]/a[i-1][l-1])/g;
            for(j=l;j<=n;j++)
            {
               s=0.0;
               for(k=l;k<=n;k++)
                  s += (a[i-1][k-1] * v[k-1][j-1]);
               for(k=l;k<=n;k++)
                  v[k-1][j-1] += (s * v[k-1][i-1]);
            }
         }
         for(j=l;j<=n;j++)
         {
            v[i-1][j-1]=0.0;
            v[j-1][i-1]=0.0;
         }
      }
      v[i-1][i-1]=1.0;
      g=svd_rv1[i-1];
      l=i;
   }

   for(i=n;i>=1;i--)
   {
      l=i+1;
      g=w[i-1];
      if(i<n)
         for(j=l;j<=n;j++)
            a[i-1][j-1]=0.0;
      if(g!=0.0)
      {
         g=1.0/g;
         if(i!=n)
         {
            for(j=l;j<=n;j++)
            {
               s=0.0;
               for(k=l;k<=m;k++)
                  s += (a[k-1][i-1] * a[k-1][j-1]);
               f=(s/a[i-1][i-1]) * g;
               for(k=i;k<=m;k++)
                  a[k-1][j-1] += (f * a[k-1][i-1]);
            }
         }
         for(j=i;j<=m;j++)
            a[j-1][i-1] *= g;
      }
      else
      {
         for(j=i;j<=m;j++)
            a[j-1][i-1]=0.0;
      }
      (a[i-1][i-1])++;
   }
   for(k=n;k>=1;k--)
   {
      for(its=0;its<30;its++)
      {
         for(l=k;l>=1;l--)
         {
            nm=l-1;
            if((abs(svd_rv1[l-1])+anorm)==anorm)
               goto jump2;
            if((abs(w[nm-1])+anorm)==anorm)
               goto jump1;
         }
         
jump1:   c=0.0;
         s=1.0;
         for(i=l;i<=k;i++)
         {
            f=s * svd_rv1[i-1];
            if((abs(f)+anorm)!=anorm)
            {
               g=w[i-1];
               h=sqrt(f * f + g * g);
               w[i-1]=h;
               h=1.0/h;
               c=(g * h);
               s= -(f * h);
               for(j=1;j<=m;j++)
               {
                  y=a[j-1][nm-1];
                  z=a[j-1][i-1];
                  a[j-1][nm-1]=(y * c)+(z * s);
                  a[j-1][i-1]= -(y * s)+(z * c);
               }
            }
         }
         
jump2:   z=w[k-1];
         if(l==k)
         {
            if(z<0.0)
            {
               w[k-1]= -z;
               for(j=1;j<=n;j++)
                  v[j-1][k-1] = -v[j-1][k-1];
            }
            goto jump3;
         }
         /* No convergence, so shift bottom 2-by-2 minor */
         if(its>=30)printf("No convergence in 30 iterations\n");
         x=w[l-1];
         nm=k-1;
         y=w[nm-1];
         g=svd_rv1[nm-1];
         h=svd_rv1[k-1];
         f=((y-z) * (y+z)+(g-h) * (g+h))/(2.0 * h * y);
         g=sqrt(f * f + 1.0);
         f=((x-z) * (x+z)+h * ((y/(f+sign(g,f)))-h))/x;
         c=1.0;
         s=1.0;

         for(j=l;j<=nm;j++)
         {
            i=j+1;
            g=svd_rv1[i-1];
            y=w[i-1];
            h=s * g;
            g *= c;
            z=sqrt(f * f + h * h);
            svd_rv1[j-1]=z;
            c=f/z;
            s=h/z;
            f=(x * c)+(g * s);
            g= -(x * s)+(g * c);
            h=y * s;
            y *= c;

            for(jj=1;jj<=n;jj++)
            {
               x=v[jj-1][j-1];
               z=v[jj-1][i-1];
               v[jj-1][j-1]=(x * c)+(z * s);
               v[jj-1][i-1]= -(x * s)+(z * c);
            }

            z=sqrt(f * f + h * h);
            w[j-1]=z;

            if(z!=0.0)
            {
               z=1.0/z;
               c=f * z;
               s=h * z;
            }

            f=(c * g)+(s * y);
            x= -(s * g)+(c * y);

            for(jj=1;jj<=m;jj++)
            {
               y=a[jj-1][j-1];
               z=a[jj-1][i-1];
               a[jj-1][j-1]=(y * c)+(z * s);
               a[jj-1][i-1]= -(y * s)+(z * c);
            }
         }
         svd_rv1[l-1]=0.0;
         svd_rv1[k-1]=f;
         w[k-1]=x;
      }
jump3: ;
   }
   
   free(svd_rv1);
   return(0);
}

/***************************************************************************/
/*>svbksb(double **u, double *w, double **v, int m,
          int n, int mp, int np, double *b, double *x)
   ---------------------------------------------------
   Returns:    int                   0: OK
                                    -1: No memory
   Back substitution for SVD.

   14.04.92 Cleaned up from AMPlot V1.0.
   27.04.92 Local array allocated dynamically
*/
static int svbksb(double *u[],
                  double *w,
                  double *v[],
                  int m,
                  int n,
                  int mp,
                  int np,
                  double *b,
                  double *x)
{
   double            *svd_tmp, s;
   int               j,i,jj;

   if((svd_tmp = (double *)malloc(np * sizeof(double))) == NULL)
      return(-1);

   for(j=1;j<=n;j++)
   {
      s=0.0;
      if(w[j-1]!=0.0)
      {
         for(i=1;i<=m;i++)
            s += (u[i-1][j-1]*b[i-1]);
         s /= w[j-1];
      }
      svd_tmp[j-1]=s;
   }
   for(j=1;j<=n;j++)
   {
      s=0.0;
      for(jj=1;jj<=n;jj++)
         s += (v[j-1][jj-1]*svd_tmp[jj-1]);
      x[j-1]=s;
   }
   
   free(svd_tmp);
   return(0);
}

/***************************************************************************/
/*>double sign(double a, double b)
   -------------------------------
   Returns the absolute value of a with the sign of b

   14.04.92 Cleaned up from AMPlot V1.0.
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
/*>svdvar(double *v[], int ma, double *w, double *var)
   -----------------------------------------------------
   Calculate the variance for each of the parameters from SVD fitting.
   Returns:    int               0: OK; -1: No memory

   01.05.92 Original, from equation in Num Rec.
*/
int svdvar(double  *v[],
       int     ma,
       double  *w,
       double  *var)
{
   double   *wti;
   int      i, j;
   
   if((wti = (double *)malloc(ma * sizeof(double))) == NULL)
      return(-1);
      
   for(i=0; i<ma; i++)
   {
      if(w[i] != 0.0)   wti[i] = 1.0/(w[i]*w[i]);
      else              wti[i] = 0.0;
   }
   

   for(i=0; i<ma; i++)
   {
      var[i] = 0.0;
      for(j=0; j<ma; j++) var[i] += v[i][j] * v[i][j] * wti[i];
   }
   return(0);
}

/***************************************************************************/
/*>svdcov(double *v[], int ma, double *w, double *cvm[])
   -----------------------------------------------------
   Calculate the covariance matrix from SVD fitting.
   Returns:    int               0: OK; -1: No memory

   01.05.92 Original, translated from Num Rec.
*/
int svdcov(double  *v[],
       int     ma,
       double  *w,
       double  *cvm[])
{
   double   *wti,
            sum;
   int      i, j, k;
   
   if((wti = (double *)malloc(ma * sizeof(double))) == NULL)
      return(-1);
      
   for(i=0; i<ma; i++)
   {
      if(w[i] != 0.0)   wti[i] = 1.0/(w[i]*w[i]);
      else              wti[i] = 0.0;
   }
   
   for(i=0; i<ma; i++)
   {
      for(j=0; j<=i; j++)
      {
         sum = 0.0;
         for(k=0; k<ma; k++) sum += v[i][k] * v[j][k] * wti[k];
         cvm[i][j] = cvm[j][i] = sum;
      }
   }
   return(0);
}

