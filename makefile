# To add new extensions to the inference rules uncomment and redefine this:
#.SUFFIXES:
#
#.SUFFIXES: \
#    .C .obj .rc .res

# compiler, linker, resource compiler, resource binder MACRO

CC = icc.exe
CL = ilink.exe
RC = rc.exe
RB = rc.exe

# compiler and linker flags

# Debug version
!ifdef DEBUG

# CFLAGS = /I..\..\headers /I..\..\lvm_tk /Ss /Ti /Rn /G5 /C
CFLAGS = /Ss /Ti /Rn /G5 /C
LFLAGS = /DE /E:2 /PM:PM /PACKD /A:4 /OPTF /NOL /M /L

!else
# RELEASE version

CFLAGS = /Ss /O /Oc /Ol /Rn /G5 /C
LFLAGS = /E:2 /PM:PM /PACKD /A:4 /OPTF /NOL /M /L

!endif

# RCFLAGS = -r -i ..\..\headers
RCFLAGS = -r
RBFLAGS = -x2

.rc.res:
   $(RC) $(RCFLAGS) $<

.C.obj:
   $(CC) $(CFLAGS) $<

all: test.exe

test.exe: \
   test.obj \
   test.res
   $(CL) @<<
      $(LFLAGS)
      /O:test.exe
      test.obj
    <<
    $(RB) $(RBFLAGS) test.res test.exe

test.res: \
   test.rc \
   test.h
