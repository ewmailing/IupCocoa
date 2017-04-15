PROJNAME = iup
LIBNAME := iup
OPT = YES

ifeq "$(TEC_SYSNAME)" "Haiku"
  USE_HAIKU = Yes
else
ifdef GTK_DEFAULT
  ifdef USE_MOTIF
    # Build Motif version in Linux and BSD
    LIBNAME := $(LIBNAME)mot
  else
    ifeq ($(findstring Win, $(TEC_SYSNAME)), )
      # Force definition if not in Windows
      USE_GTK = Yes
    endif
  endif
else  
  ifdef USE_GTK
    # Build GTK version in IRIX,SunOS,AIX,Win32
    LIBNAME := $(LIBNAME)gtk
  else
    ifeq ($(findstring Win, $(TEC_SYSNAME)), )
      # Force definition if not in Windows
      USE_MOTIF = Yes
      USE_X11 = Yes
    endif
  endif
endif
endif

ifdef DBG
  DEFINES += IUP_ASSERT
endif  

INCLUDES = ../include .
# Windows XP minimum
WIN32VER = 0x0501

SRC = iup_array.c iup_callback.c iup_dlglist.c iup_attrib.c iup_focus.c iup_font.c \
      iup_globalattrib.c iup_object.c iup_key.c iup_layout.c iup_ledlex.c iup_names.c \
      iup_ledparse.c iup_predialogs.c iup_register.c iup_scanf.c iup_show.c iup_str.c \
      iup_func.c iup_childtree.c iup.c iup_classattrib.c iup_dialog.c iup_assert.c \
      iup_messagedlg.c iup_timer.c iup_image.c iup_label.c iup_fill.c iup_zbox.c \
      iup_colordlg.c iup_fontdlg.c iup_filedlg.c iup_strmessage.c iup_menu.c iup_frame.c \
      iup_user.c iup_button.c iup_radio.c iup_toggle.c iup_progressbar.c iup_text.c iup_val.c \
      iup_box.c iup_hbox.c iup_vbox.c iup_cbox.c iup_class.c iup_classbase.c iup_maskmatch.c \
      iup_mask.c iup_maskparse.c iup_tabs.c iup_spin.c iup_list.c iup_getparam.c iup_link.c \
      iup_sbox.c iup_scrollbox.c iup_normalizer.c iup_tree.c iup_split.c iup_layoutdlg.c \
      iup_recplay.c iup_progressdlg.c iup_expander.c iup_open.c iup_table.c iup_canvas.c \
      iup_gridbox.c iup_detachbox.c iup_backgroundbox.c iup_linefile.c iup_config.c \
      iup_flatbutton.c iup_animatedlabel.c iup_draw.c iup_flatframe.c iup_flattabs.c 

ifdef USE_HAIKU
  # Since Haiku has no GTK and no Motif, we can only use the native implementation
  SRC += haiku/iuphaiku_button.cpp haiku/iuphaiku_canvas.cpp haiku/iuphaiku_clipboard.c \
         haiku/iuphaiku_colordlg.c haiku/iuphaiku_common.cpp haiku/iuphaiku_dialog.cpp \
         haiku/iuphaiku_dragdrop.c haiku/iuphaiku_draw.c haiku/iuphaiku_filedlg.cpp \
         haiku/iuphaiku_focus.cpp haiku/iuphaiku_font.cpp haiku/iuphaiku_fontdlg.c \
         haiku/iuphaiku_frame.cpp haiku/iuphaiku_globalattrib.c haiku/iuphaiku_help.c \
         haiku/iuphaiku_image.cpp haiku/iuphaiku_info.cpp haiku/iuphaiku_label.cpp \
         haiku/iuphaiku_list.cpp haiku/iuphaiku_loop.cpp haiku/iuphaiku_menu.cpp \
         haiku/iuphaiku_messagedlg.c haiku/iuphaiku_open.cpp haiku/iuphaiku_progressbar.cpp \
         haiku/iuphaiku_tabs.cpp haiku/iuphaiku_text.cpp haiku/iuphaiku_timer.cpp \
         haiku/iuphaiku_tips.c haiku/iuphaiku_toggle.cpp haiku/iuphaiku_tree.cpp \
         haiku/iuphaiku_val.cpp gtk/iupgtk_draw_cairo.c gtk/iupgtk_key.c
  INCLUDES += haiku
