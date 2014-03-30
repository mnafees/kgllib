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

#include <texture.h>

#include <renderer.h>

#include <qpixmap.h>
#include <qimage.h>
#include <qrect.h>
#include <qstring.h>
#include <QtDebug>

namespace
{

GLint determineFormat(GLint internalformat)
{
    switch (internalformat) {
        // From OpenGL 2.1 spec
        case GL_ALPHA:
        case GL_ALPHA4:
        case GL_ALPHA8:
        case GL_ALPHA12:
        case GL_ALPHA16:
            return GL_ALPHA;
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32:
            return GL_DEPTH_COMPONENT;
        case GL_LUMINANCE:
        case GL_LUMINANCE4:
        case GL_LUMINANCE8:
        case GL_LUMINANCE12:
        case GL_LUMINANCE16:
            return GL_LUMINANCE;
        case GL_LUMINANCE_ALPHA:
        case GL_LUMINANCE4_ALPHA4:
        case GL_LUMINANCE6_ALPHA2:
        case GL_LUMINANCE8_ALPHA8:
        case GL_LUMINANCE12_ALPHA4:
        case GL_LUMINANCE12_ALPHA12:
        case GL_LUMINANCE16_ALPHA16:
            return GL_LUMINANCE_ALPHA;
        case GL_INTENSITY:
        case GL_INTENSITY4:
        case GL_INTENSITY8:
        case GL_INTENSITY12:
        case GL_INTENSITY16:
            return GL_INTENSITY;
        case GL_R3_G3_B2:
        case GL_RGB:
        case GL_RGB4:
        case GL_RGB5:
        case GL_RGB8:
        case GL_RGB10:
        case GL_RGB12:
        case GL_RGB16:
            return GL_RGB;
        case GL_RGBA:
        case GL_RGBA2:
        case GL_RGBA4:
        case GL_RGB5_A1:
        case GL_RGBA8:
        case GL_RGB10_A2:
        case GL_RGBA12:
        case GL_RGBA16:
            return GL_RGBA;
        // sRGB
#ifdef GL_VERSION_2_1
        case GL_SRGB8:              return GL_RGB;
        case GL_SRGB8_ALPHA8:       return GL_RGBA;
        case GL_SLUMINANCE:         return GL_LUMINANCE;
        case GL_SLUMINANCE8_ALPHA8: return GL_LUMINANCE_ALPHA;
#else
        case GL_SRGB8_EXT:              return GL_RGB;
        case GL_SRGB8_ALPHA8_EXT:       return GL_RGBA;
        case GL_SLUMINANCE_EXT:         return GL_LUMINANCE;
        case GL_SLUMINANCE8_ALPHA8_EXT: return GL_LUMINANCE_ALPHA;
#endif
        // Generic compressed formats
        case GL_COMPRESSED_ALPHA:           return GL_ALPHA;
        case GL_COMPRESSED_LUMINANCE:       return GL_LUMINANCE;
        case GL_COMPRESSED_LUMINANCE_ALPHA: return GL_LUMINANCE_ALPHA;
        case GL_COMPRESSED_INTENSITY:       return GL_INTENSITY;
        case GL_COMPRESSED_RGB:             return GL_RGB;
        case GL_COMPRESSED_RGBA:            return GL_RGBA;
#ifdef GL_VERSION_2_1
        case GL_COMPRESSED_SRGB:            return GL_RGB;
        case GL_COMPRESSED_SRGB_ALPHA:      return GL_RGBA;
        case GL_COMPRESSED_SLUMINANCE:      return GL_LUMINANCE;
        case GL_COMPRESSED_SLUMINANCE_ALPHA: return GL_LUMINANCE_ALPHA;
#else
        case GL_COMPRESSED_SRGB_EXT:            return GL_RGB;
        case GL_COMPRESSED_SRGB_ALPHA_EXT:      return GL_RGBA;
        case GL_COMPRESSED_SLUMINANCE_EXT:      return GL_LUMINANCE;
        case GL_COMPRESSED_SLUMINANCE_ALPHA_EXT: return GL_LUMINANCE_ALPHA;
#endif
        // From EXT_texture_compression_s3tc extension
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            return GL_RGB;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return GL_RGBA;
        // From ARB_texture_float extension
        case GL_RGBA32F_ARB:                return GL_RGBA;
        case GL_RGB32F_ARB:                 return GL_RGB;
        case GL_ALPHA32F_ARB:               return GL_ALPHA;
        case GL_INTENSITY32F_ARB:           return GL_INTENSITY;
        case GL_LUMINANCE32F_ARB:           return GL_LUMINANCE;
        case GL_LUMINANCE_ALPHA32F_ARB:     return GL_LUMINANCE_ALPHA;
        case GL_RGBA16F_ARB:                return GL_RGBA;
        case GL_RGB16F_ARB:                 return GL_RGB;
        case GL_ALPHA16F_ARB:               return GL_ALPHA;
        case GL_INTENSITY16F_ARB:           return GL_INTENSITY;
        case GL_LUMINANCE16F_ARB:           return GL_LUMINANCE;
        case GL_LUMINANCE_ALPHA16F_ARB:     return GL_LUMINANCE_ALPHA;

        default:
            return 0;
    }
}

GLint determineDataType(GLint internalformat)
{
    switch (internalformat) {
        case GL_RGBA32F_ARB:
        case GL_RGB32F_ARB:
        case GL_ALPHA32F_ARB:
        case GL_INTENSITY32F_ARB:
        case GL_LUMINANCE32F_ARB:
        case GL_LUMINANCE_ALPHA32F_ARB:
            return GL_FLOAT;
        case GL_RGBA16F_ARB:
        case GL_RGB16F_ARB:
        case GL_ALPHA16F_ARB:
        case GL_INTENSITY16F_ARB:
        case GL_LUMINANCE16F_ARB:
        case GL_LUMINANCE_ALPHA16F_ARB:
            return GL_HALF_FLOAT_ARB;

        default:
            return GL_UNSIGNED_BYTE;
    }
}

}

