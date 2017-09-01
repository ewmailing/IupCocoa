# This file is here to initial Xcode to better default settings.

macro(BLURRR_CONFIGURE_XCODE_DEFAULTS _EXE_NAME)

# This iOS toolchain provides this convenience marco, but this is not in the mainline Mac, so let's define it.
if(APPLE AND NOT IOS)
	macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
		set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
	endmacro (set_xcode_property)
endif()


if(APPLE)
	if(IOS)

		# We need to make sure dead code stripping is off for "plugins" since we must statically link.
		set_xcode_property(${_EXE_NAME} DEAD_CODE_STRIPPING "No")

		set_xcode_property(${_EXE_NAME} GCC_GENERATE_DEBUGGING_SYMBOLS[variant=Debug] YES)
		set_xcode_property(${_EXE_NAME} GCC_GENERATE_DEBUGGING_SYMBOLS[variant=MinSizeRel] NO)
		set_xcode_property(${_EXE_NAME} GCC_GENERATE_DEBUGGING_SYMBOLS[variant=RelWithDebInfo] YES)
		set_xcode_property(${_EXE_NAME} GCC_GENERATE_DEBUGGING_SYMBOLS[variant=Release] NO)
	

		# Over 50% of iOS users update within the first month and in less than a year, 90% are on the latest.
		# The old Apple dev rule of thumb is to support current OS and the previous version, though supporting just the latest probably is acceptable.
		# Theoretically, the components of Blurrr could go back further, but this is unsupported as the support costs will make things more expensive,
		# and future components don't promise to support back that far.
		# Also understand that Apple changes/breaks/fixes things in every release. Particularly with OpenGL drivers, you must get the latest OS.
		# iOS 8 introduced dynamic library support to 3rd parties.
		# This will have an impact on many things including Swift which the standard libraries must be embedded with the app as dynamic libraries.
		# This makes iOS 8 a new baseline.
		set_xcode_property(${_EXE_NAME} IPHONEOS_DEPLOYMENT_TARGET 8.4)
		set_xcode_property(${_EXE_NAME} TARGETED_DEVICE_FAMILY "1,2")

		# Note: The CMAKE_OSX_SYSROOT is set to iphoneos in the toolchain file so we don't need to override it here.


		# Evidence suggests that on modern devices, using Apple's pngcrush is counterproductive.
		set_xcode_property(${_EXE_NAME} COMPRESS_PNG_FILES NO)

		# Disable LLVM IR bitcode (for now)
		set_xcode_property(${_EXE_NAME} ENABLE_BITCODE NO)

		# EVIL HACK: CMake sets the FRAMEWORK_SEARCH_PATHS *sometimes*.
		# I'm seeing it with the JavaScriptCore builds.
		# The problem is that it sets it to the iPhone9.3 SDK or whatever,
		# and when switching to the simulator, it breaks.
		# I read that frameworks may cause CMake to set the FRAMEWORK_SEARCH_PATHS.
		# What I don't understand is why I don't see this with Lua or C, but JSCore template.
		# I use other system frameworks in all of them, so there is very little difference.
		# I'm speculating maybe because I am asking for the include directory inside the framework,
		# it might have something to do with it, but OpenAL does the same thing.
		# Anyway, the hack is to override what CMake writes in the framework search paths with a blank value.
		# This will prevent it from injecting the iPhone framework path.
		# This could break 3rd party framework paths, but Blurrr still seems to work so I don't know what's going on.
		# I consider this really evil and dangerous though.
		# I think the correct fix is to overhaul the CMake internals to properly deal with iOS including the simulator.
		set_xcode_property(${_EXE_NAME} FRAMEWORK_SEARCH_PATHS " ")


	# OSX
	else()

		# The generic "macosx" is used to trigger the "Latest" setting. The full SDK path which CMake has been trying to do actually breaks functionality in subtle ways.
		set(CMAKE_OSX_SYSROOT macosx CACHE PATH "The product will be built against the headers and libraries located inside the indicated SDK." FORCE)

		set_xcode_property(${_EXE_NAME} GCC_GENERATE_DEBUGGING_SYMBOLS[variant=Debug] YES)
		set_xcode_property(${_EXE_NAME} GCC_GENERATE_DEBUGGING_SYMBOLS[variant=MinSizeRel] YES)
		set_xcode_property(${_EXE_NAME} GCC_GENERATE_DEBUGGING_SYMBOLS[variant=RelWithDebInfo] YES)
		set_xcode_property(${_EXE_NAME} GCC_GENERATE_DEBUGGING_SYMBOLS[variant=Release] YES)

		# For active paying users on the Mac App Store, it seems that 50% of Mac users update within the first month now that the OS is free.
		# The old Apple dev rule of thumb is to support current OS and the previous version.
		# Theoretically, the components of Blurrr could go back further (10.6), but this is unsupported as the support costs will make things more expensive,
		# and future components don't promise to support back that far.
		# Also understand that Apple changes/breaks/fixes things in every release. Particularly with OpenGL drivers, you must get the latest OS.
		set_xcode_property(${_EXE_NAME} MACOSX_DEPLOYMENT_TARGET 10.9)		

		set_xcode_property(${_EXE_NAME} CODE_SIGN_ENTITLEMENTS "${PROJECT_BINARY_DIR}/${BLURRR_USER_PROJECT_NAME}.entitlements")

		set_xcode_property(${_EXE_NAME} CFBundleIconFile icon.icns)


		# This will let you override some of the entitlement defaults in Blurrr-OSX-Template.entitlements.in
		set(OSX_ENTITLEMENT_APP_SANDBOX true)
		set(OSX_ENTITLEMENT_NETWORK_CLIENT true)
		set(OSX_ENTITLEMENT_NETWORK_SERVER true)
		set(OSX_ENTITLEMENT_USB true)
		set(OSX_ENTITLEMENT_BLUETOOTH true)
		set(OSX_ENTITLEMENT_GET_TASK_ALLOW false)

		# Because codesigning is kind of slow, you can disable this to skip resigning already signed components.
		# However, this may break if you change the CODESIGN_IDENTITY without cleaning.
		# Currently incomplete/broken due to rsync being re-run every build.
		set(BLURRR_OSX_FORCE_FULL_CODE_RESIGN true)

		# For JavaScript debugging with Safari, you need to codesign to use the com.apple.security.get-task-allow entitlement.		
		if(BLURRR_USE_JAVASCRIPTCORE)
			set(OSX_ENTITLEMENT_GET_TASK_ALLOW true)
		else()
			set(OSX_ENTITLEMENT_GET_TASK_ALLOW false)
			
		endif()

	endif(IOS)

	# Common to both
	# Use Apple's recommended standard architectures
	set_xcode_property(${_EXE_NAME} ARCHS "$(ARCHS_STANDARD)")

	set_xcode_property(${_EXE_NAME} GCC_OPTIMIZATION_LEVEL[variant=Debug] "0")
	set_xcode_property(${_EXE_NAME} GCC_OPTIMIZATION_LEVEL[variant=MinSizeRel] "s")
	set_xcode_property(${_EXE_NAME} GCC_OPTIMIZATION_LEVEL[variant=RelWithDebInfo] "fast")
	set_xcode_property(${_EXE_NAME} GCC_OPTIMIZATION_LEVEL[variant=Release] "fast")


	if(BLURRR_USE_SWIFT)
		set_xcode_property(${_EXE_NAME} SWIFT_OPTIMIZATION_LEVEL[variant=Debug] "-Onone")
		set_xcode_property(${_EXE_NAME} SWIFT_OPTIMIZATION_LEVEL[variant=MinSizeRel] "-O")
		set_xcode_property(${_EXE_NAME} SWIFT_OPTIMIZATION_LEVEL[variant=RelWithDebInfo] "-O")
		set_xcode_property(${_EXE_NAME} SWIFT_OPTIMIZATION_LEVEL[variant=Release] "-Owholemodule")

		set_xcode_property(${_EXE_NAME} EMBEDDED_CONTENT_CONTAINS_SWIFT "YES")
		
		# Xcode likes to name these after the project. 
		# But I don't want to have to dynamically manage names since it will 
		# slow down the build process and just make everything harder to deal with.
		set_xcode_property(${_EXE_NAME} SWIFT_OBJC_BRIDGING_HEADER "${PROJECT_SOURCE_DIR}/source/MySwiftBridgingHeader.h")
		# Again, Xcode likes to name these after the project,
		# but it's not worth dealing with.
		# Right now only iOS needs this, but Mac potentially could need this.
		set_xcode_property(${_EXE_NAME} SWIFT_OBJC_INTERFACE_HEADER_NAME "BlurrrObjCGeneratedInterfaceHeader-Swift.h")
		
		set(CMAKE_Swift_LANGUAGE_VERSION 3.0)
		
	endif()
	


	# set_xcode_property(${_EXE_NAME} ONLY_ACTIVE_ARCH NO)
		
	set_xcode_property(${_EXE_NAME} DEBUG_INFORMATION_FORMAT "dwarf-with-dsym")

	# These two lines are needed to make the Archive option work (mostly).
	# (You need to build as Release first. Then Archive, otherwise the build fails.)
	# http://public.kitware.com/pipermail/cmake/2012-December/053017.html
	# UPDATE: This breaks building frameworks as it changes the INSTALL_NAME from @rpath
#	set_xcode_property(${_EXE_NAME} INSTALL_PATH "$(LOCAL_APPS_DIR)")
#	set_xcode_property(${_EXE_NAME} SKIP_INSTALL NO)


endif(APPLE)
endmacro(BLURRR_CONFIGURE_XCODE_DEFAULTS)
