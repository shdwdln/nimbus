# CMakeLists.txt

# Copyright (C) 2007-2011 Glenn Randers-Pehrson

# This code is released under the libpng license.
# For conditions of distribution and use, see the disclaimer
# and license in png.h


cmake_minimum_required(VERSION 2.4.4)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

if(UNIX AND NOT DEFINED CMAKE_BUILD_TYPE)
  if(CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 4)
    # workaround CMake 2.4.x bug
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
        "Choose the type of build, options are:
           None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used)
           Debug
           Release
           RelWithDebInfo
           MinSizeRel.")
  else()
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
        "Choose the type of build, options are:
           None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used)
           Debug
           Release
           RelWithDebInfo
           MinSizeRel.")
  endif()
endif()

project(libpng C)
enable_testing()

set(PNGLIB_MAJOR 1)
set(PNGLIB_MINOR 5)
set(PNGLIB_RELEASE 6)
set(PNGLIB_NAME libpng${PNGLIB_MAJOR}${PNGLIB_MINOR})
set(PNGLIB_VERSION ${PNGLIB_MAJOR}.${PNGLIB_MINOR}.${PNGLIB_RELEASE})

#PhysBAM Edit: add zlib path
SET(CMAKE_PREFIX_PATH "../../")

# needed packages
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})

if(NOT WIN32)
  find_library(M_LIBRARY
    NAMES m
    PATHS /usr/lib /usr/local/lib
  )
  if(NOT M_LIBRARY)
    message(STATUS
      "math library 'libm' not found - floating point support disabled")
  endif()
else()
  # not needed on windows
  set(M_LIBRARY "")
endif()

# COMMAND LINE OPTIONS
if(DEFINED PNG_SHARED)
  option(PNG_SHARED "Build shared lib" ${PNG_SHARED})
else()
  option(PNG_SHARED "Build shared lib" ON)
endif()
#PhysBAM Edit: don't need static lib
option(PNG_STATIC "Build static lib" OFF)
# if(DEFINED PNG_STATIC)
  # option(PNG_STATIC "Build static lib" ${PNG_STATIC})
# else()
  # option(PNG_STATIC "Build static lib" ON)
# endif()

#PhysBAM Edit: don't need tests
option(PNG_TESTS  "Build libpng tests" NO)

# Many more configuration options could be added here
option(PNG_DEBUG         "Build with debug output" NO)
option(PNGARG            "Disable ANSI-C prototypes" NO)

# SET LIBNAME
set(PNG_LIB_NAME png${PNGLIB_MAJOR}${PNGLIB_MINOR})

# to distinguish between debug and release lib
set(CMAKE_DEBUG_POSTFIX "d")

