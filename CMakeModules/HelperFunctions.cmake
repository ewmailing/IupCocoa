# RPATH stuff. This MUST be set before any targets that use it are declared.
function(HELPER_SETUP_RPATH)
	IF("${CMAKE_SYSTEM}" MATCHES "Linux" AND NOT ANDROID)
		# RPATH $ORIGIN is only for Linux and Solaris
		# FIXME: match for Solaris too

		# use, i.e. don't skip the full RPATH for the build tree
		#	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

		# when building, don't use the install RPATH already
		# (but later on when installing)
		SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

		#SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

		# add the automatically determined parts of the RPATH
		# which point to directories outside the build tree to the install RPATH
		#	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


		# the RPATH to be used when installing, but only if it's not a system directory
		#	LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
		#	IF("${isSystemDir}" STREQUAL "-1")
		#   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

		#    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:\\$$ORIGIN")
		#	ENDIF("${isSystemDir}" STREQUAL "-1")
		SET(CMAKE_INSTALL_RPATH "\$ORIGIN")
		#    SET(CMAKE_INSTALL_RPATH "/usr/lib${CMAKE_INSTALL_RPATH}:\\\$ORIGIN")
	ELSEIF(APPLE)
		set(CMAKE_MACOSX_RPATH 1)
		# use, i.e. don't skip the full RPATH for the build tree
		#	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

		# when building, don't use the install RPATH already
		# (but later on when installing)
		SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

		#SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

		# add the automatically determined parts of the RPATH
		# which point to directories outside the build tree to the install RPATH
		SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)	
	ENDIF()
endfunction()

