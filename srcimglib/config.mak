PROJNAME = iup
LIBNAME = iupimglib

#Turn off optimization to speed up compilation
#OPT = YES

INCLUDES = ../include ../src

ifdef USE_IUP_IMGLIB_LARGE
  # if you want to include images larger than 48x48 (inclusive)
  DEFINES += IUP_IMGLIB_LARGE
endif

SRC = iup_image_library.c iup_imglib_circleprogress.c

ifneq ($(findstring Win, $(TEC_SYSNAME)), )
  #SRC += iup_imglib_basewin16x16.c
  SRC += iup_imglib_basewin32x32.c
  SRC += iup_imglib_logos48x48.c iup_imglib_logos32x32.c
  SRC += iup_imglib_iconswin48x48.c
else
  ifdef GTK_DEFAULT
    SRC += iup_imglib_basegtk16x16.c 
    SRC += iup_imglib_logos48x48.c iup_imglib_logos32x32.c
    SRC += iup_imglib_iconsgtk48x48.c
  else
    SRC += iup_imglib_basemot16x16_8bpp.c
    SRC +=  iup_imglib_logos48x48_8bpp.c iup_imglib_logos32x32_8bpp.c
  endif
endif

LIBS = iup
LDIR = ../lib/$(TEC_UNAME)

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
