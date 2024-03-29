# examples/MyModule/CMakeLists.txt
#
# This file is part of NEST.
#
# Copyright (C) 2004 The NEST Initiative
#
# NEST is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# NEST is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NEST.  If not, see <http://www.gnu.org/licenses/>.

include(FetchContent)
include(CPM.cmake)

cmake_minimum_required( VERSION 3.14 )
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
set( CMAKE_POSITION_INDEPENDENT_CODE ON)


# This CMakeLists.txt is configured to build your external module for NEST. For
# illustrative reasons this module is called 'my' (change SHORT_NAME to your
# preferred module name). NEST requires you to extend the 'SLIModule' (see
# mymodule.h and mymodule.cpp as an example) and provide a module header
# (see MODULE_HEADER). The subsequent instructions
#
# The configuration requires a compiled and installed NEST; if `nest-config` is
# not in the PATH, please specify the absolute path with `-Dwith-nest=...`.
#
# For more informations on how to extend and use your module see:
#           https://nest.github.io/nest-simulator/extension_modules

# 1) Name your module here, i.e. add later with -Dexternal-modules=my:
set( SHORT_NAME insite )

#    the complete module name is here:
set( MODULE_NAME ${SHORT_NAME}module )

include_directories(src)
# 2) Add all your sources here and specify dependencies, e.g., link libraries
set( MODULE_SOURCES
     src/insitemodule.h src/insitemodule.cpp
     src/parameters.h 
     src/json_serializer.h src/json_serializer.cpp
     src/recording_backend_insite.h src/recording_backend_insite.cpp
     src/http_server.hpp src/http_server.cpp
     src/serialize.hpp src/serialize.cpp
     src/storage/data_storage.hpp src/storage/data_storage.cpp
     src/storage/spikedetector_storage.hpp src/storage/spikedetector_storage.cpp
     src/storage/multimeter_storage.hpp src/storage/multimeter_storage.cpp
     src/schema_generated.h
     src/stopwatch_helper.h src/stopwatch_helper.cpp
   )

# 3) We require a header name like this:
set( MODULE_HEADER ${MODULE_NAME}.h )
# containing the class description of the class extending the SLIModule

# 4) Specify your module version
set( MODULE_VERSION_MAJOR 0 )
set( MODULE_VERSION_MINOR 1 )
set( MODULE_VERSION "${MODULE_VERSION_MAJOR}.${MODULE_VERSION_MINOR}" )

# 5) Leave the rest as is. All files in `sli` will be installed to
#    `share/nest/sli/`, so that NEST will find the during initialization.

# Leave the call to "project(...)" for after the compiler is determined.

# Set the `nest-config` executable to use during configuration.
set( with-nest OFF CACHE STRING "Specify the `nest-config` executable." )
set(ASIO_INCLUDE_DIR extern/asio/asio/include)


# If it is not set, look for a `nest-config` in the PATH.
if ( NOT with-nest )
  # try find the program ourselves
  find_program( NEST_CONFIG
      NAMES nest-config
      )
  if ( NEST_CONFIG STREQUAL "NEST_CONFIG-NOTFOUND" )
    message( FATAL_ERROR "Cannot find the program `nest-config`. Specify via -Dwith-nest=... ." )
  endif ()
else ()
  set( NEST_CONFIG ${with-nest} )
endif ()

# Use `nest-config` to get the compile and installation options used with the
# NEST installation.

