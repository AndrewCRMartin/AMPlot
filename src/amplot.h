/****************************************************************************

   Program:    AMPlot2
   File:       amplot.h
   
   Version:    V1.0
   Date:       12.03.92
   Function:   Misc external definitions.
   
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

****************************************************************************/
#ifndef AMIGA
typedef unsigned short int UWORD;
#endif

/* Defines
*/
#define DEFAULT_EDITOR "C:ED" /* Default editor to call from within AMPlot */
#define PUBSCREEN "AMPlot"    /* Public screen name                        */

#define MAXBUFF         160   /* Size of a text buffer                     */
#define NUMBUFF          16   /* Size of a number buffer                   */

#ifdef DEMO_VERSION
#define MAXDATA          10   /* Max number of datapoints                  */
#else
#define MAXDATA         100   /* Max number of datapoints                  */
#endif

#define MAXPEN            6   /* Max number of pens                        */
#define MAXSTYLE          6   /* Max number of line styles                 */
#define MAXMARKS         13   /* Max number of datapoint marks             */

#define MAXTRANS         50   /* Max number of font translations           */

#define MAXBARLAB       100   /* Max printable bar labels                  */

#define STYLE_LINE        0   /* Plot styles                               */
#define STYLE_SCATTER     1
#define STYLE_BAR         2
#define STYLE_PIE         3
#define STYLE_OVERLAYED   4   /* Barchart (normal) overlayed style         */
#define STYLE_GROUPED     5   /* Barchart grouped style                    */
#define STYLE_STACKED     6   /* Barchart stacked style                    */
#define STYLE_OUTLINE     7   /* Barchart outline style                    */

#define DEST_SCREEN       0   /* Plot destinations                         */
#define DEST_PS           1
#define DEST_HPGL         2
#define DEST_DR2D         3

#define WIDTH           360   /* Actual screen plot dimensions             */
#define HEIGHT          245

#define RASX         WIDTH    /* Max raster dimensions                     */
#define RASY         HEIGHT

#define NCOMM            75   /* Number of keywords                        */
#define MAXNUMPARAM       5   /* Max number of numeric parameters          */
#define MAXSTRPARAM       7   /* Max number of string parameters           */
#define MAXSTRLEN        80   /* Max length of returned string             */

#define NREXX           120   /* Max number of ARexx commands              */

#define PIESIZE       100.0   /* Size of pie circle                        */
#define PIEBOUNDS     120.0   /* Size of bounds around pie chart           */

#define ABUFFSIZE       400   /* Number of points in area buffer *2.5      */

#define GRAPHICS_REV     29
#define INTUITION_REV    29
#define DISKFONT_REV     29
#define ASL_REV          36
#define DOS_REV          29

#define DEF_FONT "Times.font" /* Default font                              */

/***************************************************************************/
/* Structures
*/

/* PEN is used to store details of each of the 6 pens.                     */
typedef struct
{
   double      thickness;     /* Pen thickness in points                   */
   int         mark,          /* Datapoint mark number                     */
               join,          /* Join datapoints                           */
               colour,        /* Output pen colour                         */
               screencolour;  /* Screen pen colour                         */
}  PEN;

/* GRAPH is used to store overall details of the graph.                    */
typedef struct
{
   double      xmin,          /* Min X to be displayed                     */
               xmax,          /* Max X to be displayed                     */
               ymin,          /* Min Y to be displayed                     */
               ymax,          /* Max Y to be displayed                     */
               xtick,         /* Spacing of x-ticks                        */
               ytick,         /* Spacing of y-ticks                        */
               xsub,          /* Spacing of x-subticks                     */
               ysub,          /* Spacing of y-subticks                     */
               xkey,          /* X-position of key (top-left)              */
               ykey,          /* Y-position of key (top-left)              */
               ebarwidth,     /* Error bar width (pt)                      */
               dpsize,        /* Size of datapoint mark (pt)               */
               XScale,        /* X-scaling factor                          */
               YScale,        /* Y-scaling factor                          */
               XPScale,       /* PostScript X-scaling factor               */
               YPScale;       /* PostScript Y-scaling factor               */
   int         style,         /* 0:Line, 1:Scatter, 2:Bar, 3:Pie           */
               errors,        /* Display error bars                        */
               columns,       /* Number of data columns                    */
               logx,          /* Do log x-axis                             */
               logy,          /* Do log y-axis                             */
               boxed,         /* Use box                                   */
               grid,          /* Use grid                                  */
               fzero,         /* Axes to follow zero                       */
               noxlabels,     /* No x-axis labels                          */
               noylabels,     /* No y-axis labels                          */
               centrexlabels, /* X-labels to be centred along tick space   */
               xprecision,    /* Number of d.p.'s                          */
               yprecision,    /* Number of d.p.'s                          */
               usekey,        /* Key is to be displayed                    */
               userbounds,    /* User has specified graph limits           */
               userticks,     /* User has specified ticks                  */
               usersub,       /* USer has specified subticks               */
               usertitlepos,  /* User has specified title position         */
               userprec,      /* User has specified label precision        */
               clip,          /* Clip graph to bounds                      */
               subtick,       /* Use subticks                              */
               CurrentKey,    /* Current dataset number for key requester  */
               nplots,        /* Number of plots                           */
               nlabels,       /* Number of `extra' labels in use           */
               CurrentLabel,  /* Current `extra' label                     */
               maxdata;       /* Max number of datapoints in any plot      */
}  GRAPH;
               
