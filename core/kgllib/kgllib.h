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

#ifndef KGLLIB_KGLLIB_H
#define KGLLIB_KGLLIB_H

#include "GL/glew.h"

#include "kgllib_export.h"
#include "kgllib_version.h"


class QString;

/**
 * @brief KGLLib namespace
 *
 * Everything within KGLLib is contained in this namespace.
 **/
namespace KGLLib
{

class Renderer;

/**
 * Initializes KGLLib. Note that you must have a current OpenGL context when
 * you call this, otherwise it will fail.
 * @param r if given then @ref renderer pointer is set to this object and it's
 *   init() method is called. If not given then default @ref Renderer object is
 *   created.
 * @return whether initialization succeeded.
 **/
KGLLIB_EXPORT bool init(Renderer* r = 0);
/**
 * Checks if any OpenGL errors have occurred.
 * If an error has occurred, the error is printed out along with the given
 * description.
 * @return whether error has _not_ occurred (i.e. if error has occurred, return false)
 * TODO: maybe change it to return whether errors _has_ occurred?
 **/
KGLLIB_EXPORT bool checkGLError(const QString& desc);

/**
 * @return error string corresponding to the given OpenGL error code.
 *
 * This is similar to the gluErrorString() function, except that it does not handle GLU-specific
 *  error codes. The function is included in KGLLib to make it independant from libglu.
 **/
KGLLIB_EXPORT const char* glErrorString(GLenum errorCode);

/**
 * Pointer to the global renderer object.
 * Renderer object is guaranteed to be available after @ref init() has been
 *  called
 **/
extern KGLLIB_EXPORT Renderer* renderer;

}

#endif
