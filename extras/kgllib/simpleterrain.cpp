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

#include "simpleterrain.h"

#include <kgllib.h>

#include <QImage>
#include <QColor>

#include <Eigen/Geometry>

using namespace Eigen;

namespace KGLLib
{

class SimpleTerrain::Chunk
{
    
};

SimpleTerrain::SimpleTerrain(const QString& filename) : Mesh()
{
    mHeight = mWidth = 0;
    mHeightmap = 0;
    mNormalmap = 0;
    mVertices = mNormals = 0;
    mIndices = 0;

    QImage img(filename);
    if (img.isNull()) {
        return;
    }
    mWidth = img.width();
    mHeight = img.height();
    mMinHeight = 0;
    mMaxHeight = 1;
    mTileSize = 1;

    mNormalmap = 0;
    mIndices = 0;
    mVertices = 0;
    mNormals = 0;


    mHeightmap = new float*[mWidth];
    for (int x = 0; x < mWidth; x++) {
        mHeightmap[x] = new float[mHeight];
        for (int z = 0; z < mHeight; z++) {
            QColor c(img.pixel(x, z));
            mHeightmap[x][z] = c.redF()*0.30 + c.greenF()*0.60 + c.blueF()*0.10;
        }
    }

    mDirty = true;  // Don't call setDirty() here
    setPrimitiveType(GL_TRIANGLE_STRIP);
}

SimpleTerrain::~SimpleTerrain()
{
    if (mHeightmap) {
        for (int x = 0; x < mWidth; x++) {
            delete[] mHeightmap[x];
        }
        delete[] mHeightmap;
    }
    if (mNormalmap) {
        for (int x = 0; x < mWidth; x++) {
            delete[] mNormalmap[x];
        }
        delete[] mNormalmap;
    }
}

void SimpleTerrain::setHeightRange(float min, float max)
{
    mMinHeight = min;
    mMaxHeight = max;
    setDirty();
}

void SimpleTerrain::setTileSize(float size)
{
    mTileSize = size;
    setDirty();
}

bool SimpleTerrain::isValid() const
{
    return mWidth != 0 && mHeight != 0;
}

void SimpleTerrain::setDirty()
{
    if (!mDirty) {
        // Delete current arrays
        for (int x = 0; x < mWidth; x++) {
            delete[] mNormalmap[x];
        }
        delete[] mNormalmap;
        delete[] mIndices;
        delete[] mVertices;
        delete[] mNormals;
    }
    mDirty = true;
}

void SimpleTerrain::render()
{
    if (!isValid()) {
        return;
    }
    if (mDirty) {
        cookMesh(false);
    }
    Mesh::render();
}

void SimpleTerrain::recalcNormalmap()
{
    if (mNormalmap) {
        for (int x = 0; x < mWidth; x++) {
            delete[] mNormalmap[x];
        }
        delete[] mNormalmap;
    }

    mNormalmap = new Vector3f*[mWidth];
    for (int x = 0; x < mWidth; x++) {
        mNormalmap[x] = new Vector3f[mHeight];
        for (int z = 0; z < mHeight; z++) {
            mNormalmap[x][z] = Vector3f(0, 0, 0);
            if (x > 0) {
                if (z > 0) {
                    mNormalmap[x][z] += cellNormal(x, z,  x, z-1,  x-1, z);
                }
                if (z+1 < mHeight) {
                    mNormalmap[x][z] += cellNormal(x, z,  x-1, z,  x, z+1);
                }
            }
            if (x+1 < mWidth) {
                if (z > 0) {
                    mNormalmap[x][z] += cellNormal(x, z,  x+1, z,  x, z-1);
                }
                if (z+1 < mHeight) {
                    mNormalmap[x][z] += cellNormal(x, z,  x, z+1,  x+1, z);
                }
            }
            mNormalmap[x][z].normalize();
        }
    }
}

Vector3f SimpleTerrain::cellNormal(int x1, int z1,  int x2, int z2,  int x3, int z3)
{
    Vector3f a(x1*mTileSize, height(x1, z1), z1*mTileSize);
    Vector3f b(x2*mTileSize, height(x2, z2), z2*mTileSize);
    Vector3f c(x3*mTileSize, height(x3, z3), z3*mTileSize);
    return (c - b).cross(a - b).normalized();
}

void SimpleTerrain::cookMesh(bool useIndices)
{
    if (!isValid()) {
        return;
    }

    mDirty = false;
    recalcNormalmap();

    delete[] mVertices;
    delete[] mNormals;
    delete[] mIndices;

    int vcount = mWidth * mHeight;
    int icount = (mWidth-1) * (2 + 2*mHeight);

    if (useIndices) {
        mIndices = new unsigned int[icount];
    } else {
        vcount = icount;
    }

    mVertices = new Vector3f[vcount];
    mNormals = new Vector3f[vcount];
    int index = 0;

    if (!useIndices) {
        for (int x = 1; x < mWidth; x++) {
            addVertex(x-1, 0, index++);
            for (int z = 0; z < mHeight; z++) {
                addVertex(x-1, z, index++);
                addVertex(x  , z, index++);
            }
            addVertex(x, mHeight-1, index++);
        }
    } else {
        for (int x = 0; x < mWidth; x++) {
            for (int z = 0; z < mHeight; z++) {
                addVertex(x  , z  , index++);
            }
        }
        index = 0;
        for (int x = 1; x < mWidth; x++) {
            mIndices[index++] = (x-1)*mHeight + 0;
            for (int z = 0; z < mHeight; z++) {
                mIndices[index++] = (x-1)*mHeight + z;
                mIndices[index++] = (x  )*mHeight + z;
            }
            mIndices[index++] = (x  )*mHeight + mHeight-1;
        }
    }

    setVertexCount(vcount);
    setVertices(mVertices);
    setNormals(mNormals);
    if (useIndices) {
        setIndices(mIndices, icount);
    }
}

void SimpleTerrain::addVertex(int x, int z, int index)
{
    mNormals[index] = mNormalmap[x][z];
    mVertices[index] = Vector3f(x*mTileSize, height(x, z), z*mTileSize);
}

}
