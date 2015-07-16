int AMMove(int                 dest,
       FH   *fh,
       double              x,
       double              y)
;
int AMDraw(int                 dest,
       FH   *fh,
       double              x,
       double              y)
;
int AMSetPen(int               dest,
         FH *fh,
         int               pen)
;
int AMLine(int                 dest,
       FH   *fh,
       double              x1,
       double              y1,
       double              x2,
       double              y2)
;
int AMCTText(int               dest,
         FH *fh,
         double            x,
         double            y,
         char              *text)
;
int AMCBText(int               dest,
         FH *fh,
         double            x,
         double            y,
         char              *text)
;
int AMRText(int               dest,
        FH *fh,
        double            x,
        double            y,
        double            offset,
        char              *text)
;
int AMText(int               dest,
       FH *fh,
       double            x,
       double            y,
       char              *text)
;
int AMCText(int               dest,
        FH *fh,
        double            x,
        double            y,
        char              *text)
;
int AMSetFont(int                 dest, 
          FH   *fh, 
          STATICTEXT          *stext)
;
int AMYTitle(int dest, 
         FH *fh, 
         double x, 
         double y, 
         char *text)
;
char *ftostr(char    *str,
             int     maxlen,
             double  x,
             int     precision)
;
int AMSetLineStyle(int               dest,
               FH *fh, 
               int               style)
;
int AMDPMark(int               dest,
         FH *fh, 
         double            x, 
         double            y, 
         int               mark)
;
char *SimplifyText(char *string)
;
int AMInitPlot(int                dest,
           FH  *fh, 
           double             xsize, 
           double             ysize, 
           double             xoff, 
           double             yoff)
;
int AMEndPlot(int                dest,
          FH  *fh)
;
int PSMark(double x, 
       double y, 
       int    mark, 
       double dpsize)
;
int AMEndLine(int               dest, 
          FH *fh)
;
int DataToScreen(double x,
             double y, 
             int    *xs,
             int    *ys)
;
int AMSetPattern(int dest, FH *fh, int pattern)
;
int AMRect(int                 dest,
       FH   *fh,
       double              x1,
       double              y1,
       double              x2,
       double              y2,
       int                 pattern,
       int                 pen)
;
int CalcWedgeBounds(double xcent, 
                double ycent, 
                double size,
                double startangle, 
                double endangle,
                double *x1, 
                double *y1, 
                double *x2, 
                double *y2)
;
int AMWedge(int                dest, 
        FH  *fh, 
        double             xcent, 
        double             ycent,
        double             startangle, 
        double             endangle, 
        int                pattern)
;
int WedgeClip(double xc,          /* Centre of circle                          */
          double yc,
          double r,           /* Radius of circle                          */
          double m,           /* Slope of line                             */
          double c,           /* y-offset of line                          */
          double alpha1,      /* Start angle                               */
          double alpha2,      /* End angle                                 */
          double *x1,         /* End of clipped line                       */
          double *y1,
          double *x2,
          double *y2)
;
void InstallDR2DFonts(void)
;
