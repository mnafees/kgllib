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

#include "modelloader.h"

#include "batch.h"
#include "mesh.h"

#include <QString>
#include <QFile>
#include <QByteArray>
#include <QList>
#include <QtDebug>

#include <Eigen/Geometry>

using namespace Eigen;


namespace KGLLib
{
// FaceVertex is a combination of position, normal and texcoord, each of which
//  might have a different index in the corresponding array.
// Later (in generateSplitArrays()), we'll create arrays where the indices of
//  vertex components are same in every array.
struct ModelLoader::FaceVertex
{
    FaceVertex(const QByteArray& descr)
    {
        vertex = normal = texcoord = 0;

        QList<QByteArray> tokens = descr.split('/');
        if (tokens.count() == 1) {
            vertex = tokens[0].toInt() - 1;
        } else if (tokens.count() == 2) {
            vertex = tokens[0].toInt() - 1;
            texcoord = tokens[1].toInt() - 1;
        } else if (tokens.count() == 3) {
            vertex = tokens[0].toInt() - 1;
            texcoord = tokens[1].toInt() - 1;
            normal = tokens[2].toInt() - 1;
        } else {
            // Invalid face specification?
        }
    }

    bool operator==(const FaceVertex& v) const
    {
        return vertex == v.vertex && normal == v.normal && texcoord == v.texcoord;
    }

    // Indices of vertex/normal/texture coordinates
    int vertex;
    int normal;
    int texcoord;
};

uint qHash(const ModelLoader::FaceVertex& v)
{
    return qHash(qMakePair(v.vertex, qMakePair(v.texcoord, v.normal)));
}

namespace
{
template<typename T> T* listToArray(const QList<T>& list)
{
    T* array = new T[list.count()];
    int index = 0;
    foreach (T element, list) {
        array[index++] = element;
    }
    return array;
}
}


QDebug& operator<<(QDebug& debug, const Vector3f& v)
{
    return debug << "(" << v.x() << "," << v.y() << "," << v.z() << ")";
}
QDebug& operator<<(QDebug& debug, const Vector2f& v)
{
    return debug << "(" << v.x() << "," << v.y() << ")";
}


ModelLoader::ModelLoader()
{
    mValid = false;
}

ModelLoader::ModelLoader(const QString& filename)
{
    mValid = false;
    load(filename);
}

ModelLoader::~ModelLoader()
{
}

bool ModelLoader::load(const QString& filename)
{
    mValid = false;

    // Determine extension of the filename
    int dotpos = filename.lastIndexOf('.');
    if (dotpos == -1) {
        qCritical() << "ModelLoader::load(" << filename << "): filename doesn't have extension";
        return false;
    }
    QString ext = filename.mid(dotpos+1).toLower();

    if (ext == "obj") {
        mValid = loadFromObj(filename);
    } else {
        qCritical() << "ModelLoader::load(" << filename << "): Unknown file type";
    }
    return mValid;
}

int ModelLoader::vertexCount() const
{
    return mVertices.count();
}

int ModelLoader::indexCount() const
{
    return mIndices.count();
}

Batch* ModelLoader::createBatch()
{
    if (!isValid()) {
        return 0;
    }

    Batch* b = new Batch();
    loadIntoBatch(b);
    return b;
}

Mesh* ModelLoader::createMesh()
{
    if (!isValid()) {
        return 0;
    }

    Mesh* m = new Mesh();
    loadIntoBatch(m);
    return m;
}

void ModelLoader::loadIntoBatch(Batch* batch)
{
    if (!isValid()) {
        return;
    }
    // Number of components in each non-empty list must be equal and vertices
    //  list must not be empty.
    if (mVertices.isEmpty()) {
        qCritical() << "ERROR: ModelLoader::createAndSetArrays(): no vertices";
        return;
    }
    if (!mNormals.isEmpty() && mNormals.count() != mVertices.count()) {
        qCritical() << "ERROR: different number of vertices and normals:" <<
                mVertices.count() << "vs" << mNormals.count();
        return;
    }
    if (!mTexcoords.isEmpty() && mTexcoords.count() != mVertices.count()) {
        qCritical() << "ERROR: different number of vertices and texcoords:" <<
                mVertices.count() << "vs" << mTexcoords.count();
        return;
    }

    // Indices
    batch->setIndices(listToArray(mIndices), mIndices.count());

    // Vertices
    batch->setVertexCount(mVertices.count());
    batch->setVertices(listToArray(mVertices));

    // Normals
    if (!mNormals.isEmpty()) {
        batch->setNormals(listToArray(mNormals));
    }

    // Texcoords
    if (!mTexcoords.isEmpty()) {
        batch->setTexcoords(listToArray(mTexcoords));
    }
}

bool ModelLoader::loadFromObj(const QString& filename)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    while (!f.atEnd()) {
        QByteArray line = f.readLine();
        if (line.endsWith("\r\n")) {
            line.truncate(line.length() - 2);
        } else if (line.endsWith('\n')) {
            line.truncate(line.length() - 1);
        }
        QList<QByteArray> tokens = line.split(' ');
        if (tokens[0] == "v") {
            // It's a new vertex
            mVertices.append(Vector3f(tokens[1].toFloat(), tokens[2].toFloat(), tokens[3].toFloat()));
        } else if (tokens[0] == "vn") {
            // It's a vertex normal
            mNormals.append(Vector3f(tokens[1].toFloat(), tokens[2].toFloat(), tokens[3].toFloat()));
        } else if (tokens[0] == "vt") {
            // It's a texcoord
            mTexcoords.append(Vector2f(tokens[1].toFloat(), tokens[2].toFloat()));
        } else if (tokens[0] == "f") {
            // For now this assumes that vertex/texture/normal ids are equal
            //  and just uses the first one.
            mFaceVertices.append(FaceVertex(tokens[1]));
            mFaceVertices.append(FaceVertex(tokens[2]));
            mFaceVertices.append(FaceVertex(tokens[3]));
        } else if (tokens[0] == "o") {
            // New object
        } else {
            // Unknown token, just ignore it.
        }
    }
    qDebug() << "ModelLoader::loadFromObj(): Loaded model has total of" << mVertices.count() << "vertices and" << mFaceVertices.count()/3 << "tris";

