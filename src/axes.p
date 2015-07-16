int CalcBounds(double *xmin,
           double *xmax,
           double *ymin,
           double *ymax)
;
int RoundUp(double *value,
        double Size)
;
int RoundDown(double *value,
          double Size)
;
int CalcTicks(int    usebuff,
          double *xtick,
          double *ytick,
          double *xsub,
          double *ysub)
;
int DrawAxes(int               dest,
         FH *fh,
         int               parts)
;
int XAxisTitle(int                dest, 
           FH  *fh)
;
int YAxisTitle(int                dest, 
           FH  *fh)
;
int DrawXTick(int               dest,
          FH *fh,
          double            x,
          double            y,
          int               ticknum)
;
int DrawYTick(int               dest,
          FH *fh,
          double            x,
          double            y)
;
int DrawXSub(int               dest,
         FH *fh,
         double            x,
         double            y,
         int               special)
;
int DrawYSub(int               dest,
         FH *fh,
         double            x,
         double            y,
         int               special)
;
