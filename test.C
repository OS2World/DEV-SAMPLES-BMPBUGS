/* --------------------------------------------------------------------------
 test.c :

 2004-- - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#include "test.h"


// prototypes ---------------------------------------------------------------
MRESULT EXPENTRY clientProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
VOID handleClientCreation(HWND hwnd);
VOID handleClientSizing(HWND hwnd, ULONG cx);
VOID handleClientPaint(HWND hwnd);
MRESULT EXPENTRY testProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
VOID drawBackground(HPS hps, ULONG style, PSIZEL psize);
BOOL bmpPaintAt(HPS hps, HBITMAP hbmp, INT x, INT y);
BOOL bmpRectPaintAt(HPS hps, HBITMAP hbmp,
                    INT x0s, INT y0s, INT x1s, INT y1s, INT x0d, INT y0d);
BOOL bmpRectPaintToRect(HPS hps, HBITMAP hbmp,
                        INT x0s, INT y0s, INT x1s, INT y1s,
                        INT x0d, INT y0d, INT x1d, INT y1d);


// globals ------------------------------------------------------------------
GLOBAL g;


// --------------------------------------------------------------------------
INT main(VOID) {
   HWND hwndf, hwndc;
   HAB hab;
   HMQ hmq;
   QMSG qmsg;
   ULONG fl = FRAMEFLAGS;

   hmq = WinCreateMsgQueue(hab = WinInitialize(0), 0);
   WinRegisterClass(hab, SZ_CLASS, clientProc, CS_SIZEREDRAW, 0);
   WinRegisterClass(hab, SZ_CLASS2, testProc, CS_SIZEREDRAW, 0);
   hwndf = WinCreateStdWindow(HWND_DESKTOP,    // parent
                              WS_VISIBLE,      // frame window style
                              &fl,             // frame flags
                              SZ_CLASS,        // client class name
                              SZ_TITLE,        // window title
                              0,               // client window style
                              NULLHANDLE,      // resource module handle
                              1,               // resources ID
                              &hwndc);         // client window handle
   if (hwndf) {
      while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
         WinDispatchMsg(hab, &qmsg);
   }
   WinDestroyWindow(hwndf);
   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
   return 0;
} /* end main */


/* --------------------------------------------------------------------------
 client window procedure
*/
MRESULT EXPENTRY clientProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg) {
      case WM_CREATE:
         handleClientCreation(hwnd);
         break;
      case WM_SIZE:
         handleClientSizing(hwnd, SHORT1FROMMP(mp2));
         break;
      case WM_PAINT:
         handleClientPaint(hwnd);
         break;
      default:
         return WinDefWindowProc(hwnd, msg, mp1, mp2);
   } /* endswitch */
   return (MRESULT)FALSE;
}


/* --------------------------------------------------------------------------
 Handling of client window creation event.
*/
VOID handleClientCreation(HWND hwnd) {
   HPS hps;
   RECTL r;
   INT i, cyTest, cyTot;
   hps = WinGetPS(hwnd);
   // get the bitmap handle
   g.bmpp.hbmp = GpiLoadBitmap(hps, 0, 1, 0, 0);
   WinReleasePS(hps);
   // initialize the bitmap size and thickness of the bitmap border
   g.bmpp.dx = CX_BMPBORDER;
   g.bmpp.dy = CY_BMPBORDER;
   g.bmpp.cx = CX_BMP;
   g.bmpp.cy = CY_BMP;
   // create the test child windows
   WinQueryWindowRect(hwnd, &r);
   for (i = 0; i < 4; ++i) {
      g.ahwnd[i] = createTestWin(hwnd, 1 << i, 10, 10 + i * 32,
                                 r.xRight - 20, 22, 6 - i);
   } /* endfor */
   g.ahwnd[4] = createTestWin(hwnd, 0, 10, 138, g.bmpp.cx, g.bmpp.cy, 1);
   g.ahwnd[5] = createTestWin(hwnd, 8, 20 + g.bmpp.cx, 138, 100, 100, 2);
}