namespace KGLLib
{

/*** TextureBase ***/
TextureBase::TextureBase()
{
    mValid = false;
    mGLId = 0;
}

TextureBase::TextureBase(const QString& name)
{
    mValid = false;
    mGLId = 0;
    setName(name);
}

TextureBase::~TextureBase()
{
    if (mGLId) {
        glDeleteTextures(1, &mGLId);
    }
}

void TextureBase::bind() const
{
    renderer->bindTexture(this);
}

void TextureBase::unbind() const
{
    renderer->unbindTexture(this);
}

void TextureBase::enable(bool _bind) const
{
    renderer->enableTexture(this);
    if (_bind) {
        bind();
    }
}

void TextureBase::disable(bool _unbind) const
{
    renderer->disableTexture(this);
    if (_unbind) {
        unbind();
    }
}

void TextureBase::setFilter(GLenum filter)
{
    GLenum magfilter = (filter == GL_NEAREST) ? GL_NEAREST : GL_LINEAR;
    glTexParameteri(glTarget(), GL_TEXTURE_MAG_FILTER, magfilter);
    glTexParameteri(glTarget(), GL_TEXTURE_MIN_FILTER, filter);
}

void TextureBase::setCoordinateWrapMode(GLenum coordinate, GLenum mode)
{
    glTexParameteri(glTarget(), coordinate, mode);
}

void TextureBase::setName(const QString& name)
{
    mName = name;
}

QString TextureBase::debugString() const
{
    if (name().isEmpty()) {
        return QString("<Texture id %1>").arg(glId());
    } else {
        return QString("<Texture name %1>").arg(name());
    }
}


/*** Texture ***/
Texture::Texture(int width, int height, GLint internalformat, GLint format)
{
    mValid = init(width, height, internalformat, format);
}

Texture::Texture(const QImage& img, GLenum filter)
{
    mValid = init(img, filter);
}

Texture::Texture(const QPixmap& pix, GLenum filter)
{
    mValid = init(pix.toImage(), filter);
}

Texture::Texture(const QString& filename, GLenum filter) : TextureBase(filename)
{
    mValid = init(filename, filter);
}

Texture::~Texture()
{
}

bool Texture::init(int width, int height, GLint internalformat, GLint format)
{
    mWidth = width;
    mHeight = height;

    glGenTextures(1, &mGLId);
    bind();
    setFilter(GL_LINEAR);

    if (!format) {
        format = determineFormat(internalformat);
        if (!format) {
            qCritical() << "Texture::init(): Couldn't determine matching format for internalformat" << internalformat;
            return false;
        }
    }
    mInternalFormat = internalformat;
    mFormat = format;
    glTexImage2D( glTarget(), 0, mInternalFormat, mWidth, mHeight, 0,
        mFormat, determineDataType(mInternalFormat), 0);

    checkGLError(QString("Texture::init(%1, %2)").arg(mWidth).arg(mHeight));

    return true;
}

bool Texture::init(const QImage& img, GLenum filter)
{
    if (img.isNull()) {
        qWarning() << "Texture::init(): NULL QImage!";
        return false;
    }
    if (!init(img.width(), img.height())) {
        return false;
    }

    setFilter(filter);
    QImage glimg = convertToGLFormat( img );
    // TODO: make sure generate_mipmap is supported and use gluBuild2DMipmaps if it isn't
    // TODO: generate mipmaps only if filter requires them
    glTexParameteri(glTarget(), GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D( glTarget(), 0, mInternalFormat, glimg.width(), glimg.height(), 0,
        mFormat, GL_UNSIGNED_BYTE, glimg.bits());
    checkGLError("Texture::init(Qimage)");

    return true;
}

bool Texture::init(const QString& filename, GLenum filter)
{
    QImage img(filename);
    if (img.isNull()) {
        qWarning() << "Texture::init(): failed to load from file" << filename;
        return false;
    }
    return init(img, filter);
}

void Texture::setWrapMode(GLenum mode)
{
    TextureBase::setCoordinateWrapMode(GL_TEXTURE_WRAP_S, mode);
    TextureBase::setCoordinateWrapMode(GL_TEXTURE_WRAP_T, mode);
}

QImage Texture::convertToGLFormat(const QImage& img) const
{
    // This method has been copied from Qt's QGLWidget::convertToGLFormat()
    QImage res = img.convertToFormat(QImage::Format_ARGB32);
    res = res.mirrored();

    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        // Qt has ARGB; OpenGL wants RGBA
        for (int i=0; i < res.height(); i++) {
            uint *p = (uint*)res.scanLine(i);
            uint *end = p + res.width();
            while (p < end) {
                *p = (*p << 8) | ((*p >> 24) & 0xFF);
                p++;
            }
        }
    } else {
        // Qt has ARGB; OpenGL wants ABGR (i.e. RGBA backwards)
        res = res.rgbSwapped();
    }
    return res;
}


/*** Texture ***/

Texture3D::~Texture3D()
{
}

void Texture3D::setWrapMode(GLenum mode)
{
    TextureBase::setCoordinateWrapMode(GL_TEXTURE_WRAP_S, mode);
    TextureBase::setCoordinateWrapMode(GL_TEXTURE_WRAP_T, mode);
    TextureBase::setCoordinateWrapMode(GL_TEXTURE_WRAP_R, mode);
}

}