# This is an uber-ADD_LIBRARY wrapper function
# which supports Apple style frameworks and regular libraries.
# It also deals with TARGET_LINK_LIBRARIES.
# It takes a uri_name which can also be useful for other platforms that need it.
# This also takes a list of foriegn source files which is useful for Emscripten (.js) and potentially other systems (Java/Android).
function(HELPER_CREATE_LIBRARY library_name wants_build_shared_library wants_build_framework source_file_list public_headers private_headers source_headers foreign_source_files linked_libs_list library_c_flags uri_name)

	IF(wants_build_shared_library)
		ADD_LIBRARY(${library_name} SHARED 
			${source_file_list}
			${public_headers}
	)
	ELSE()
		ADD_LIBRARY(${library_name} STATIC 
			${source_file_list}
			${public_headers}
		)
	ENDIF()

	TARGET_LINK_LIBRARIES(${library_name}
		${linked_libs_list}
	)

	IF(EMSCRIPTEN)
		em_link_js_library(${library_name} ${foreign_source_files})
	ENDIF()


	IF(APPLE)
		INCLUDE(XcodeDefaults)
		BLURRR_CONFIGURE_XCODE_DEFAULTS(${library_name})
	ENDIF()



	# Set Dynamic Library and Framework properties
	IF(wants_build_shared_library AND wants_build_framework)

		SET_TARGET_PROPERTIES(${library_name} PROPERTIES
			FRAMEWORK TRUE
	#		FRAMEWORK_VERSION "${LUA_FRAMEWORK_VERSION_NUMBER}"
			#  PRIVATE_HEADER "fooPrivate.h;fooBoth.h"
			PUBLIC_HEADER "${public_headers}"
	#		RESOURCE "${RESOURCE_FILES}"
			# INSTALL_NAME_DIR ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR}
			# BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH} 
	#		VERSION ${LUA_VERSION}
	#		SOVERSION ${LUA_COMPATIBILITY_VERSION}
			COMPILE_FLAGS "${library_c_flags}"
		)
		
		# I moved the INSTALL_NAME_DIR to use SET_PROPERTY instead because
		# SET_TARGET_PROPERTIES will fail if the variable is empty.
		#	SET_PROPERTY(TARGET lua_library_dynamic PROPERTY INSTALL_NAME_DIR ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR})
		#	SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH NO)
		# If the user deletes the install_name path, use the gcc default
		# and disable the option completely. But CMake by default places
		# the name of the library in the install_name if 
		# BUILD_WITH_INSTALL_RPATH is ON. So to avoid this, I need to 
		# disable the switch.
	#	IF(CMAKE_FRAMEWORK_INSTALL_NAME_DIR)
			#		MESSAGE("got dir ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR}")
	#		SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH})

	#	ELSE(CMAKE_FRAMEWORK_INSTALL_NAME_DIR)
			#		MESSAGE("no dir ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR}")
	#		SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH NO)

	#	ENDIF(CMAKE_FRAMEWORK_INSTALL_NAME_DIR)



		# Short Version is the "marketing version". It is the version
		# the user sees in an information panel.
	#	SET(MACOSX_FRAMEWORK_SHORT_VERSION_STRING "${LUA_MAJOR_VERSION}.${LUA_MINOR_VERSION}.${LUA_PATCH_VERSION}")
		# Bundle version is the version the OS looks at.
	#	SET(MACOSX_FRAMEWORK_BUNDLE_VERSION "${LUA_MAJOR_VERSION}.${LUA_MINOR_VERSION}.${LUA_PATCH_VERSION}")
		SET(MACOSX_FRAMEWORK_IDENTIFIER ${uri_name})
		#	SET(MACOSX_BUNDLE_BUNDLE_NAME "lua" )

	ELSEIF(wants_build_shared_library)
		IF(APPLE)
			SET_TARGET_PROPERTIES(${library_name} PROPERTIES
				FRAMEWORK FALSE
				# INSTALL_NAME_DIR ${CMAKE_DYLIB_INSTALL_NAME_DIR}
				# BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH} 
	#			VERSION ${LUA_VERSION}
	#			SOVERSION ${LUA_COMPATIBILITY_VERSION}
				COMPILE_FLAGS "${library_c_flags}"
			)
			# I moved the INSTALL_NAME_DIR to use SET_PROPERTY instead because
			# SET_TARGET_PROPERTIES will fail if the variable is empty.
	#		SET_PROPERTY(TARGET lua_library_dynamic PROPERTY INSTALL_NAME_DIR ${CMAKE_DYLIB_INSTALL_NAME_DIR})
	#		SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH NO)
			# If the user deletes the install_name path, use the gcc default
			# and disable the option completely. But CMake by default places
			# the name of the library in the install_name if 
			# BUILD_WITH_INSTALL_RPATH is ON. So to avoid this, I need to 
			# disable the switch.
	#		IF(CMAKE_DYLIB_INSTALL_NAME_DIR)
	#			SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH})
	#		ELSE(CMAKE_DYLIB_INSTALL_NAME_DIR)
	#			SET_PROPERTY(TARGET lua_library_dynamic PROPERTY BUILD_WITH_INSTALL_RPATH OFF)
	#		ENDIF(CMAKE_DYLIB_INSTALL_NAME_DIR)



		ELSEIF(UNIX)
			IF(NOT ANDROID)
				SET_TARGET_PROPERTIES(${library_name} PROPERTIES
					INSTALL_RPATH ${CMAKE_INSTALL_RPATH}
					BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH} 
	#				VERSION ${LUA_VERSION}
	#				SOVERSION ${LUA_SOVERSION}
					COMPILE_FLAGS "${library_c_flags}"
				)
			ELSE(ANDROID)
				SET_TARGET_PROPERTIES(${library_name} PROPERTIES
					COMPILE_FLAGS "${library_c_flags}"
				)
			ENDIF(NOT ANDROID)
		
		ELSEIF(WIN32)
			SET_TARGET_PROPERTIES(${library_name} PROPERTIES
	#			VERSION ${LUA_VERSION}
	#			SOVERSION ${LUA_COMPATIBILITY_VERSION}
				COMPILE_FLAGS "${library_c_flags}"
			)

		ELSE(APPLE)
			SET_TARGET_PROPERTIES(${library_name} PROPERTIES
	#			VERSION ${LUA_VERSION}
	#			SOVERSION ${LUA_COMPATIBILITY_VERSION}
				COMPILE_FLAGS "${library_c_flags}"
			)

		ENDIF(APPLE)

	ELSE()
		SET_TARGET_PROPERTIES(${library_name} PROPERTIES
			COMPILE_FLAGS "${library_c_flags}"
		)



	ENDIF(wants_build_shared_library AND wants_build_framework)


	# Install location stuff. Maybe this should be broken into a separate function.
	IF(NOT wants_build_framework)
		INSTALL(TARGETS
			${library_name}
			RUNTIME DESTINATION lib		
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
			PUBLIC_HEADER DESTINATION include
		)
		# Hmmm...install TARGETS is not installing headers
		INSTALL(FILES ${public_headers} DESTINATION include)
	ELSE()
		INSTALL(TARGETS
			${library_name}
			FRAMEWORK DESTINATION Frameworks
		)


	ENDIF()

