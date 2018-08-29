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

if(POLICY CMP0054)
	cmake_policy(SET CMP0054 NEW)
endif()

PROJECT(IUP)

INCLUDE(CMakeDependentOption)
INCLUDE(CheckFunctionExists)
#SET(CPACK_SET_DESTDIR TRUE)
#INCLUDE(CPack)

# By default, we'll exclude the tests from the ALL target. But for convenience, we also will provide a testsiup aggregate target.
OPTION(IUP_BUILD_TESTS "Build tests with ALL target" OFF)
SET(IUP_TESTS_CUSTOM_TARGET_NAME "testsiup")
ADD_CUSTOM_TARGET(${IUP_TESTS_CUSTOM_TARGET_NAME})

IF(EMSCRIPTEN)
	# Emscripten documentation says .so is better than .a, but also implies .bc may be best.
	# TODO: Figure out how .bc fits into all of this.
	OPTION(WANTS_BUILD_SHARED_LIBRARY "Build dynamic library" ON)
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
IF(APPLE)
	# FIXME: Expand to support GNUStep
	INCLUDE(HelperApple)
ENDIF()

OPTION(IUP_OPTION_LUA "Build Lua if available" ON)
# I'm keeping this off by default for now, because of a cross-dependency problem of other iuplua* modules depending on symbols in iuplua.
# http://lua-users.org/lists/lua-l/2015-05/msg00263.html
# It's giving me a lot of pain on Mac, and I worry about iOS, Android, and Emscripten, and how it affects portability of your code.
# I worry some platforms may not be able to support modules at all.
# And unfortunately, it seems that your setup code will need to differ depending on which you use,
# which will make writing your cross-platform code more painful.
OPTION(IUP_OPTION_LUA_BUILD_AS_MODULES "Build IupLua* as modules instead of dynamic libraries (when not static)" OFF)

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

ELSEIF(ANDROID)
	OPTION(IUP_OPTION_ANDROID "Use Android backend" ON)
	OPTION(IUP_OPTION_COCOA "Use Cocoa backend" OFF)
	OPTION(IUP_OPTION_COCOATOUCH "Use CocoaTouch backend" OFF)
	OPTION(IUP_OPTION_EMSCRIPTEN "Use Emscripten backend" OFF)		
	OPTION(IUP_OPTION_GTK2 "Use GTK2 backend" OFF)
	OPTION(IUP_OPTION_GTK3 "Use GTK3 backend" OFF)
	OPTION(IUP_OPTION_MOTIF "Use Motif backend" OFF)
	OPTION(IUP_OPTION_WIN32 "Use Win32 backend" OFF)

	MESSAGE("In android")

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
	find_package(PkgConfig REQUIRED)
	# I originally tried find_package() for 2.0 but linking failed on Ubuntu 16.10 (but worked on 12.04)
	# pkg_check_modules with 2.0 fixed the problem.
	#find_package(GTK2 2.0 QUIET COMPONENTS gtk)
	pkg_check_modules(GTK2 gtk+-2.0)
	# Use the package PkgConfig to detect GTK+ headers/library files
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

	MESSAGE("option gtk2")
	# NOTE: This has limited effect since I already ran these for the Option selections on Unix.
	# This is here only for people who select this on the platforms where GTK is non-native (Mac, Windows)
	find_package(GTK2 2.0 REQUIRED COMPONENTS gtk)

	if(NOT GTK2_FOUND)
		MESSAGE(FATAL_ERROR "GTK2 dependencies not found")
	endif()

	pkg_check_modules(WEBKIT QUIET webkit-1.0)
	if(NOT WEBKIT_FOUND)
		pkg_check_modules(WEBKIT QUIET webkitgtk-2.0)
	endif()

ELSEIF(IUP_OPTION_GTK3)
	SET(IUP_USE_GTK3 1)

	MESSAGE("option gtk3")
	# NOTE: This has limited effect since I already ran these for the Option selections on Unix.
	# This is here only for people who select this on the platforms where GTK is non-native (Mac, Windows)
	# Use the package PkgConfig to detect GTK+ headers/library files
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

	if(NOT GTK3_FOUND)
		MESSAGE(FATAL_ERROR "GTK3 dependencies not found")
	endif()

	pkg_check_modules(WEBKIT QUIET webkit-1.0)
	if(NOT WEBKIT_FOUND)
		pkg_check_modules(WEBKIT QUIET webkitgtk-3.0)
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
	# Needed for UTF8MODE on Windows
	OPTION(IUP_OPTION_USE_UNICODE "Comple with Unidode" ON)
ELSE()
	MESSAGE(FATAL_ERROR "Unsupported platform backend")

ENDIF()

# Not sure what the default should be for this, and if it should be backend dependent
OPTION(IUPIMGLIB_OPTION_USE_IMGLIB_LARGE "Include images larger than 48x48 (inclusive) for imglib" ON)


FIND_PACKAGE(CD QUIET)
IF(CD_FOUND)
	SET(IUP_USE_CD 1)
	MESSAGE("CD found")
ELSE()
	MESSAGE("CD not found")
ENDIF()


SET(IUP_USE_LUA 0)
IF(IUP_OPTION_LUA)
message("init LUA_LIBRARY: ${LUA_LIBRARY}")
message("init LUA_INCLUDE_DIR: ${LUA_INCLUDE_DIR}")
	MESSAGE("finding lua package")
	FIND_PACKAGE(Lua QUIET)
	IF(LUA_FOUND)
		SET(IUP_USE_LUA 1)
		MESSAGE("Lua found")
	ELSE()
		MESSAGE("Lua not found")
	ENDIF()
ENDIF()

# RPATH stuff. This MUST be set before any targets that use it are declared.
# Only affects shared libraries.
if(WANTS_BUILD_SHARED_LIBRARY)
	HELPER_SETUP_RPATH()
endif()


SET(IUP_C_FLAGS "")
SET(IUPIMGLIB_C_FLAGS "")
SET(IUPOLE_C_FLAGS "")
# Do we need CXX flags for web because of Windows?
SET(IUPWEB_C_FLAGS "")
SET(IUPCD_C_FLAGS "")
SET(IUPCONTROLS_C_FLAGS "")
SET(IUPLUA_C_FLAGS "")
SET(IUPLUAWEB_C_FLAGS "")
SET(IUPEXE_C_FLAGS "")
SET(IUPEXE_LINK_FLAGS "")