# Get the compiler that was used for NEST.
execute_process(
    COMMAND ${NEST_CONFIG} --compiler
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_COMPILER
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# One check on first execution, if `nest-config` is working.
if ( NOT RES_VAR EQUAL 0 )
  message( FATAL_ERROR "Cannot run `${NEST_CONFIG}`. Please specify correct `nest-config` via -Dwith-nest=... " )
endif ()

# Setting the compiler has to happen before the call to "project(...)" function.
set( CMAKE_CXX_COMPILER "${NEST_COMPILER}" )


CPMAddPackage("gh:zaphoyd/websocketpp#0.8.2")
CPMAddPackage("gh:gabime/spdlog@1.10.0")
CPMAddPackage("gh:google/flatbuffers@2.0.6")
CPMAddPackage("gh:OlivierLDff/asio.cmake@1.1.3")
CPMAddPackage("gh:TartanLlama/optional@1.0.0")


CPMAddPackage(
    NAME rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson
    GIT_TAG master
    DOWNLOAD_ONLY TRUE
    EXCLUDE_FROM_ALL TRUE
)

CPMAddPackage(
    NAME Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow
    GIT_TAG master
    EXCLUDE_FROM_ALL TRUE
    OPTIONS "CROW_INSTALL OFF"
)
project( ${MODULE_NAME} CXX )

# Get the Python executable (for help generation).
execute_process(
    COMMAND ${NEST_CONFIG} --python-executable
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE PYTHON_EXECUTABLE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the Python executable (for help generation).
execute_process(
    COMMAND ${NEST_CONFIG} --python-version
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE PYTHON_VERSION_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the install prefix.
execute_process(
    COMMAND ${NEST_CONFIG} --prefix
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_INSTALL_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the CXXFLAGS.
execute_process(
    COMMAND ${NEST_CONFIG} --cflags
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_CXXFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the Includes.
execute_process(
    COMMAND ${NEST_CONFIG} --includes
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_INCLUDES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if ( NEST_INCLUDES )
  # make a cmake list
  string( REPLACE " " ";" NEST_INCLUDES_LIST "${NEST_INCLUDES}" )
  foreach ( inc_complete ${NEST_INCLUDES_LIST} )
    # if it is actually a -Iincludedir
    if ( "${inc_complete}" MATCHES "^-I.*" )
      # get the directory
      string( REGEX REPLACE "^-I(.*)" "\\1" inc "${inc_complete}" )
      # and check whether it is a directory
      if ( IS_DIRECTORY "${inc}" )
        include_directories( "${inc}" )
      endif ()
    endif ()
  endforeach ()
endif ()

# Get, if NEST is build as a (mostly) static application. If yes, also only build
# static library.
execute_process(
    COMMAND ${NEST_CONFIG} --static-libraries
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_STATIC_LIB
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if ( NEST_STATIC_LIB )
  set( BUILD_SHARED_LIBS OFF )
else ()
  set( BUILD_SHARED_LIBS ON )
endif ()

# Get all linked libraries.
execute_process(
    COMMAND ${NEST_CONFIG} --libs
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the data install dir.
execute_process(
    COMMAND ${NEST_CONFIG} --datadir
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_INSTALL_DATADIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the documentation install dir.
execute_process(
    COMMAND ${NEST_CONFIG} --docdir
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_INSTALL_DOCDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the library install dir.
execute_process(
    COMMAND ${NEST_CONFIG} --libdir
    RESULT_VARIABLE RES_VAR
    OUTPUT_VARIABLE NEST_LIBDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# on OS X
set( CMAKE_MACOSX_RPATH ON )

# Use the NEST_INSTALL_* variables as CMAKE_INSTALL_*, if not set explicitly.
if ( "${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local" )
  set( CMAKE_INSTALL_PREFIX "${NEST_INSTALL_PREFIX}" CACHE STRING "Install path prefix, prepended onto install directories." FORCE )
  set( CMAKE_INSTALL_LIBDIR "${NEST_LIBDIR}/nest" CACHE STRING "object code libraries (lib/nest or lib64/nest or lib/<multiarch-tuple>/nest on Debian)" FORCE )
  set( CMAKE_INSTALL_DOCDIR "${NEST_INSTALL_DOCDIR}" CACHE STRING "documentation root (DATAROOTDIR/doc/nest)" FORCE )
  set( CMAKE_INSTALL_DATADIR "${NEST_INSTALL_DATADIR}" CACHE STRING "read-only architecture-independent data (DATAROOTDIR/nest)" FORCE )
endif ()

include( GNUInstallDirs )

# CPack stuff. Required for target `dist`.
set( CPACK_GENERATOR TGZ )
set( CPACK_SOURCE_GENERATOR TGZ )

set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "NEST Module ${MODULE_NAME}" )
set( CPACK_PACKAGE_VENDOR "NEST Initiative (https://www.nest-initiative.org/)" )

set( CPACK_PACKAGE_VERSION_MAJOR ${MODULE_VERSION_MAJOR} )
set( CPACK_PACKAGE_VERSION_MINOR ${MODULE_VERSION_MINOR} )
set( CPACK_PACKAGE_VERSION ${MODULE_VERSION} )

set( CPACK_SOURCE_IGNORE_FILES
    "\\\\.gitignore"
    "\\\\.git/"
    "\\\\.travis\\\\.yml"

    # if we have in source builds
    "/build/"
    "/_CPack_Packages/"
    "CMakeFiles/"
    "cmake_install\\\\.cmake"
    "Makefile.*"
    "CMakeCache\\\\.txt"
    "CPackConfig\\\\.cmake"
    "CPackSourceConfig\\\\.cmake"
    )
set( CPACK_SOURCE_PACKAGE_FILE_NAME ${MODULE_NAME} )

set( CPACK_PACKAGE_INSTALL_DIRECTORY "${MODULE_NAME} ${MODULE_VERSION}" )
include( CPack )

# add make dist target
add_custom_target( dist
    COMMAND ${CMAKE_MAKE_PROGRAM} package_source
    # not sure about this... seems, that it will be removed before dist...
    # DEPENDS doc
    COMMENT "Creating a source distribution from ${MODULE_NAME}..."
    )

find_package(Threads REQUIRED)
add_compile_definitions(ASIO_STANDALONE _WEBSOCKETPP_CPP11_STRICT_)

include_directories(
  insite-access-node
  PRIVATE
  ${CMAKE_BINARY_DIR}/_deps/rapidjson-src/include
  ${CMAKE_BINARY_DIR}/_deps/websocketpp-src
)
if ( BUILD_SHARED_LIBS )
  # When building shared libraries, also create a module for loading at runtime
  # with the `Install` command.
  add_library( ${MODULE_NAME}_module MODULE ${MODULE_SOURCES} )
  target_link_libraries(${MODULE_NAME}_module PRIVATE ${USER_LINK_LIBRARIES})
  set_target_properties( ${MODULE_NAME}_module
      PROPERTIES
      COMPILE_FLAGS "${NEST_CXXFLAGS} -DLTX_MODULE"
      LINK_FLAGS "${NEST_LIBS}"
      PREFIX ""
      OUTPUT_NAME ${MODULE_NAME} )
  install( TARGETS ${MODULE_NAME}_module
      DESTINATION ${CMAKE_INSTALL_LIBDIR}
      )
    # target_link_libraries(${MODULE_NAME}_module PRIVATE spdlog::spdlog_header_only cpprestsdk::cpprest FlatBuffers::FlatBuffers Crow::Crow)
    target_link_libraries(${MODULE_NAME}_module PRIVATE spdlog::spdlog FlatBuffers::FlatBuffers Crow::Crow optional)
    target_include_directories(${MODULE_NAME}_module PRIVATE extern/rapidjson/include)
endif ()

# Build dynamic/static library for standard linking from NEST.
add_library( ${MODULE_NAME}_lib ${MODULE_SOURCES} )
if ( BUILD_SHARED_LIBS )
  # Dynamic libraries are initiated by a `global` variable of the `SLIModule`,
  # which is included, when the flag `LINKED_MODULE` is set.
  target_compile_definitions( ${MODULE_NAME}_lib PRIVATE -DLINKED_MODULE )
endif ()
set_target_properties( ${MODULE_NAME}_lib
    PROPERTIES
    COMPILE_FLAGS "${NEST_CXXFLAGS}"
    LINK_FLAGS "${NEST_LIBS}"
    OUTPUT_NAME ${MODULE_NAME} )
  # target_link_libraries(${MODULE_NAME}_lib PRIVATE spdlog::spdlog_header_only cpprestsdk::cpprest FlatBuffers::FlatBuffers Crow::Crow )
  target_link_libraries(${MODULE_NAME}_lib PRIVATE spdlog::spdlog  FlatBuffers::FlatBuffers Crow::Crow optional )
  target_include_directories(${MODULE_NAME}_lib PRIVATE extern/rapidjson/include)
# Install library, header and sli init files.
install( TARGETS ${MODULE_NAME}_lib DESTINATION ${CMAKE_INSTALL_LIBDIR} )
install( FILES src/${MODULE_HEADER} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${MODULE_NAME} )
install( DIRECTORY src/sli DESTINATION ${CMAKE_INSTALL_DATADIR} )

# Install help --- based on doc/CMakeLists.txt
# Install only if Py >= 2.7.8 and NEST was installed with help
if ( ( NOT CMAKE_CROSSCOMPILING )
     AND PYTHON_EXECUTABLE
     AND ( ${PYTHON_VERSION_STRING} VERSION_GREATER "2.7.7" )
     AND ( IS_DIRECTORY ${NEST_INSTALL_PREFIX}/${NEST_INSTALL_DOCDIR}/help )
   )

    # Extract help from all source files in the source code, put
    # them in doc/help and generate a local help index in the
    # build directory containing links to the help files.
    install( CODE
      "execute_process(
         COMMAND ${PYTHON_EXECUTABLE} -B generate_help.py \"${PROJECT_SOURCE_DIR}\" \"${PROJECT_BINARY_DIR}\"
         WORKING_DIRECTORY \"${NEST_INSTALL_PREFIX}/${NEST_INSTALL_DATADIR}/help_generator\"
         )"
      )

    # Copy the local doc/help directory to the global installation
    # directory for documentation.
    install( DIRECTORY "${PROJECT_BINARY_DIR}/doc/help"
      DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}"
      OPTIONAL
      )

    # Update the global help index to include all help files in
    # the global installation directory for documentation.
    install( CODE
      "execute_process(
         COMMAND ${PYTHON_EXECUTABLE} -B generate_helpindex.py \"${NEST_INSTALL_PREFIX}/${NEST_INSTALL_DOCDIR}\"
         WORKING_DIRECTORY \"${NEST_INSTALL_PREFIX}/${NEST_INSTALL_DATADIR}/help_generator\"
         )"
      )

endif ()

configure_file(
  ${CMAKE_SOURCE_DIR}/src/scripts/start_script.sh.in
  ${CMAKE_BINARY_DIR}/start_script.sh
  @ONLY
)

configure_file(
  ${CMAKE_SOURCE_DIR}/src/scripts/run_simulation.sh.in
  ${CMAKE_BINARY_DIR}/run_simulation.sh
  @ONLY
)

message( "" )
message( "-------------------------------------------------------" )
message( "${MODULE_NAME} Configuration Summary" )
message( "-------------------------------------------------------" )
message( "" )
message( "C++ compiler         : ${CMAKE_CXX_COMPILER}" )
message( "Build static libs    : ${NEST_STATIC_LIB}" )
message( "C++ compiler flags   : ${CMAKE_CXX_FLAGS}" )
message( "NEST compiler flags  : ${NEST_CXXFLAGS}" )
message( "NEST include dirs    : ${NEST_INCLUDES}" )
message( "NEST libraries flags : ${NEST_LIBS}" )
message( "User link libraries  : ${USER_LINK_LIBRARIES}" )
message( "" )
message( "-------------------------------------------------------" )
message( "" )
message( "You can now build and install '${MODULE_NAME}' using" )
message( "  make" )
message( "  make install" )
message( "" )
message( "The library file lib${MODULE_NAME}.so will be installed to" )
message( "  ${CMAKE_INSTALL_FULL_LIBDIR}" )
message( "Help files will be installed to" )
message( "  ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}" )
message( "" )
message( "The module can be loaded into NEST using" )
message( "  (${MODULE_NAME}) Install        (in SLI)" )
message( "  nest.Install('${MODULE_NAME}')  (in PyNEST)" )
message( "" )

if( NOT "${CMAKE_INSTALL_PREFIX}" EQUAL "${NEST_INSTALL_PREFIX}" )
  message( "The module will be installed into a non-default location!" )
  message( "Make sure to set the environment variables:" )
  message( "  export NEST_MODULE_PATH=${CMAKE_INSTALL_FULL_LIBDIR}:$NEST_MODULE_PATH" )
  message( "  export SLI_PATH=${CMAKE_INSTALL_FULL_DATADIR}/sli:$SLI_PATH" )
  message( "" )
endif ()
