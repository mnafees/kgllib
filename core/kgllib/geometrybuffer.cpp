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

#include "geometrybuffer.h"

#include <QDebug>

namespace KGLLib
{

/**  GeometryBufferFormat  **/
GeometryBufferFormat::GeometryBufferFormat()
{
    init(0, 0);
}

GeometryBufferFormat::GeometryBufferFormat(int vertexCount, int indexCount)
{
    init(vertexCount, indexCount);
}

GeometryBufferFormat::GeometryBufferFormat(Format fmt, int vertexCount, int indexCount)
{
    init(vertexCount, indexCount);

    if (fmt & Vertex3) {
        mVertexSize = 3;
    } else if (fmt & Vertex4) {
        mVertexSize = 4;
    }
    if (fmt & Color3) {
        mColorSize = 3;
    } else if (fmt & Color4) {
        mColorSize = 4;
    }
    if (fmt & Normal) {
        mNormalSize = 3;
    }
    if (fmt & TexCoord2) {
        mTexCoordSize = 2;
    }
}

void GeometryBufferFormat::init(int vertexCount, int indexCount)
{
    mVertexCount = vertexCount;
    mIndexCount = indexCount;

    mVertexSize = 0;
    mColorSize = 0;
    mNormalSize = 0;
    mTexCoordSize = 0;
}

void GeometryBufferFormat::setVertexCount(int count)
{
    mVertexCount = count;
}

void GeometryBufferFormat::setIndexCount(int count)
{
    mIndexCount = count;
}


/**  Static buffer creation helper methods  **/
GeometryBuffer* GeometryBuffer::createBuffer(const GeometryBufferFormat& format)
{
    if (GLEW_ARB_vertex_buffer_object) {
        return new GeometryBufferVBO(format);
    } else {
        return new GeometryBufferVertexArray(format);
    }
}

GeometryBuffer* GeometryBuffer::createBuffer(GeometryBufferFormat::Format format, int vertexCount, int indexCount)
{
    GeometryBufferFormat fmt(format, vertexCount, indexCount);
    return createBuffer(fmt);
}

/**  GeometryBuffer  **/
GeometryBuffer::GeometryBuffer(const GeometryBufferFormat& format)
{
    init(format);
}

GeometryBuffer::~GeometryBuffer()
{
}

void GeometryBuffer::init(const GeometryBufferFormat& format)
{
    mFormat = format;
    mPrimitiveType = GL_TRIANGLES;

    int offset = 0;
    if (format.vertexSize()) {
        mVertexData.size = format.vertexSize() * sizeof(float);
        offset += format.vertexSize() * format.vertexCount() * sizeof(float);
    }
    if (format.colorSize()) {
        mColorData.size = format.colorSize() * sizeof(float);
        mColorData.offset = offset;
        offset += format.colorSize() * format.vertexCount() * sizeof(float);
    }
    if (format.normalSize()) {
        mNormalData.size = format.normalSize() * sizeof(float);
        mNormalData.offset = offset;
        offset += format.normalSize() * format.vertexCount() * sizeof(float);
    }
    if (format.texCoordSize()) {
        mTexCoordData.size = format.texCoordSize() * sizeof(float);
        mTexCoordData.offset = offset;
        offset += format.texCoordSize() * format.vertexCount() * sizeof(float);
    }
}

void GeometryBuffer::setPrimitiveType(GLenum type)
{
    mPrimitiveType = type;
}

int GeometryBuffer::bufferSize() const
{
    int elemsize = 0;
    elemsize += format().vertexSize() * sizeof(float);
    elemsize += format().colorSize() * sizeof(float);
    elemsize += format().normalSize() * sizeof(float);
    elemsize += format().texCoordSize() * sizeof(float);

    return elemsize * format().vertexCount();
}

int GeometryBuffer::indexBufferSize() const
{
    if (!format().isIndexed()) {
        return 0;
    }
    // TODO: support other formats
    return sizeof(unsigned int) * format().indexCount();
}

void GeometryBuffer::render()
{
    if (format().isIndexed()) {
        renderIndexedSubset(format().indexCount(), 0);
    } else {
        renderSubset(format().vertexCount(), 0);
    }
}

void GeometryBuffer::addVertices(void* vertices, int count, int offset)
{
    qDebug() << "addVertices(): count=" << count << ", offset=" << offset;
    addData(vertices, count * mVertexData.size, mVertexData.offset + offset * mVertexData.size);
}

void GeometryBuffer::addColors(void* colors, int count, int offset)
{
    qDebug() << "addColors(): count=" << count << ", offset=" << offset;
    addData(colors, count * mColorData.size, mColorData.offset + offset * mColorData.size);
}

void GeometryBuffer::addNormals(void* normals, int count, int offset)
{
    qDebug() << "addNormals(): count=" << count << ", offset=" << offset;
    addData(normals, count * mNormalData.size, mNormalData.offset + offset * mNormalData.size);
}

void GeometryBuffer::addTexCoords(void* texcoords, int count, int offset)
{
    qDebug() << "addTexCoords(): count=" << count << ", offset=" << offset;
    addData(texcoords, count * mTexCoordData.size, mTexCoordData.offset + offset * mTexCoordData.size);
}


/**  GeometryBufferVertexArray  **/
GeometryBufferVertexArray::GeometryBufferVertexArray(const GeometryBufferFormat& format) :
    GeometryBuffer(format)
{
    createArrays();
}

GeometryBufferVertexArray::GeometryBufferVertexArray(const GeometryBufferFormat& format, bool init) :
    GeometryBuffer(format)
{
    if (init) {
        createArrays();
    } else {
        mBuffer = 0;
        mIndexBuffer = 0;
    }
}

GeometryBufferVertexArray::~GeometryBufferVertexArray()
{
    delete mBuffer;
    delete mIndexBuffer;
}

void GeometryBufferVertexArray::createArrays()
{
    qDebug() << "VA::createArrays(): creating buffer of" << bufferSize() << "bytes";
    mBuffer = new char[bufferSize()];

    mIndexBuffer = 0;
    if (format().isIndexed()) {
        qDebug() << "VA::createArrays(): creating index buffer of" << indexBufferSize() << "bytes";
        mIndexBuffer = new char[indexBufferSize()];
    }
}

void GeometryBufferVertexArray::addData(void* data, int size, int offset)
{
    qDebug() << "  VA::addData(): size=" << size << ", offset=" << offset;
    memcpy(mBuffer + offset, data, size);
}

void GeometryBufferVertexArray::addIndices(unsigned int* indices, int count, int offset)
{
    qDebug() << "VA::addIndices(): count=" << count << ", offset=" << offset;
    int byteoffset = offset * sizeof(unsigned int);
    int bytecount = count * sizeof(unsigned int);
    memcpy(mIndexBuffer + byteoffset, indices, bytecount);
}

bool GeometryBufferVertexArray::bind()
{
    // Enable client states
    if (mVertexData.size) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(mVertexData.size/sizeof(float), GL_FLOAT, 0, mBuffer + mVertexData.offset);
    }
    if (mColorData.size) {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(mColorData.size/sizeof(float), GL_FLOAT, 0, mBuffer + mColorData.offset);
    }
    if (mNormalData.size) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, mBuffer + mNormalData.offset);
    }
    if (mTexCoordData.size) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(mTexCoordData.size/sizeof(float), GL_FLOAT, 0, mBuffer + mTexCoordData.offset);
    }

    return true;
}

