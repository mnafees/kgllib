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

#ifndef KGLLIB_BATCH_H
#define KGLLIB_BATCH_H

#include "kgllib.h"
#include "geometrybuffer.h"

#include <QtCore/QList>

#include <Eigen/Core>


namespace KGLLib
{
class GeometryBuffer;

/**
 * @brief A set of geometry.
 *
 * Batch class contains geometry data which is necessary for rendering
 *  something. This data can include vertices, normals, texture coordinates and
 *  vertex colors.
 *
 * Batch object can automatically use VBOs (vertex buffer objects) for
 *  improved rendering performance while falling back to traditional vertex
 *  arrays on systems where VBOs are not supported.
 *
 * To use it, you need to construct the Batch object, set the necessary arrays
 *  and then call the render() method:
 * @code
 *  // Create the Batch object
 *  Batch* b = new Batch();
 *  b->setVertices(myvertices);
 *  b->setTexCoords(mytexcoords);
 *  b->setPrimitiveType(GL_TRIANGLES);
 *  ...
 *  // In your rendering loop:
 *  b->render();
 *  // some textured triangles will be rendered.
 * @endcode
 *
 * @section Modes
 * Batch uses a GeometryBuffer object to store its data and can be used in two
 *  modes: with internal or shared buffer.
 *
 * @subsection internal With internal buffer
 * With internal buffer, Batch creates is own GeometryBuffer object when
 *  update() is called (which is called automatically by render() and bind()
 *  when batch's data has changed).
 *
 * This is the easiest mode to use: you only need to create the Batch objects,
 *  give it some data and then call render() to render the geometry.
 *
 * @subsection shared With shared buffer
 * The other mode uses a single GeometryBuffer object which is shared between
 *  multiple Batch objects. The advantage of this mode is that bind() and
 *  unbind(), which might be expensive operations, only need to be called once
 *  per frame instead of once per rendered Batch.
 *
 * To make multiple batches use a shared buffer, the easiest way is to call
 *  createSharedBuffer() method. It will automatically create a large enough
 *  buffer and make all the batches use it.
 * @code
 * // First create all batches and specify data (using setVertices() and friends)
 * QList<Batch*> models = loadAllModels();
 * // Then create a shared GeometryBuffer
 * Batch::createSharedBuffer(models);
 * @endcode
 *
 * After that you will need to bind() the buffer or batch only once and then
 *  just call renderOnce() for each batch that uses the same buffer. Of course
 *  finally unbind() has to be called as well:
 * @code
 * // It doesn't matter which Batch we bind() since they all use the same buffer
 * models.first()-> bind();
 * // Render all models
 * foreach (Batch* b, models) {
 *     b-> renderOnce();
 * }
 * // Finally unbind the buffer
 * models.first()-> unbind();
 * @endcode
 *
 * @see Mesh, GeometryBuffer
 **/
class KGLLIB_EXPORT Batch
{
public:
    /**
     * Constructs new Batch object.
     *
     * An internal @ref GeometryBuffer object will be created before rendering.
     * You will need to set at least vertices array and vertex count before the
     *  batch can be rendered.
     **/
    Batch();
    /**
     * Constructs new Batch object, using a shared @ref GeometryBuffer.
     * Shared buffers can improve performance since buffers needn't be bound
     *  and unbound while rendering.
     *
     * @param buffer shared @ref GeometryBuffer object to use
     * @param offset offset for the vertex data in the buffer
     * @param indexOffset offset for the index data in the buffer
     **/
    Batch(GeometryBuffer* buffer, int offset, int indexOffset);

    /**
     * Deletes the Batch.
     * If an internal @ref GeometryBuffer is used then it is also deleted.
     **/
    virtual ~Batch();

    /**
     * Renders the batch.
     *
     * This is same as calling first @ref bind(), then @ref renderOnce() and
     *  finally @ref unbind().
     **/
    virtual void render();

    /**
     * Binds the @ref GeometryBuffer used by this batch.
     *
     * @ref update() is automatically called if Batch's data has changed.
     **/
    virtual void bind();
    /**
     * Renders the batch without doing bind and unbind operations on the
     *  geometry buffer.
     *
     * The geometry buffer used by this batch has to be already bound before
     *  you can use this method.
     **/
    virtual void renderOnce();
    /**
     * Unbinds the @ref GeometryBuffer used by this batch.
     */
    virtual void unbind();

    /**
     * Set the number of vertices in the batch to @p count.
     * Each specified array must contain at least @p count entries (if they
     *  contain more, then the remaining ones will be unused).
     **/
    void setVertexCount(int count);
    /**
     * @return number of vertices in this batch
     **/
    int vertexCount() const  { return mVertexCount; }