/* BARSTYLE is used to store details of the barchart style                 */
typedef struct
{
   double      shrink;        /* Barchart shrink factor                    */
   int         style,         /* Details of bar type                       */
               shrinkall,     /* Shrink all bars                           */
               fillall,       /* Fill all bars                             */
               D3,            /* 3D option                                 */
               nbar,          /* Number of bars                            */
               CurrentBar,    /* Current bar number                        */
               uselabels;     /* Use barlabels rather than normal x-labels */
}  BARSTYLE;

/* PIESTYLE is used to store details of the piechart style                 */
typedef struct
{
   double      start,         /* Position from which to start the chart    */
               eject;         /* % of pie size by which to eject slice     */
   int         D3;            /* 3D option                                 */
}  PIESTYLE;

/* OUTPUT is used to store details of the output format.                   */
typedef struct
{
   double            xsize,               /* Plot x-size (inches)          */
                     ysize,               /* Plot y-size (inches)          */
                     xoffset,             /* Plot x-offset (inches)        */
                     yoffset,             /* Plot y-offset (inches)        */
                     ticklen,             /* Length of tick (pt)           */
                     subticklen,          /* Length of sub-tick (pt)       */
                     hatchthick,          /* Thickness of hatching lines   */
                     hatchspace,          /* Spacing of hatching lines     */
                     xmargin,             /* Paper print margin for HPGL   */
                     ymargin;
   FH                *fh;                 /* Output file                   */
   int               plottype,            /* 0:PS, 1:HPGL, 2:DR2D          */
                     hatching,            /* Fills should be hatched       */
                     fixbounds;           /* Flag to call PSFixBoundingBox */
   char              filename[MAXBUFF],   /* PS filename                   */
                     AltFont[MAXBUFF];    /* Altername PS font (`greek')   */
}  OUTPUT;

/* STATICTEXT is used to store text details. Character arrays are stored
   within the structure. 
*/
typedef struct
{
   double      PSSize,              /* PostScript font size (pt)           */
               x,                   /* x-position                          */
               y;                   /* y-position                          */
   int         AmigaSize;           /* Amiga screen font size              */
   char        text[MAXBUFF],       /* Text                                */
               PSFont[MAXBUFF],     /* PostScript font                     */
               AmigaFont[MAXBUFF];  /* Amiga screen font                   */
}  STATICTEXT;

/* TEXTLIST is used for linked lists of static text. 
*/
typedef struct tlist
{
   struct tlist   *next,            /* Linked list                         */
                  *prev;
   STATICTEXT     stext;            /* The text                            */
}  TEXTLIST;

/* SIMPLETEXT is like STATICTEXT, but does not contain font information.
*/
typedef struct
{
   double      x,                   /* x-position                          */
               y;                   /* y-position                          */
   char        text[MAXBUFF];       /* Text                                */
}  SIMPLETEXT;

/* DATASET is used to store details about each dataset with pointers to
   the actual data arrays.
*/
typedef struct dset
{
   struct dset *next;         /* Pointer to next dataset                   */
   double      *x,            /* Pointer to x-coordinates                  */
               *y,            /* Pointer to y-coordinates                  */
               *ep,           /* Pointer to +ve error bars                 */
               *em,           /* Pointer to -ve error bars                 */
               xmin,          /* Min x value in dataset                    */
               xmax,          /* Max x value in dataset                    */
               ymin,          /* Min y value in dataset                    */
               ymax;          /* Max y value in dataset                    */
   STATICTEXT  key;           /* Key associated with this dataset          */
   int         pen,           /* Pen number for this dataset               */
               linestyle,     /* Linestyle for this dataset                */
               ndata,         /* Number of data points in this dataset     */
               maxdata;       /* Max datapoints in this dataset            */
}  DATASET;

