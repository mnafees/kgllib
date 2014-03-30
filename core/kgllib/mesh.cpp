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

#include "mesh.h"

#include "texture.h"
#include "program.h"


namespace KGLLib
{

Mesh::Mesh() : Batch()
{
    mProgram = 0;
}

Mesh::Mesh(GeometryBuffer* buffer, int offset, int indexOffset) :
    Batch(buffer, offset, indexOffset)
{
    mProgram = 0;
}

Mesh::~Mesh()
{
}

void Mesh::setTexture(int index, KGLLib::Texture* tex)
{
    if (index >= mTextures.count()) {
        mTextures.resize(index+1);
    }
    mTextures[index] = tex;
}

KGLLib::Texture* Mesh::texture(int index) const
{
    if (index < 0 || index >= mTextures.count()) {
        return 0;
    } else {
        return mTextures[index];
    }
}

void Mesh::setProgram(KGLLib::Program* program)
{
    mProgram = program;
}

void Mesh::bind()
{
    // Bind texture and program if they're set
    for (int i = mTextures.count()-1; i >= 0; i--) {
        if (mTextures[i]) {
            glActiveTexture(GL_TEXTURE0 + i);
            mTextures[i]->enable();
        }
    }
    if (mProgram) {
        mProgram->bind();
    }

    Batch::bind();
}

void Mesh::unbind()
{
    Batch::unbind();

    for (int i = mTextures.count()-1; i >= 0; i--) {
        if (mTextures[i]) {
            // TODO: maybe just unbind? Maybe do nothing? Maybe have an option?
            glActiveTexture(GL_TEXTURE0 + i);
            mTextures[i]->disable();
        }
    }
    if (mProgram) {
        mProgram->unbind();
    }
}


}

