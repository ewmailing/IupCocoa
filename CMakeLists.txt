# For Emscripten, invoke something like:
# source ~/emsdk_portable/emsdk_env.sh;
# mkdir BUILD; cd BUILD;
# cmake -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release ..

# For Mac, you must use the Xcode generator. Makefile is currently broken.
# mkdir BUILD; cd BUILD;
# cmake -GXcode ..

# For iOS, you must use the Xcode generator and the bundled toolchain
# mkdir BUILD; cd BUILD;
# cmake -DCMAKE_TOOLCHAIN_FILE=../CMakeModules/iOS.toolchain.cmake -GXcode ..


if(APPLE)
	# We're using the new Mac RPATH policy.
	CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
else()
	# Ubuntu 12.04 LTS ships 2.8.7
	CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
endif()

PROJECT(IUP)

INCLUDE(CMakeDependentOption)
INCLUDE(CheckFunctionExists)
#SET(CPACK_SET_DESTDIR TRUE)
#INCLUDE(CPack)

OPTION(IUP_BUILD_TESTS "Build tests" OFF)

IF(EMSCRIPTEN)
	OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" OFF)
ELSEIF(APPLE)
	IF(IOS)
		OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" OFF)
	ELSE()
		OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)
		CMAKE_DEPENDENT_OPTION(WANTS_BUILD_FRAMEWORK "Set to ON to build framework instead of dylib. Only valid if BUILD_SHARED_LIBRARY is ON an is OS X." ON "WANTS_BUILD_SHARED_LIBRARY" ON)
	ENDIF()
ELSEIF(UNIX)
	OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)

ELSE()
	OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)
ENDIF()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules/" ${CMAKE_MODULE_PATH})
INCLUDE(HelperFunctions)

IF(EMSCRIPTEN)
		OPTION(IUP_OPTION_ANDROID "Use Android backend" OFF)
		OPTION(IUP_OPTION_COCOA "Use Cocoa backend" OFF)
		OPTION(IUP_OPTION_COCOATOUCH "Use CocoaTouch backend" OFF)
		OPTION(IUP_OPTION_EMSCRIPTEN "Use Emscripten backend" ON)		
		OPTION(IUP_OPTION_GTK2 "Use GTK2 backend" OFF)
		OPTION(IUP_OPTION_GTK3 "Use GTK3 backend" OFF)
		OPTION(IUP_OPTION_MOTIF "Use Motif backend" OFF)
		OPTION(IUP_OPTION_WIN32 "Use Win32 backend" OFF)

ELSEIF(APPLE)
	IF(IOS)
		OPTION(IUP_OPTION_ANDROID "Use Android backend" OFF)
		OPTION(IUP_OPTION_COCOA "Use Cocoa backend" OFF)
		OPTION(IUP_OPTION_COCOATOUCH "Use CocoaTouch backend" ON)
		OPTION(IUP_OPTION_EMSCRIPTEN "Use Emscripten backend" OFF)		
		OPTION(IUP_OPTION_GTK2 "Use GTK2 backend" OFF)
		OPTION(IUP_OPTION_GTK3 "Use GTK3 backend" OFF)
		OPTION(IUP_OPTION_MOTIF "Use Motif backend" OFF)
		OPTION(IUP_OPTION_WIN32 "Use Win32 backend" OFF)

	ELSE()
		OPTION(IUP_OPTION_ANDROID "Use Android backend" OFF)
		OPTION(IUP_OPTION_COCOA "Use Cocoa backend" ON)
		OPTION(IUP_OPTION_COCOATOUCH "Use CocoaTouch backend" OFF)
		OPTION(IUP_OPTION_EMSCRIPTEN "Use Emscripten backend" OFF)		
		OPTION(IUP_OPTION_GTK2 "Use GTK2 backend" OFF)
		OPTION(IUP_OPTION_GTK3 "Use GTK3 backend" OFF)
		OPTION(IUP_OPTION_MOTIF "Use Motif backend" OFF)
		OPTION(IUP_OPTION_WIN32 "Use Win32 backend" OFF)

	ENDIF()

