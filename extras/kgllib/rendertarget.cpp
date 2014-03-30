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

#include "rendertarget.h"

#include "texture.h"

#include <QtDebug>


namespace KGLLib
{

bool RenderTarget::isSupported()
{
    return GLEW_EXT_framebuffer_object;
}

RenderTarget::RenderTarget(Texture* color)
{
    if (!startInit()) {
        return;
    }

    // Attach given color texture
    mColorTexture = color;
    attachColorTexture(mColorTexture);

    endInit();
}

RenderTarget::RenderTarget(int width, int height, bool hasDepth, GLint textureFormat)
{
    // Create the color texture. Apparently this has to be done before we bind
    //  the FBO
    Texture* color = createColorTexture(width, height, textureFormat);

    if (!startInit()) {
        return;
    }

    // Attach color texture
    mColorTexture = color;
    attachColorTexture(mColorTexture);

    // Create and attach depth buffer if necessary
    if (hasDepth) {
        mDepthBuffer = createDepthBuffer(width, height);
        attachDepthBuffer(mDepthBuffer);
    }

    endInit();
}

RenderTarget::~RenderTarget()
{
    if (mValid) {
        if (mDepthBuffer) {
            glDeleteRenderbuffersEXT(1, &mDepthBuffer);
        }

        glDeleteFramebuffersEXT(1, &mFramebuffer);

        if (mOwnColorTexture) {
            delete mColorTexture;
        }
    }
}

bool RenderTarget::startInit()
{
    mValid = false;
    mColorTexture = 0;
    mOwnColorTexture = false;
//     mDepthTexture = 0;
    mDepthBuffer = 0;

    // Make sure FBOs are supported
    if (!isSupported()) {
        qDebug() << "RenderTarget is not supported";
        return false;
    }

    // Create the FBO
    glGenFramebuffersEXT(1, &mFramebuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFramebuffer);
    return true;
}

void RenderTarget::endInit()
{
    // Make sure the FBO object is complete
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        if (status == GL_FRAMEBUFFER_UNSUPPORTED_EXT) {
            qCritical() << "RenderTarget::RenderTarget(): unsupported framebuffer";
        } else {
            qCritical() << "RenderTarget::RenderTarget(): Invalid fb status:" << status;
        }
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return;
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    mValid = true;
}

Texture* RenderTarget::createColorTexture(int w, int h, GLint textureFormat)
{
    mOwnColorTexture = true;
    // TODO: what if NPOT textures aren't supported?
    Texture* tex = new Texture(w, h, textureFormat);
    tex->bind();
    tex->setWrapMode(GL_CLAMP);
    tex->unbind();
    return tex;
}

Texture* RenderTarget::createDepthTexture(int w, int h)
{
    qCritical() << "RenderTarget::createDepthTexture(): not supported yet";
    return 0;
}

GLuint RenderTarget::createDepthBuffer(int w, int h)
{
    GLuint buffer;
    glGenRenderbuffersEXT(1, &buffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, buffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, w, h);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
    return buffer;
}

void RenderTarget::attachColorTexture(Texture* tex)
{
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
            tex->glTarget(), tex->glId(), 0);
}

void RenderTarget::attachDepthTexture(Texture* tex)
{
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
            tex->glTarget(), tex->glId(), 0);
}

void RenderTarget::attachDepthBuffer(GLuint buffer)
{
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
            GL_RENDERBUFFER_EXT, buffer);
}

bool RenderTarget::enable()
{
    if (!isValid()) {
        qCritical() << "RenderTarget::enable(): Can't enable invalid render target!";
        return false;
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFramebuffer);

    return true;
}

bool RenderTarget::disable()
{
    if (!isValid()) {
        qCritical() << "RenderTarget::disable(): Can't disable invalid render target!";
        return false;
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    return true;
}

int RenderTarget::width() const
{
    // TODO: in the future, texture() can be null when we're using rendertarget
    return texture()->width();
}

int RenderTarget::height() const
{
    // TODO: in the future, texture() can be null when we're using rendertarget
    return texture()->height();
}

QSize RenderTarget::size() const
{
    // TODO: in the future, texture() can be null when we're using rendertarget
    return texture()->size();
}

}

