PROJNAME = iup
LIBNAME = iupimglib

#Turn off optimization to speed up compilation
#OPT = YES

INCLUDES = ../include ../src

ifdef USE_IUP_IMGLIB_LARGE
  # if you want to include images larger than 48x48 (inclusive)
  DEFINES += IUP_IMGLIB_LARGE
endif

SRC = iup_image_library.c iup_imglib_bitmapsgtk.c iup_imglib_icons.c iup_imglib_logos.c

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  SRC += iup_imglib_bitmaps.c iup_imglib_logosw32.c
else
  # Used only by the Motif driver, but must include for all Posix because it is only one lib */
  SRC += iup_imglib_bitmaps8.c iup_imglib_icons8.c iup_imglib_logos8.c
endif

LIBS = iup
LDIR = ../lib/$(TEC_UNAME)

ifneq ($(findstring AIX, $(TEC_UNAME)), )
  DEFINES += IUP_IMGLIB_DUMMY
endif

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
