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

#ifndef KGLLIB_TEXTURE_H
#define KGLLIB_TEXTURE_H

#include "kgllib.h"

#include <QString>
#include <QSize>


class QPixmap;
class QImage;
class QRectF;

namespace KGLLib
{

/**
 * @brief Abstract base class for all textures.
 *
 * This class includes basic functionality such as bind/unbind and
 *  enable/disable methods which all textures have.
 *
 * See Texture class documentation for more detailed introduction and examples.
 *
 * @see Texture
 **/
class KGLLIB_EXPORT TextureBase
{
public:
    /**
     * Creates new invalid texture.
     * It is up to subclasses to actually initialize the texture.
     * TODO: should we do glGenTextures() here or in subclasses? Probably latter.
     **/
    TextureBase();
    /**
     * Same as above but also sets texture name to @p name.
     **/
    TextureBase(const QString& name);
    /**
     * Deletes the OpenGL texture associated with this object.
     **/
    virtual ~TextureBase();

    /**
     * @return whether the texture is valid
     **/
    bool isValid() const  { return mValid; }

    /**
     * Binds the texture onto currently active texture unit.
     **/
    virtual void bind() const;
    /**
     * Unbinds the texture.
     * After calling this, no texture is bound but texturing may still be
     *  enabled (in which case texturing results are undefined).
     * TODO: is this meanful or should it be removed (as users usually want to
     *  either disable() texturing or just bind() another texture)?
     **/
    virtual void unbind() const;
    /**
     * Enables texturing with this texture type for currently active texture
     *  unit.
     * @param bind if true then bind() is called after enabling texturing
     **/
    virtual void enable(bool bind = true) const;
    /**
     * Disables texturing with this texture type for currently active texture
     *  unit.
     * @param unbind if true then unbind() is called before disabling texturing
     **/
    virtual void disable(bool unbind = true) const;

    /**
     * Sets texture's filter to @p filter.
     * @p filter is minification filter, magnification filter is set to
     *  GL_NEAREST if @p filter is GL_NEAREST and to GL_LINEAR otherwise.
     *
     * Common values are:
     * @li GL_NEAREST - return nearest pixel. Usually you don't want this.
     * @li GL_LINEAR - return weighed average of neighboring pixels.
     * @li GL_NEAREST_MIPMAP_LINEAR - same as GL_LINEAR but uses nearest MIPMAP (aka bilinear).
     * @li GL_LINEAR_MIPMAP_LINEAR - uses GL_LINEAR on two neighboring mipmaps and then takes
     *     weighed average of them (aka trilinear)
     * TODO: some better wording here
     * GL_LINEAR_MIPMAP_LINEAR usually gives best quality but uses a third more
     *  memory and has a bit lower performance.
     *
     * TODO: maybe set filter to GL_CLAMP by default because it seems to suit
     *  smaller apps better. OTOH I really don't like having defaults differ
     *  from OpenGL ones. Maybe add Renderer::setDefaultTextureFilter() method???
     **/
    virtual void setFilter(GLenum filter);

    /**
     * Sets texture wrap mode for given coordinate.
     * Wrap mode controls how exactly the texture coordinates are interpreted:
     * @li GL_REPEAT is default and ignores integer part of the texture
     *     coordinate, causing the texture to be repeated.
     * @li GL_CLAMP clamps coordinates to range [0; 1].
     * TODO: explain other modes as well
     *
     * Usually you should use GL_REPEAT when you want texture to repeat across
     *  surfaces (e.g. for terrain with repeating texture) and GL_CLAMP when
     *  you want just one instance of the texture (e.g. for a logo).
     **/
    virtual void setCoordinateWrapMode(GLenum coordinate, GLenum mode);
    /**
     * Sets wrap mode for all coordinates of this texture.
     * Subclasses need to reimplement this, calling setCoordinateWrapMode() for every
     *  possible coordinate.
     **/
    virtual void setWrapMode(GLenum mode) = 0;

    /**
     * @return name of this texture.
     * It can be used to identify textures e.g. to ease debugging.
     **/
    QString name() const  { return mName; }
    /**
     * Sets texture's name to @p name.
     * The name doesn't have any connection with OpenGL but it can be used
     *  to identify textures when debugging.
     **/
    void setName(const QString& name);

    // TODO: rename?
    virtual QString debugString() const;