ELSEIF(WIN32)
	OPTION(IUP_OPTION_ANDROID "Use Android backend" OFF)
	OPTION(IUP_OPTION_COCOA "Use Cocoa backend" OFF)
	OPTION(IUP_OPTION_COCOATOUCH "Use CocoaTouch backend" OFF)
	OPTION(IUP_OPTION_EMSCRIPTEN "Use Emscripten backend" OFF)		
	OPTION(IUP_OPTION_GTK2 "Use GTK2 backend" OFF)
	OPTION(IUP_OPTION_GTK3 "Use GTK3 backend" OFF)
	OPTION(IUP_OPTION_MOTIF "Use Motif backend" OFF)
	OPTION(IUP_OPTION_WIN32 "Use Win32 backend" ON)

ELSEIF(UNIX)
	# WARNING: I know GTK can be used on Windows and Mac, 
	# but this file currently does not support that configuration.
	# To make that work, the FIND stuff needs to be re-run 

	OPTION(IUP_OPTION_ANDROID "Use Android backend" OFF)
	OPTION(IUP_OPTION_COCOA "Use Cocoa backend" OFF)
	OPTION(IUP_OPTION_COCOATOUCH "Use CocoaTouch backend" OFF)
	OPTION(IUP_OPTION_EMSCRIPTEN "Use Emscripten backend" OFF)		
	OPTION(IUP_OPTION_WIN32 "Use Win32 backend" OFF)

	
	# Not sure the best way of doing this.
	# Find both GTK2 and GTK3 in case the user wants to change the config.
	# Otherwise I might accidentally prevent detection.
	find_package(GTK2 2.0 QUIET COMPONENTS gtk)
	# Use the package PkgConfig to detect GTK+ headers/library files
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 gtk+-3.0)

	find_package(Motif QUIET)

	if(GTK2_FOUND)
		OPTION(IUP_OPTION_GTK2 "Use GTK2 backend" ON)
		OPTION(IUP_OPTION_GTK3 "Use GTK3 backend" OFF)
		OPTION(IUP_OPTION_MOTIF "Use Motif backend" OFF)
	elseif(GTK3_FOUND)
		OPTION(IUP_OPTION_GTK2 "Use GTK2 backend" OFF)
		OPTION(IUP_OPTION_GTK3 "Use GTK3 backend" ON)
		OPTION(IUP_OPTION_MOTIF "Use Motif backend" OFF)
	elseif(MOTIF_FOUND)
		OPTION(IUP_OPTION_GTK2 "Use GTK2 backend" OFF)
		OPTION(IUP_OPTION_GTK3 "Use GTK3 backend" OFF)
		OPTION(IUP_OPTION_MOTIF "Use Motif backend" ON)
	else()
		MESSAGE(FATAL_ERROR "Platform backend dependencies were not found.")
	endif()

ENDIF()

IF(IUP_OPTION_EMSCRIPTEN)
	SET(IUP_USE_EMSCRIPTEN 1)
ELSEIF(IUP_OPTION_ANDROID)
	SET(IUP_USE_ANDROID 1)
ELSEIF(IUP_OPTION_COCOA)
	SET(IUP_USE_COCOA 1)
ELSEIF(IUP_OPTION_COCOATOUCH)
	SET(IUP_USE_COCOATOUCH 1)
ELSEIF(IUP_OPTION_GTK2)
	SET(IUP_USE_GTK2 1)

	# NOTE: This has limited effect since I already ran these for the Option selections on Unix.
	# This is here only for people who select this on the platforms where GTK is non-native (Mac, Windows)
	find_package(GTK2 2.0 REQUIRED COMPONENTS gtk)

	if(NOT GTK2_FOUND)
		MESSAGE(FATAL_ERROR "GTK2 dependencies not found")
	endif()

ELSEIF(IUP_OPTION_GTK3)
	SET(IUP_USE_GTK3 1)

	# NOTE: This has limited effect since I already ran these for the Option selections on Unix.
	# This is here only for people who select this on the platforms where GTK is non-native (Mac, Windows)
	# Use the package PkgConfig to detect GTK+ headers/library files
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

	if(NOT GTK3_FOUND)
		MESSAGE(FATAL_ERROR "GTK3 dependencies not found")
	endif()

