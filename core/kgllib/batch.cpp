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

#include "batch.h"

#include "geometrybuffer.h"

#include <QtDebug>

using namespace Eigen;


namespace KGLLib
{

Batch::Batch()
{
    init();
}

Batch::Batch(GeometryBuffer* buffer, int offset, int indexoffset)
{
    init();
    setBuffer(buffer, offset, indexoffset);
}

void Batch::init()
{
    mDirty = true;
    mVertices = mColors = mNormals = mTexcoords = 0;
    mVertexSize = mColorSize = mNormalSize = mTexcoordSize = 0;

    mIndices = 0;
    mIndexCount = 0;
    mPrimitiveType = GL_TRIANGLES;
    mBuffer = 0;
    mBufferOffset = 0;
    mBufferIndexOffset = 0;
    mOwnBuffer = true;
}

Batch::~Batch()
{
    if (mOwnBuffer) {
        delete mBuffer;
    }
}

void Batch::setVertexCount(int count)
{
    mVertexCount = count;
    mDirty = true;
}

void Batch::setVertices(void* vertices, int size)
{
    mVertices = vertices;
    mVertexSize = vertices ? size : 0;
    mDirty = true;
}

void Batch::setColors(void* colors, int size)
{
    mColors = colors;
    mColorSize = colors ? size : 0;
    mDirty = true;
}

void Batch::setNormals(Eigen::Vector3f* normals)
{
    mNormals = normals;
    mNormalSize = normals ? 3 : 0;
    mDirty = true;
}

void Batch::setTexcoords(void* texcoords, int size)
{
    mTexcoords = texcoords;
    mTexcoordSize = texcoords ? size : 0;
    mDirty = true;
}

void Batch::setIndices(unsigned int* indices, int indexCount)
{
    mIndices = indices;
    mIndexCount = indexCount;
    mDirty = true;
}

GLenum Batch::primitiveType() const
{
    return mPrimitiveType;
}

void Batch::setPrimitiveType(GLenum type)
{
    mPrimitiveType = type;
    if (mBuffer) {
        mBuffer->setPrimitiveType(type);
    }
}

void Batch::render()
{
    bind();
    renderOnce();
    unbind();
}

void Batch::bind()
{
    if (mDirty) {
        update();
    }

    mBuffer->bind();
}

void Batch::renderOnce()
{
    if (mBuffer->format().isIndexed()) {
        mBuffer->renderIndexedSubset(mIndexCount, mBufferIndexOffset);
    } else {
        mBuffer->renderSubset(mVertexCount, mBufferOffset);
    }
}

void Batch::unbind()
{
    mBuffer->unbind();
}

void Batch::setBuffer(GeometryBuffer* buffer, int offset, int indexOffset)
{
    if (mOwnBuffer) {
        delete mBuffer;
    }

    mBuffer = buffer;
    mOwnBuffer = (!buffer) ? true : false;
    mBufferOffset = offset;
    mBufferIndexOffset = indexOffset;

    mDirty = true;
}

GeometryBufferFormat Batch::bestBufferFormat() const
{
    GeometryBufferFormat bufferformat(mVertexCount, mIndices ? mIndexCount : 0);
    bufferformat.addVertices(mVertexSize);
    bufferformat.addColors(mColorSize);
    if (mNormals) {
        bufferformat.addNormals();
    }
    bufferformat.addTexCoords(mTexcoordSize);

    return bufferformat;
}

GeometryBuffer* Batch::createSharedBuffer(const QList<Batch*>& batches)
{
    if (batches.isEmpty()) {
        return 0;
    }

    int vertexcount = 0;
    int indexcount = 0;
    foreach (Batch* b, batches) {
        vertexcount += b->vertexCount();
        indexcount += b->indicesCount();
    }

    GeometryBufferFormat format = batches.first()->bestBufferFormat();
    format.setVertexCount(vertexcount);
    format.setIndexCount(indexcount);

    GeometryBuffer* buffer = GeometryBuffer::createBuffer(format);

    int vertexoffset = 0;
    int indexoffset = 0;
    foreach (Batch* b, batches) {
        b->setBuffer(buffer, vertexoffset, indexoffset),
        vertexoffset += b->vertexCount();
        indexoffset += b->indicesCount();
    }

    return buffer;
}

void Batch::update()
{
    if (!mDirty) {
        return;
    }

    mDirty = false;

    if (mOwnBuffer) {
        //qDebug() << "Batch::update(): deleting old buffer";
        delete mBuffer;

        //qDebug() << "Batch::update(): create GeometryBufferFormat";

        //qDebug() << "Batch::update(): create GeometryBuffer";
        mBuffer = GeometryBuffer::createBuffer(bestBufferFormat());
        mBuffer->setPrimitiveType(mPrimitiveType);
    }
    //qDebug() << "Batch::update(): add data";
    mBuffer->bind();
    mBuffer->addVertices(mVertices, mVertexCount, mBufferOffset);
    if (mColors) {
        mBuffer->addColors(mColors, mVertexCount, mBufferOffset);
    }
    if (mNormals) {
        mBuffer->addNormals(mNormals, mVertexCount, mBufferOffset);
    }
    if (mTexcoords) {
        mBuffer->addTexCoords(mTexcoords, mVertexCount, mBufferOffset);
    }
    if (mIndices) {
        if (mBufferOffset) {
            // If our vertices have offset, then we must add this offset to every index.
            // Create temporary index array
            unsigned int* offsetIndices = new unsigned int[mIndexCount];
            for (int i = 0; i < mIndexCount; i++) {
                offsetIndices[i] = reinterpret_cast<unsigned int*>(mIndices)[i] + mBufferOffset;
            }
            mBuffer->addIndices(offsetIndices, mIndexCount, mBufferIndexOffset);
            delete[] offsetIndices;
        } else {
            mBuffer->addIndices(reinterpret_cast<unsigned int*>(mIndices), mIndexCount, mBufferIndexOffset);
        }
    }
    mBuffer->unbind();
    //qDebug() << "Batch::update(): all done";
}

}

