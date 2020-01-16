cmake_minimum_required(VERSION 2.6)

SET(LIB_NAME nanut)

OPTION(BUILD_SHARED_LIBRARY "Build shared library" OFF)

OPTION(ENABLE_MEMORY_STATISTICS "" OFF)


ADD_DEFINITIONS(-pipe --std=gnu99 -ftabstop=4 -Wno-unused-function)
ADD_DEFINITIONS(-Wall -Wmissing-declarations -Winit-self -Wswitch-enum -Wundef)
ADD_DEFINITIONS(-Wmissing-field-initializers -Wredundant-decls -Wpointer-arith)

ADD_DEFINITIONS(-D_GNU_SOURCE)

SET(static_library_target ${LIB_NAME}_static)
SET(shared_library_target ${LIB_NAME})


FILE(GLOB LIB_SOURCES ut_*.c)

ADD_LIBRARY(${static_library_target} STATIC ${LIB_SOURCES})

IF(BUILD_SHARED_LIBRARY)
	ADD_LIBRARY(${shared_library_target} SHARED ${LIB_SOURCES})
ENDIF(BUILD_SHARED_LIBRARY)


SET(CMAKE_C_FLAGS_RELEASE "-Os -Werror -ffast-math -fmerge-all-constants -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables")
SET(CMAKE_C_FLAGS_DEBUG   "-O0 -Werror -g3")


SET_TARGET_PROPERTIES(${static_library_target} PROPERTIES LINK_FLAGS -Wl,--gc-sections,--build-id=none)

IF(BUILD_SHARED_LIBRARY)
	SET_TARGET_PROPERTIES(${shared_library_target} PROPERTIES LINK_FLAGS -Wl,--gc-sections,--build-id=none)
ENDIF()


INSTALL(TARGETS ${static_library_target}
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

IF(BUILD_SHARED_LIBRARY)
	INSTALL(TARGETS ${shared_library_target}
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin
	)
ENDIF()
