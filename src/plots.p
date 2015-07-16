int SetScaling(int force)
;
int DoPlot(int destination,
       FH *fh)
;
int LineGraph(int               dest,
          FH *fh)
;
int ScatterGraph(int               dest,
             FH *fh)
;
int DoGraphTitle(int                 dest, 
             FH   *fh)
;
int DoErrorBars(int               dest, 
            FH *fh,
            int               barchart)
;
int DoExtraLabels(int               dest,
              FH *fh)
;
int DoDPMarks(int               dest,
          FH *fh)
;
double alog(double x)
;
int DoFitting(int               dest, 
          FH *fh)
;
int DoLineKey(int               dest,
          FH *fh)
;
int DoSmooth(int               dest,
         FH *fh)
;
int DoRobust(int               dest,
         FH *fh)
;
int DoSVDFit(int               dest,
         FH *fh,
         int               np,      /* Number of polynomials for SVDFIT    */
         double            tol,
         char              *write,  /* Pointer to name of output data      */
         void (*Func)(double xf, double *pf, int nf))
;
int WriteFitInfo(FH   *Fh,
             int                 dsetnum,
             double              *coef,
             double              *v[],
             int                 ma,
             double              *w)
;