/* --------------------------------------------------------------------------
 On client window size change update the width of the child windows
*/
VOID handleClientSizing(HWND hwnd, ULONG cx) {
   INT i;
   cx -= 20;
   for (i = 0; i < 4; ++i) {
      WinSetWindowPos(g.ahwnd[i], 0, 0, 0, cx, 22, SWP_SIZE);
   } /* endfor */
}


/* --------------------------------------------------------------------------
 Paint the background of the client window.
*/
VOID handleClientPaint(HWND hwnd) {
   RECTL r;
   HPS hps = WinBeginPaint(hwnd, NULLHANDLE, &r);
   WinFillRect(hps, &r, SYSCLR_DIALOGBACKGROUND);
   WinEndPaint(hps);
}


/* --------------------------------------------------------------------------
 Child window procedure.
*/

MRESULT EXPENTRY testProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   if (msg == WM_PAINT) {
      CHAR buf[32];
      HPS hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
      ULONG style;
      RECTL r;
      ULONG id;
      WinQueryWindowRect(hwnd, &r);
      style = WinQueryWindowULong(hwnd, QWL_STYLE);
      drawBackground(hps, style, ((PSIZEL)&r) + 1);
      id = WinQueryWindowUShort(hwnd, QWS_ID);
      sprintf(buf, "Sample n. %d", id);
      if (id == 2) {
         r.xLeft = 4, r.yBottom = 40, r.xRight = 96, r.yTop = 60;
         WinDrawText(hps, -1, buf, &r, SYSCLR_MENUTEXT, SYSCLR_MENU,
                     DT_CENTER | DT_VCENTER | DT_ERASERECT);
      } else {
         WinDrawText(hps, -1, buf, &r, SYSCLR_WINDOWTEXT, SYSCLR_WINDOW,
                     DT_CENTER | DT_VCENTER | DT_TEXTATTRS);
      } /* endif */
      WinEndPaint(hps);
      return (MRESULT)FALSE;
   } /* endif */
   return WinDefWindowProc(hwnd, msg, mp1, mp2);
}


/* --------------------------------------------------------------------------
 9-parts bitmap drawing procedure.
*/

