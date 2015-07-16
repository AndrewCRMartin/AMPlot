#define IEEE   float


/***************************************************************************/
/* Chunk ID defines
*/
#define  ID_DR2D  MAKE_ID('D','R','2','D')
#define  ID_DRHD  MAKE_ID('D','R','H','D')
#define  ID_PPRF  MAKE_ID('P','P','R','F')
#define  ID_CMAP  MAKE_ID('C','M','A','P')
#define  ID_FONS  MAKE_ID('F','O','N','S')
#define  ID_DASH  MAKE_ID('D','A','S','H')
#define  ID_AROW  MAKE_ID('A','R','O','W')
#define  ID_FILL  MAKE_ID('F','I','L','L')
#define  ID_LAYR  MAKE_ID('L','A','Y','R')
#define  ID_ATTR  MAKE_ID('A','T','T','R')
#define  ID_BBOX  MAKE_ID('B','B','O','X')
#define  ID_XTRN  MAKE_ID('X','T','R','N')
#define  ID_VBM   MAKE_ID('V','B','M',' ')
#define  ID_CPLY  MAKE_ID('C','P','L','Y')
#define  ID_OPLY  MAKE_ID('O','P','L','Y')
#define  ID_GRUP  MAKE_ID('G','R','U','P')
#define  ID_STXT  MAKE_ID('S','T','X','T')
#define  ID_TPTH  MAKE_ID('T','P','T','H')

/***************************************************************************/
/* DRHD: Drawing header
*/
typedef struct
{
   IEEE  XLeft,   YTop,
         XRight,  YBot;
}  DRHDstruct;

/***************************************************************************/
/* PPRF: Page preferences
*/

/***************************************************************************/
/* CMAP: Colour map (same as ILBM CMAP)
*/


/***************************************************************************/
/* FONS: Font chunk
*/
typedef struct
{
   UBYTE FontID;        /* Identifier the font is referenced by   */
   UBYTE Pad1;          /* Always 0                               */
   UBYTE Proportional;  /* Is it proportional?                    */
   UBYTE Serif;         /* Is it a serif font?                    */
}  FONSstruct;

/* TODO: write this, then the font name (CHAR) of length Size-4   */

/***************************************************************************/
/* DASH: Line dash pattern for edges
*/
typedef struct
{
   USHORT   DashID;     /* Identifier the pattern is referenced by         */
   USHORT   NumDashes;  /* Should always be even                           */
}  DASHstruct;
/* TODO: write this, then the on-off dash pattern (IEEE)                   */

/***************************************************************************/
/* AROW: An arrow head
*/
#define  ARROW_FIRST 0x01  /* Arrow at OPLY's first point                  */
#define  ARROW_LAST  0x02  /* Arrow at OPLY's last point                   */

typedef struct
{
   UBYTE    Flags;      /* ARROW_*, as above                               */
   UBYTE    Pad0;       /* Always 0                                        */
   USHORT   ArrowID;    /* Identifier the arrow head is referenced by      */
   USHORT   NumPoints;
}  AROWstruct;
/* TODO: write this, then the 2*NumPoints arrow ArrowPoints (IEEE)         */

/***************************************************************************/
/* FILL: Object-orientated fill pattern
*/

/***************************************************************************/
/* LAYR: Define a layer
*/
#define  LF_ACTIVE      0x01  /* Active for editing                        */
#define  LF_DISPLAYED   0x02  /* Displayed on screen                       */

typedef struct
{
   USHORT   LayerID;
   char     LayerName[16];
   UBYTE    Flags;
   UBYTE    Pad0;
}  LAYRstruct;

/***************************************************************************/
/* ATTR: Object attributes
*/
#define  FT_NONE     0     /* No fill                                      */
#define  FT_COLOR    1     /* Fill with colour from palette                */
#define  FT_OBJECTS  2     /* Fill with tiled objects                      */
#define  JT_NONE     0     /* Don't do line joins                          */
#define  JT_MITER    1     /* Mitered join                                 */
#define  JT_BEVEL    2     /* Beveled join                                 */
#define  JT_ROUND    3     /* Round join                                   */

