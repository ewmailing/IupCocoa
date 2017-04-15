PROJNAME = ShellExtensionHandler
OPT = YES

#DEFINE_THUMBNAILHANDLER = Yes
DEFINE_PREVIEWHANDLER = Yes

SRC = ClassFactory.cpp dllmain.cpp Reg.cpp
DEF_FILE = GlobalExportFunctions.def
LIBS = shlwapi
DEFINES = UNICODE 

ifdef DEFINE_THUMBNAILHANDLER
  LIBNAME := TecgrafThumbnailHandlerTGA
  DEFINES += DEFINE_THUMBNAILHANDLER
  USE_IM=Yes
  SRC += ThumbnailProvider.cpp imThumbnail.cpp
  LIBS += im_process
else
ifdef DEFINE_PREVIEWHANDLER
  LIBNAME := TecgrafPreviewHandlerTGA
  DEFINES += DEFINE_PREVIEWHANDLER
  USE_IUP=Yes
  USE_CD=Yes
  #USE_OPENGL = Yes
  #DEFINES += USE_OPENGL
  USE_IM=Yes
  SRC += PreviewHandler.cpp IupPreviewWindow.cpp
endif
endif