/* DEFAULTS is used to store various default values which may be modified
   by tooltypes.
*/
typedef struct
{
   double   KeyPSSize,              /* PostScript font size for key        */
            LabelPSSize,
            KeyX,                   /* Position of key                     */
            KeyY;
   int      KeyAmigaSize,           /* Amiga screen font size for key      */
            LabelAmigaSize;
   char     KeyPSFont[MAXBUFF],     /* PostScript font for key             */
            LabelPSFont[MAXBUFF],
            KeyAmigaFont[MAXBUFF],  /* Amiga font for key                  */
            LabelAmigaFont[MAXBUFF],
            directory[MAXBUFF];     /* Default directory                   */
}  DEFAULTS;

/* DISPLAY is used to store characteristics of the Amiga display.
*/
typedef struct
{
   int      WXSize,           /* Window X-size                             */
            WYSize,           /* Window Y-size                             */
            NColours,         /* Number of colours                         */
            xoff,             /* Screen offset of graph axes               */
            yoff,
            width,            /* Pixel size of axes                        */
            height,
            shanghai,         /* Flag to use SHANGHAI mode under V2        */
            FromCLI,          /* Flag for having started from CLI          */
            OSVersion;        /* OS version                                */
}  DISPLAY;

/* FITTING is used to store details of the fitting methods to use
*/
typedef struct
{
   double   smooth_pts,             /* Fourier smoothing factor            */
            regress_tol,            /* Regression tolerence                */
            rexx_tol;               /* Rexx regression tolerence           */
   int      regression,             /* Do regression fit                   */
            robust,                 /* Do robust straight-line fit         */
            fourier,                /* Do fourier smoothing                */
            rexx,                   /* Do fitting via ARexx                */
            regress_npoly,          /* Regression polynomial degree        */
            rexx_npoly,             /* Rexx regression polynomial degree   */
            regress_write,          /* Flag for writing regression info    */
            rexx_write;             /* Flag for writing rexx fit info      */
   char     rexxmacro[MAXBUFF];     /* ARexx macro name                    */
}  FITTING;

/* REXX is used to store bits and pieces for ARexx interface               */
typedef struct
{
   double      value;               /* Values set through RVI              */
   DATASET     *current;            /* Current dataset                     */
   int         gotrexx,             /* Flag for Rexx port found            */
               fromrexx,            /* Flag that command came from Rexx    */
               rexxplot,            /* Flag for plot initiated from Rexx   */
               replot,              /* Flag to replot after a Rexx command */
               quitprogram,         /* Flag for message from Rexx to quit  */
               empty,               /* Plot an empty graph from Rexx       */
               count,               /* Count of returned values            */
               debug,               /* Flag for debugging Rexx             */
               autoreplot,          /* Flag for auto replotting            */
               forcereplot;         /* Flag to force replot when auto off  */
   char        PortName[MAXBUFF],   /* ARexx port name                     */
               RexxMacro[MAXBUFF];  /* ARexx macro name                    */
} REXX;

/* FONTTRANS is used to store the font translation table                   */
typedef struct
{
   char  *PSFont,
         *AmigaFont;
   int   HPGLFont;
} FONTTRANS;

/* FILLITEM is used to store info on fills                                 */
typedef struct fillitem
{
   struct fillitem   *next;
   double            x,
                     y,
                     angle;
   int               dataset,
                     pattern;
} FILLITEM;

#ifdef MAIN /***************************************************************/
#ifdef AMIGA
struct Screen        *MyScreen      = NULL;  /* Amiga screen               */
struct Window        *MainWind      = NULL;  /* Main plot window           */
struct IntuitionBase *IntuitionBase = NULL;  /* Intuition                  */
struct DiskfontBase  *DiskfontBase  = NULL;  /* Disk font                  */
struct GfxBase       *GfxBase       = NULL;  /* Graphics                   */
struct Library       *AslBase       = NULL;  /* ASL library                */
struct DosBase       *DosBase       = NULL;  /* Dos library                */
struct Library       *IconBase      = NULL;  /* Icon library               */
struct Library       *IFFParseBase  = NULL;  /* iffparse library           */
struct Menu          *MenuStrip     = NULL;  /* The start of the menus     */
struct RastPort      *rp            = NULL;  /* RastPort                   */
#else
int                  MainWind       = 0;     /* Dummy for non-Amiga        */
#endif
int                  tlpen          = 2,     /* Top-left screen pen        */
                     brpen          = 1;     /* Bottom-right screen pen    */