ELSEIF(IUP_OPTION_MOTIF)
	SET(IUP_USE_MOTIF 1)

	# NOTE: This has limited effect since I already ran these for the Option selections on Unix.
	# This is here only for people who select this on the platforms where GTK is non-native (Mac, Windows)
	find_package(Motif REQUIRED)

	if(NOT MOTIF_FOUND)
		MESSAGE(FATAL_ERROR "Motif dependencies not found")
	endif()

ELSEIF(IUP_OPTION_WIN32)
	SET(IUP_USE_WIN32 1)
ELSE()
	MESSAGE(FATAL_ERROR "Unsupported platform backend")

ENDIF()

# RPATH stuff. This MUST be set before any targets that use it are declared.
# Only affects shared libraries.
if(WANTS_BUILD_SHARED_LIBRARY)
	HELPER_SETUP_RPATH()
endif()


SET(IUP_C_FLAGS "")
SET(IUPEXE_C_FLAGS "")
SET(IUPEXE_LINK_FLAGS "")

# TODO: We must add visibility annotations to IUP to support Emscripten.
# We can then reuse them for other platforms too.
IF(IUP_USE_EMSCRIPTEN)
	# we always need to set this when building a libray with Emscripten
	SET(IUP_C_FLAGS "${IUP_C_FLAGS} -DIUP_BUILD_LIBRARY")
	# we always need to set this when using an Emscripten built library, otherwise symbols get stripped.
	SET(IUPEXE_C_FLAGS "${IUPEXE_C_FLAGS} -DIUP_BUILD_LIBRARY")

	# Without this, all executables only generate .js files without the html wrapper.
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")

	# IUP executables will always need this switch because IUP is event driven and doesn't quit when Emscripten tries to run/quit main().
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s NO_EXIT_RUNTIME=1")



