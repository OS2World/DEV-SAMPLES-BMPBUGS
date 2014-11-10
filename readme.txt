Introduction

I intended to paint titlebars and buttons using some nice looking
images. In order to get a consistent look regardless of the titlebar
or button sizes I decided to split the image in 9 parts: corners,
borders and middle part drawing them to the screen with the needed
transformations (i.e. scaling, no scaling, etc.).

This is a sample programs to show how WinDrawBitmaps (and other similar OS/2
APIs) do not work correctly when they have to scale a portion of a bitmap.

The test program takes a 200x30 pixels bitmap and uses various methods to
draw it to the screen.
The size of the samples 1-2 is fixed while the width of samples 3-6 changes
according to the width of the application window.

sample n. 1
  this is provided for comparison. The bitmap is painted without any
  transformation.

sample n. 2 (bug)
  the bitmap is stretched to a rectangle higher than the original bitmap
  heigth and narrower than the original bitmap width.
  The sample text is painted with a solid color background to make it
  readable. This sample shows how bitmap scaling fails when the bitmap
  size is increased in one direction and decreased in the other one.
  The SOFTDRAW.DLL versions distributed past the year 2000 fixes this bug
  only when the destination rectangle is lower and wider then the
  original bitmap rectangle.

sample n. 3 (bug ?)
  the whole bitmap is scaled to a rectangle that is lower and wider
  than the bitmap rectangle (the rectangle can become narrower if the
  application window is resized).
  This doesn't work correctly with older versions of SOFTDRAW.DLL.

sample n. 4 (bug)
  the bitmap is divided into a left border, a middle part and a right
  border. The borders are stretched along the vertical axis, while
  the middle part is stretched in both directions.
  Besides the bug previously described (if you have an old version of
  SOFTDRAW.DLL) another bug appears as soon as the sample window is
  narrowed so that the width of the samples 3-6 is less than the width
  of the reference bitmap (sample 1). The OS API ignores the left coordinate
  of the bitmap subrectangle (middle part - coordinate 10) but takes
  into account the whole bitmap (including the left border - ccordinate 0).
  So as soon as the application window is resized and sample 4 becomes
  narrower than sample 1 the middle part of the window will include the
  left border which will appear thicker on the screen.

sample n. 5 (bug)
  the bitmap is divided into a bottom border, a middle part and a top
  border. The borders are stretched along the horizontal axis only, while
  the middle part is stretched in both directions.
  The bug appears regardless of the sample size: the OS API swaps the
  position of the bottom and the top border.

sample n. 6 (bug)
  the bitmap is divided into 9 parts : 4 corners, left, bottom, right and
  top borders and a middle part. The corners are painted to the corners
  of the destination rectangle without any transformation, the left and
  right borders are stretched along the vertical axis, the bottom and
  top border are stretched along the horizontal axis and the middle part
  is stretched in both directions.
  This sample suffers of all possible bugs: sample 3 bug (if you have an
  old version of SOFTDRAW.DLL), sample 4 bug and sample 5 bug.

Probably there are other bugs which are not exposed by this sample
application.

Currently the only sure work around for these bugs is to split the
bitmap before drawing it, get 3-9 separate bitmap handles and draw
the individual bitmaps according to the needs.

