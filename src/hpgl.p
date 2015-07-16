int HPGLInit(double xmargin, 
         double ymargin)
;
int HPGLPen(int num)
;
int HPGLMove(double x,
         double y)
;
int HPGLDraw(double x,
         double y)
;
int HPGLLText(double x,
          double y,
          char   *string)
;
int HPGLFont(int    font,
         double size)
;
int HPGLCBText(double x, 
           double y, 
           double offset, 
           char   *text)
;
int HPGLROffText(double x, 
             double y, 
             double offset, 
             char   *text)
;
int HPGLLCText(double x, 
           double y, 
           char   *text)
;
int HPGLSetDash(int style)
;
int HPGLMark(double x, 
         double y, 
         int    mark, 
         double size)
;
int HPGLFillRect(double x1, 
             double y1, 
             double x2, 
             double y2, 
             int    pattern,
             double spacing)
;
int HPGLVText(double x, 
          double y, 
          double xoff, 
          char   *text, 
          int    TitleFont, 
          double TitleSize, 
          char   *label, 
          int    LabelFont, 
          double LabelSize)
;
int HPGLArc(double xc, 
        double yc, 
        double dlength, 
        double startangle, 
        double endangle, 
        int    pattern,
        double hatchstep)
;
int HPGLShowText(char *text, 
             int  orientation,
             int  XBase,
             int  YBase)
;
int HPGLCTText(double x, 
           double y, 
           double offset, 
           char   *text)
;
