int ScreenMark(struct RastPort *RasP,
           int             xscreen, 
           int             yscreen, 
           int             mark, 
           double          dpsize)
;
int ScreenArc(struct RastPort   *RasP,
          int               xc,
          int               yc,
          int               size,
          double            start,
          double            end,
          int               close)
;
int ScreenFillArc(struct RastPort   *RasP,
              int               xc,
              int               yc,
              int               size,
              double            start,
              double            end)
;
