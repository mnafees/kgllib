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

#include <renderer.h>

#include "texture.h"
#include "program.h"

#include <QtDebug>


namespace KGLLib
{

Renderer::Renderer()
{
    mDefaultTextureFilter = GL_LINEAR_MIPMAP_LINEAR;
    mDefaultTextureWrapMode = GL_CLAMP;
    mAutoDebugOutput = false;
}

Renderer::~Renderer()
{
}

bool Renderer::init()
{
    return true;
}

bool Renderer::bindTexture(const TextureBase* tex)
{
    glBindTexture(tex->glTarget(), tex->glId());
    return checkGLError("Renderer::bindTexture(" + tex->debugString() + ')');
}

bool Renderer::unbindTexture(const TextureBase* tex)
{
    glBindTexture(tex->glTarget(), 0);
    return checkGLError("Renderer::unbindTexture(" + tex->debugString() + ')');
}
bool Renderer::enableTexture(const TextureBase* tex)
{
    glEnable(tex->glTarget());
    return checkGLError("Renderer::enableTexture(" + tex->debugString() + ')');
}

bool Renderer::disableTexture(const TextureBase* tex)
{
    glDisable(tex->glTarget());
    return checkGLError("Renderer::disableTexture(" + tex->debugString() + ')');
}

bool Renderer::bindProgram(const Program* prog)
{
    if (prog) {
        glUseProgram(prog->glId());
        return checkGLError("Renderer::bindProgram()");
    } else {
        glUseProgram(0);
        return checkGLError("Renderer::bindProgram(0)");
    }
}

void Renderer::setDefaultTextureFilter(GLenum filter)
{
    mDefaultTextureFilter = filter;
}

void Renderer::setDefaultTextureWrapMode(GLenum mode)
{
    mDefaultTextureWrapMode = mode;
}

void Renderer::setAutoDebugOutput(bool output)
{
    mAutoDebugOutput = output;
}


}

