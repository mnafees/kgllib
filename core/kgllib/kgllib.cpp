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

#include "kgllib.h"

#include "renderer.h"

#include <QString>
#include <QtDebug>

namespace KGLLib
{

Renderer* renderer = 0;

bool init(Renderer* r)
{
    // Init GLEW
    GLenum ret = glewInit();
    if (ret != GLEW_NO_ERROR) {
        qCritical() << "GLEW init failed with code" << ret;
        return false;
    }

    // Init renderer
    renderer = r ? r : new Renderer();
    if (!renderer->init()) {
        return false;
    }
    // ...
    return true;
}

bool checkGLError(const QString& desc)
{
    GLenum error = glGetError();
    if (error == GL_NO_ERROR) {
        return true;
    } else {
        qCritical() << qPrintable(desc) << ":" << glErrorString(error);
        return false;
    }
}

// This function has been taked (and somewhat adapted) from Mesa's libglu, which is licenced under LGPL2+
const char* glErrorString(GLenum errorCode)
{
    /* GL Errors */
    if (errorCode == GL_NO_ERROR) {
        return "no error";
    } else if (errorCode == GL_INVALID_VALUE) {
        return "invalid value";
    } else if (errorCode == GL_INVALID_ENUM) {
        return "invalid enum";
    } else if (errorCode == GL_INVALID_OPERATION) {
        return "invalid operation";
    } else if (errorCode == GL_STACK_OVERFLOW) {
        return "stack overflow";
    } else if (errorCode == GL_STACK_UNDERFLOW) {
        return "stack underflow";
    } else if (errorCode == GL_OUT_OF_MEMORY) {
        return "out of memory";
    } else {
        return 0;
    }
}

}