ELSEIF(WANTS_BUILD_SHARED_LIBRARY)
	IF("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
		SET(IUP_C_FLAGS "${IUP_C_FLAGS} -fvisibility=hidden")
	ELSEIF("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
		SET(IUP_C_FLAGS "${IUP_C_FLAGS} -fvisibility=hidden")
	ENDIF()
	
	SET(IUP_C_FLAGS "${IUP_C_FLAGS} -DIUP_BUILD_LIBRARY")

ENDIF()



SET(IUP_PUBLIC_HEADERS
	${CMAKE_SOURCE_DIR}/include/iup.h
	${CMAKE_SOURCE_DIR}/include/iupcbs.h
	${CMAKE_SOURCE_DIR}/include/iupdef.h
	${CMAKE_SOURCE_DIR}/include/iupkey.h
	${CMAKE_SOURCE_DIR}/include/iup_config.h
)

SET(IUP_COMMON_SOURCE_FILES 
	# ${CMAKE_SOURCE_DIR}/src/iup.def

	${CMAKE_SOURCE_DIR}/src/iup.c
	${CMAKE_SOURCE_DIR}/src/iup_array.c
	${CMAKE_SOURCE_DIR}/src/iup_array.h
	${CMAKE_SOURCE_DIR}/src/iup_assert.c
	${CMAKE_SOURCE_DIR}/src/iup_assert.h
	${CMAKE_SOURCE_DIR}/src/iup_attrib.c
	${CMAKE_SOURCE_DIR}/src/iup_attrib.h
	${CMAKE_SOURCE_DIR}/src/iup_backgroundbox.c
	${CMAKE_SOURCE_DIR}/src/iup_box.c
	${CMAKE_SOURCE_DIR}/src/iup_box.h
	${CMAKE_SOURCE_DIR}/src/iup_button.c
	${CMAKE_SOURCE_DIR}/src/iup_button.h
	${CMAKE_SOURCE_DIR}/src/iup_callback.c
	${CMAKE_SOURCE_DIR}/src/iup_canvas.c
	${CMAKE_SOURCE_DIR}/src/iup_canvas.h
	${CMAKE_SOURCE_DIR}/src/iup_cbox.c
	${CMAKE_SOURCE_DIR}/src/iup_childtree.c
	${CMAKE_SOURCE_DIR}/src/iup_childtree.h
	${CMAKE_SOURCE_DIR}/src/iup_class.c
	${CMAKE_SOURCE_DIR}/src/iup_class.h
	${CMAKE_SOURCE_DIR}/src/iup_classattrib.c
	${CMAKE_SOURCE_DIR}/src/iup_classbase.c
	${CMAKE_SOURCE_DIR}/src/iup_classbase.h
	${CMAKE_SOURCE_DIR}/src/iup_colordlg.c
	${CMAKE_SOURCE_DIR}/src/iup_config.c
	${CMAKE_SOURCE_DIR}/src/iup_detachbox.c
	${CMAKE_SOURCE_DIR}/src/iup_dialog.c
	${CMAKE_SOURCE_DIR}/src/iup_dialog.h
	${CMAKE_SOURCE_DIR}/src/iup_dlglist.c
	${CMAKE_SOURCE_DIR}/src/iup_dlglist.h
	${CMAKE_SOURCE_DIR}/src/iup_draw.h
	${CMAKE_SOURCE_DIR}/src/iup_drv.h
	${CMAKE_SOURCE_DIR}/src/iup_drvfont.h
	${CMAKE_SOURCE_DIR}/src/iup_drvinfo.h
	${CMAKE_SOURCE_DIR}/src/iup_expander.c
	${CMAKE_SOURCE_DIR}/src/iup_filedlg.c
	${CMAKE_SOURCE_DIR}/src/iup_fill.c
	${CMAKE_SOURCE_DIR}/src/iup_flatbutton.c
	${CMAKE_SOURCE_DIR}/src/iup_focus.c
	${CMAKE_SOURCE_DIR}/src/iup_focus.h
	${CMAKE_SOURCE_DIR}/src/iup_font.c
	${CMAKE_SOURCE_DIR}/src/iup_fontdlg.c
	${CMAKE_SOURCE_DIR}/src/iup_frame.c
	${CMAKE_SOURCE_DIR}/src/iup_frame.h
	${CMAKE_SOURCE_DIR}/src/iup_func.c
	${CMAKE_SOURCE_DIR}/src/iup_func.h
	${CMAKE_SOURCE_DIR}/src/iup_getparam.c
	${CMAKE_SOURCE_DIR}/src/iup_globalattrib.c
	${CMAKE_SOURCE_DIR}/src/iup_globalattrib.h
	${CMAKE_SOURCE_DIR}/src/iup_gridbox.c
	${CMAKE_SOURCE_DIR}/src/iup_hbox.c
	${CMAKE_SOURCE_DIR}/src/iup_image.c
	${CMAKE_SOURCE_DIR}/src/iup_image.h
	${CMAKE_SOURCE_DIR}/src/iup_key.c
	${CMAKE_SOURCE_DIR}/src/iup_key.h
	${CMAKE_SOURCE_DIR}/src/iup_label.c
	${CMAKE_SOURCE_DIR}/src/iup_label.h
	${CMAKE_SOURCE_DIR}/src/iup_layout.c
	${CMAKE_SOURCE_DIR}/src/iup_layout.h
	${CMAKE_SOURCE_DIR}/src/iup_layoutdlg.c
	${CMAKE_SOURCE_DIR}/src/iup_ledlex.c
	${CMAKE_SOURCE_DIR}/src/iup_ledlex.h
	${CMAKE_SOURCE_DIR}/src/iup_ledparse.c
	${CMAKE_SOURCE_DIR}/src/iup_linefile.c
	${CMAKE_SOURCE_DIR}/src/iup_linefile.h
	${CMAKE_SOURCE_DIR}/src/iup_link.c
	${CMAKE_SOURCE_DIR}/src/iup_list.c
	${CMAKE_SOURCE_DIR}/src/iup_list.h
	${CMAKE_SOURCE_DIR}/src/iup_mask.c
	${CMAKE_SOURCE_DIR}/src/iup_mask.h
	${CMAKE_SOURCE_DIR}/src/iup_maskmatch.c
	${CMAKE_SOURCE_DIR}/src/iup_maskmatch.h
	${CMAKE_SOURCE_DIR}/src/iup_maskparse.c
	${CMAKE_SOURCE_DIR}/src/iup_maskparse.h
	${CMAKE_SOURCE_DIR}/src/iup_menu.c
	${CMAKE_SOURCE_DIR}/src/iup_menu.h
	${CMAKE_SOURCE_DIR}/src/iup_messagedlg.c
	${CMAKE_SOURCE_DIR}/src/iup_names.c
	${CMAKE_SOURCE_DIR}/src/iup_names.h
	${CMAKE_SOURCE_DIR}/src/iup_normalizer.c
	${CMAKE_SOURCE_DIR}/src/iup_normalizer.h
	${CMAKE_SOURCE_DIR}/src/iup_object.c
	${CMAKE_SOURCE_DIR}/src/iup_object.h
	${CMAKE_SOURCE_DIR}/src/iup_open.c
	${CMAKE_SOURCE_DIR}/src/iup_predialogs.c
	${CMAKE_SOURCE_DIR}/src/iup_predialogs.h
	${CMAKE_SOURCE_DIR}/src/iup_progressbar.c
	${CMAKE_SOURCE_DIR}/src/iup_progressbar.h
	${CMAKE_SOURCE_DIR}/src/iup_progressdlg.c
	${CMAKE_SOURCE_DIR}/src/iup_radio.c
	${CMAKE_SOURCE_DIR}/src/iup_recplay.c
	${CMAKE_SOURCE_DIR}/src/iup_register.c
	${CMAKE_SOURCE_DIR}/src/iup_register.h
	${CMAKE_SOURCE_DIR}/src/iup_sbox.c
	${CMAKE_SOURCE_DIR}/src/iup_scanf.c
	${CMAKE_SOURCE_DIR}/src/iup_scrollbox.c
	${CMAKE_SOURCE_DIR}/src/iup_show.c
	${CMAKE_SOURCE_DIR}/src/iup_spin.c
	${CMAKE_SOURCE_DIR}/src/iup_split.c
	${CMAKE_SOURCE_DIR}/src/iup_stdcontrols.h
	${CMAKE_SOURCE_DIR}/src/iup_str.c
	${CMAKE_SOURCE_DIR}/src/iup_str.h
	${CMAKE_SOURCE_DIR}/src/iup_strmessage.c
	${CMAKE_SOURCE_DIR}/src/iup_strmessage.h
	${CMAKE_SOURCE_DIR}/src/iup_table.c
	${CMAKE_SOURCE_DIR}/src/iup_table.h
	${CMAKE_SOURCE_DIR}/src/iup_tabs.c
	${CMAKE_SOURCE_DIR}/src/iup_tabs.h
	${CMAKE_SOURCE_DIR}/src/iup_text.c
	${CMAKE_SOURCE_DIR}/src/iup_text.h
	${CMAKE_SOURCE_DIR}/src/iup_timer.c
	${CMAKE_SOURCE_DIR}/src/iup_timer.h
	${CMAKE_SOURCE_DIR}/src/iup_toggle.c
	${CMAKE_SOURCE_DIR}/src/iup_toggle.h
	${CMAKE_SOURCE_DIR}/src/iup_tree.c
	${CMAKE_SOURCE_DIR}/src/iup_tree.h
	${CMAKE_SOURCE_DIR}/src/iup_user.c
	${CMAKE_SOURCE_DIR}/src/iup_val.c
	${CMAKE_SOURCE_DIR}/src/iup_val.h
	${CMAKE_SOURCE_DIR}/src/iup_vbox.c
	${CMAKE_SOURCE_DIR}/src/iup_zbox.c
)


SET(IUP_LINK_LIBRARIES "")
SET(IUP_SOURCE_FILES ${IUP_COMMON_SOURCE_FILES})
SET(IUP_FOREIGN_SOURCE_FILES "")

IF(IUP_USE_EMSCRIPTEN)

	SET(IUP_EMSCRIPTEN_SOURCE_FILES 
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_button.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_canvas.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_clipboard.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_colordlg.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_common.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_dialog.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_dragdrop.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_draw.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_filedlg.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_focus.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_font.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_fontdlg.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_frame.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_globalattrib.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_help.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_image.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_info.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_key.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_label.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_list.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_loop.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_menu.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_messagedlg.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_open.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_progressbar.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_tabs.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_text.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_timer.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_tips.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_toggle.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_tree.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_val.c
	)

	SET(IUP_EMSCRIPTEN_FOREIGN_SOURCE_FILES 
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_button.js
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_common.js
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_dialog.js
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_label.js
	)

	LIST(APPEND IUP_SOURCE_FILES
			${IUP_EMSCRIPTEN_SOURCE_FILES}
		)
	LIST(APPEND IUP_FOREIGN_SOURCE_FILES
			${IUP_EMSCRIPTEN_FOREIGN_SOURCE_FILES}
		)
ELSEIF(IUP_USE_ANDROID)
	
ELSEIF(IUP_USE_COCOATOUCH)

	SET(IUP_COCOATOUCH_SOURCE_FILES 
		${CMAKE_SOURCE_DIR}/src/cocoatouch/IupAppDelegate.h
		${CMAKE_SOURCE_DIR}/src/cocoatouch/IupAppDelegate.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/IupAppDelegateProtocol.h
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_button.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_canvas.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_clipboard.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_colordlg.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_common.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_dialog.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_dragdrop.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_draw.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_drv.h
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_filedlg.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_focus.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_font.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_fontdlg.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_frame.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_globalattrib.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_help.c
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_image.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_info.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_key.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_label.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_list.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_loop.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_menu.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_messagedlg.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_open.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_progressbar.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_tabs.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_text.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_timer.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_tips.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_toggle.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_tree.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_val.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/IupLaunchViewController.h
		${CMAKE_SOURCE_DIR}/src/cocoatouch/IupLaunchViewController.m
#		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupmac_info.m
	)

	LIST(APPEND IUP_SOURCE_FILES
		${IUP_COCOATOUCH_SOURCE_FILES}
	)

	FIND_LIBRARY(APPLE_UIKIT_LIBRARY UIKit)
	FIND_LIBRARY(APPLE_FOUNDATION_LIBRARY Foundation)
	FIND_LIBRARY(APPLE_QUARTZCORE_LIBRARY QuartzCore)
	FIND_LIBRARY(APPLE_COREGRAPHICS_LIBRARY CoreGraphics)
	LIST(APPEND IUP_LINK_LIBRARIES 
			${APPLE_UIKIT_LIBRARY}
			${APPLE_QUARTZCORE_LIBRARY}
			${APPLE_COREGRAPHICS_LIBRARY}
			${APPLE_FOUNDATION_LIBRARY}
	)

ELSEIF(IUP_USE_COCOA)

	SET(IUP_COCOA_SOURCE_FILES 
		${CMAKE_SOURCE_DIR}/src/cocoa/IupAppDelegate.h
		${CMAKE_SOURCE_DIR}/src/cocoa/IupAppDelegate.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_button.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_canvas.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_clipboard.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_colordlg.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_common.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_dialog.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_dragdrop.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_draw.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_drv.h
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_filedlg.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_focus.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_font.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_fontdlg.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_frame.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_image.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_info.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_label.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_list.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_loop.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_menu.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_messagedlg.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_open.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_progressbar.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_tabs.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_text.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_timer.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_tips.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_toggle.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_tree.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_val.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupmac_globalattrib.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupmac_help.m
		# This file was replaced with iupcocoa_info.m
#		${CMAKE_SOURCE_DIR}/src/cocoa/iupmac_info.m
		# File does not compile
#		${CMAKE_SOURCE_DIR}/src/cocoa/iupmac_key.m

		# Stop gap
#		${CMAKE_SOURCE_DIR}/src/gtk/iupmac_help.c
	)

	LIST(APPEND IUP_SOURCE_FILES
		${IUP_COCOA_SOURCE_FILES}
	)
	# Not sure how to handle GNUStep
	FIND_LIBRARY(APPLE_COCOA_LIBRARY Cocoa)
	FIND_LIBRARY(APPLE_QUARTZCORE_LIBRARY QuartzCore)
	FIND_LIBRARY(APPLE_SYSTEMCONFIGURATION_LIBRARY SystemConfiguration)
	LIST(APPEND IUP_LINK_LIBRARIES 
		${APPLE_COCOA_LIBRARY}
		${APPLE_QUARTZCORE_LIBRARY}
		${APPLE_SYSTEMCONFIGURATION_LIBRARY}
	)

ELSEIF(IUP_USE_WIN32)


	SET(IUP_WIN32_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/win/iupwin_brush.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_brush.h
		${CMAKE_SOURCE_DIR}/src/win/iupwin_button.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_canvas.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_clipboard.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_colordlg.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_common.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_dialog.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_dragdrop.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_draw.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_draw.h
		${CMAKE_SOURCE_DIR}/src/win/iupwin_drv.h
		${CMAKE_SOURCE_DIR}/src/win/iupwin_filedlg.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_focus.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_font.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_fontdlg.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_frame.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_globalattrib.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_handle.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_handle.h
		${CMAKE_SOURCE_DIR}/src/win/iupwin_image.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_info.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_info.h
		${CMAKE_SOURCE_DIR}/src/win/iupwin_key.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_label.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_list.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_loop.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_menu.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_messagedlg.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_open.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_progressbar.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_str.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_str.h
		${CMAKE_SOURCE_DIR}/src/win/iupwin_tabs.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_text.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_timer.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_tips.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_toggle.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_touch.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_tree.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_val.c
		${CMAKE_SOURCE_DIR}/src/win/iupwindows_help.c
		${CMAKE_SOURCE_DIR}/src/win/iupwindows_info.c
		${CMAKE_SOURCE_DIR}/src/win/iupwindows_main.c
	)

	LIST(APPEND IUP_SOURCE_FILES
		${IUP_WIN32_SOURCE_FILES}
	)

	FIND_LIBRARY(WINDOWS_COMCTL_LIBRARY comtl32)
	LIST(APPEND IUP_LINK_LIBRARIES 
		${WINDOWS_COMCTL_LIBRARY}
	)
	IF(MSVC)
		ADD_DEFINITIONS(/D _CRT_SECURE_NO_WARNINGS)
	ENDIF()

ELSEIF(IUP_USE_GTK3 OR IUP_USE_GTK2)

	SET(IUP_GTK_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_button.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_canvas.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_clipboard.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_colordlg.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_common.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_dialog.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_dragdrop.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_drv.h
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_filedlg.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_focus.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_font.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_fontdlg.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_frame.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_globalattrib.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_image.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_key.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_label.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_list.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_loop.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_menu.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_messagedlg.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_open.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_progressbar.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_str.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_tabs.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_text.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_timer.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_tips.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_toggle.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_tree.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_val.c
	)
	LIST(APPEND IUP_SOURCE_FILES
		${IUP_GTK_SOURCE_FILES}
	)

	SET(IUP_GTK2_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_draw.c
	)

	SET(IUP_GTK3_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_draw_cairo.c
	)

	# These files are platform specific for the GTK backend
	IF(APPLE)
		LIST(APPEND IUP_SOURCE_FILES
			${CMAKE_SOURCE_DIR}/src/gtk/iupmac_help.c
			${CMAKE_SOURCE_DIR}/src/gtk/iupmac_info.c
		)
	ELSEIF(WIN32)
		LIST(APPEND IUP_SOURCE_FILES
			${CMAKE_SOURCE_DIR}/src/win/iupwindows_help.c
			${CMAKE_SOURCE_DIR}/src/win/iupwindows_info.c
			${CMAKE_SOURCE_DIR}/src/win/iupwindows_main.c
		)
	ELSEIF(UNIX)
		LIST(APPEND IUP_SOURCE_FILES
			${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_help.c
			${CMAKE_SOURCE_DIR}/src/mot/iupunix_info.c
		)

	ENDIF()

	IF(IUP_USE_GTK2)
		LIST(APPEND IUP_SOURCE_FILES
			${IUP_GTK2_SOURCE_FILES}
		)
		INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS})
		LIST(APPEND IUP_LINK_LIBRARIES 
			${GTK2_LIBRARIES}
		)

	ELSEIF(IUP_USE_GTK3)
		LIST(APPEND IUP_SOURCE_FILES
			${IUP_GTK3_SOURCE_FILES}
		)
		INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
		LIST(APPEND IUP_LINK_LIBRARIES 
			${GTK3_LIBRARIES}
		)
		
	ELSE()
		MESSAGE("Unsupported GTK version")
	ENDIF()

ELSEIF(IUP_USE_MOTIF)

	SET(IUP_MOTIF_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_button.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_canvas.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_clipboard.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_color.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_color.h
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_colordlg.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_common.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_dialog.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_dragdrop.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_draw.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_drv.h
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_filedlg.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_focus.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_font.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_fontdlg.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_frame.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_globalattrib.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_image.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_key.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_label.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_list.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_loop.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_menu.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_messagedlg.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_open.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_progressbar.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_str.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_tabs.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_text.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_timer.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_tips.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_toggle.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_tree.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_val.c
		${CMAKE_SOURCE_DIR}/src/mot/iupunix_help.c
		${CMAKE_SOURCE_DIR}/src/mot/iupunix_info.c
	)
	LIST(APPEND IUP_SOURCE_FILES
		${IUP_MOTIF_SOURCE_FILES}
	)
	INCLUDE_DIRECTORIES(${MOTIF_INCLUDE_DIRS})
	LIST(APPEND IUP_LINK_LIBRARIES 
		${MOTIF_LIBRARIES}
	)

ELSE()
	MESSAGE(FATAL_ERROR "Unhandled platform backend")

ENDIF()

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/src
)
HELPER_CREATE_LIBRARY(iup "${WANTS_BUILD_SHARED_LIBRARY}" "${WANTS_BUILD_FRAMEWORK}" "${IUP_SOURCE_FILES}" "${IUP_PUBLIC_HEADERS}" "" "" "${IUP_FOREIGN_SOURCE_FILES}" "${IUP_LINK_LIBRARIES}" "${IUP_C_FLAGS}" "br.pucrio.tecgraf.iup")


