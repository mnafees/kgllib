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

#ifndef KGLLIB_SIMPLETERRAIN_H
#define KGLLIB_SIMPLETERRAIN_H


#include "mesh.h"

#include <Eigen/Core>


class QString;


namespace KGLLib
{
class Batch;

class KGLLIB_EXTRAS_EXPORT SimpleTerrain : public Mesh
{
public:
    SimpleTerrain(const QString& imgfilename);
    virtual ~SimpleTerrain();

    virtual void render();

    void setHeightRange(float min, float max);
    void setTileSize(float size);

    int width() const  { return mWidth; }
    int height() const  { return mHeight; }
    float tileSize() const  { return mTileSize; }
    bool isValid() const;

protected:
    void addVertex(int x, int z, int index);
    void recalcNormalmap();
    void cookMesh(bool useIndices);
    inline float height(int x, int z) const  { return mMinHeight + mHeightmap[x][z] * (mMaxHeight - mMinHeight); }
    Eigen::Vector3f cellNormal(int x1, int z1,  int x2, int z2,  int x3, int z3);
    void setDirty();

protected:
    class Chunk;
    float** mHeightmap;
    Eigen::Vector3f** mNormalmap;
    bool mDirty;

    Eigen::Vector3f* mVertices;
    Eigen::Vector3f* mNormals;
    unsigned int* mIndices;

    int mWidth;
    int mHeight;

    float mMinHeight;
    float mMaxHeight;
    float mTileSize;
};

}

#endif
