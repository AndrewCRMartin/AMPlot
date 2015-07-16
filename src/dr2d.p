int InitDR2D(char     *filename,
         double   xmin,
         double   xmax,
         double   ymin,
         double   ymax,
         char     *AltFontName)
;
double DR2DStrWidth(char *string, int fontnum, double size)
;
int DefineDR2DFonts(void)
;
int UseDR2DFont(char *fontname)
;
int PutFONS(char *Font)
;
int SetDR2DAttributes(int dash, int fill, int colour, IEEE thick)
;
int DefineDashes(void)
;
int PutDashDef(int id, int ndash, IEEE *dashes)
;
void EndDR2D(void)
;
int DR2DMove(double            x,
         double            y)
;
int DR2DDraw(double            x,
         double            y)
;
int DR2DStroke(void)
;
int DR2DCloseStroke(void)
;
int DR2DThick(double thick)
;
int DR2DSetDash(int pattern)
;
int DR2DFillRect(double x1, 
             double y1, 
             double x2, 
             double y2,
             int    pattern, 
             int    hatch, 
             double spacing,
             double hatchthick)

;
int DR2DSetFont(char     *Font,
            double   size)
;
int FindDR2DFont(char *font)
;
int DefineLayer(int id, char *name)
;
int DR2DLText(double x,
          double y, 
          char   *string)
;
int DR2DCTText(double x, 
           double y, 
           double offset, 
           char   *text)
;
int DR2DCBText(double x, 
           double y, 
           char   *string)
;
int DR2DLCText(double x, 
           double y, 
           char   *string)
;
int DR2DVText(double x, 
          double y, 
          double xoff, 
          char   *text, 
          char   *TitleFont, 
          double TitleSize, 
          char   *label, 
          char   *LabelFont, 
          double LabelSize)
;
int DR2DArc(double xc, 
        double yc, 
        double dlength, 
        double startangle, 
        double endangle, 
        int    pattern,
        int    hatch,
        double hatchstep,
        double hatchthick)
;
int DR2DMark(double x, 
         double y, 
         int    mark, 
         double size)
;
int DR2DROffText(double x, 
             double y, 
             double offset,
             char   *string)
;
int DR2DShowText(char    *text, 
             int     orientation,
             double  XBase,
             double  YBase)
;
int DR2DOutputString(char *string, double x, double y, int orientation)
;
