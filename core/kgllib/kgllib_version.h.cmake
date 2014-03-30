/*
 * Copyright (C) 2008 Rivo Laks <rivolaks@hot.ee>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KGLLIB_VERSION_H
#define KGLLIB_VERSION_H

#include "kgllib_export.h"

/**
 * @file kgllib_version.h
 **/

/**
 * Major number of KGLLib's compile-time version
 **/
#define KGLLIB_VERSION_MAJOR @KGLLIB_VERSION_MAJOR@
/**
 * Minor number of KGLLib's compile-time version
 **/
#define KGLLIB_VERSION_MINOR @KGLLIB_VERSION_MINOR@
/**
 * Release number of KGLLib's compile-time version
 **/
#define KGLLIB_VERSION_RELEASE @KGLLIB_VERSION_RELEASE@

/**
 * Compile-time version of KGLLib, as a string
 **/
#define KGLLIB_VERSION_STRING \
        "@KGLLIB_VERSION_MAJOR@.@KGLLIB_VERSION_MINOR@.@KGLLIB_VERSION_RELEASE@"

/**
 * Macro to encode major, minor and release number into a version number.
 **/
#define KGLLIB_MAKE_VERSION(a, b, c) (((a) << 16) | ((b) << 8) | (c))

/**
 * Compile-time version of KGLLib, encoded with KGLLIB_MAKE_VERSION macro
 **/
#define KGLLIB_VERSION \
        KGLLIB_MAKE_VERSION(KGLLIB_VERSION_MAJOR, KGLLIB_VERSION_MINOR, KGLLIB_VERSION_RELEASE)

/**
 * Checks whether KGLLib's compile-time version is equal to or higher than the
 *  given version.
 **/
#define KGLLIB_IS_VERSION(a, b, c) (KGLLIB_VERSION >= KGLLIB_MAKE_VERSION(a, b, c))

namespace KGLLib
{
/**
 * Returns the encoded runtime version of KGLLib.
 *
 * The returned value is encoded according to the KGLLIB_MAKE_VERSION macro.
 **/
KGLLIB_EXPORT unsigned int version();

/**
 * Returns the major number of KGLLib's version, e.g. 0 for 0.1.2
 **/
KGLLIB_EXPORT unsigned int versionMajor();
/**
 * Returns the minor number of KGLLib's version, e.g. 1 for 0.1.2
 **/
KGLLIB_EXPORT unsigned int versionMinor();
/**
 * Returns the release number of KGLLib's version, e.g. 2 for 0.1.2
 **/
KGLLIB_EXPORT unsigned int versionRelease();

/**
 * Checks if KGLLib's version is at least the given version.
 **/
KGLLIB_EXPORT bool isVersion(unsigned int a, unsigned int b, unsigned int c);

/**
 * Returns KGLLib's version as a string, e.g. "0.1.2"
 **/
KGLLIB_EXPORT const char* versionString();
}

#endif