# Use the prebuilt pnglibconf.h file from the scripts folder
# TODO: fix this by building with awk; without this no cmake build can be
# configured directly (to do so indirectly use your local awk to build a
# pnglibconf.h in the build directory.)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf.h.prebuilt
               ${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# OUR SOURCES
set(libpng_public_hdrs
  png.h
  pngconf.h
  ${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h
)
set(libpng_sources
  ${libpng_public_hdrs}
  pngdebug.h
  pnginfo.h
  pngpriv.h
  pngstruct.h
  png.c
  pngerror.c
  pngget.c
  pngmem.c
  pngpread.c
  pngread.c
  pngrio.c
  pngrtran.c
  pngrutil.c
  pngset.c
  pngtrans.c
  pngwio.c
  pngwrite.c
  pngwtran.c
  pngwutil.c
)
set(pngtest_sources
  pngtest.c
)
set(pngvalid_sources
  pngvalid.c
)
# SOME NEEDED DEFINITIONS

add_definitions(-DPNG_CONFIGURE_LIBPNG)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

if(PNG_DEBUG)
  add_definitions(-DPNG_DEBUG)
endif()

# NOW BUILD OUR TARGET
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${ZLIB_INCLUDE_DIR})

if(PNG_SHARED)
  add_library(${PNG_LIB_NAME} SHARED ${libpng_sources})
  if(MSVC)
    # msvc does not append 'lib' - do it here to have consistent name
    set_target_properties(${PNG_LIB_NAME} PROPERTIES PREFIX "lib")
    set_target_properties(${PNG_LIB_NAME} PROPERTIES IMPORT_PREFIX "lib")
  endif()
  target_link_libraries(${PNG_LIB_NAME} ${ZLIB_LIBRARY} ${M_LIBRARY})
endif()

if(PNG_STATIC)
# does not work without changing name
  set(PNG_LIB_NAME_STATIC ${PNG_LIB_NAME}_static)
  add_library(${PNG_LIB_NAME_STATIC} STATIC ${libpng_sources})
  if(MSVC)
    # msvc does not append 'lib' - do it here to have consistent name
    set_target_properties(${PNG_LIB_NAME_STATIC} PROPERTIES PREFIX "lib")
  endif()
  target_link_libraries(${PNG_LIB_NAME_STATIC} ${ZLIB_LIBRARY} ${M_LIBRARY})
endif()

if(PNG_SHARED AND WIN32)
  set_target_properties(${PNG_LIB_NAME} PROPERTIES DEFINE_SYMBOL PNG_BUILD_DLL)
endif()

if(PNG_TESTS AND PNG_SHARED)
  # does not work with msvc due to png_lib_ver issue
  add_executable(pngtest ${pngtest_sources})
  target_link_libraries(pngtest ${PNG_LIB_NAME})
  add_test(pngtest ./pngtest ${CMAKE_CURRENT_SOURCE_DIR}/pngtest.png)
  #
  add_executable(pngvalid ${pngvalid_sources})
  target_link_libraries(pngvalid ${PNG_LIB_NAME})
  add_test(pngvalid ./pngvalid)
endif()

#PhysBAM Edit: set install path
SET(CMAKE_INSTALL_PREFIX "../../")

# Ensure the CMAKE_LIBRARY_OUTPUT_DIRECTORY is set
IF(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  IF(WIN32)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "bin")
  ELSE(WIN32)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib")
  ENDIF(WIN32)
ENDIF(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)

# Set a variable with CMake code which:
# Creates a symlink from src to dest (if possible) or alternatively
# copies if different.
macro(CREATE_SYMLINK SRC_FILE DEST_FILE)
  FILE(REMOVE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DEST_FILE})
  if(WIN32 AND NOT CYGWIN)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DEST_FILE}   ${CMAKE_CURRENT_BINARY_DIR}/${DEST_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy        ${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILE} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DEST_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy        ${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${DEST_FILE}
        DEPENDS ${PNG_LIB_NAME} ${PNG_LIB_NAME_STATIC}
        )
    ADD_CUSTOM_TARGET(${DEST_FILE}_COPY ALL DEPENDS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DEST_FILE})
  else(WIN32 AND NOT CYGWIN)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC_FILE} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DEST_FILE} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC_FILE} ${DEST_FILE} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  endif(WIN32 AND NOT CYGWIN)
endmacro()

# libpng is a library so default to 'lib'
if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)

# CREATE PKGCONFIG FILES
# we use the same files like ./configure, so we have to set its vars
# Only do this on Windows for Cygwin - the files don't make much sense outside
# a UNIX look alike
if(NOT WIN32 OR CYGWIN)
set(prefix      ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(libdir      ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(includedir  ${CMAKE_INSTALL_PREFIX}/include)
set(LIBS        "-lz -lm")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libpng.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PNGLIB_NAME}.pc @ONLY)
CREATE_SYMLINK(${PNGLIB_NAME}.pc libpng.pc)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libpng-config.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PNGLIB_NAME}-config @ONLY)
CREATE_SYMLINK(${PNGLIB_NAME}-config libpng-config)
endif(NOT WIN32 OR CYGWIN)

# SET UP LINKS
if(PNG_SHARED)
  set_target_properties(${PNG_LIB_NAME} PROPERTIES
#   VERSION 15.${PNGLIB_RELEASE}.1.5.6
    VERSION 15.${PNGLIB_RELEASE}.0
    SOVERSION 15
    CLEAN_DIRECT_OUTPUT 1)
endif()
if(PNG_STATIC)
  # MSVC doesn't use a different file extension for shared vs. static
  # libs.  We are able to change OUTPUT_NAME to remove the _static
  # for all other platforms.
  if(NOT MSVC)
    set_target_properties(${PNG_LIB_NAME_STATIC} PROPERTIES
      OUTPUT_NAME ${PNG_LIB_NAME}
      CLEAN_DIRECT_OUTPUT 1)
  endif()
endif()

