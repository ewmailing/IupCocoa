# For Emscripten, invoke something like:
# source ~/emsdk_portable/emsdk_env.sh;
# mkdir BUILD; cd BUILD;
# cmake -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release ..

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
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
	OPTION(IUP_OPTION_ANDROID "Use Android backend" OFF)
	OPTION(IUP_OPTION_COCOA "Use Cocoa backend" OFF)
	OPTION(IUP_OPTION_COCOATOUCH "Use CocoaTouch backend" OFF)
	OPTION(IUP_OPTION_EMSCRIPTEN "Use Emscripten backend" OFF)		
	OPTION(IUP_OPTION_GTK2 "Use GTK2 backend" ON)
	OPTION(IUP_OPTION_GTK3 "Use GTK3 backend" OFF)
	OPTION(IUP_OPTION_MOTIF "Use Motif backend" OFF)
	OPTION(IUP_OPTION_WIN32 "Use Win32 backend" OFF)
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
ELSEIF(IUP_OPTION_GTK3)
	SET(IUP_USE_GTK3 1)
ELSEIF(IUP_OPTION_MOTIF)
	SET(IUP_USE_MOTIF 1)
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
	FIND_LIBRARY(APPLE_UIKIT_LIBRARY UIKit)
	FIND_LIBRARY(APPLE_FOUNDATION_LIBRARY Foundation)
	LIST(APPEND IUP_LINK_LIBRARIES 
			${APPLE_UIKIT_LIBRARY}
			${APPLE_FOUNDATION_LIBRARY}
	)

ELSEIF(IUP_USE_COCOA)
	LIST(APPEND IUP_SOURCE_FILES
		${IUP_COCOA_SOURCE_FILES}
	)
	# Not sure how to handle GNUStep
	FIND_LIBRARY(APPLE_COCOA_LIBRARY Cocoa)
	LIST(APPEND IUP_LINK_LIBRARIES 
		${APPLE_COCOA_LIBRARY}
	)

ELSEIF(IUP_USE_WIN32)

	FIND_LIBRARY(WINDOWS_COMCTL_LIBRARY comtl32)
	LIST(APPEND IUP_LINK_LIBRARIES 
		${WINDOWS_COMCTL_LIBRARY}
	)
	IF(MSVC)
		ADD_DEFINITIONS(/D _CRT_SECURE_NO_WARNINGS)
	ENDIF()

ELSEIF(IUP_USE_GTK3)
ELSEIF(IUP_USE_GTK2)

ELSEIF(IUP_USE_MOTIF)

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



IF(IUP_BUILD_TESTS)

	HELPER_CREATE_EXECUTABLE(button 
		"${CMAKE_SOURCE_DIR}/test/button.c"
		"${WANTS_BUILD_SHARED_LIBRARY}"
		"iup"
		"${IUP_LINK_LIBRARIES}"
		"${IUPEXE_C_FLAGS}"
		"${IUPEXE_LINK_FLAGS}"
	)


ENDIF()

# Hack: Used to build simple test programs as we bootstrap each new implementation
if(IUP_USE_EMSCRIPTEN)
	HELPER_CREATE_EXECUTABLE(dev_test 
		"${CMAKE_SOURCE_DIR}/src/emscripten/test/main_c.c"
		"${WANTS_BUILD_SHARED_LIBRARY}"
		"iup"
		"${IUP_LINK_LIBRARIES}"
		"${IUPEXE_C_FLAGS}"
		"${IUPEXE_LINK_FLAGS}"
	)
endif()



# For uninstall
HELPER_SETUP_UNINSTALL_TARGET()