MESSAGE("CMAKE_C_COMPILER_ID ${CMAKE_C_COMPILER_ID}")
IF("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
	SET(CMAKE_C_FLAGS "-Wno-invalid-source-encoding ${CMAKE_C_FLAGS}")
	SET(CMAKE_CXX_FLAGS "-Wno-invalid-source-encoding ${CMAKE_CXX_FLAGS}")
ELSEIF("${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
	SET(CMAKE_C_FLAGS "-Wno-invalid-source-encoding ${CMAKE_C_FLAGS}")
	SET(CMAKE_CXX_FLAGS "-Wno-invalid-source-encoding ${CMAKE_CXX_FLAGS}")
ENDIF()


# TODO: We must add visibility annotations to IUP to support Emscripten.
# We can then reuse them for other platforms too.
IF(IUP_USE_EMSCRIPTEN)
	# we always need to set this when building a libray with Emscripten
	SET(IUP_C_FLAGS "${IUP_C_FLAGS} -DIUP_BUILD_LIBRARY -DIUP_DLL -DIUPWEB_BUILD_LIBRARY")
	# we always need to set this when using an Emscripten built library, otherwise symbols get stripped.
	SET(IUPEXE_C_FLAGS "${IUPEXE_C_FLAGS} -DIUP_BUILD_LIBRARY -DIUP_DLL -DIUPWEB_BUILD_LIBRARY")

	# Without this, all executables only generate .js files without the html wrapper.
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")

	# IUP executables will always need this switch because IUP is event driven and doesn't quit when Emscripten tries to run/quit main().
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s NO_EXIT_RUNTIME=1")



ELSEIF(WANTS_BUILD_SHARED_LIBRARY)
	IF("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
		SET(IUP_C_FLAGS "${IUP_C_FLAGS} -fvisibility=hidden")
		SET(IUPIMGLIB_C_FLAGS "${IUPIMGLIB_C_FLAGS} -fvisibility=hidden")
		SET(IUPOLE_C_FLAGS "${IUPOLE_C_FLAGS} -fvisibility=hidden")
		SET(IUPWEB_C_FLAGS "${IUPWEB_C_FLAGS} -fvisibility=hidden")
		SET(IUPCD_C_FLAGS "${IUPCD_C_FLAGS} -fvisibility=hidden")
		SET(IUPCONTROLS_C_FLAGS "${IUPCONTROLS_C_FLAGS} -fvisibility=hidden")
		SET(IUPLUA_C_FLAGS "${IUPLUA_C_FLAGS} -fvisibility=hidden")
		SET(IUPLUAWEB_C_FLAGS "${IUPLUAWEB_C_FLAGS} -fvisibility=hidden")

	ELSEIF("${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
		SET(IUP_C_FLAGS "${IUP_C_FLAGS} -fvisibility=hidden")
		SET(IUPIMGLIB_C_FLAGS "${IUPIMGLIB_C_FLAGS} -fvisibility=hidden")
		SET(IUPOLE_C_FLAGS "${IUPOLE_C_FLAGS} -fvisibility=hidden")
		SET(IUPWEB_C_FLAGS "${IUPWEB_C_FLAGS} -fvisibility=hidden")
		SET(IUPCD_C_FLAGS "${IUPCD_C_FLAGS} -fvisibility=hidden")
		SET(IUPCONTROLS_C_FLAGS "${IUPCONTROLS_C_FLAGS} -fvisibility=hidden")
		SET(IUPLUAWEB_C_FLAGS "${IUPLUAWEB_C_FLAGS} -fvisibility=hidden")

	ENDIF()
	
	SET(IUP_C_FLAGS "${IUP_C_FLAGS} -DIUP_BUILD_LIBRARY -DIUP_DLL")
		
	# Yes, IUP_BUILD_LIBRARY and not IUPIMG_BUILD_LIBRARY because the header is in iup.h
	SET(IUPIMGLIB_C_FLAGS "${IUPIMGLIB_C_FLAGS} -DIUP_BUILD_LIBRARY -DIUP_DLL")
	SET(IUPOLE_C_FLAGS "${IUPOLE_C_FLAGS} -DIUPOLE_BUILD_LIBRARY")
	SET(IUPWEB_C_FLAGS "${IUPWEB_C_FLAGS} -DIUPWEB_BUILD_LIBRARY")
	SET(IUPCD_C_FLAGS "${IUPCD_C_FLAGS} -DIUPCD_BUILD_LIBRARY")
	SET(IUPCONTROLS_C_FLAGS "${IUPCONTROLS_C_FLAGS} -DIUPCONTROLS_BUILD_LIBRARY")
	SET(IUPLUA_C_FLAGS "${IUPLUA_C_FLAGS} -DIUPLUA_BUILD_LIBRARY")
	SET(IUPLUAWEB_C_FLAGS "${IUPLUAWEB_C_FLAGS} -DIUPLUAWEB_BUILD_LIBRARY")

ENDIF()

IF(MSVC)
	# Kind of a hack. Need to get Windows exe's to link and not show a console window. Not sure why it is defaulting to /SUBSYSTEM:CONSOLE even though WIN32 is specified in the ADD_EXECUTABLE
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
ENDIF()

IF(IUPIMGLIB_OPTION_USE_IMGLIB_LARGE)
	SET(IUPIMGLIB_C_FLAGS "${IUPIMGLIB_C_FLAGS} -DIUP_IMAGELIB_LARGE")
ENDIF()



SET(IUP_PUBLIC_HEADERS
	${CMAKE_SOURCE_DIR}/include/iup.h
	${CMAKE_SOURCE_DIR}/include/iupcbs.h
	${CMAKE_SOURCE_DIR}/include/iupdef.h
	${CMAKE_SOURCE_DIR}/include/iupdraw.h
	${CMAKE_SOURCE_DIR}/include/iupkey.h
	${CMAKE_SOURCE_DIR}/include/iup_config.h
	${CMAKE_SOURCE_DIR}/include/iup_varg.h
)
SET(IUPIMGLIB_PUBLIC_HEADERS
)
SET(IUPOLE_PUBLIC_HEADERS
	${CMAKE_SOURCE_DIR}/include/iupole.h
)
SET(IUPWEB_PUBLIC_HEADERS
	${CMAKE_SOURCE_DIR}/include/iupweb.h
)
SET(IUPCD_PUBLIC_HEADERS
)
SET(IUPCONTROLS_PUBLIC_HEADERS
	${CMAKE_SOURCE_DIR}/include/iupcontrols.h
)
SET(IUPLUA_PUBLIC_HEADERS
	${CMAKE_SOURCE_DIR}/include/iuplua.h
)

SET(IUPLUAWEB_PUBLIC_HEADERS
	${CMAKE_SOURCE_DIR}/include/iupluaweb.h
)


SET(IUP_COMMON_SOURCE_FILES 
	# ${CMAKE_SOURCE_DIR}/src/iup.def

	${CMAKE_SOURCE_DIR}/src/iup.c
	${CMAKE_SOURCE_DIR}/src/iup_animatedlabel.c
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
	${CMAKE_SOURCE_DIR}/src/iup_colorbar.c
	${CMAKE_SOURCE_DIR}/src/iup_colorbrowser.c
	${CMAKE_SOURCE_DIR}/src/iup_colordlg.c
	${CMAKE_SOURCE_DIR}/src/iup_colorhsi.c
	${CMAKE_SOURCE_DIR}/src/iup_colorhsi.h
	${CMAKE_SOURCE_DIR}/src/iup_config.c
#	${CMAKE_SOURCE_DIR}/src/iup_datepick.c
	${CMAKE_SOURCE_DIR}/src/iup_detachbox.c
	${CMAKE_SOURCE_DIR}/src/iup_dial.c
	${CMAKE_SOURCE_DIR}/src/iup_dialog.c
	${CMAKE_SOURCE_DIR}/src/iup_dialog.h
	${CMAKE_SOURCE_DIR}/src/iup_dlglist.c
	${CMAKE_SOURCE_DIR}/src/iup_dlglist.h
	${CMAKE_SOURCE_DIR}/src/iup_draw.c
	${CMAKE_SOURCE_DIR}/src/iup_draw.h
	${CMAKE_SOURCE_DIR}/src/iup_dropbutton.c
	${CMAKE_SOURCE_DIR}/src/iup_drv.h
	${CMAKE_SOURCE_DIR}/src/iup_drvdraw.h
	${CMAKE_SOURCE_DIR}/src/iup_drvfont.h
	${CMAKE_SOURCE_DIR}/src/iup_drvinfo.h
	${CMAKE_SOURCE_DIR}/src/iup_expander.c
	${CMAKE_SOURCE_DIR}/src/iup_filedlg.c
	${CMAKE_SOURCE_DIR}/src/iup_fill.c
	${CMAKE_SOURCE_DIR}/src/iup_flatbutton.c
	${CMAKE_SOURCE_DIR}/src/iup_flatframe.c
	${CMAKE_SOURCE_DIR}/src/iup_flatlabel.c
	${CMAKE_SOURCE_DIR}/src/iup_flatscrollbar.c
	${CMAKE_SOURCE_DIR}/src/iup_flatscrollbar.h
	${CMAKE_SOURCE_DIR}/src/iup_flatscrollbox.c
	${CMAKE_SOURCE_DIR}/src/iup_flatseparator.c
	${CMAKE_SOURCE_DIR}/src/iup_flattabs.c
	${CMAKE_SOURCE_DIR}/src/iup_flattoggle.c
	${CMAKE_SOURCE_DIR}/src/iup_focus.c
	${CMAKE_SOURCE_DIR}/src/iup_focus.h
	${CMAKE_SOURCE_DIR}/src/iup_font.c
	${CMAKE_SOURCE_DIR}/src/iup_fontdlg.c
	${CMAKE_SOURCE_DIR}/src/iup_frame.c
	${CMAKE_SOURCE_DIR}/src/iup_frame.h
	${CMAKE_SOURCE_DIR}/src/iup_func.c
	${CMAKE_SOURCE_DIR}/src/iup_func.h
	${CMAKE_SOURCE_DIR}/src/iup_gauge.c
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
	${CMAKE_SOURCE_DIR}/src/iup_loop.c
	${CMAKE_SOURCE_DIR}/src/iup_loop.h
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
SET(IUPIMGLIB_COMMON_SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/srcimglib/iup_image_library.c
	${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_circleprogress.c
)
SET(IUPOLE_COMMON_SOURCE_FILES 
)
SET(IUPWEB_COMMON_SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/srcweb/iup_webbrowser.c
	${CMAKE_SOURCE_DIR}/srcweb/iup_webbrowser.h
)
SET(IUPCD_COMMON_SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/srccd/iup_cd.c
	${CMAKE_SOURCE_DIR}/srccd/iup_cdutil.c
	${CMAKE_SOURCE_DIR}/srccd/iup_cdutil.h
)
SET(IUPCONTROLS_COMMON_SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/srccontrols/iup_cells.c
	${CMAKE_SOURCE_DIR}/srccontrols/iup_controls.c
	${CMAKE_SOURCE_DIR}/srccontrols/iup_controls.h
	${CMAKE_SOURCE_DIR}/srccontrols/iup_matrixlist.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_aux.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_aux.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_cd.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_colres.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_colres.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_def.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_draw.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_draw.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_edit.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_edit.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_ex.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_getset.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_getset.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_key.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_key.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_mark.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_mark.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_mem.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_mem.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_mouse.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_mouse.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_numlc.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_numlc.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_scroll.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmat_scroll.h
	${CMAKE_SOURCE_DIR}/srccontrols/matrix/iupmatrix.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iupmatex_busy.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iupmatex_clipboard.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iupmatex_copy.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iupmatex_export.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iupmatex_find.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iupmatex_sort.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iupmatex_undo.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iupmatex_units.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iupmatex_visible.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iup_matrixex.c
	${CMAKE_SOURCE_DIR}/srccontrols/matrixex/iup_matrixex.h
)

SET(IUP_LINK_LIBRARIES "")
SET(IUP_SOURCE_FILES ${IUP_COMMON_SOURCE_FILES})
SET(IUP_FOREIGN_SOURCE_FILES "")

SET(IUPIMGLIB_LINK_LIBRARIES "iup")
SET(IUPIMGLIB_SOURCE_FILES ${IUPIMGLIB_COMMON_SOURCE_FILES})
SET(IUPIMGLIB_FOREIGN_SOURCE_FILES "")

SET(IUPOLE_LINK_LIBRARIES "")
SET(IUPOLE_SOURCE_FILES ${IUPOLE_COMMON_SOURCE_FILES})
SET(IUPOLE_FOREIGN_SOURCE_FILES "")

SET(IUPWEB_LINK_LIBRARIES "iup")
SET(IUPWEB_SOURCE_FILES ${IUPWEB_COMMON_SOURCE_FILES})
SET(IUPWEB_FOREIGN_SOURCE_FILES "")

SET(IUP_SHOULD_BUILD_IUPWEB FALSE)

SET(IUPCD_LINK_LIBRARIES "iup;${CD_LIBRARY}")
SET(IUPCD_SOURCE_FILES ${IUPCD_COMMON_SOURCE_FILES})
SET(IUPCD_FOREIGN_SOURCE_FILES "")
SET(IUP_SHOULD_BUILD_IUPCD FALSE)

SET(IUPCONTROLS_LINK_LIBRARIES "iupcd;iup")
SET(IUPCONTROLS_SOURCE_FILES ${IUPCONTROLS_COMMON_SOURCE_FILES})
SET(IUPCONTROLS_FOREIGN_SOURCE_FILES "")
SET(IUP_SHOULD_BUILD_IUPCONTROLS FALSE)

IF(IUP_USE_EMSCRIPTEN)

	SET(IUP_EMSCRIPTEN_SOURCE_FILES 
		# TODO: Get clarification on iup_datepick.c
		${CMAKE_SOURCE_DIR}/src/iup_datepick.c

		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_button.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_calendar.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_canvas.c
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_clipboard.c
#		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_colordlg.c # Needs to be deleted
		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_common.c
#		${CMAKE_SOURCE_DIR}/src/emscripten/iupemscripten_datepick.c
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


	SET(IUPIMGLIB_EMSCRIPTEN_SOURCE_FILES
		# TODO:
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos48x48.c 
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos32x32.c
	)
	SET(IUPIMGLIB_EMSCRIPTEN_FOREIGN_SOURCE_FILES 
	)

	LIST(APPEND IUPIMGLIB_SOURCE_FILES
		${IUPIMGLIB_EMSCRIPTEN_SOURCE_FILES}
	)
	LIST(APPEND IUPIMGLIB_FOREIGN_SOURCE_FILES
		${IUPIMGLIB_EMSCRIPTEN_FOREIGN_SOURCE_FILES}
	)


	SET(IUPWEB_EMSCRIPTEN_SOURCE_FILES
		# TODO:
	)
	SET(IUPWEB_EMSCRIPTEN_FOREIGN_SOURCE_FILES 
	)

	LIST(APPEND IUPWEB_SOURCE_FILES
		${IUPWEB_EMSCRIPTEN_SOURCE_FILES}
	)
	LIST(APPEND IUPWEB_FOREIGN_SOURCE_FILES
		${IUPWEB_EMSCRIPTEN_FOREIGN_SOURCE_FILES}
	)

	SET(IUP_SHOULD_BUILD_IUPWEB FALSE)


ELSEIF(IUP_USE_ANDROID)

		SET(IUP_ANDROID_SOURCE_FILES 
		# TODO: Get clarification on iup_datepick.c
		${CMAKE_SOURCE_DIR}/src/iup_datepick.c

		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_button.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_calendar.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_canvas.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_clipboard.c
#		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_colordlg.c # Needs to be deleted
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_common.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_dialog.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_dragdrop.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_draw.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_drv.h
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_filedlg.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_focus.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_font.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_fontdlg.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_frame.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_globalattrib.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_help.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_image.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_info.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_jni_IupApplication.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_jni_IupActivity.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_jni_IupCommon.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_jni_IupLaunchActivity.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_jnicacheglobals.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_jnicacheglobals.h
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_jnimacros.h
		#${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_key.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_label.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_list.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_loop.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_menu.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_messagedlg.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_open.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_progressbar.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_tabs.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_text.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_timer.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_tips.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_toggle.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_tree.c
		${CMAKE_SOURCE_DIR}/src/android/source/iupandroid_val.c
	)

	LIST(APPEND IUP_SOURCE_FILES
		${IUP_ANDROID_SOURCE_FILES}
	)

	FIND_LIBRARY(ANDROID_LOG_LIBRARY log)
	FIND_LIBRARY(ANDROID_JNIGRAPHICS_LIBRARY jnigraphics)

	LIST(APPEND IUP_LINK_LIBRARIES 
			${ANDROID_JNIGRAPHICS_LIBRARY}
			${ANDROID_LOG_LIBRARY}
	)


	SET(IUPIMGLIB_ANDROID_SOURCE_FILES
		# TODO:
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos48x48.c 
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos32x32.c
	)
	LIST(APPEND IUPIMGLIB_SOURCE_FILES
		${IUPIMGLIB_ANDROID_SOURCE_FILES}
	)


	SET(IUPWEB_ANDROID_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/srcweb/android/iupandroid_webbrowser.c
		${CMAKE_SOURCE_DIR}/srcweb/android/iupandroid_jni_IupWebView.c
	)
	LIST(APPEND IUPWEB_SOURCE_FILES
		${IUPWEB_ANDROID_SOURCE_FILES}
	)

	# Annoying hack to get around Android Studio always rebuilding everything for every target.
	# arguments "-DIUP_ANDROID_BUILD_IUPWEB=1"
	IF(IUP_ANDROID_BUILD_IUPWEB)
		SET(IUP_SHOULD_BUILD_IUPWEB TRUE)
	ELSE()
		SET(IUP_SHOULD_BUILD_IUPWEB FALSE)
	ENDIF()

ELSEIF(IUP_USE_COCOATOUCH)

	SET(IUP_COCOATOUCH_SOURCE_FILES 
		# TODO: Get clarification on iup_datepick.c
		${CMAKE_SOURCE_DIR}/src/iup_datepick.c

		${CMAKE_SOURCE_DIR}/src/cocoatouch/IupAppDelegate.h
		${CMAKE_SOURCE_DIR}/src/cocoatouch/IupAppDelegate.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/IupAppDelegateProtocol.h
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_button.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_calendar.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_canvas.m
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_clipboard.m
#		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_colordlg.m # Needs to be deleted
		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_common.m
#		${CMAKE_SOURCE_DIR}/src/cocoatouch/iupcocoatouch_datepick.m
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


	SET(IUPIMGLIB_COCOATOUCH_SOURCE_FILES
		# TODO:
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos48x48.c 
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos32x32.c
	)
	LIST(APPEND IUPIMGLIB_SOURCE_FILES
		${IUPIMGLIB_COCOATOUCH_SOURCE_FILES}
	)


	FIND_LIBRARY(APPLE_WEBKIT_LIBRARY WebKit)
	SET(IUPWEB_COCOATOUCH_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/srcweb/iupcocoatouch_webbrowser.m
	)
	LIST(APPEND IUPWEB_SOURCE_FILES
		${IUPWEB_COCOATOUCH_SOURCE_FILES}
	)
	LIST(APPEND IUPWEB_LINK_LIBRARIES 
		${APPLE_WEBKIT_LIBRARY}
	)

	SET(IUP_SHOULD_BUILD_IUPWEB TRUE)

ELSEIF(IUP_USE_COCOA)

	SET(IUP_COCOA_SOURCE_FILES 
		# TODO: Get clarification on iup_datepick.c
		${CMAKE_SOURCE_DIR}/src/iup_datepick.c

		${CMAKE_SOURCE_DIR}/src/cocoa/IUPCocoaVerticalAlignmentTextFieldCell.h
		${CMAKE_SOURCE_DIR}/src/cocoa/IUPCocoaVerticalAlignmentTextFieldCell.m
		${CMAKE_SOURCE_DIR}/src/cocoa/IUPTextSpinnerFilesOwner.h
		${CMAKE_SOURCE_DIR}/src/cocoa/IUPTextSpinnerFilesOwner.m
		${CMAKE_SOURCE_DIR}/src/cocoa/IupAppDelegate.h
		${CMAKE_SOURCE_DIR}/src/cocoa/IupAppDelegate.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_button.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_calendar.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_canvas.m
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_clipboard.m
#		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_colordlg.m # Needs to be deleted
		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_common.m
#		${CMAKE_SOURCE_DIR}/src/cocoa/iupcocoa_datepick.m
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

	LIST(APPEND IUP_COCOA_XIB_FILES 
		${CMAKE_SOURCE_DIR}/src/cocoa/xib/Base.lproj/CanonicalServiceMenu.xib
		${CMAKE_SOURCE_DIR}/src/cocoa/xib/Base.lproj/CanonicalWindowMenu.xib
		${CMAKE_SOURCE_DIR}/src/cocoa/xib/Base.lproj/IupCocoaOutlineView.xib
		${CMAKE_SOURCE_DIR}/src/cocoa/xib/Base.lproj/IupMainMenu.xib
		${CMAKE_SOURCE_DIR}/src/cocoa/xib/Base.lproj/IupTextSpinner.xib	
	)

	LIST(APPEND IUP_SOURCE_FILES
		${IUP_COCOA_XIB_FILES}
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

	SET(IUPIMGLIB_COCOA_SOURCE_FILES
		# TODO:
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos48x48.c 
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos32x32.c
	)
	LIST(APPEND IUPIMGLIB_SOURCE_FILES
		${IUPIMGLIB_COCOA_SOURCE_FILES}
	)

	FIND_LIBRARY(APPLE_WEBKIT_LIBRARY WebKit)
	SET(IUPWEB_COCOA_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/srcweb/iupcocoa_webbrowser.m
	)
	LIST(APPEND IUPWEB_SOURCE_FILES
		${IUPWEB_COCOA_SOURCE_FILES}
	)
	LIST(APPEND IUPWEB_LINK_LIBRARIES 
		${APPLE_WEBKIT_LIBRARY}
	)
	SET(IUP_SHOULD_BUILD_IUPWEB TRUE)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/cocoa)
ELSEIF(IUP_USE_WIN32)


	SET(IUP_WIN32_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/win/iupwin_brush.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_brush.h
		${CMAKE_SOURCE_DIR}/src/win/iupwin_button.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_calendar.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_canvas.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_clipboard.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_common.c
		${CMAKE_SOURCE_DIR}/src/win/iupwin_datepick.c
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

	# Only works if in the VS command prompt environment
#	FIND_LIBRARY(WINDOWS_OLE32_LIBRARY ole32)
#	FIND_LIBRARY(WINDOWS_COMDLG32_LIBRARY comdlg32)
#	FIND_LIBRARY(WINDOWS_COMCTL32_LIBRARY comctl32)
#	FIND_LIBRARY(WINDOWS_GDI32_LIBRARY gdi32)
	
#	LIST(APPEND IUP_LINK_LIBRARIES 
#		${WINDOWS_OLE32_LIBRARY}
#		${WINDOWS_COMDLG32_LIBRARY}
#		${WINDOWS_COMCTL32_LIBRARY}
#		${WINDOWS_GDI32_LIBRARY}
#	)

	# Use explicit names instead
	LIST(APPEND IUP_LINK_LIBRARIES 
		ole32 comdlg32 comctl32 gdi32
	)	

	# FIXME: The other build systems pass -DNOTREEVIEW, but searching the codebase, it seems to only exist in one place and it is to #undef NOTREEVIEW. Maybe this flag can be removed.
	SET(IUP_C_FLAGS "${IUP_C_FLAGS} -D_WIN32_WINNT=0x0501 -D_WIN32_IE=0x0600 -DWINVER=0x0501 -DNOTREEVIEW")
	SET(IUPIMGLIB_C_FLAGS "${IUP_C_FLAGS} -D_WIN32_WINNT=0x0501 -D_WIN32_IE=0x0600 -DWINVER=0x0501 -DNOTREEVIEW")
	IF(IUP_OPTION_USE_UNICODE)
		SET(IUP_C_FLAGS "${IUP_C_FLAGS} -DUNICODE")
		# FIXME: Should check if other libraries use the UNICODE flag
		SET(IUPIMGLIB_C_FLAGS "${IUPIMGLIB_C_FLAGS} -DUNICODE")	
	ENDIF()
	

	IF(MSVC)
		ADD_DEFINITIONS(/D _CRT_SECURE_NO_WARNINGS)
		SET(IUP_SHOULD_BUILD_IUPWEB TRUE)
	ELSE()
		SET(IUP_SHOULD_BUILD_IUPWEB FALSE)
	ENDIF()

	SET(IUPIMGLIB_WIN32_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_basewin32x32.c
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos48x48.c 
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos32x32.c
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_iconswin48x48.c
	)
	LIST(APPEND IUPIMGLIB_SOURCE_FILES
		${IUPIMGLIB_WIN32_SOURCE_FILES}
	)

	SET(IUPOLE_WIN32_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/srcole/iup_olecontrol.cpp
		${CMAKE_SOURCE_DIR}/srcole/tAmbientProperties.cpp
		${CMAKE_SOURCE_DIR}/srcole/tAmbientProperties.h
		${CMAKE_SOURCE_DIR}/srcole/tDispatch.cpp
		${CMAKE_SOURCE_DIR}/srcole/tDispatch.h
		${CMAKE_SOURCE_DIR}/srcole/tLegacy.cpp
		${CMAKE_SOURCE_DIR}/srcole/tLegacy.h
		${CMAKE_SOURCE_DIR}/srcole/tOleClientSite.cpp
		${CMAKE_SOURCE_DIR}/srcole/tOleClientSite.h
		${CMAKE_SOURCE_DIR}/srcole/tOleControlSite.cpp
		${CMAKE_SOURCE_DIR}/srcole/tOleControlSite.h
		${CMAKE_SOURCE_DIR}/srcole/tOleHandler.cpp
		${CMAKE_SOURCE_DIR}/srcole/tOleHandler.h
		${CMAKE_SOURCE_DIR}/srcole/tOleInPlaceFrame.cpp
		${CMAKE_SOURCE_DIR}/srcole/tOleInPlaceFrame.h
		${CMAKE_SOURCE_DIR}/srcole/tOleInPlaceSite.cpp
		${CMAKE_SOURCE_DIR}/srcole/tOleInPlaceSite.h
	)
	LIST(APPEND IUPOLE_SOURCE_FILES
		${IUPOLE_WIN32_SOURCE_FILES}
	)
	IF("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
		LIST(APPEND IUPOLE_LINK_LIBRARIES 
			iup uuid gdi32 oleaut32 ole32
		)
	ELSE()
		LIST(APPEND IUPOLE_LINK_LIBRARIES 
			iup gdi32 ole32
		)
	ENDIF()

	SET(IUPWEB_WIN32_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/srcweb/iupwin_webbrowser.cpp
	)
	LIST(APPEND IUPWEB_SOURCE_FILES
		${IUPWEB_WIN32_SOURCE_FILES}
	)
	# Use explicit names instead
	LIST(APPEND IUPWEB_LINK_LIBRARIES 
		iupole comsuppw
	)


ELSEIF(IUP_USE_GTK3 OR IUP_USE_GTK2)

	SET(IUP_GTK_SOURCE_FILES
		# TODO: Get clarification on iup_datepick.c
		${CMAKE_SOURCE_DIR}/src/iup_datepick.c

		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_button.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_calendar.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_canvas.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_clipboard.c
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
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_draw_gdk.c
		${CMAKE_SOURCE_DIR}/src/mot/iupx11_info.c
	)

	SET(IUP_GTK3_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_draw_cairo.c
		${CMAKE_SOURCE_DIR}/src/gtk/iupgtk_info.c
	)


	SET(IUPIMGLIB_GTK_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_basegtk16x16.h
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_basegtk24x24.c
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_basegtk324x24.c
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_basegtk324x24.h
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_iconsgtk348x48.c
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_iconsgtk348x48.h
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_iconsgtk48x48.c
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos48x48.c 
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos32x32.c
	)
	LIST(APPEND IUPIMGLIB_SOURCE_FILES
		${IUPIMGLIB_GTK_SOURCE_FILES}
	)


	SET(IUPWEB_GTK_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/srcweb/iupgtk_webbrowser.c
	)
	LIST(APPEND IUPWEB_SOURCE_FILES
		${IUPWEB_GTK_SOURCE_FILES}
	)

	IF(WEBKIT_FOUND)
		SET(IUP_SHOULD_BUILD_IUPWEB TRUE)
		INCLUDE_DIRECTORIES(${WEBKIT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src/gtk)
		LIST(APPEND IUPWEB_LINK_LIBRARIES 
			${WEBKIT_LIBRARIES}
		)
	ELSE()
		SET(IUP_SHOULD_BUILD_IUPWEB FALSE)
	ENDIF()


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
		LIST(APPEND IUPWEB_LINK_LIBRARIES 
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
		LIST(APPEND IUPWEB_LINK_LIBRARIES 
			${GTK3_LIBRARIES}
		)
		
	ELSE()
		MESSAGE("Unsupported GTK version")
	ENDIF()

ELSEIF(IUP_USE_MOTIF)

	SET(IUP_MOTIF_SOURCE_FILES
		# TODO: Get clarification on iup_datepick.c
		${CMAKE_SOURCE_DIR}/src/iup_datepick.c

		${CMAKE_SOURCE_DIR}/src/mot/iupmot_button.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_calendar.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_canvas.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_clipboard.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_color.c
		${CMAKE_SOURCE_DIR}/src/mot/iupmot_color.h
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
		${CMAKE_SOURCE_DIR}/src/mot/iupx11_info.c
	)
	LIST(APPEND IUP_SOURCE_FILES
		${IUP_MOTIF_SOURCE_FILES}
	)
	INCLUDE_DIRECTORIES(${MOTIF_INCLUDE_DIRS})
	LIST(APPEND IUP_LINK_LIBRARIES 
		${MOTIF_LIBRARIES}
	)

	SET(IUPIMGLIB_MOTIF_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_basemot16x16_8bpp.c
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos48x48_8bpp.c 
		${CMAKE_SOURCE_DIR}/srcimglib/iup_imglib_logos32x32_8bpp.c
	)
	LIST(APPEND IUPIMGLIB_SOURCE_FILES
		${IUPIMGLIB_MOTIF_SOURCE_FILES}
	)

	SET(IUPWEB_MOTIF_SOURCE_FILES
	)
	LIST(APPEND IUPWEB_SOURCE_FILES
		${IUPWEB_MOTIF_SOURCE_FILES}
	)

	SET(IUP_SHOULD_BUILD_IUPWEB FALSE)

ELSE()
	MESSAGE(FATAL_ERROR "Unhandled platform backend")

ENDIF()

# Would prefer TARGET_INCLUDE_DIRECTORIES or something, but supporting CMake 2.8.7
INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/src
)
HELPER_CREATE_LIBRARY(iup "${WANTS_BUILD_SHARED_LIBRARY}" "${WANTS_BUILD_FRAMEWORK}" "${IUP_SOURCE_FILES}" "${IUP_PUBLIC_HEADERS}" "" "" "${IUP_FOREIGN_SOURCE_FILES}" "${IUP_LINK_LIBRARIES}" "${IUP_C_FLAGS}" "br.puc-rio.tecgraf.iup")
IF(IUP_USE_COCOA)
	# Reuse the Info.plist from the Xcode project
	HELPERAPPLE_SET_INFOPLIST_FOR_FRAMEWORK(iup "${CMAKE_SOURCE_DIR}/Xcode/iup/iup/Info.plist" "${WANTS_BUILD_FRAMEWORK}")
	# set_target_properties doesn't work correctly in CMake because it loses the localization directories
	# set_target_properties(iup PROPERTIES RESOURCE "${IUP_COCOA_XIB_FILES}") 
	HELPERIB_COMPILE_XIBS_INTO_FRAMEWORK("${IUP_COCOA_XIB_FILES}" iup "iup" "${WANTS_BUILD_FRAMEWORK}")
ENDIF()

HELPER_CREATE_LIBRARY(iupimglib "${WANTS_BUILD_SHARED_LIBRARY}" "${WANTS_BUILD_FRAMEWORK}" "${IUPIMGLIB_SOURCE_FILES}" "${IUP_PUBLIC_HEADERS}" "" "" "${IUPIMGLIB_FOREIGN_SOURCE_FILES}" "${IUPIMGLIB_LINK_LIBRARIES}" "${IUPIMGLIB_C_FLAGS}" "br.puc-rio.tecgraf.iupimglib")

IF(IUP_USE_CD)
	HELPER_CREATE_LIBRARY(iupcd "${WANTS_BUILD_SHARED_LIBRARY}" "${WANTS_BUILD_FRAMEWORK}" "${IUPCD_SOURCE_FILES}" "${IUPCD_PUBLIC_HEADERS}" "" "" "${IUPCD_FOREIGN_SOURCE_FILES}" "${IUPCD_LINK_LIBRARIES}" "${IUPCD_C_FLAGS}" "br.puc-rio.tecgraf.iupcd")
	HELPER_CREATE_LIBRARY(iupcontrols "${WANTS_BUILD_SHARED_LIBRARY}" "${WANTS_BUILD_FRAMEWORK}" "${IUPCONTROLS_SOURCE_FILES}" "${IUPCONTROLS_PUBLIC_HEADERS}" "" "" "${IUPCONTROLS_FOREIGN_SOURCE_FILES}" "${IUPCONTROLS_LINK_LIBRARIES}" "${IUPCONTROLS_C_FLAGS}" "br.puc-rio.tecgraf.iupcontrols")
	TARGET_INCLUDE_DIRECTORIES(iupcd PRIVATE 
		"${CMAKE_SOURCE_DIR}/srccd"
		"${CD_INCLUDE_DIR}"
	)
	TARGET_INCLUDE_DIRECTORIES(iupcontrols PRIVATE 
		"${CMAKE_SOURCE_DIR}/srccontrols"
		"${CMAKE_SOURCE_DIR}/srccd"
		"${CD_INCLUDE_DIR}"
	)

	IF(IUP_USE_LUA)
	ENDIF()
ENDIF()

if(WIN32)
	HELPER_CREATE_LIBRARY(iupole "${WANTS_BUILD_SHARED_LIBRARY}" "${WANTS_BUILD_FRAMEWORK}" "${IUPOLE_SOURCE_FILES}" "${IUPOLE_PUBLIC_HEADERS}" "" "" "${IUPOLE_FOREIGN_SOURCE_FILES}" "${IUPOLE_LINK_LIBRARIES}" "${IUPOLE_C_FLAGS}" "br.puc-rio.tecgraf.iupole")
endif()

if(IUP_SHOULD_BUILD_IUPWEB)
	HELPER_CREATE_LIBRARY(iupweb "${WANTS_BUILD_SHARED_LIBRARY}" "${WANTS_BUILD_FRAMEWORK}" "${IUPWEB_SOURCE_FILES}" "${IUPWEB_PUBLIC_HEADERS}" "" "" "${IUPWEB_FOREIGN_SOURCE_FILES}" "${IUPWEB_LINK_LIBRARIES}" "${IUPWEB_C_FLAGS}" "br.puc-rio.tecgraf.iupweb")

	# Having problem with include paths on Android, partly because everything is spread out
	IF(ANDROID)
		# TARGET_INCLUDE_DIRECTORIES is guaranteed to be available on Android
		TARGET_INCLUDE_DIRECTORIES(iupweb PRIVATE 
			"${CMAKE_SOURCE_DIR}/src"
			"${CMAKE_SOURCE_DIR}/src/android/source"
			"${CMAKE_SOURCE_DIR}/srcweb"
			"${CMAKE_SOURCE_DIR}/srcweb/android"
		)
	ELSEIF(IOS)
		# TARGET_INCLUDE_DIRECTORIES is guaranteed to be available on Android
		TARGET_INCLUDE_DIRECTORIES(iupweb PRIVATE 
			"${CMAKE_SOURCE_DIR}/src"
			"${CMAKE_SOURCE_DIR}/src/cocoatouch"
			"${CMAKE_SOURCE_DIR}/srcweb"
		)
	ENDIF()


	IF(IUP_USE_COCOA)
		# Reuse the Info.plist from the Xcode project
		HELPERAPPLE_SET_INFOPLIST_FOR_FRAMEWORK(iupweb "${CMAKE_SOURCE_DIR}/Xcode/iup/iupweb-Info.plist" "${WANTS_BUILD_FRAMEWORK}")
	ENDIF()

else()
	MESSAGE("Not building iupweb")
endif()


# Decided to put everything in this CMakeLists.txt to make keeping in sync easier since this isn't official.
#ADD_SUBDIRECTORY(test)

function(BUILD_IUP_TEST exe_name source_list resource_list)
	if(NOT ANDROID)
		set(_should_exclude_from_all "")
		if(NOT IUP_BUILD_TESTS)
			set(_should_exclude_from_all 1)
		endif()
		
		HELPER_CREATE_EXECUTABLE(${exe_name} 
			"${source_list}"
			"${WANTS_BUILD_SHARED_LIBRARY}"
			"iup"
			"${IUP_LINK_LIBRARIES}"
			"${IUPEXE_C_FLAGS}"
			"${IUPEXE_LINK_FLAGS}"
			"${_should_exclude_from_all}"
			"${resource_list}"
		)

		# This adds the current test to the aggregate target for tests
		ADD_DEPENDENCIES(${IUP_TESTS_CUSTOM_TARGET_NAME} ${exe_name})
	endif()
endfunction()

function(BUILD_IUPIMGLIB_TEST exe_name source_list resource_list)
	if(NOT ANDROID)
		set(_should_exclude_from_all "")
		if(NOT IUP_BUILD_TESTS)
			set(_should_exclude_from_all 1)
		endif()
		
		HELPER_CREATE_EXECUTABLE(${exe_name} 
			"${source_list}"
			"${WANTS_BUILD_SHARED_LIBRARY}"
			"iupimglib;iup"
			"${IUP_LINK_LIBRARIES}"
			"${IUPEXE_C_FLAGS}"
			"${IUPEXE_LINK_FLAGS}"
			"${_should_exclude_from_all}"
			"${resource_list}"
		)

		# This adds the current test to the aggregate target for tests
		ADD_DEPENDENCIES(${IUP_TESTS_CUSTOM_TARGET_NAME} ${exe_name})
	endif()
endfunction()

function(BUILD_IUPWEB_TEST exe_name source_list resource_list)
	if(NOT ANDROID)
		set(_should_exclude_from_all "")
		if(NOT IUP_BUILD_TESTS)
			set(_should_exclude_from_all 1)
		endif()
		
		if(IUP_SHOULD_BUILD_IUPWEB)
			HELPER_CREATE_EXECUTABLE(${exe_name} 
				"${source_list}"
				"${WANTS_BUILD_SHARED_LIBRARY}"
				"iupweb;iup"
				"${IUP_LINK_LIBRARIES}"
				"${IUPEXE_C_FLAGS}"
				"${IUPEXE_LINK_FLAGS}"
				"${_should_exclude_from_all}"
				"${resource_list}"
			)
		endif()

		# This adds the current test to the aggregate target for tests
		ADD_DEPENDENCIES(${IUP_TESTS_CUSTOM_TARGET_NAME} ${exe_name})
	endif()
endfunction()

function(BUILD_IUPCONTROLS_TEST exe_name source_list resource_list)
	if(NOT ANDROID)
		set(_should_exclude_from_all "")
		if(NOT IUP_BUILD_TESTS)
			set(_should_exclude_from_all 1)
		endif()
		
		HELPER_CREATE_EXECUTABLE(${exe_name} 
			"${source_list}"
			"${WANTS_BUILD_SHARED_LIBRARY}"
			"iupcontrols;iupcd;iup"
			"${IUP_LINK_LIBRARIES}"
			"${IUPEXE_C_FLAGS}"
			"${IUPEXE_LINK_FLAGS}"
			"${_should_exclude_from_all}"
			"${resource_list}"
		)

		# This adds the current test to the aggregate target for tests
		ADD_DEPENDENCIES(${IUP_TESTS_CUSTOM_TARGET_NAME} ${exe_name})
	endif()
endfunction()

function(BUILD_IUPLUA_TEST exe_name source_list resource_list)
	if(NOT ANDROID)
		set(_should_exclude_from_all "")
		if(NOT IUP_BUILD_TESTS)
			set(_should_exclude_from_all 1)
		endif()
		
		if(IUP_USE_LUA)
			
			IF(IUP_OPTION_LUA_BUILD_AS_MODULES)
				SET(_libs_to_link "${LUA_LIBRARY};iupweb;iup")
			ELSE()
				SET(_libs_to_link "${LUA_LIBRARY};${IUPLUAWEB_LIBRARY_NAME};${IUPLUA_LIBRARY_NAME};iupweb;iup")
			ENDIF()


			HELPER_CREATE_EXECUTABLE(${exe_name} 
				"${source_list}"
				"${WANTS_BUILD_SHARED_LIBRARY}"
				"${_libs_to_link}"
				"${IUP_LINK_LIBRARIES}"
				"${IUPEXE_C_FLAGS}"
				"${IUPEXE_LINK_FLAGS}"
				"${_should_exclude_from_all}"
				"${resource_list}"
			)
		endif()

		# This adds the current test to the aggregate target for tests
		ADD_DEPENDENCIES(${IUP_TESTS_CUSTOM_TARGET_NAME} ${exe_name})
	endif()
endfunction()

BUILD_IUP_TEST(canvas "${CMAKE_SOURCE_DIR}/html/examples/tests/canvas.c" "")
BUILD_IUP_TEST(hbox "${CMAKE_SOURCE_DIR}/html/examples/tests/hbox.c" "")
BUILD_IUP_TEST(menu "${CMAKE_SOURCE_DIR}/html/examples/tests/menu.c" "")
BUILD_IUP_TEST(timer "${CMAKE_SOURCE_DIR}/html/examples/tests/timer.c" "")
BUILD_IUP_TEST(tree "${CMAKE_SOURCE_DIR}/html/examples/tests/tree.c" "")
#BUILD_IUP_TEST(postmessage "${CMAKE_SOURCE_DIR}/html/examples/tests/postmessage.c" "")

# Things with dependencies on iupimglib
#BUILD_IUPIMGLIB_TEST(bigtest "${CMAKE_SOURCE_DIR}/html/examples/tests/bigtest.c" "")
BUILD_IUPIMGLIB_TEST(button "${CMAKE_SOURCE_DIR}/html/examples/tests/button.c" "")
BUILD_IUPIMGLIB_TEST(flatbutton "${CMAKE_SOURCE_DIR}/html/examples/tests/flatbutton.c" "")

LIST(APPEND label_resources
	"${CMAKE_SOURCE_DIR}/html/examples/tests/tecgraf.bmp"
	"${CMAKE_SOURCE_DIR}/html/examples/tests/file_large.xbm"
	"${CMAKE_SOURCE_DIR}/html/examples/tests/loading.gif"
)
BUILD_IUPIMGLIB_TEST(label "${CMAKE_SOURCE_DIR}/html/examples/tests/label.c" "${label_resources}")
# depends on cdlib
#BUILD_IUPIMGLIB_TEST(matrix_cbs "${CMAKE_SOURCE_DIR}/html/examples/tests/matrix_cbs.c")

BUILD_IUPWEB_TEST(webbrowser "${CMAKE_SOURCE_DIR}/html/examples/tests/webbrowser.c" "")

# 7GUI: TODO: Need to handle bundling images.
#BUILD_IUP_TEST(7gui_cells "${CMAKE_SOURCE_DIR}/html/en/7gui/cells.c" "") # needs IupControlsOpen, IupMatrix
BUILD_IUP_TEST(7gui_circle_drawer "${CMAKE_SOURCE_DIR}/html/en/7gui/circle_drawer.c" "")
BUILD_IUP_TEST(7gui_counter "${CMAKE_SOURCE_DIR}/html/en/7gui/counter.c" "")
#BUILD_IUP_TEST(7gui_crud "${CMAKE_SOURCE_DIR}/html/en/7gui/crud.c" "") # compiler errors
BUILD_IUP_TEST(7gui_flight_booker "${CMAKE_SOURCE_DIR}/html/en/7gui/flight_booker.c" "")
BUILD_IUP_TEST(7gui_temperature_converter "${CMAKE_SOURCE_DIR}/html/en/7gui/temperature_converter.c" "")
BUILD_IUP_TEST(7gui_timer "${CMAKE_SOURCE_DIR}/html/en/7gui/timer.c" "")

BUILD_IUPCONTROLS_TEST(matrix "${CMAKE_SOURCE_DIR}/html/examples/tests/matrix.c" "")

function(BUILD_DEV_TEST exe_name source_list)
	HELPER_CREATE_EXECUTABLE(${exe_name} 
		"${source_list}"
		"${WANTS_BUILD_SHARED_LIBRARY}"
		"iup"
		"${IUP_LINK_LIBRARIES}"
		"${IUPEXE_C_FLAGS}"
		"${IUPEXE_LINK_FLAGS}"
		"" # exclude from all
		"" # resource list
	)
endfunction()


IF(IUP_USE_LUA)

	FILE(GLOB LUA_IL_FILES ${CMAKE_SOURCE_DIR}/srclua5/elem/*.c)
	
	#	   SET(CMAKE_MODULE_LINKER_FLAGS
	#   "${CMAKE_MODULE_LINKER_FLAGS} -flat_namespace -undefined suppress")

	SET(IUPLUA_SOURCE_FILES
#		${CMAKE_SOURCE_DIR}/srclua5/il.h
		${CMAKE_SOURCE_DIR}/srclua5/iuplua.c
		${CMAKE_SOURCE_DIR}/srclua5/iuplua_api.c 
		${CMAKE_SOURCE_DIR}/srclua5/iuplua_config.c 		
		${CMAKE_SOURCE_DIR}/srclua5/iuplua_draw.c 
		${CMAKE_SOURCE_DIR}/srclua5/iuplua_getcolor.c 
		${CMAKE_SOURCE_DIR}/srclua5/iuplua_getparam.c 
		${CMAKE_SOURCE_DIR}/srclua5/iuplua_scanf.c 
		${CMAKE_SOURCE_DIR}/srclua5/iuplua_tree_aux.c 
		${LUA_IL_FILES}
	)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/srclua5 ${CMAKE_SOURCE_DIR}/srclua5/lh ${LUA_INCLUDE_DIR})
	SET(IUPLUA_LINK_LIBRARIES "")
	LIST(APPEND IUPLUA_LINK_LIBRARIES 
		iup
	)

	SET(IUPLUA_C_FLAGS "${IUPLUA_C_FLAGS} -DIUPLUA_USELH")
	#ADD_DEFINITIONS("-DIUPLUA_USELH")
	#SET(IUPLUA_C_FLAGS "-DIUPLUA_USELOH")

	SET(IUPLUA_LIBRARY_NAME "iuplua")
	SET(IUP_LUA_VERSION_SUFFIX "${LUA_VERSION_MAJOR}${LUA_VERSION_MINOR}")
	#	MESSAGE("IUP_LUA_VERSION_SUFFIX ${IUP_LUA_VERSION_SUFFIX}")
	#SET(IUPLUA_LIBRARY_NAME "iuplua${IUPLUA_VERSION_SUFFIX}")	

	IF(IUP_OPTION_LUA_BUILD_AS_MODULES)
		# If building as a module, we must link to Lua on Windows, but not on ELF, Darwin.
		IF(WIN32)
			LIST(APPEND IUPLUA_LINK_LIBRARIES 
				${LUA_LIBRARY}
			)
		ENDIF()

		HELPER_CREATE_MODULE(${IUPLUA_LIBRARY_NAME} "${WANTS_BUILD_SHARED_LIBRARY}" "${IUPLUA_SOURCE_FILES}" "${IUPLUA_PUBLIC_HEADERS}" "" "" "${IUPLUA_FOREIGN_SOURCE_FILES}" "${IUPLUA_LINK_LIBRARIES}" "${IUPLUA_C_FLAGS}" "br.puc-rio.tecgraf.iuplua")
	ELSE()
		# If building as a dynamic library, we can link directly for all platforms.
		LIST(APPEND IUPLUA_LINK_LIBRARIES 
			${LUA_LIBRARY}
		)

		HELPER_CREATE_LIBRARY(${IUPLUA_LIBRARY_NAME} "${WANTS_BUILD_SHARED_LIBRARY}" NO "${IUPLUA_SOURCE_FILES}" "${IUPLUA_PUBLIC_HEADERS}" "" "" "${IUPLUA_FOREIGN_SOURCE_FILES}" "${IUPLUA_LINK_LIBRARIES}" "${IUPLUA_C_FLAGS}" "br.puc-rio.tecgraf.iuplua")
	ENDIF()


	IF(IUP_SHOULD_BUILD_IUPWEB)
		# FIXME: We must add a IUPLUAWEB_EXPORT for the lua/web open functions, but there is no obvious place to add it
		SET(IUPLUAWEB_SOURCE_FILES
			${CMAKE_SOURCE_DIR}/srclua5/ctrl/il_webbrowser.c
			${CMAKE_SOURCE_DIR}/srclua5/lh/webbrowser.lh
			${CMAKE_SOURCE_DIR}/srclua5/lh/webbrowser.lh
		)

		INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/srclua5 ${CMAKE_SOURCE_DIR}/srclua5/lh ${LUA_INCLUDE_DIR})
		SET(IUPLUAWEB_LINK_LIBRARIES "")

		SET(IUPLUAWEB_C_FLAGS "${IUPLUAWEB_C_FLAGS} -DIUPLUA_USELH")
		#ADD_DEFINITIONS("-DIUPLUA_USELH")
		#SET(IUPLUA_C_FLAGS "-DIUPLUA_USELOH")
		SET(IUPLUAWEB_LIBRARY_NAME "iupluaweb")

		IF(IUP_OPTION_LUA_BUILD_AS_MODULES)
			# Modules cannot link to other modules, i.e. we don't link to iuplua
			LIST(APPEND IUPLUAWEB_LINK_LIBRARIES 
				iupweb
				iup
			)
			# If building as a module, we must link to Lua on Windows, but not on ELF, Darwin.
			IF(WIN32)
				LIST(APPEND IUPLUAWEB_LINK_LIBRARIES 
					${LUA_LIBRARY}
				)
			ENDIF()
			HELPER_CREATE_MODULE(${IUPLUAWEB_LIBRARY_NAME} "${WANTS_BUILD_SHARED_LIBRARY}" "${IUPLUAWEB_SOURCE_FILES}" "${IUPLUAWEB_PUBLIC_HEADERS}" "" "" "${IUPLUAWEB_FOREIGN_SOURCE_FILES}" "${IUPLUAWEB_LINK_LIBRARIES}" "${IUPLUAWEB_C_FLAGS}" "br.puc-rio.tecgraf.iupluaweb")

		ELSE()
			LIST(APPEND IUPLUAWEB_LINK_LIBRARIES 
				${IUPLUA_LIBRARY_NAME}
				iupweb
				iup
				${LUA_LIBRARY}
			)
			HELPER_CREATE_LIBRARY(${IUPLUAWEB_LIBRARY_NAME} "${WANTS_BUILD_SHARED_LIBRARY}" NO "${IUPLUAWEB_SOURCE_FILES}" "${IUPLUAWEB_PUBLIC_HEADERS}" "" "" "${IUPLUAWEB_FOREIGN_SOURCE_FILES}" "${IUPLUAWEB_LINK_LIBRARIES}" "${IUPLUAWEB_C_FLAGS}" "br.puc-rio.tecgraf.iupluaweb")
		ENDIF()

		# IupLua depends on IupWeb
		BUILD_IUPLUA_TEST(lua_c_app "${CMAKE_SOURCE_DIR}/html/examples/lua_c_app/lua_app_main.c" "")

		IF(IUP_USE_ANDROID)
			# Android Studio/Gradle lacks a mechanism to copy pre-built libraries to be bundled in the .apk,
			# so we need to manally copy the library into a staging area that Gradle will intercept.
			file(COPY ${LUA_LIBRARY} DESTINATION ${CMAKE_SOURCE_DIR}/Android/iuptestapplua/build/jniLibs/${ANDROID_ABI}/)
		ENDIF()
		

	ENDIF(IUP_SHOULD_BUILD_IUPWEB)



ENDIF(IUP_USE_LUA)


# Hack: Used to build simple test programs as we bootstrap each new implementation
if(IUP_USE_EMSCRIPTEN)
	BUILD_DEV_TEST(dev_test "${CMAKE_SOURCE_DIR}/src/emscripten/test/main_c.c")
elseif(IUP_USE_COCOATOUCH)
	BUILD_DEV_TEST(dev_test "${CMAKE_SOURCE_DIR}/XcodeiOS/iup/TestApp/main.c")
elseif(IUP_USE_COCOA)
	BUILD_DEV_TEST(dev_test "${CMAKE_SOURCE_DIR}/Xcode/iup/TestApp/main.c")
elseif(IUP_USE_ANDROID)

	# This is a hack to get around Android Studio/Gradle deficiencies.
	# Being able to build an AAR, and then using it in the same project is horribly broken.
	# After some hackery, I got the testapp target to refer to the AAR, but it only looks at the Java side.
	# The CMake/NDK side can't find the libraries.
	# So the hack is to force the NDK side to rebuild all the libraries, plus the libMyIupProgram.so library.
	# But we don't want the libMyIupProgram.so built when just building the AAR, otherwise it goes into the AAR.
	# So the build.gradle is required to set -DIUP_ANDROID_BUILD_TEST_LIBRARY=1 when building the testapp.
	IF(IUP_ANDROID_BUILD_TEST_LIBRARY)
#		add_library(iuptestapp SHARED EXCLUDE_FROM_ALL
#			${CMAKE_SOURCE_DIR}/src/android/source/main_c.c
#		)
#		target_link_libraries(iuptestapp iupimglib iup)

		# if we need to bundle resources in the assets directory, list them here.
		LIST(APPEND iuptestapp_resources
			"${CMAKE_SOURCE_DIR}/html/examples/tests/tecgraf.bmp"
			"${CMAKE_SOURCE_DIR}/html/examples/tests/file_large.xbm"
			"${CMAKE_SOURCE_DIR}/html/examples/tests/loading.gif"
		)
		# This function knows to build libraries for Android. It also is aware of how to bundle our assets.
		HELPER_CREATE_EXECUTABLE(iuptestapp 
			"${CMAKE_SOURCE_DIR}/src/android/source/main_c.c"
			"${WANTS_BUILD_SHARED_LIBRARY}"
			"iupimglib;iup"
			"${IUP_LINK_LIBRARIES}"
			"${IUPEXE_C_FLAGS}"
			"${IUPEXE_LINK_FLAGS}"
			1 # "${_should_exclude_from_all}"
			"${iuptestapp_resources}"
		)

	ENDIF()

	IF(IUPWEB_ANDROID_BUILD_TEST_LIBRARY)

#		add_library(iuptestappwebbrowser SHARED EXCLUDE_FROM_ALL
#			${CMAKE_SOURCE_DIR}/html/examples/tests/webbrowser.c
#		)
#		target_link_libraries(iuptestappwebbrowser iupweb iup)

		# if we need to bundle resources in the assets directory, list them here.
		LIST(APPEND iuptestappwebbrowser_resources
#			"${CMAKE_SOURCE_DIR}/html/examples/tests/tecgraf.bmp"
		)
		# This function knows to build libraries for Android. It also is aware of how to bundle our assets.
		HELPER_CREATE_EXECUTABLE(iuptestappwebbrowser 
			"${CMAKE_SOURCE_DIR}/html/examples/tests/webbrowser.c"
			"${WANTS_BUILD_SHARED_LIBRARY}"
			"iupweb;iup"
			"${IUP_LINK_LIBRARIES}"
			"${IUPEXE_C_FLAGS}"
			"${IUPEXE_LINK_FLAGS}"
			1 # "${_should_exclude_from_all}"
			"${iuptestappwebbrowser_resources}"
		)

	ENDIF()

	# TODO: Need a new main.c launcher app to drive Lua programs. Also need to bundle Lua scripts.
	IF(IUPLUA_ANDROID_BUILD_TEST_LIBRARY)

#		add_library(iuptestapplua SHARED EXCLUDE_FROM_ALL
#			${CMAKE_SOURCE_DIR}/html/examples/lua_c_app/lua_app_main.c
#		)
#		IF(IUP_OPTION_LUA_BUILD_AS_MODULES)
#			target_link_libraries(iuptestapplua iupweb iup)
#		ELSE()
#			target_link_libraries(iuptestapplua ${IUPLUAWEB_LIBRARY_NAME} ${IUPLUA_LIBRARY_NAME} iupweb iup)
#		ENDIF()


		# if we need to bundle resources in the assets directory, list them here.
		LIST(APPEND iuptestapplua_resources
#			"${CMAKE_SOURCE_DIR}/html/examples/tests/tecgraf.bmp"
		)
		IF(IUP_OPTION_LUA_BUILD_AS_MODULES)
			SET(iuptestapplua_libs_to_link "${LUA_LIBRARY};iupweb;iup")
		ELSE()
			SET(iuptestapplua_libs_to_link "${LUA_LIBRARY};${IUPLUAWEB_LIBRARY_NAME};${IUPLUA_LIBRARY_NAME};iupweb;iup")
		ENDIF()

		# This function knows to build libraries for Android. It also is aware of how to bundle our assets.
		HELPER_CREATE_EXECUTABLE(iuptestapplua 
			"${CMAKE_SOURCE_DIR}/html/examples/lua_c_app/lua_app_main.c"
			"${WANTS_BUILD_SHARED_LIBRARY}"
			"${iuptestapplua_libs_to_link}"
			"${IUP_LINK_LIBRARIES}"
			"${IUPEXE_C_FLAGS}"
			"${IUPEXE_LINK_FLAGS}"
			1 # "${_should_exclude_from_all}"
			"${iuptestapplua_resources}"
		)


	ENDIF()

endif()


# For uninstall
HELPER_SETUP_UNINSTALL_TARGET()

