cmake_minimum_required(VERSION 2.6)

PROJECT(nanut_tests)

ADD_DEFINITIONS(-pipe --std=gnu99 -ftabstop=4 -Wno-unused-function)
ADD_DEFINITIONS(-Wall -Wmissing-declarations -Winit-self -Wswitch-enum -Wundef)
ADD_DEFINITIONS(-Wmissing-field-initializers -Wredundant-decls -Wpointer-arith)

ADD_DEFINITIONS(-D_GNU_SOURCE)

file(GLOB sources "*.c")

ADD_EXECUTABLE(tests ${sources})
TARGET_LINK_LIBRARIES(tests nanut_static)


SET(CMAKE_C_FLAGS_RELEASE "-Os -Werror -ffast-math -fmerge-all-constants -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables")
SET(CMAKE_C_FLAGS_DEBUG   "-O0 -g3")


SET_TARGET_PROPERTIES(tests PROPERTIES COMPILE_FLAGS -fvisibility=hidden)
SET_TARGET_PROPERTIES(tests PROPERTIES LINK_FLAGS -Wl,--gc-sections,--build-id=none)


INSTALL(TARGETS tests
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

