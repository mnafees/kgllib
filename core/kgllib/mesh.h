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

#ifndef KGLLIB_MESH_H
#define KGLLIB_MESH_H

#include "batch.h"

#include <QVector>


namespace KGLLib
{
class Texture;
class Program;

/**
 * @brief Set of geometry with program and texture(s)
 *
 * Mesh expands the functionality of Batch by adding a program and textures.
 *
 * You can specify a Program object and one or multiple Texture objects that
 *  are used when rendering the Mesh. Both of those features are optional.
 **/
class KGLLIB_EXPORT Mesh : public Batch
{
public:
    /**
     * Creates new Mesh object.
     **/
    Mesh();
    /**
     * Creates new Mesh object, using a shared GeometryBuffer
     **/
    Mesh(GeometryBuffer* buffer, int offset, int indexOffset);
    virtual ~Mesh();

    virtual void bind();
    virtual void unbind();

    /**
     * Uses given texture in texture unit 0 for rendering.
     **/
    void setTexture(KGLLib::Texture* tex)  { setTexture(0, tex); }
    /**
     * @return Texture used for texture unit 0
     **/
    KGLLib::Texture* texture() const  { return texture(0); }
    /**
    * Uses given texture in given texture unit for rendering.
    *
    * Maximum allowed texture unit depends on hardware.
    **/
    virtual void setTexture(int index, KGLLib::Texture* tex);
    /**
    * @return Texture used for the given texture unit
    **/
    KGLLib::Texture* texture(int index) const;

    /**
     * Sets the Program used for rendering.
     **/
    virtual void setProgram(KGLLib::Program* program);
    /**
     * @return Program used for rendering
     **/
    KGLLib::Program* program() const  { return mProgram; }

protected:

private:
    QVector<KGLLib::Texture*> mTextures;
    KGLLib::Program* mProgram;
};

}

#endif