bool GeometryBufferVertexArray::unbind()
{
    if (mVertexData.size) {
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    if (mColorData.size) {
        glDisableClientState(GL_COLOR_ARRAY);
    }
    if (mNormalData.size) {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    if (mTexCoordData.size) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    return true;
}

void GeometryBufferVertexArray::renderSubset(int vertices, int offset)
{
//     qDebug() << "VA::render(), count=" << vertices << ", offset=" << offset;
    glDrawArrays(mPrimitiveType, offset, vertices);
}

void GeometryBufferVertexArray::renderIndexedSubset(int indices, int offset)
{
    glDrawElements(mPrimitiveType, indices, GL_UNSIGNED_INT, mIndexBuffer + offset * sizeof(unsigned int));
}


/**  GeometryBufferVBO  **/
GeometryBufferVBO::GeometryBufferVBO(const GeometryBufferFormat& format) :
    GeometryBufferVertexArray(format, false)
{
    createArrays();
}

GeometryBufferVBO::~GeometryBufferVBO()
{
}

void GeometryBufferVBO::createArrays()
{
    qDebug() << "VBO: creating buffer of" << bufferSize() << "bytes";
    glGenBuffers(1, &mVBOId);
    glBindBuffer(GL_ARRAY_BUFFER, mVBOId);
    glBufferData(GL_ARRAY_BUFFER, bufferSize(), 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (format().isIndexed()) {
        qDebug() << "VBO: creating index buffer of" << indexBufferSize() << "bytes";
        glGenBuffers(1, &mIndexVBOId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexVBOId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufferSize(), 0, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
}

bool GeometryBufferVBO::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, mVBOId);
    if (format().isIndexed()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexVBOId);
    }

    return GeometryBufferVertexArray::bind();
}

bool GeometryBufferVBO::unbind()
{
    if (!GeometryBufferVertexArray::unbind()) {
        return false;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (format().isIndexed()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
    return true;
}

void GeometryBufferVBO::addData(void* data, int size, int offset)
{
    qDebug() << "  VBO::addData(): size=" << size << ", offset=" << offset;
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void GeometryBufferVBO::addIndices(unsigned int* indices, int count, int offset)
{
    qDebug() << "  VBO::addIndices(): count=" << count << ", offset=" << offset;
    int byteoffset = offset * sizeof(unsigned int);
    int bytecount = count * sizeof(unsigned int);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER_ARB, byteoffset, bytecount, indices);
}


}  // namespace
