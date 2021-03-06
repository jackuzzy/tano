#############################################################################
# Tano - An Open IP TV Player
# Copyright (C) 2013 Tadej Novak <tadej@tano.si>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>./
#############################################################################

IF(NOT DEFINED BRANDING)
    SET(BRANDING "")
ENDIF(NOT DEFINED BRANDING)

IF(NOT BRANDING STREQUAL "")
    PROJECT(${BRANDING})
ELSE(NOT BRANDING STREQUAL "")
    PROJECT(Tano)
ENDIF(NOT BRANDING STREQUAL "")

STRING(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

IF("${CMAKE_VERSION}" VERSION_GREATER 2.8.11.1)
    CMAKE_POLICY(SET CMP0020 NEW)
ENDIF("${CMAKE_VERSION}" VERSION_GREATER 2.8.11.1)

CONFIGURE_FILE(
    "${CMAKE_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")

SET(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
)

INCLUDE(ManageString)

OPTION(BUILD_PLAYER "Build Tano Player" ON)
MESSAGE("Build Tano Player: ${BUILD_PLAYER}")
OPTION(LOGGING "Enable logging" ON)
MESSAGE("Enable logging: ${LOGGING}")
OPTION(STATIC "Build statically" OFF)
MESSAGE("Build statically: ${STATIC}")

IF(LOGGING)
    SET(LOGGING_SET 1)
ELSE(LOGGING)
    SET(LOGGING_SET 0)
ENDIF(LOGGING)


###########
# Version #
###########
# Tano version number.
FILE(READ ${CMAKE_SOURCE_DIR}/VERSION TANO_VERSION)

STRING(REGEX REPLACE "\n" "" TANO_VERSION "${TANO_VERSION}") # get rid of the newline at the end
STRING_SPLIT(TANO_VERSION_LIST "." ${TANO_VERSION})
LIST(LENGTH TANO_VERSION_LIST TANO_VERSION_SIZE)
LIST(GET TANO_VERSION_LIST 0 TANO_VERSION_MAJOR)
LIST(GET TANO_VERSION_LIST 1 TANO_VERSION_MINOR)
IF(TANO_VERSION_SIZE EQUAL 3)
    LIST(GET TANO_VERSION_LIST 2 TANO_VERSION_BUGFIX)
ELSE(TANO_VERSION_SIZE EQUAL 3)
    SET(TANO_VERSION_BUGFIX 0)
ENDIF(TANO_VERSION_SIZE EQUAL 3)
MESSAGE("You are compiling Tano ${TANO_VERSION}")

IF(NOT BRANDING STREQUAL "")
    # Branding version number.
    FILE(READ ${CMAKE_SOURCE_DIR}/src/branding/${BRANDING}/VERSION TANO_BRANDING_VERSION)

    STRING(REGEX REPLACE "\n" "" TANO_BRANDING_VERSION "${TANO_BRANDING_VERSION}") # get rid of the newline at the end
    STRING_SPLIT(TANO_BRANDING_VERSION_LIST "." ${TANO_BRANDING_VERSION})
    LIST(LENGTH TANO_BRANDING_VERSION_LIST TANO_BRANDING_VERSION_SIZE)
    LIST(GET TANO_BRANDING_VERSION_LIST 0 TANO_BRANDING_VERSION_MAJOR)
    LIST(GET TANO_BRANDING_VERSION_LIST 1 TANO_BRANDING_VERSION_MINOR)
    IF(TANO_BRANDING_VERSION_SIZE EQUAL 3)
        LIST(GET TANO_BRANDING_VERSION_LIST 2 TANO_BRANDING_VERSION_BUGFIX)
    ELSE(TANO_BRANDING_VERSION_SIZE EQUAL 3)
        SET(TANO_BRANDING_VERSION_BUGFIX 0)
    ENDIF(TANO_BRANDING_VERSION_SIZE EQUAL 3)
    MESSAGE("You are compiling ${PROJECT_NAME} ${TANO_BRANDING_VERSION}")
ENDIF(NOT BRANDING STREQUAL "")


# Find Git Version Patch
FIND_PROGRAM(GIT git)
IF(GIT)
    EXECUTE_PROCESS(
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ${GIT} rev-parse --short HEAD
        OUTPUT_VARIABLE GIT_OUT OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    SET(PROJECT_VERSION_PATCH "${GIT_OUT}")
    MESSAGE("Git Version Patch: ${GIT_OUT}")
ELSE(GIT)
    IF(GITBUILD)
        SET(PROJECT_VERSION_PATCH "${GITBUILD}")
    ELSE(GITBUILD)
        SET(GIT_OUT 0)
    ENDIF(GITBUILD)
ENDIF(GIT)

MESSAGE("\n")

# Find the architecture
SET(BITS 0)
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(BITS 1)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)


####################
# Compiler options #
####################
# Automatically run moc, if required
SET(CMAKE_AUTOMOC ON)
# Verbose makefile
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Show all warnings
IF(NOT MSVC)
    SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra")
ENDIF(NOT MSVC)

# Make final release build smaller
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(MINGW)
# Windows compatibility
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
# Statically link with libgcc
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
ELSEIF(MSVC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nologo -Zc:wchar_t")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nologo -Zc:wchar_t -w34100 -w34189")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NOLOGO /DYNAMICBASE /NXCOMPAT /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
ENDIF(MINGW)

# Support 64-bit OS X only
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(CMAKE_OSX_ARCHITECTURES x86_64)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Set MingW bin dir, if MingW present
IF(MINGW)
    STRING(REGEX REPLACE "([^ ]+)[/\\].*" "\\1" MINGW_BIN_DIR_TMP "${CMAKE_CXX_COMPILER}")
    STRING(REGEX REPLACE "\\\\" "/" MINGW_BIN_DIR "${MINGW_BIN_DIR_TMP}") # Replace back slashes to slashes
ENDIF(MINGW)

# Search for Qt and other libraries
# Qt
IF(FORCE_QT5)
    SET(FORCE_QT5 ${FORCE_QT5})
    SET(QT_VERSION_MAJOR 5)
ELSE(FORCE_QT5)
    SET(QT_MIN_VERSION "4.8.0")
    FIND_PACKAGE(Qt4)
ENDIF(FORCE_QT5)

IF(NOT QT4_FOUND OR QT_VERSION_MAJOR MATCHES 5)
    FIND_PACKAGE(Qt5Widgets REQUIRED)
    IF(BUILD_PLAYER)
        FIND_PACKAGE(Qt5LinguistTools REQUIRED)
    ENDIF(BUILD_PLAYER)
    SET(QT5 1)
    ADD_DEFINITIONS(-DQt5)
    MESSAGE("Build with Qt5")
ELSE(NOT QT4_FOUND OR QT_VERSION_MAJOR MATCHES 5)
    SET(QT5 0)
    ADD_DEFINITIONS(-DQt4)
    MESSAGE("Build with Qt4")
ENDIF(NOT QT4_FOUND OR QT_VERSION_MAJOR MATCHES 5)

IF(QT5)
    # Add compiler flags for building executables (-fPIE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

    IF(Qt5_POSITION_INDEPENDENT_CODE)
        SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
    ENDIF(Qt5_POSITION_INDEPENDENT_CODE)
ELSE(QT5)
    IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        SET(QT_USE_QTDBUS True)
    ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(QT_USE_QTNETWORK True)
    SET(QT_USE_QTSQL True)
    SET(QT_USE_QTXML True)

    ADD_DEFINITIONS(${QT_DEFINITIONS})
    INCLUDE(${QT_USE_FILE})
ENDIF(QT5)

IF(BUILD_PLAYER)
    FIND_PACKAGE(LIBVLCQT REQUIRED)
    IF(STATIC)
        FIND_PACKAGE(LIBVLC REQUIRED)
    ENDIF(STATIC)
ENDIF(BUILD_PLAYER)

# Set Qt bin dir to find QtCoreX.dll and other libs to install
STRING(REGEX REPLACE "([^ ]+)[/\\].*" "\\1" QT_BIN_DIR_TMP "${QT_MOC_EXECUTABLE}")
STRING(REGEX REPLACE "\\\\" "/" QT_BIN_DIR "${QT_BIN_DIR_TMP}")  # Replace back slashes to slashes

# Set default install path:
# In *nix environments this is /usr/local 
# Under M$ Windows this is C:\Program files\Tano
# WARNING: Leading slash should be added - this is for easier concatenation of the strings due to slash/backslash issues
IF(MINGW OR MSVC)
    IF(NOT CMAKE_INSTALL_PREFIX)
        SET(CMAKE_INSTALL_PREFIX "C:/Program files/${PROJECT_NAME}")
    ENDIF(NOT CMAKE_INSTALL_PREFIX)
    SET(TANO_INSTALL_DATA_DIR "")
    SET(TANO_INSTALL_BIN_DIR "")
    SET(TANO_INSTALL_LIB_DIR "lib")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    IF(NOT CMAKE_INSTALL_PREFIX)
        SET(CMAKE_INSTALL_PREFIX "/Applications")
    ENDIF(NOT CMAKE_INSTALL_PREFIX)
    SET(TANO_INSTALL_DATA_DIR "${PROJECT_NAME}.app/Contents/Resources")
    SET(TANO_INSTALL_DATA_DIR_CORE "Tano.app/Contents/Resources")
    SET(TANO_INSTALL_BIN_DIR "${PROJECT_NAME}.app/Contents/MacOS")
    SET(TANO_INSTALL_LIB_DIR "${PROJECT_NAME}.app/Contents/MacOS/lib")

    SET(DEFAULT_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${TANO_INSTALL_DATA_DIR_CORE}")
    SET(APPLICATION_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${TANO_INSTALL_DATA_DIR}")
ELSE(MINGW OR MSVC)
    IF(NOT CMAKE_INSTALL_PREFIX)
        SET(CMAKE_INSTALL_PREFIX "/usr/local")
    ENDIF(NOT CMAKE_INSTALL_PREFIX)
    SET(TANO_INSTALL_DATA_DIR "share/${PROJECT_NAME_LOWER}")
    SET(TANO_INSTALL_DATA_DIR_CORE "share/tano")
    SET(TANO_INSTALL_BIN_DIR "bin")
    SET(TANO_INSTALL_LIB_DIR "lib")

    SET(DEFAULT_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${TANO_INSTALL_DATA_DIR_CORE}")
    SET(APPLICATION_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${TANO_INSTALL_DATA_DIR}")
ENDIF(MINGW OR MSVC)

# Set the OS X Bundle specific CMake variables which will be used to populate the plist for
# the application bundle
SET(MACOSX_BUNDLE_COPYRIGHT "2013 Tadej Novak")
SET(MACOSX_BUNDLE_ICON_FILE "${PROJECT_NAME}.icns")
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "si.tano.${PROJECT_NAME_LOWER}")


#################
# Configuration #
#################
OPTION(REFRESH_TRANSLATIONS "Refresh translations on compile" OFF)
MESSAGE("Refresh translations on compile: ${REFRESH_TRANSLATIONS}")
MESSAGE("\n")

IF(MSVC)
    SET(LIBSTART "")
ELSE(MSVC)
    SET(LIBSTART lib)
ENDIF(MSVC)

IF(STATIC)
    SET(LIBEND a)
ELSEIF(MINGW)
    SET(LIBEND dll.a)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(LIBEND dylib)
ELSEIF(MSVC)
    SET(LIBEND lib)
ELSE(STATIC)
    SET(LIBEND so)
ENDIF(STATIC)

SET(TANO_CORE tanocore)
SET(TANO_WIDGETS tanowidgets)
SET(TANO_CONFIG tanoconfigplugin)
SET(TANO_EDITOR tano-editor)

SET(TANO_CORE_LIB "${CMAKE_BINARY_DIR}/src/core/${LIBSTART}${TANO_CORE}.${LIBEND}")
SET(TANO_WIDGETS_LIB "${CMAKE_BINARY_DIR}/src/widgets/${LIBSTART}${TANO_WIDGETS}.${LIBEND}")
IF(MSVC)
    SET(TANO_CONFIG_LIB "${CMAKE_BINARY_DIR}/src/config/${LIBSTART}${TANO_CONFIG}.lib")
ELSE(MSVC)
    SET(TANO_CONFIG_LIB "${CMAKE_BINARY_DIR}/src/config/${LIBSTART}${TANO_CONFIG}.a")
ENDIF(MSVC)


###################
# Compile and SET #
###################
IF(BUILD_PLAYER)
    ADD_SUBDIRECTORY(lang)
ENDIF(BUILD_PLAYER)

IF(MINGW OR MSVC)
    ADD_SUBDIRECTORY(windows)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    ADD_SUBDIRECTORY(osx)
ENDIF(MINGW OR MSVC)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(doc)

SET(Tano_Icons
    data/icons/tano-default
)


################
# Installation #
################
# Install data
IF(BUILD_PLAYER)
    INSTALL(DIRECTORY ${Tano_Icons} DESTINATION "${CMAKE_INSTALL_PREFIX}/${TANO_INSTALL_DATA_DIR}/icons")
ENDIF(BUILD_PLAYER)