# If CMake > 2.4.x, we set a variable used below to export
# targets to an export file.
# TODO: Use VERSION_GREATER after our cmake_minimum_required >= 2.6.2
if(CMAKE_MAJOR_VERSION GREATER 1 AND CMAKE_MINOR_VERSION GREATER 4)
  set(PNG_EXPORT_RULE EXPORT libpng)
elseif(CMAKE_MAJOR_VERSION GREATER 2) # future proof
  set(PNG_EXPORT_RULE EXPORT libpng)
endif()

# INSTALL
if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL )
  if(PNG_SHARED)
    install(TARGETS ${PNG_LIB_NAME}
        ${PNG_EXPORT_RULE}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

    # Create a symlink for libpng.dll.a => libpng15.dll.a on Cygwin
    if(CYGWIN)
       get_target_property(BUILD_TARGET_LOCATION ${PNG_LIB_NAME} LOCATION_${CMAKE_BUILD_TYPE})
       get_filename_component(BUILD_TARGET_FILE ${BUILD_TARGET_LOCATION} NAME)
       CREATE_SYMLINK(${BUILD_TARGET_FILE} libpng${CMAKE_IMPORT_LIBRARY_SUFFIX})
       install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libpng${CMAKE_IMPORT_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif(CYGWIN)

    if(NOT WIN32)
      get_target_property(BUILD_TARGET_LOCATION ${PNG_LIB_NAME} LOCATION_${CMAKE_BUILD_TYPE})
      get_filename_component(BUILD_TARGET_FILE ${BUILD_TARGET_LOCATION} NAME)
      CREATE_SYMLINK(${BUILD_TARGET_FILE} libpng${CMAKE_SHARED_LIBRARY_SUFFIX})
      install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libpng${CMAKE_SHARED_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif(NOT WIN32)
  endif(PNG_SHARED)

  if(PNG_STATIC)
    install(TARGETS ${PNG_LIB_NAME_STATIC}
        ${PNG_EXPORT_RULE}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    if(NOT WIN32 OR CYGWIN)
      get_target_property(BUILD_TARGET_LOCATION ${PNG_LIB_NAME_STATIC} LOCATION_${CMAKE_BUILD_TYPE})
      get_filename_component(BUILD_TARGET_FILE ${BUILD_TARGET_LOCATION} NAME)
      CREATE_SYMLINK(${BUILD_TARGET_FILE} libpng${CMAKE_STATIC_LIBRARY_SUFFIX})
      install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libpng${CMAKE_STATIC_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif(NOT WIN32 OR CYGWIN)
 endif()
endif()

if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL )
  install(FILES ${libpng_public_hdrs}   DESTINATION include)
  #PhysBAM Edit: don't need the extra directory
  #install(FILES ${libpng_public_hdrs}   DESTINATION include/${PNGLIB_NAME})
endif()
#PhysBAM Edit: WIN32 doesn't need the config files -> AND NOT WIN32
if(NOT SKIP_INSTALL_EXECUTABLES AND NOT SKIP_INSTALL_ALL AND NOT WIN32)
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/libpng-config DESTINATION bin)
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PNGLIB_NAME}-config
          DESTINATION bin)
endif()
#PhysBAM Edit: WIN32 doesn't need the config files -> AND NOT WIN32, and don't need manpages
if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL AND NOT WIN32)
  # Install man pages
  # if(NOT PNG_MAN_DIR)
    # set(PNG_MAN_DIR "share/man")
  # endif()
  # install(FILES libpng.3 libpngpf.3      DESTINATION ${PNG_MAN_DIR}/man3)
  # install(FILES png.5                    DESTINATION ${PNG_MAN_DIR}/man5)
  # Install pkg-config files
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libpng.pc
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/libpng-config
          DESTINATION bin)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PNGLIB_NAME}.pc
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PNGLIB_NAME}-config
          DESTINATION bin)
endif()

# On versions of CMake that support it, create an export file CMake
# users can include() to import our targets
#PhysBAM Edit: also not needed
#if(PNG_EXPORT_RULE AND NOT SKIP_INSTALL_EXPORT AND NOT SKIP_INSTALL_ALL )
#  install(EXPORT libpng DESTINATION lib/libpng FILE lib${PNG_LIB_NAME}.cmake)
#endif()

# what's with libpng-$VER%.txt and all the extra files?

# UNINSTALL
# do we need this?

# DIST
# do we need this?

# to create msvc import lib for mingw compiled shared lib
# pexports libpng.dll > libpng.def
# lib /def:libpng.def /machine:x86

