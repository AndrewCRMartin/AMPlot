double trueangle(double opp, double adj)
;
int ScreenToPieData(int    xscreen, 
                int    yscreen, 
                double *ang,
                double *dist)
;
int PieGraph(int               dest,
         FH *fh)
;
int Wedge(int               dest, 
      FH *fh,
      double            lastangle, 
      double            endangle, 
      int               pattern,
      int               eject,
      int               wedge)
;
int DoExtraPieLabels(int               dest,
                 FH *fh)
;
int DoPieKey(int               dest,
         FH *fh)
;
