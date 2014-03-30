project(kgllib)
cmake_minimum_required(VERSION 2.4)

# version info
set(KGLLIB_VERSION_MAJOR 0)
set(KGLLIB_VERSION_MINOR 0)
set(KGLLIB_VERSION_RELEASE 5)
set(KGLLIB_LIB_VERSION "${KGLLIB_VERSION_MAJOR}.${KGLLIB_VERSION_MINOR}.${KGLLIB_VERSION_RELEASE}")
set(KGLLIB_LIB_SOVERSION "${KGLLIB_VERSION_MAJOR}.${KGLLIB_VERSION_MINOR}")

SET(CMAKE_AUTOMOC TRUE)

# Add our own cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

# These are set here so that find_package() calls can make use of them
set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set(EXEC_INSTALL_PREFIX  ${CMAKE_INSTALL_PREFIX}  CACHE PATH  "Base directory for executables and libraries" FORCE)
set(BIN_INSTALL_DIR      "${EXEC_INSTALL_PREFIX}/bin"    CACHE PATH "The subdirectory to the binaries prefix (default prefix/bin)" FORCE)
set(LIB_INSTALL_DIR      "${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX}"  CACHE PATH "The subdirectory relative to the install prefix where libraries will be installed (default is /lib${LIB_SUFFIX})" FORCE)
set(INCLUDE_INSTALL_DIR  "${CMAKE_INSTALL_PREFIX}/include"  CACHE PATH "The subdirectory to the header prefix" FORCE)
set(DATA_INSTALL_DIR  "${CMAKE_INSTALL_PREFIX}/share/apps"  CACHE PATH "The subdirectory where apps can install their data" FORCE)

# Find required packages
set(QT_USE_OPENGL true)
find_package(Qt5 COMPONENTS Core Gui OpenGL)
find_package(OpenGL REQUIRED)
find_package(GLEW)
find_package(Eigen2 REQUIRED)

# Taken from FindKDE4Internal.cmake
set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}" COMPONENT Devel )
# on the Mac support an extra install directory for application bundles starting with cmake 2.6
if(APPLE)
   if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER "2.5")
      set(INSTALL_TARGETS_DEFAULT_ARGS  ${INSTALL_TARGETS_DEFAULT_ARGS}
                                  BUNDLE DESTINATION "${BUNDLE_INSTALL_DIR}" )
   endif("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER "2.5")
endif(APPLE)

if(WIN32)
    set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

# If GLEW wasn't found then use our own internal version
if(NOT GLEW_FOUND)
    message(STATUS "GLEW was not found. Using internal GLEW")

   include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glew)
    add_definitions(-DGLEW_BUILD)
    add_subdirectory(glew)
    set(GLEW_GLEW_LIBRARY glew)
    if(WIN32)
        set(GLEW_GLEW_LIBRARY ${GLEW_GLEW_LIBRARY} -lopengl32)
    endif(WIN32)
endif(NOT GLEW_FOUND)

add_subdirectory(core)
add_subdirectory(extras)
add_subdirectory(examples)
add_subdirectory(cmake)
