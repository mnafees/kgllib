/*
 * Copyright 2008  Rivo Laks <rivolaks@hot.ee>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KGLLIB_MODELLOADER_H
#define KGLLIB_MODELLOADER_H

#include "kgllib.h"

#include <Eigen/Core>

#include <QList>

class QString;


namespace KGLLib
{
class Batch;
class Mesh;

class KGLLIB_EXTRAS_EXPORT ModelLoader
{
public:
    ModelLoader();
    ModelLoader(const QString& filename);
    virtual ~ModelLoader();

    virtual bool load(const QString& filename);

    bool isValid() const  { return mValid; }

    KGLLib::Batch* createBatch();
    KGLLib::Mesh* createMesh();
    // TODO: rename
    void loadIntoBatch(KGLLib::Batch* b);

    void centerModel();
    void recalcNormals();
    void scaleModel(float scale);
    void translateModel(const Eigen::Vector3f& trans);

    int vertexCount() const;
    int indexCount() const;

protected:
    bool loadFromObj(const QString& filename);

    struct FaceVertex;
    friend uint qHash(const FaceVertex& v);
    void createDuplicateVertices();

private:
    bool mValid;

    QList<Eigen::Vector3f> mVertices;
    QList<Eigen::Vector3f> mNormals;
    QList<Eigen::Vector2f> mTexcoords;
    QList<unsigned int> mIndices;

    QList<FaceVertex> mFaceVertices;
};

}

#endif