    /**
     * Sets the vertices array to @p vertices.
     **/
    void setVertices(Eigen::Vector2f* vertices)  { setVertices(vertices, 2); }
    void setVertices(Eigen::Vector3f* vertices)  { setVertices(vertices, 3); }
    void setVertices(Eigen::Vector4f* vertices)  { setVertices(vertices, 4); }
    /**
     * @return array of vertices used for this batch
     **/
    const void* verticesArray() const  { return mVertices; }
    /**
     * Sets the per-vertex colors array to @p colors.
     **/
    void setColors(Eigen::Vector3f* colors)  { setColors(colors, 3); }
    void setColors(Eigen::Vector4f* colors)  { setColors(colors, 4); }
    /**
     * @return array of colors used for this batch
     **/
    const void* colorsArray() const  { return mColors; }
    /**
     * Sets the normals array to @p normals.
     **/
    void setNormals(Eigen::Vector3f* normals);
    /**
     * @return array of normals used for this batch
     **/
    const void* normalsArray() const  { return mNormals; }
    /**
     * Sets the texture coordinates array to @p texcoords.
     **/
    void setTexcoords(float* texcoords)  { setTexcoords(texcoords, 1); }
    void setTexcoords(Eigen::Vector2f* texcoords)  { setTexcoords(texcoords, 2); }
    void setTexcoords(Eigen::Vector3f* texcoords)  { setTexcoords(texcoords, 3); }
    void setTexcoords(Eigen::Vector4f* texcoords)  { setTexcoords(texcoords, 4); }
    /**
     * @return array of texture coordinates used for this batch
     **/
    const void* texcoordsArray() const  { return mTexcoords; }
    /**
     * Sets the indices array to @p indices. The array must contain at least
     *  @p count entries (if it contains more, then the remaining ones will be
     *  unused).
     **/
    void setIndices(unsigned int* indices, int count);
    /**
     * @return array of indices used for this batch
     **/
    const void* indicesArray() const  { return mIndices; }
    /**
     * @return number of indices in this batch
     **/
    int indicesCount() const  { return mIndexCount; }

    /**
     * Sets the primitive type used to render this batch (e.g. GL_QUADS).
     *
     * Default value is GL_TRIANGLES.
     **/
    virtual void setPrimitiveType(GLenum type);
    /**
     * @return OpenGL primitive mode used to render this batch.
     **/
    GLenum primitiveType() const;

    /**
     * Updates the used GeometryBuffer if something has changed.
     *
     * This method is automatically called from @ref bind() in case something
     *  has changed, but you can also call it manually to remove the delay at
     *  first rendering.
     **/
    virtual void update();

    /**
     * @return GeometryBuffer object used by this batch
     **/
    GeometryBuffer* buffer() const  { return mBuffer; }
    /**
     * Changes the GeometryBuffer object used by this batch.
     *
     * With shared buffer, every Batch uses a subsection of the buffer,
     *  limitied by its buffer offsets and vertex/index counts.
     *
     * If buffer is 0, then an internal buffer will be created and used.
     *
     * Note that the data isn't copied immediately but the next time when
     *  update() or bind() is called. Thus all data specified for this Batch
     *  must be valid until then.
     *
     * @param buffer shared @ref GeometryBuffer object to use.
     * @param offset offset for the vertex data in the buffer.
     * @param indexOffset offset for the index data in the buffer.
     **/
    void setBuffer(GeometryBuffer* buffer, int offset, int indexOffset);

    /**
     * Returns GeometryBufferFormat object that could be used to create a
     *  GeometryBuffer object for this Batch.
     *
     * The returned format contains all attributes (vertices, colors, etc)
     *  which are used by this batch.
     **/
    GeometryBufferFormat bestBufferFormat() const;

    /**
     * Creates a GeometryBuffer object shared by the given list of Batch
     *  objects.
     *
     * The created buffer is big enough to hold data of all specified batches.
     *  The batches are automatically set to use the created buffer.
     *
     * All the batches must have the same format, i.e. the GeometryBufferFormat
     *  objects returned by their bestBufferFormat() method can only differ in
     *  vertex and index counts.
     **/
    static GeometryBuffer* createSharedBuffer(const QList<Batch*>& batches);

protected:
    void setVertices(void* vertices, int size);
    void setColors(void* colors, int size);
    void setTexcoords(void* texcoords, int size);

    void init();

private:
    // Pointers to corresponding arrays
    void* mVertices;
    void* mColors;
    void* mNormals;
    void* mTexcoords;
    // How many float components does each element have
    int mVertexSize;
    int mColorSize;
    int mNormalSize;
    int mTexcoordSize;
    // Indices array
    void* mIndices;

    int mVertexCount;
    int mIndexCount;

    bool mDirty;
    GLenum mPrimitiveType;

    GeometryBuffer* mBuffer;
    int mBufferOffset;
    int mBufferIndexOffset;
    bool mOwnBuffer;
};

}

#endif
