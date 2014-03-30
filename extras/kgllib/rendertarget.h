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

#ifndef KGLLIB_RENDERTARGET_H
#define KGLLIB_RENDERTARGET_H

#include "kgllib.h"

#include <QSize>

namespace KGLLib
{

class Texture;

class KGLLIB_EXTRAS_EXPORT RenderTarget
{
public:
    /**
     * Creates RenderTarget object for rendering onto @p color. @p color must
     *  be a valid RGBA texture.
     **/
    explicit RenderTarget(Texture* color);
    /**
     * Creates a render target of size @p width x @p height. Internal color
     *  texture is created and can be accessed using @ref texture(). The
     *  internal format of the color texture can be specified using
     *  @p textureFormat parameter.
     * If @p depth is true then internal depth renderbuffer is created but you
     *  will have no access to it.
     **/
    RenderTarget(int width, int height, bool hasDepth = false, GLint textureFormat = GL_RGBA);
    /**
     * Creates RenderTarget object for rendering onto @p color and @p depth.
     *  If either of them is null then internal renderbuffer is created for
     *  it, but you won't have access to it.
     **/
//     RenderTarget(Texture* color, Texture* depth);

    virtual ~RenderTarget();

    bool enable();
    bool disable();

    Texture* texture() const  { return mColorTexture; }
    int width() const;
    int height() const;
    QSize size() const;

    bool isValid() const  { return mValid; }

    static bool isSupported();

protected:
    Texture* createColorTexture(int w, int h, GLint textureFormat = GL_RGBA);
    Texture* createDepthTexture(int w, int h);
    GLuint createDepthBuffer(int w, int h);

    void attachColorTexture(Texture* tex);
    void attachDepthTexture(Texture* tex);
    void attachDepthBuffer(GLuint buffer);

    bool startInit();
    void endInit();

private:
    bool mValid;
    bool mOwnColorTexture;
    Texture* mColorTexture;
//     Texture* mDepthTexture;

    GLuint mFramebuffer;
    GLuint mDepthBuffer;
};

}

#endif