    createDuplicateVertices();
    if (mNormals.isEmpty()) {
        recalcNormals();
    }

    return true;
}

void ModelLoader::createDuplicateVertices()
{
    QList<Vector3f> dupVertices;
    QList<Vector3f> dupNormals;
    QList<Vector2f> dupTexcoords;
    QHash<FaceVertex, int> faceVertex2Index;

    mIndices.clear();

    for (int i = 0; i < mFaceVertices.count(); i++) {
        if (!faceVertex2Index.contains(mFaceVertices.at(i))) {
            faceVertex2Index[mFaceVertices.at(i)] = dupVertices.count();
            dupVertices.append(mVertices[mFaceVertices[i].vertex]);
            if (!mNormals.isEmpty()) {
                dupNormals.append(mNormals[mFaceVertices[i].normal]);
            }
            if (!mTexcoords.isEmpty()) {
                dupTexcoords.append(mTexcoords[mFaceVertices[i].texcoord]);
            }
        }
        mIndices.append(faceVertex2Index[mFaceVertices[i]]);
    }

    mVertices = dupVertices;
    mNormals = dupNormals;
    mTexcoords = dupTexcoords;
}

void ModelLoader::centerModel()
{
    if (mVertices.isEmpty()) {
        return;
    }

    // Calculate bounding box of the model
    Vector3f minpos, maxpos;
    minpos = maxpos = mVertices[0];
    for (int i = 0; i < mVertices.count(); i++) {
        minpos.x() = qMin(minpos.x(), mVertices[i].x());
        maxpos.x() = qMax(maxpos.x(), mVertices[i].x());
        minpos.y() = qMin(minpos.y(), mVertices[i].y());
        maxpos.y() = qMax(maxpos.y(), mVertices[i].y());
        minpos.z() = qMin(minpos.z(), mVertices[i].z());
        maxpos.z() = qMax(maxpos.z(), mVertices[i].z());
    }
    // Center the model
    Vector3f translate = -(maxpos + minpos) /2;
    qDebug() << "ModelLoader::centerModel(): translating model by" << translate;
    translateModel(translate);
}

void ModelLoader::translateModel(const Eigen::Vector3f& trans)
{
    for (int i = 0; i < mVertices.count(); i++) {
        mVertices[i] += trans;
    }
}

void ModelLoader::recalcNormals()
{
    if (mVertices.isEmpty() || mIndices.isEmpty()) {
        return;
    }
    if (mNormals.count() != 0 && mVertices.count() != mNormals.count()) {
        qCritical() << "ModelLoader::recalcNormals(): different number of vertices and normals:" <<
                mVertices.count() << "vs" << mNormals.count();
        return;
    }
    mNormals.clear();

    // Zero current vertex normals
    for (int i = 0; i < mVertices.count(); i++) {
        mNormals.append(Vector3f(0, 0, 0));
    }
    // Calculate face normals
    for (int i = 0; i < mIndices.count()/3; i++) {
        int a = mIndices[i*3 + 0];
        int b = mIndices[i*3 + 1];
        int c = mIndices[i*3 + 2];
        Vector3f facenormal = (mVertices[c] - mVertices[b]).cross(mVertices[a] - mVertices[b]);
        facenormal.normalize();
        mNormals[a] += facenormal;
        mNormals[b] += facenormal;
        mNormals[c] += facenormal;
    }
    // Normalize vertex normals
    for (int i = 0; i < mVertices.count(); i++) {
        mNormals[i].normalize();
    }
}

void ModelLoader::scaleModel(float scale)
{
    for (int i = 0; i < mVertices.count(); i++) {
        mVertices[i] *= scale;
    }
}

}