PEN                  pens[6];                /* The six pens               */
DATASET              *data          = NULL;  /* The first dataset          */
GRAPH                graph;                  /* Overall graph details      */
OUTPUT               output;                 /* Output details             */
STATICTEXT           GTitle,                 /* Graph title                */
                     XTitle,                 /* X-axis title               */
                     YTitle,                 /* Y-axis title               */
                     XLabel,                 /* X-axis labels              */
                     YLabel;                 /* Y-axis labels              */
TEXTLIST             *labels        = NULL;  /* Extra labels               */
SIMPLETEXT           *barlabels     = NULL;  /* Text labels for barcharts  */
DEFAULTS             Defaults;               /* Various defaults           */
DISPLAY              display;                /* Amiga display size, etc.   */
BARSTYLE             barstyle;               /* Barchart style details     */
PIESTYLE             piestyle;               /* Piechart style details     */
FITTING              fitting;                /* Details of fitting         */
KeyWd                keywd[NCOMM];           /* Array to store keywords    */
char                 *strparam[MAXSTRPARAM]; /* Array for returned strings */

#ifdef REXX_SUPPORT
REXX                 rexx;                   /* Misc Rexx flags            */
struct RxilFunction  rexx_cmd_table[NREXX+1];/* Rexx command table         */
#endif

FONTTRANS            FontTable[MAXTRANS];    /* Font translation table     */
UWORD                AreaBuf[ABUFFSIZE];     /* Buffer for AreaDraw() etc. */

#ifdef AMIGA
struct AreaInfo      AInfo;                  /* Area info for "            */
PLANEPTR             TBuf           = NULL;  /* Temp raster buffer         */
struct TmpRas        TRas;                   /* Temp raster                */
#endif

FILLITEM             *filllist      = NULL;  /* Linked list of fills       */
char                 **LabCache     = NULL;  /* Datafile barlabel cache    */
int                  LabCacheSize   = 0;     /* Size of barlabel cache     */
char                 **KeyCache     = NULL;  /* Datafile key text cache    */
int                  KeyCacheSize   = 0;     /* Size of key text cache     */
               
#else /*********************************************************************/
#ifdef AMIGA
extern struct Screen          *MyScreen;
extern struct Window          *MainWind;
extern struct IntuitionBase   *IntuitionBase;
extern struct DiskfontBase    *DiskfontBase;
extern struct GfxBase         *GfxBase;
extern struct Library         *AslBase;
extern struct DosBase         *DosBase;
extern struct Library         *IconBase;
extern struct Library         *IFFParseBase;
extern struct Menu            *MenuStrip;
extern struct RastPort        *rp;
#else
extern int                    MainWind;
#endif
extern int                    tlpen,
                              brpen;
extern PEN                    pens[6];
extern DATASET                *data;
extern GRAPH                  graph;
extern OUTPUT                 output;
extern GRAPH                  graph;
extern OUTPUT                 output;
extern STATICTEXT             GTitle,
                              XTitle,
                              YTitle,
                              XLabel,
                              YLabel;
extern TEXTLIST               *labels;
extern SIMPLETEXT             *barlabels;
extern DEFAULTS               Defaults;
extern DISPLAY                display;
extern BARSTYLE               barstyle;
extern PIESTYLE               piestyle;
extern FITTING                fitting;
extern KeyWd                  keywd[NCOMM];
extern char                   *strparam[MAXSTRPARAM];

#ifdef REXX_SUPPORT
extern REXX                   rexx;
extern struct RxilFunction    rexx_cmd_table[NREXX+1];
#endif

extern FONTTRANS              FontTable[MAXTRANS];
extern UWORD                  AreaBuf[ABUFFSIZE];

#ifdef AMIGA
extern struct AreaInfo        AInfo;
extern PLANEPTR               TBuf;
extern struct TmpRas          TRas;
#endif

extern FILLITEM               *filllist;
extern char                   **LabCache;
extern int                    LabCacheSize;
extern char                   **KeyCache;
extern int                    KeyCacheSize;

#endif /********************************************************************/
