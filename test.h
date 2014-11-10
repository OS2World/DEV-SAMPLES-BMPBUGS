/* --------------------------------------------------------------------------
 test.h :

 2004-- - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */



#ifndef _TEST_H_
   #define _TEST_H_
   #define INCL_WIN
   #define INCL_GPI

   #include <os2.h>
   #include <stdio.h>


#define SZ_TITLE    "9-parts bitmap test"
#define SZ_CLASS    "clientWindow"
#define SZ_CLASS2   "9-partsBitmap"
#define FRAMEFLAGS   FCF_TITLEBAR       \
                   | FCF_SYSMENU        \
                   | FCF_MINMAX         \
                   | FCF_SIZEBORDER     \
                   | FCF_SHELLPOSITION  \
                   | FCF_TASKLIST       \
                   | FCF_ICON


#define createTestWin(hwnd, style, x, y, cx, cy, id) \
   WinCreateWindow(hwnd, SZ_CLASS2, NULL, WS_VISIBLE | (style),\
                   (x), (y), (cx), (cy), hwnd, HWND_TOP, (id), NULL, NULL)


/* --------------------------------------------------------------------------
 BMPPAINT          data used to paint a bitmap.
                   These data allow to draw a bitmap with fixed margins
                   and a variable size stretched or tiled inner part.
                   The bitmap is divided in 3 or 9 parts and those parts
                   are individually painted with different attributes.
                 +--------+-------------------------------------+--------+
         |   dy    |        |                                     |        |
         |        +--------+-------------------------------------+--------+
        cy         |  dx  |   this part is stretched            |  dx  |
         |        +--------+-------------------------------------+--------+
         |   dy    |        |                                     |        |
                 +--------+-------------------------------------+--------+
                   -----------------------cx------------------------------

*/

typedef struct {
   HBITMAP hbmp;            // bitmap handle
   ULONG dx,                // thickness of the vertical borders
         dy,                // thickness of the horizontal borders
         cx,                // overall bitmap width
         cy;                // overall bitmap height
} BMPPAINT, * PBMPPAINT;

// change these constants to use a different test bitmap
#define CX_BMPBORDER       10
#define CY_BMPBORDER        8
#define CX_BMP            200
#define CY_BMP             30


typedef struct {
   HWND ahwnd[6];
   BMPPAINT bmpp;
} GLOBAL, * PGLOBAL;


/* --------------------------------------------------------------------------
 coordinates

(0,cy)        (dx,cy)                              (cx-dx,cy)    (cx,cy)
    +------------+-------------------------------------+------------+
    |            |                                    |            |
    |            |                 dy                  |            |
(0,cy-dy)   (dx,cy-dy)                          (cx-dx,cy-dy)   (cx,cy-dy)
    +------------+-------------------------------------+------------+
    |            |                                     |            |
    | -- dx -- |   this part is stretched            | -- dx -- |
    |            |                                     |            |
(0,dy)        (dx,dy)                              (cx-dx,dy)    (cx,dy)
    +------------+-------------------------------------+------------+
    |            |                                    |            |
    |            |                 dy                  |            |
    |            |                                    |            |
    +------------+-------------------------------------+------------+
(0,0)          (dx,0)                              (cx-dx,0)      (cx,0)


*/


#endif // #ifndef _TEST_H_