    /**
     * TODO: maybe rename to glType()
     * @return type of this texture (e.g. GL_TEXTURE_2D)
     **/
    virtual GLenum glTarget() const = 0;
    /**
     * @return internal OpenGL texture id of this texture.
     **/
    virtual GLuint glId() const  { return mGLId; }

protected:
    void setValid(bool valid)  { mValid = valid; }

protected:
    bool mValid;
    GLuint mGLId;
    QString mName;
    GLenum mFormat;
    GLenum mInternalFormat;
};


/**
 * @short 2D texture class
 *
 * @section creation Creation
 * There are several ways how a Texture can be created.
 *
 * You can convert QImage or QPixmap into a texture. QImage is more efficient
 *  here because QPixmap will be first converted into QImage and then into
 *  texture.
 *
 * You can also specify a filename where the texture image should be loaded
 *  from. The image file has to be supported by Qt.
 *
 * Finally, you can create an empty texture, specifying only its size (width
 *  and height). In that case you should usually upload the texture contents
 *  yourself.
 *
 * If the creation fails for some reason, the resulting texture cannot be used
 *  and isValid() will return false.
 *
 * @section usage Basic usage
 * To use a texture, first you need to enable texturing and bind the texture.
 *  Both of those operations can be achieved by calling enable(). This enables
 *  texturing with specific texture type (in this case, 2d textures) and also
 *  binds this texture, making it ready to use. Once you are done using the
 *  texture, you can call disable() to unbind it and disable texturing.
 * @code
 * // Load texture from image file
 * Texture* tex = new Texture("logo.jpg");
 *
 * // Enable texturing with this texture
 * tex->enable();
 * // Render textured geometry
 * glBegin(GL_TRIANGLES);
 * ...
 * glEnd();
 * // Disable texturing
 * tex->disable();
 * @endcode
 *
 * @see TextureBase, Mesh
 **/
class KGLLIB_EXPORT Texture : public TextureBase
{
public:
    /**
     * Creates texture from given QImage.
     * If the image is null image, then the resulting texture will be invalid.
     *
     * Mipmaps are created automatically unless filter is GL_NEAREST or GL_LINEAR.
     **/
    explicit Texture(const QImage& img, GLenum filter = GL_LINEAR_MIPMAP_LINEAR);
    /**
     * Creates texture from QPixmap.
     * If the pixmap is null pixmap, then the resulting texture will be invalid.
     *
     * Mipmaps are created automatically unless filter is GL_NEAREST or GL_LINEAR.
     **/
    explicit Texture(const QPixmap& pix, GLenum filter = GL_LINEAR_MIPMAP_LINEAR);
    /**
     * Creates texture from image loaded from the given file.
     * If the image cannot be loaded, then the resulting texture will be invalid.
     *
     * Mipmaps are created automatically unless filter is GL_NEAREST or GL_LINEAR.
     **/
    explicit Texture(const QString& filename, GLenum filter = GL_LINEAR_MIPMAP_LINEAR);
    /**
     * Creates texture with the given size.
     * The texture contents will be undefined until they're specified by user.
     *
     * @p internalformat and @p format parameters describe format of the
     *  texture. If @p format is 0 (default value), then it will be computed
     *  automatically based on the value of @p internalformat.
     **/
    Texture(int width, int height, GLint internalformat = GL_RGBA, GLint format = 0);
    virtual ~Texture();

    /**
     * Return width of this texture.
     *
     * @see height(), size()
     **/
    int width() const  { return mWidth; }
    /**
     * Return height of this texture.
     *
     * @see width(), size()
     **/
    int height() const  { return mHeight; }
    /**
     * Return size (width and height) of this texture.
     *
     * @see width(), height()
     **/
    QSize size() const  { return QSize(mWidth, mHeight); }

    /**
     * Sets wrap mode for both horizontal as well as vertical coordinates of
     *  this texture.
     *
     * @see Texture::setCoordinateWrapMode(GLenum coordinate, GLenum mode)
     **/
    virtual void setWrapMode(GLenum mode);

    virtual GLenum glTarget() const { return GL_TEXTURE_2D; }

    void render(const QRectF& rect) const;
    QImage convertToGLFormat(const QImage& img) const;

protected:
    bool init(int width, int height, GLint internalformat = GL_RGBA, GLint format = 0);
    bool init(const QImage& img, GLenum filter);
    bool init(const QString& filename, GLenum filter);

protected:
    int mWidth;
    int mHeight;
};

/**
 * @short 3D texture class
 **/
class KGLLIB_EXPORT Texture3D : public TextureBase
{
public:
    Texture3D(int width, int height, int depth);
    virtual ~Texture3D();

    virtual void setWrapMode(GLenum mode);

    virtual GLenum glTarget() const { return GL_TEXTURE_3D; }
};

}

#endif