# Decided to put everything in this CMakeLists.txt to make keeping in sync easier since this isn't official.
#ADD_SUBDIRECTORY(test)

function(BUILD_IUP_TEST exe_name source_list)
	HELPER_CREATE_EXECUTABLE(${exe_name} 
		"${source_list}"
		"${WANTS_BUILD_SHARED_LIBRARY}"
		"iup"
		"${IUP_LINK_LIBRARIES}"
		"${IUPEXE_C_FLAGS}"
		"${IUPEXE_LINK_FLAGS}"
	)
endfunction()


IF(IUP_BUILD_TESTS)

	BUILD_IUP_TEST(button "${CMAKE_SOURCE_DIR}/test/button.c")


ENDIF()

# Hack: Used to build simple test programs as we bootstrap each new implementation
if(IUP_USE_EMSCRIPTEN)
	BUILD_IUP_TEST(dev_test "${CMAKE_SOURCE_DIR}/src/emscripten/test/main_c.c")
elseif(IUP_USE_COCOATOUCH)
	BUILD_IUP_TEST(dev_test "${CMAKE_SOURCE_DIR}/XcodeiOS/iup/TestApp/main.c")
elseif(IUP_USE_COCOA)
	BUILD_IUP_TEST(dev_test "${CMAKE_SOURCE_DIR}/Xcode/iup/TestApp/main.c")
endif()



# For uninstall
HELPER_SETUP_UNINSTALL_TARGET()

