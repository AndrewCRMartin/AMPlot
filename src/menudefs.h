/***************************************************************************

   Program:    AMPlot2
   File:       menudefs.h
   
   Version:    V1.0
   Date:       10.07.92
   Function:   Menu number definitions
   
   Copyright:  SciTech Software 1992
   Author:     Andrew C. R. Martin
   Address:    SciTech Software
               23, Stag Leys,
               Ashtead,
               Surrey,
               KT21 2TD.
   Phone:      +44 (0372) 275775
   EMail:      UUCP: cbmuk!cbmuka!scitec!amartin
               JANET: andrew@uk.ac.ox.biop
               
****************************************************************************

   This program is copyright. Any copying without the permission of
   SciTech Software is illegal.

****************************************************************************

   Description:
   ============
   Specify menu numbers for menus and items which need to be switched on
   and off

****************************************************************************

   Usage:
   ======

****************************************************************************

   Revision History:
   =================

***************************************************************************/
/* Top-level menus */
#define MenuNumProject     0x0000|(NOITEM<<5)
#define MenuNumStyle       0x0001|(NOITEM<<5)
#define MenuNumAxes        0x0002|(NOITEM<<5)
#define MenuNumText        0x0003|(NOITEM<<5)
#define MenuNumLines       0x0004|(NOITEM<<5)
#define MenuNumFit         0x0005|(NOITEM<<5)
#define MenuNumARexx       0x0006|(NOITEM<<5)

/* Specific menu items */
/*                                                  Sub  Item  Menu
                                                   <---><----><--->  */
#define MenuNumPlot        0x0080|(NOSUB<<11)   /* 0000000010000000  */
#define MenuNumDR2D        0x1080               /* 0001000010000000  */
#define MenuNumPie         0x0061               /* 0000000001100001  */
#define MenuNumPieSetup    0x0101               /* 0000000100000001  */
#define MenuNumErrors      0x0081               /* 0000000010000001  */
#define MenuNumLogX        0x0002               /* 0000000000000010  */
#define MenuNumLogY        0x0022               /* 0000000000100010  */
#define MenuNumGrid        0x0062               /* 0000000001100010  */
#define MenuNumFZero       0x00A2               /* 0000000010100010  */
#define MenuNumAxTitle     0x0023               /* 0000000000100011  */
#define MenuNumPen         0x0004               /* 0000000000000100  */
#define MenuNumLStyle      0x0024|(NOSUB<<11)   /* 0000000000100100  */
#define MenuNumRexxFit     0x0065               /* 0000000001100101  */