VOID drawBackground(HPS hps, ULONG style, PSIZEL psize) {
   // 9 parts bitmaps
   if (style & 1) {
      // bottom-left corner (not scaled)
      bmpRectPaintAt(hps, g.bmpp.hbmp, 0, 0, g.bmpp.dx, g.bmpp.dy, 0, 0);
      // left border (scaled vertically)
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         0, g.bmpp.dy, g.bmpp.dx, g.bmpp.cy - g.bmpp.dy,
                         0, g.bmpp.dy, g.bmpp.dx, psize->cy - g.bmpp.dy);
      // top-left corner (not scaled)
      bmpRectPaintAt(hps, g.bmpp.hbmp,
                    0, g.bmpp.cy - g.bmpp.dy, g.bmpp.dx, g.bmpp.cy,
                    0, psize->cy - g.bmpp.dy);
      // bottom border (scaled horizontally)
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         g.bmpp.dx, 0,
                         g.bmpp.cx - g.bmpp.dx, g.bmpp.dy,
                         g.bmpp.dx, 0,
                         psize->cx - g.bmpp.dx, g.bmpp.dy);
      // middle (scaled both horizontally and vertically
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         g.bmpp.dx, g.bmpp.dy,
                         g.bmpp.cx - g.bmpp.dx, g.bmpp.cy - g.bmpp.dy,
                         g.bmpp.dx, g.bmpp.dy,
                         psize->cx - g.bmpp.dx, psize->cy - g.bmpp.dy);
      // top border (scaled horizontally)
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         g.bmpp.dx, g.bmpp.cy - g.bmpp.dy,
                         g.bmpp.cx - g.bmpp.dx, g.bmpp.cy,
                         g.bmpp.dx, psize->cy - g.bmpp.dy,
                         psize->cx - g.bmpp.dx, psize->cy);
      // bottom-right corner (not scaled)
      bmpRectPaintAt(hps, g.bmpp.hbmp,
                    g.bmpp.cx - g.bmpp.dx, 0, g.bmpp.cx, g.bmpp.dy,
                    psize->cx - g.bmpp.dx, 0);
      // right border (scaled vertically)
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         g.bmpp.cx - g.bmpp.dx, g.bmpp.dy,
                         g.bmpp.cx, g.bmpp.cy - g.bmpp.dy,
                         psize->cx - g.bmpp.dx, g.bmpp.dy,
                         psize->cx, psize->cy - g.bmpp.dy);
      // top-right corner (not scaled)
      bmpRectPaintAt(hps, g.bmpp.hbmp,
                    g.bmpp.cx - g.bmpp.dx, g.bmpp.cy - g.bmpp.dy,
                    g.bmpp.cx, g.bmpp.cy,
                    psize->cx - g.bmpp.dx, psize->cy - g.bmpp.dy);
   // bitmap splitted in 3 parts by horizontal lines
   } else if (style & 2) {
      // bottom part
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         0, 0, g.bmpp.cx, g.bmpp.dy,
                         0, 0, psize->cx, g.bmpp.dy);
      // middle part
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         0, g.bmpp.dy, g.bmpp.cx, g.bmpp.cy - g.bmpp.dy,
                         0, g.bmpp.dy, psize->cx, psize->cy - g.bmpp.dy);
      // top part
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         0, g.bmpp.cy - g.bmpp.dy, g.bmpp.cx, g.bmpp.cy,
                         0, psize->cy - g.bmpp.dy, psize->cx, psize->cy);
   // bitmap splitted in 3 parts by vertical lines
   } else if (style & 4) {
      // left border
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         0, 0, g.bmpp.dx, g.bmpp.cy,
                         0, 0, g.bmpp.dx, psize->cy);
      // middle
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         g.bmpp.dx, 0, g.bmpp.cx - g.bmpp.dx, g.bmpp.cy,
                         g.bmpp.dx, 0, psize->cx - g.bmpp.dx, psize->cy);
      // right border
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         g.bmpp.cx - g.bmpp.dx, 0, g.bmpp.cx, g.bmpp.cy,
                         psize->cx - g.bmpp.dx, 0, psize->cx, psize->cy);
   // scaled bitmap
   } else if (style & 8) {
      bmpRectPaintToRect(hps, g.bmpp.hbmp,
                         0, 0, g.bmpp.cx, g.bmpp.cy,
                         0, 0, psize->cx, psize->cy);
   // untrasformed bitmap
   } else {
      bmpPaintAt(hps, g.bmpp.hbmp, 0, 0);
   } /* endif */
}


/* --------------------------------------------------------------------------
 Draw a whole bitmap at the coordinates x,y.
*/
BOOL bmpPaintAt(HPS hps, HBITMAP hbmp, INT x, INT y) {
   POINTL pt = {x, y};
   return WinDrawBitmap(hps, hbmp, NULL, &pt, 0, 0, DBM_NORMAL);
}


/* --------------------------------------------------------------------------
 Draw a part of a bitmap to a given position
*/
BOOL bmpRectPaintAt(HPS hps, HBITMAP hbmp,
                    INT x0s, INT y0s, INT x1s, INT y1s, INT x0d, INT y0d) {
   POINTL aptl[3];
   aptl[0].x = x0s, aptl[0].y = y0s;
   aptl[1].x = x1s, aptl[1].y = y1s;
   aptl[2].x = x0d, aptl[2].y = y0d;
   return WinDrawBitmap(hps, hbmp, (PRECTL)aptl, aptl + 2, 0, 0, DBM_NORMAL);
}


/* --------------------------------------------------------------------------
 Draw a part of a bitmap ( (x0s,y0s) - (x1s,y1s) ) scaling it to a given
 rectangle ( (x0d,y0d) - (x1d,y1d) ).
*/
BOOL bmpRectPaintToRect(HPS hps, HBITMAP hbmp,
                        INT x0s, INT y0s, INT x1s, INT y1s,
                        INT x0d, INT y0d, INT x1d, INT y1d) {
   POINTL aptl[4];
   aptl[0].x = x0s, aptl[0].y = y0s;
   aptl[1].x = x1s, aptl[1].y = y1s;
   aptl[2].x = x0d, aptl[2].y = y0d;
   aptl[3].x = x1d, aptl[3].y = y1d;
   return WinDrawBitmap(hps, hbmp, (PRECTL)aptl, aptl + 2, 0, 0, DBM_STRETCH);
}