typedef struct
{
   UBYTE    FillType;      /* One of FT_*, above                           */
   UBYTE    JoinType;      /* One of JT_*, above                           */
   UBYTE    DashPattern;   /* ID of edge dash pattern                      */
   UBYTE    ArrowHead;     /* ID of arrowhead to use                       */
   USHORT   FillValue;     /* Colour of object with which to fill          */
   USHORT   EdgeValue;     /* Edge colour index                            */
   USHORT   WhichLayer;    /* ID of layer it's in                          */
   IEEE     EdgeThick;     /* Line width                                   */
}  ATTRstruct;

/***************************************************************************/
/* BBOX: Bounding box of next object in FORM. Includes line width.
*/
typedef struct
{
   IEEE  XMin, YMin,
         XMax, YMax;
}  BBOXstruct;

/***************************************************************************/
/* XTRN: Externally controlled object
*/
#define  X_CLONE     0x0001   /* Object has been cloned                    */
#define  X_MOVE      0x0002   /* Object has been moved                     */
#define  X_ROTATE    0x0004   /* Object has been rotated                   */
#define  X_RESIZE    0x0008   /* Object has been resized                   */
#define  X_CHANGE    0x0010   /* Object attribute (ATTR) has changed       */
#define  X_DELETE    0x0020   /* Object has been deleted                   */
#define  X_CUT       0x0040   /* Object has been cut to clipboard          */
#define  X_COPY      0x0080   /* Object has been copied to clipboard       */
#define  X_UNGROUP   0x0100   /* Object has been ungrouped                 */

typedef struct
{
   short ApplCallBacks;       /* From defines, above                       */
   short ApplNameLength;
}  XTRNstruct;
/* TODO: write this, then the ApplName (char), the ARexx function to call  */


/***************************************************************************/
/* VBM:  Virtual bitmap
*/
typedef struct
{
   IEEE     XPos,    YPos,    /* Virtual coordinates                       */
            XSize,   YSize,   /* Virtual size                              */
            Rotation;         /* in degrees                                */
   USHORT   PathLen;          /* Length of dir path                        */
}  VBMstruct;
/* TODO: write this, then the null-term path of the file (char)            */

/***************************************************************************/
/* CPLY, OPLY: Close & open polygons
*/
/* TODO: Number of points (USHORT) followed by
   PolyPoints[2*NumPoints] (IEEE)
*/

/***************************************************************************/
/* GRUP: Group DR2D objects
*/
/* Write NumObj (USHORT)

/***************************************************************************/
/* STXT: Simple text
*/
typedef struct
{
   UBYTE    Pad0;             /* Always 0                                  */
   UBYTE    WhichFont;        /* Which font to use                         */
   IEEE     CharW,   CharH,   /* W/H of an individual char                 */
            BaseX,   BaseY,   /* Start of baseline                         */
            Rotation;         /* Angle of text (in degrees)                */
   USHORT   NumChars;         /* Length of text                            */
}  STXTstruct;
/* TODO: write this, then the String (char)                                */

/***************************************************************************/
/* TPTH: Text along a path
*/
#define  J_LEFT      0x00     /* Left justified                            */
#define  J_RIGHT     0x01     /* Right justified                           */
#define  J_CENTER    0x02     /* Center justified                          */
#define  J_SPREAD    0x03     /* Spread along path                         */

typedef struct
{
   UBYTE    Justification;    /* See defines, above                        */
   UBYTE    WhichFont;        /* Which font to use                         */
   IEEE     CharW,   CharH;   /* W/H of an individual char                 */
   USHORT   NumChars;         /* Length of text                            */
   USHORT   NumPoints;        /* Number of points in path                  */
}  TPTHstruct;
/* TODO: write this, then the String (char) padded to even, then the path
   array 2*NumPoints (IEEE)
*/


