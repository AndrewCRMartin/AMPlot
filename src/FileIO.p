int ReadData(FH *fh)
;
int AllocBarLabels(void)
;
int UncacheBarLabels(void)
;
int UncacheKeytext(void)
;
int SetFillDatasets(void)
;
int ClearPlots(int force)     /* Input flag for prompt */
;
DATASET *AllocDataset(int     ndata,
                      DATASET *reset)
;
int SaveGraph(FH *fh)
;
