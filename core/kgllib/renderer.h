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

#ifndef KGLLIB_RENDERER_H
#define KGLLIB_RENDERER_H

#include "kgllib.h"

namespace KGLLib
{
class TextureBase;
class Program;

class KGLLIB_EXPORT Renderer
{
public:
    Renderer();
    virtual ~Renderer();

    virtual bool init();

    virtual bool bindTexture(const TextureBase* tex);
    virtual bool unbindTexture(const TextureBase* tex);
    virtual bool enableTexture(const TextureBase* tex);
    virtual bool disableTexture(const TextureBase* tex);

    virtual bool bindProgram(const Program* prog);

    // The highly experimental API follows:
    void setDefaultTextureFilter(GLenum filter);
    void setDefaultTextureWrapMode(GLenum mode);
    void setAutoDebugOutput(bool output);

    GLenum defaultTextureFilter() const  { return mDefaultTextureFilter; }
    GLenum defaultTextureWrapMode() const  { return mDefaultTextureWrapMode; }
    bool autoDebugOutput() const  { return mAutoDebugOutput; }

private:
    GLenum mDefaultTextureFilter;
    GLenum mDefaultTextureWrapMode;
    bool mAutoDebugOutput;
};

}

#endif
