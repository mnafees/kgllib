/*
 * Copyright (C) 2008-2009 Rivo Laks <rivolaks@hot.ee>
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

#ifndef KGLLIB_SHAPES_H
#define KGLLIB_SHAPES_H

#include "kgllib.h"

#include <Eigen/Core>


namespace KGLLib
{

class Batch;

/**
 * @brief Helper class for creating simple geometrical objects.
 *
 * This class contains static methods for creating simple geometrical objects.
 * Its main purpose is to allow rapid development by providing fast and easy way of creating basic
 *  geometric objects.
 *
 * Currently spheres and cubes can be created.
 *
 * For each object type, there are 3 methods: the first only creates arrays of vertices and texture
 *  coordinates. This can be useful if you want to do further processing on the data.
 *
 * The second one also adds the created data to a specified Batch object. This is useful when you
 *  actually want to create a Mesh object or other subclass of Batch. In that case you'll need to
 *  first create the Mesh object yourself and then use the Shapes function to add geometry to it:
 * @code
 * // Create Mesh object
 * Mesh* m = new Mesh();
 * m->setTexture(myCubeTexture);
 * // Add cube geometry to the mesh
 * Shapes::createCube(m);
 * @endcode
 *
 * Finally, the third method creates a new Batch object and adds the geometry to it.
 **/
class KGLLIB_EXTRAS_EXPORT Shapes
{
public:
    /**
     * Creates a Batch object with spherical geometry.
     * The detail parameter can be used to control how detailed the sphere will be. The detail
     *  level is actually a subdivision level, so note that the number of used vertices and faces
     *  will rise exponentially when increasing the detail level.
     **/
    static Batch* createSphere(int detail = 3);
    static void createSphere(Batch* batch, int detail = 3);
    static void createSphereGeometry(int& vertexcount, Eigen::Vector3f*& vertices, Eigen::Vector2f*& texcoords, int detail = 3);

    /**
     * Creates a Batch object with cube geometry.
     **/
    static Batch* createCube();
    static void createCube(Batch* batch);
    static void createCubeGeometry(int& vertexcount, Eigen::Vector3f*& vertices, Eigen::Vector2f*& texcoords);
};

}

#endif

