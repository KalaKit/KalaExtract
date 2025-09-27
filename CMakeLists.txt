cmake_minimum_required(VERSION 3.29.2)

set(PROGRAM_VERSION_NUMBER 1.0.0.0)

project("KalaExtract" VERSION ${PROGRAM_VERSION_NUMBER} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform Detection
if (WIN32)
    message(STATUS "[KalaExtract] Platform = Windows")
else()
    message(FATAL_ERROR "[KalaExtract] Unsupported platform. Only Windows is supported.")
endif()

# Build Type Detection
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(IS_RELEASE TRUE)
else()
    message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'! Must be Debug, Release, or RelWithDebInfo.")
endif()

# Runtime lib handling for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Directory Setup
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(EXT_SHARED_DIR "${CMAKE_SOURCE_DIR}/_external_shared")

# Source Files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*/*.cpp"
)

# Executable
add_executable(KalaExtract ${SOURCE_FILES})

# Enable exception handling for MSVC
if (MSVC)
    target_compile_options(KalaExtract PRIVATE /EHsc)
endif()

set_target_properties(KalaExtract PROPERTIES OUTPUT_NAME "KalaExtract")
target_compile_features(KalaExtract PRIVATE cxx_std_20)

# Includes
file(GLOB_RECURSE HEADERS
	configure_depends
	"${CMAKE_SOURCE_DIR}/include/*.hpp"
)
target_sources(KalaExtract PRIVATE ${HEADERS})
target_include_directories(KalaExtract PRIVATE
	"${INCLUDE_DIR}"
	"${EXT_SHARED_DIR}"
)

# Preprocessor Defines
target_compile_definitions(KalaExtract PRIVATE 
	WIN32_LEAN_AND_MEAN
	NOMINMAX)

# Hide console in release mode
#if(IS_RELEASE)
#    set_target_properties(KalaExtract PROPERTIES WIN32_EXECUTABLE TRUE)
#    if (MSVC)
#        set_target_properties(KalaExtract PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
#    endif()
#endif()

# Installation
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS KalaExtract DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy important files
set(COPY_TARGETS README.md LICENSE.md)
foreach(FILE ${COPY_TARGETS})
add_custom_command(TARGET KalaExtract POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f 
        "$<TARGET_FILE_DIR:KalaExtract>/${FILE}"
    COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_SOURCE_DIR}/${FILE}"
		"$<TARGET_FILE_DIR:KalaExtract>/${FILE}"
)
endforeach()

# Copy entire docs folder
add_custom_command(TARGET KalaExtract POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        "$<TARGET_FILE_DIR:KalaExtract>/docs"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/docs"
        "$<TARGET_FILE_DIR:KalaExtract>/docs"
)

# Package
include(CPack)