#  DEFINES += 
else
ifdef USE_GTK
  CHECK_GTK = Yes
  DEFINES += GTK_DISABLE_DEPRECATED 
  ifdef USE_GTK3
    DEFINES += GDK_DISABLE_DEPRECATED GSEAL_ENABLE
  endif
  INCLUDES += gtk
  SRC += gtk/iupgtk_focus.c gtk/iupgtk_clipboard.c gtk/iupgtk_val.c \
         gtk/iupgtk_globalattrib.c gtk/iupgtk_key.c gtk/iupgtk_tips.c \
         gtk/iupgtk_loop.c gtk/iupgtk_open.c gtk/iupgtk_messagedlg.c \
         gtk/iupgtk_timer.c gtk/iupgtk_label.c gtk/iupgtk_font.c \
         gtk/iupgtk_colordlg.c gtk/iupgtk_fontdlg.c gtk/iupgtk_filedlg.c \
         gtk/iupgtk_button.c gtk/iupgtk_toggle.c gtk/iupgtk_dragdrop.c \
         gtk/iupgtk_text.c gtk/iupgtk_frame.c gtk/iupgtk_progressbar.c \
         gtk/iupgtk_tabs.c gtk/iupgtk_list.c gtk/iupgtk_tree.c \
         gtk/iupgtk_canvas.c gtk/iupgtk_image.c gtk/iupgtk_dialog.c \
         gtk/iupgtk_common.c gtk/iupgtk_str.c gtk/iupgtk_menu.c \
         gtk/iupgtk_calendar.c iup_datepick.c
           
  ifdef USE_GTK3
    SRC += gtk/iupgtk_draw_cairo.c
  else
    SRC += gtk/iupgtk_draw.c
  endif
  
  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    DEFINES += _WIN32_WINNT=$(WIN32VER) _WIN32_IE=0x600 WINVER=$(WIN32VER) NOTREEVIEW
    SRC += win/iupwindows_main.c win/iupwindows_help.c win/iupwindows_info.c
  else
    ifdef GTK_MAC
      SRC += gtk/iupmac_help.c gtk/iupmac_info.c
      DEFINES += GTK_MAC
    else
      USE_X11 = Yes
      SRC += gtk/iupgtk_help.c mot/iupunix_info.c
    endif
  endif
  
  ifdef USE_HILDON
    DEFINES += HILDON
    STDINCS += /usr/include/hildon-1
    LIBS += hildon-1
  endif
else
ifdef USE_MOTIF
  SRC += mot/iupmot_common.c mot/iupmot_color.c mot/iupmot_focus.c mot/iupmot_font.c \
         mot/iupmot_key.c mot/iupmot_loop.c mot/iupmot_open.c mot/iupmot_tips.c \
         mot/iupmot_globalattrib.c mot/iupmot_dialog.c mot/iupmot_messagedlg.c mot/iupmot_draw.c \
         mot/iupmot_timer.c mot/iupmot_image.c mot/iupmot_label.c mot/iupmot_canvas.c \
         mot/iupmot_colordlg.c mot/iupmot_fontdlg.c mot/iupmot_filedlg.c mot/iupmot_frame.c \
         mot/iupmot_button.c mot/iupmot_toggle.c mot/iupmot_progressbar.c mot/iupmot_clipboard.c \
         mot/iupmot_text.c mot/iupmot_val.c mot/iupmot_tabs.c mot/iupmot_menu.c \
         mot/iupmot_list.c mot/iupmot_tree.c mot/iupmot_dragdrop.c mot/iupmot_str.c \
         mot/iupmot_calendar.c iup_datepick.c
         
  SRC += mot/iupunix_help.c mot/iupunix_info.c
  USE_X11 = Yes

  INCLUDES += mot
else
  SRC += win/iupwin_common.c win/iupwin_brush.c win/iupwin_focus.c win/iupwin_font.c \
         win/iupwin_globalattrib.c win/iupwin_handle.c win/iupwin_key.c win/iupwin_str.c \
         win/iupwin_loop.c win/iupwin_open.c win/iupwin_tips.c win/iupwin_info.c \
         win/iupwin_dialog.c win/iupwin_messagedlg.c win/iupwin_timer.c \
         win/iupwin_image.c win/iupwin_label.c win/iupwin_canvas.c win/iupwin_frame.c \
         win/iupwin_colordlg.c win/iupwin_fontdlg.c win/iupwin_filedlg.c win/iupwin_dragdrop.c \
         win/iupwin_button.c win/iupwin_draw.c win/iupwin_toggle.c win/iupwin_clipboard.c \
         win/iupwin_progressbar.c win/iupwin_text.c win/iupwin_val.c win/iupwin_touch.c \
         win/iupwin_tabs.c win/iupwin_menu.c win/iupwin_list.c win/iupwin_tree.c \
         win/iupwin_calendar.c win/iupwin_datepick.c
         
  SRC += win/iupwindows_main.c win/iupwindows_help.c win/iupwindows_info.c

  INCLUDES += win
  DEFINES += _WIN32_WINNT=$(WIN32VER) _WIN32_IE=0x600 WINVER=$(WIN32VER) NOTREEVIEW
  DEFINES += UNICODE
endif
endif
endif

ifneq ($(findstring dll, $(TEC_UNAME)), )
  DEFINES += IUP_DLL
  INCLUDES += ../etc
  SRC += ../etc/iup.rc
  ifdef USE_GTK
    DEF_FILE = iupgtk.def
  else
    DEF_FILE = iup.def
  endif
endif

ifeq "$(TEC_UNAME)" "vc6"
  # Necessary because VC6 has an old WinSDK
  PLATSDK = $(VC7)/PlatformSDK
endif

ifeq "$(TEC_UNAME)" "dll"
  # Necessary because VC6 has an old WinSDK
  PLATSDK = $(VC7)/PlatformSDK
endif

ifeq "$(TEC_UNAME)" "owc1"
  # Necessary or IUP 3 will not work in Open Watcom (needs to be retested)
  DBG=Yes
endif

ifeq "$(TEC_SYSNAME)" "SunOS"
  # Necessary or the fileopen will not work in SunOS (needs to be retested)
  #DEFINES += NO_PATH_MODE_RELATIVE
endif

ifneq ($(findstring MacOS, $(TEC_UNAME)), )
  ifneq ($(TEC_SYSMINOR), 4)
    BUILD_DYLIB=Yes
  endif
endif
