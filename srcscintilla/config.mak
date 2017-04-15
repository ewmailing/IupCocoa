PROJNAME = iup
LIBNAME = iup_scintilla
OPT = YES

ifdef DBG
  DEFINES += IUP_ASSERT
  ifndef DBG_DIR
    ifneq ($(findstring Win, $(TEC_SYSNAME)), )
      LIBNAME := $(LIBNAME)_debug
    endif
  endif
endif  

DEF_FILE = iup_scintilla.def

INCLUDES =  ../include ../src .
LDIR = ../lib/$(TEC_UNAME)
LIBS = iup

DEFINES += STATIC_BUILD SCI_LEXER SCI_NAMESPACE

# Supported only in Windows and GTK

ifeq ($(findstring Win, $(TEC_SYSNAME)), )
  # Force definition if not in Windows
  USE_GTK = Yes
endif

ifndef GTK_DEFAULT
  ifdef USE_GTK
    # Build GTK version in IRIX,SunOS,AIX,Win32
    LIBNAME := $(LIBNAME)gtk
  endif
endif

ifdef USE_GTK
  CHECK_GTK = Yes
  DEFINES += NO_CXX11_REGEX
  DEFINES += GTK GTK_DISABLE_DEPRECATED 
  ifdef USE_GTK3
    DEFINES += GDK_DISABLE_DEPRECATED GSEAL_ENABLE G_HAVE_ISO_VARARGS
  endif
  INCLUDES += ../src/gtk lexlib src include gtk
  INCLUDES += gtk
  ifneq ($(findstring cygw, $(TEC_UNAME)), )
    INCLUDES += $(GTK)/include/cairo
    LIBS += pangocairo-1.0 cairo
  endif
else
  INCLUDES += ../src/win lexlib src include win32
  INCLUDES += win
  LIBS += imm32
  DEFINES += UNICODE
  
  ifneq ($(findstring gcc, $(TEC_UNAME)), )
    DEFINES += _WIN32 DISABLE_D2D NO_CXX11_REGEX
    #FLAGS = -std=c++11
  endif
  ifneq ($(findstring dllg, $(TEC_UNAME)), )
    DEFINES += _WIN32 DISABLE_D2D NO_CXX11_REGEX
  endif
  ifneq ($(findstring mingw, $(TEC_UNAME)), )
    DEFINES += _WIN32 DISABLE_D2D NO_CXX11_REGEX
  endif
  ifneq ($(findstring dllw, $(TEC_UNAME)), )
    DEFINES += _WIN32 DISABLE_D2D NO_CXX11_REGEX
  endif
endif

SRCSCINTILLA = src/AutoComplete.cxx src/CallTip.cxx src/Catalogue.cxx src/CellBuffer.cxx src/CharClassify.cxx \
               src/ContractionState.cxx src/Decoration.cxx src/Document.cxx src/Editor.cxx src/ExternalLexer.cxx \
               src/Indicator.cxx src/KeyMap.cxx src/LineMarker.cxx src/PerLine.cxx src/PositionCache.cxx \
               src/RESearch.cxx src/RunStyles.cxx src/ScintillaBase.cxx src/Selection.cxx src/Style.cxx \
               src/UniConversion.cxx src/ViewStyle.cxx src/XPM.cxx src/CaseConvert.cxx src/CaseFolder.cxx \
               src/EditModel.cxx src/EditView.cxx src/MarginView.cxx