endfunction()

# indirect_link_libs is for static libraries, where all dependencies must be explicitly linked 
function(HELPER_CREATE_EXECUTABLE exe_name source_file_list is_using_shared_libs direct_link_libs indirect_link_libs c_flags link_flags exclude_from_all_target)
	if(NOT ANDROID)
	
		if(exclude_from_all_target)
			ADD_EXECUTABLE(${exe_name} WIN32 MACOSX_BUNDLE EXCLUDE_FROM_ALL
				${source_file_list}
			)
		else()
			ADD_EXECUTABLE(${exe_name} WIN32 MACOSX_BUNDLE
				${source_file_list}
			)
		endif()


		if(is_using_shared_libs)
			TARGET_LINK_LIBRARIES(${exe_name} ${direct_link_libs})
		else()
			TARGET_LINK_LIBRARIES(${exe_name} ${direct_link_libs} ${indirect_link_libs})
		endif()

		SET_TARGET_PROPERTIES(${exe_name} PROPERTIES
			COMPILE_FLAGS "${c_flags}"
			LINK_FLAGS "${link_flags}"
		)

	endif()

endfunction()


function(HELPER_CREATE_MODULE library_name wants_build_shared_library source_file_list public_headers private_headers source_headers foreign_source_files linked_libs_list library_c_flags uri_name)

	IF(wants_build_shared_library)
		ADD_LIBRARY(${library_name} MODULE 
			${source_file_list}
			${public_headers}
	)
	ELSE()
		ADD_LIBRARY(${library_name} STATIC 
			${source_file_list}
			${public_headers}
		)
	ENDIF()

	TARGET_LINK_LIBRARIES(${library_name}
		${linked_libs_list}
	)

	IF(EMSCRIPTEN)
		em_link_js_library(${library_name} ${foreign_source_files})
	ENDIF()


	IF(APPLE)
		INCLUDE(XcodeDefaults)
		BLURRR_CONFIGURE_XCODE_DEFAULTS(${library_name})
	ENDIF()

	# Android must keep the lib prefix
	IF(ANDROID)
			SET_TARGET_PROPERTIES(${library_name} PROPERTIES
				COMPILE_FLAGS "${library_c_flags}"
				LINK_FLAGS "-shared -fpic"
			)
	ELSEIF(APPLE)
		IF(wants_build_shared_library)
			MESSAGE("apple wants module")
			SET_TARGET_PROPERTIES(${library_name} PROPERTIES
				PREFIX ""
				COMPILE_FLAGS "${library_c_flags}"
				LINK_FLAGS "-flat_namespace -undefined suppress"
				#LINK_FLAGS "-undefined dynamic_lookup"
			)
		ELSE()
			MESSAGE("apple nowants module")
			SET_TARGET_PROPERTIES(${library_name} PROPERTIES
				PREFIX ""
				COMPILE_FLAGS "${library_c_flags}"
			)
		ENDIF()
	ELSEIF(UNIX)
			SET_TARGET_PROPERTIES(${library_name} PROPERTIES
				PREFIX ""
				COMPILE_FLAGS "${library_c_flags}"
				LINK_FLAGS "-shared -fpic"
			)

	ELSE()
			SET_TARGET_PROPERTIES(${library_name} PROPERTIES
				PREFIX ""
				COMPILE_FLAGS "${library_c_flags}"
			)
	ENDIF()




	

endfunction()

function(HELPER_SETUP_UNINSTALL_TARGET)
	CONFIGURE_FILE(
		"${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/CMakeModules/cmake_uninstall.cmake"
		IMMEDIATE @ONLY)
	ADD_CUSTOM_TARGET(uninstall
		"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/CMakeModules/cmake_uninstall.cmake")
endfunction()

