

# We need to compile the interface builder *.xib files to *.nib and then add them to the correct location in the bundle.
# Doing:
#  set_target_properties(iup PROPERTIES RESOURCE "${IUP_COCOA_XIB_FILES}") 
# does not work correctly with CMake because CMake doesn't preserve the localized directory structure.
# So we must do it manually. Sadly, this will not show correctly in Xcode.
# CMake really should be changed to handle this natively. 

# FIXME: Expand to support GNUStep. Apple check ensures this won't break other platforms.
IF(APPLE)
# Make sure we can find the 'ibtool' program.
	FIND_PROGRAM(IBTOOL ibtool HINTS "${OSX_DEVELOPER_ROOT}/usr/bin" "/usr/bin")
	IF(${IBTOOL} STREQUAL "IBTOOL-NOTFOUND")
		MESSAGE(SEND_ERROR "ibtool can not be found")
	ENDIF()
ENDIF()

# xib_files is expected to be a list of full path and file names to each xib.
# e.g. /Users/ewing/IupCocoa/src/cocoa/xib/Base.lproj/CanonicalServiceMenu.xib
# target_name is the CMake target_name
# framework_base_name is expected to be just the base library name, 
# e.g. "iup", not "iup.framework"
# This is typically the same as the target_name unless you manually changed the framework name.
function(HELPERIB_COMPILE_XIBS_INTO_FRAMEWORK xib_files target_name framework_base_name wants_build_framework)
	# FIXME: Expand to support GNUStep. Apple check ensures this won't break other platforms.
	IF(APPLE)
		IF(wants_build_framework)	
			set(framework_name "${framework_base_name}.framework")

			IF(IOS)
				MESSAGE(SEND_ERROR "HELPERIB_COMPILE_XIBS_INTO_FRAMEWORK not implemented yet for iOS")
				# TODO: What is this dir for iOS?
#			set(intermediate_resource_dir "Versions/Current/Resources/")	
			ELSE()
				set(intermediate_resource_dir "Versions/Current/Resources/")
			ENDIF()

			# Compile the .xib files using the 'ibtool' program with the destination being the app package
			FOREACH( xib_path_and_file ${IUP_COCOA_XIB_FILES} )
				# xib_path_and_file has something like:
				# /Users/ewing/IupCocoa/src/cocoa/xib/Base.lproj/CanonicalServiceMenu.xib

				# This will extract Users/ewing/IupCocoa/src/cocoa/xib/Base.lproj to xib_path
				get_filename_component(xib_path "${xib_path_and_file}" DIRECTORY)
				# This will extract Base.lproj to localized_path_component	
				get_filename_component(localized_path_component "${xib_path}" NAME)
				# This will extract the CanonicalServiceMenu to base_file_name
				get_filename_component(base_file_name "${xib_path_and_file}" NAME_WE)
				




				ADD_CUSTOM_COMMAND( TARGET ${target_name} POST_BUILD
				COMMAND ${IBTOOL} --errors --warnings --notices --output-format human-readable-text --compile 
					# /foo/build/Debug/iup.framework/Versions/Current/Resources/Base.lproj/CanonicalServiceMenu.nib
					"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${framework_name}/${intermediate_resource_dir}/${localized_path_component}/${base_file_name}.nib"
					"${xib_path_and_file}"
					COMMENT "Compiling {xib_path_and_file}"
				)
			ENDFOREACH()
		ELSE()
			if(xib_files)
				MESSAGE(SEND_ERROR "xib files require wants_build_framework")
			endif()

		ENDIF(wants_build_framework)
	ENDIF(APPLE)
endfunction()

# Allows us to use an existing Info.plist
function(HELPERAPPLE_SET_INFOPLIST_FOR_FRAMEWORK target_name info_plist_file wants_build_framework)
	IF(APPLE)
		IF(wants_build_framework)
			set_target_properties(${target_name} PROPERTIES
				MACOSX_FRAMEWORK_INFO_PLIST ${info_plist_file}
		)
		ENDIF()
	ENDIF(APPLE)
endfunction()