SRCSCINTILLA += lexers/LexA68k.cxx lexers/LexAbaqus.cxx lexers/LexAda.cxx lexers/LexAPDL.cxx \
				lexers/LexAsn1.cxx lexers/LexASY.cxx lexers/LexAU3.cxx lexers/LexAVE.cxx lexers/LexAVS.cxx \
				lexers/LexBaan.cxx lexers/LexBash.cxx lexers/LexBasic.cxx lexers/LexBullant.cxx lexers/LexCaml.cxx \
				lexers/LexCLW.cxx lexers/LexCmake.cxx lexers/LexCOBOL.cxx lexers/LexCoffeeScript.cxx \
				lexers/LexConf.cxx lexers/LexCPP.cxx lexers/LexCrontab.cxx lexers/LexCsound.cxx lexers/LexCSS.cxx \
				lexers/LexD.cxx lexers/LexECL.cxx lexers/LexEiffel.cxx lexers/LexErlang.cxx lexers/LexEScript.cxx \
				lexers/LexFlagship.cxx lexers/LexForth.cxx lexers/LexFortran.cxx lexers/LexGAP.cxx \
				lexers/LexGui4Cli.cxx lexers/LexHaskell.cxx lexers/LexHTML.cxx lexers/LexInno.cxx \
				lexers/LexKix.cxx lexers/LexLisp.cxx lexers/LexLout.cxx lexers/LexLua.cxx lexers/LexMagik.cxx \
				lexers/LexMarkdown.cxx lexers/LexMatlab.cxx lexers/LexMetapost.cxx lexers/LexMMIXAL.cxx \
				lexers/LexModula.cxx lexers/LexMPT.cxx lexers/LexMSSQL.cxx lexers/LexMySQL.cxx \
				lexers/LexNimrod.cxx lexers/LexNsis.cxx lexers/LexOpal.cxx lexers/LexOScript.cxx \
				lexers/LexPascal.cxx lexers/LexPB.cxx lexers/LexPerl.cxx \
				lexers/LexPLM.cxx lexers/LexPO.cxx lexers/LexPOV.cxx lexers/LexPowerPro.cxx \
				lexers/LexPowerShell.cxx lexers/LexProgress.cxx lexers/LexPS.cxx lexers/LexPython.cxx \
				lexers/LexR.cxx lexers/LexRebol.cxx lexers/LexRuby.cxx lexers/LexScriptol.cxx \
				lexers/LexSmalltalk.cxx lexers/LexSML.cxx lexers/LexSorcus.cxx lexers/LexSpecman.cxx \
				lexers/LexSpice.cxx lexers/LexSQL.cxx lexers/LexTACL.cxx lexers/LexTADS3.cxx lexers/LexTAL.cxx \
				lexers/LexTCL.cxx lexers/LexTCMD.cxx lexers/LexTeX.cxx lexers/LexTxt2tags.cxx lexers/LexVB.cxx \
				lexers/LexVerilog.cxx lexers/LexVHDL.cxx lexers/LexVisualProlog.cxx lexers/LexYAML.cxx \
        lexers/LexKVIrc.cxx lexers/LexLaTeX.cxx lexers/LexSTTXT.cxx lexers/LexRust.cxx \
        lexers/LexDMAP.cxx lexers/LexDMIS.cxx lexers/LexBibTeX.cxx lexers/LexHex.cxx lexers/LexAsm.cxx \
        lexers/LexRegistry.cxx lexers/LexBatch.cxx lexers/LexDiff.cxx lexers/LexErrorList.cxx \
        lexers/LexMake.cxx lexers/LexNull.cxx lexers/LexProps.cxx lexers/LexJSON.cxx
				
SRCSCINTILLA += lexlib/Accessor.cxx lexlib/CharacterSet.cxx lexlib/LexerBase.cxx lexlib/LexerModule.cxx \
                lexlib/LexerNoExceptions.cxx lexlib/LexerSimple.cxx lexlib/PropSetSimple.cxx \
                lexlib/StyleContext.cxx lexlib/WordList.cxx lexlib/CharacterCategory.cxx

ifdef USE_GTK
  SRCSCINTILLA += gtk/PlatGTK.cxx gtk/ScintillaGTK.cxx gtk/scintilla-marshal.c
else
  SRCSCINTILLA += win32/PlatWin.cxx win32/ScintillaWin.cxx win32/HanjaDic.cxx
endif

SRC = $(SRCSCINTILLA) iupsci_clipboard.c iupsci_folding.c iupsci_lexer.c iupsci_margin.c \
      iupsci_overtype.c iupsci_scrolling.c iupsci_selection.c iupsci_style.c iupsci_tab.c \
      iupsci_text.c iupsci_wordwrap.c iupsci_markers.c iupsci_bracelight.c iupsci_cursor.c \
      iupsci_whitespace.c iupsci_annotation.c iup_scintilla.cpp iupsci_autocompletion.c \
      iupsci_searching.c
      
ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
