int EjectSlice(APTR object)
;
int OpenFile(APTR object)
;
int SaveAsFile(APTR object)
;
int SetPaper(APTR object)
;
int About(APTR object)
;
int QuitProgram(APTR object)
;
int PlotPS(APTR object)
;
int PlotHPGL(APTR object)
;
int PlotDR2D(APTR object)
;
int SetPens(APTR object)
;
int StyleXY(APTR object)
;
int StyleScatter(APTR object)
;
int StyleBar(APTR object)
;
int StylePie(APTR object)
;
int ToggleErrors(APTR object)
;
int SetLine(APTR object)
;
int SetBar(APTR object)
;
int SetPie(APTR object)
;
int ToggleBoxed(APTR object)
;
int ToggleGrid(APTR object)
;
int SetAxes(APTR object)
;
int ToggleFZero(APTR object)
;
int SetTitle(APTR object)
;
int SetAxTitle(APTR object)
;
int SetAxLabel(APTR object)
;
int SetKey(APTR object)
;
int ClearAllKeys(void)
;
char *ChangeKey(struct Window *ReqWind,
                struct Gadget *gadget)
;
int SetRegress(APTR object)
;
int ToggleRobust(APTR object)
;
int SetFourier(APTR object)
;
int FillControl(APTR object)
;
int BarLabels(struct Window *ParentWind)
;
char *ChangeBarLab(struct Window *ReqWind, 
                   struct Gadget *gadget)
;
TEXTLIST *ChangeExtraLabel(struct Window *ReqWind,
                           struct Gadget *gadget)
;
int ChangePen(APTR object)
;
int WaitForClick(struct Window *wind,
             int           *x,
             int           *y)
;
int ScreenToData(int    xscreen, 
             int    yscreen, 
             double *x, 
             double *y)
;
DATASET *FindDataSet(int x,
                     int y)
;
int InstallMacro(APTR object)
;
int RexxFit(APTR object)
;
int RunMacro(APTR object)
;
int ToggleRexxDeb(APTR object)
;
int SetLineDash(APTR object)
;
int SetLabel(APTR object)
;
int SetFillType(APTR object)
;
int SetPalette(APTR object)
;
int RemoveFills(APTR object)
;
