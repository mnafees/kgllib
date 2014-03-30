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

#include "shapes.h"

#include "batch.h"

#include <QtDebug>

#ifndef M_PI
#define M_PI 3.14159265
#endif

using namespace Eigen;

namespace KGLLib
{

void Shapes::createSphereGeometry(int& vertexcount, Vector3f*& vertices, Vector2f*& texcoords, int detail)
{
    Vector3f xp( 1,  0,  0);
    Vector3f xn(-1,  0,  0);
    Vector3f yp( 0,  1,  0);
    Vector3f yn( 0, -1,  0);
    Vector3f zp( 0,  0,  1);
    Vector3f zn( 0,  0, -1);
    Vector3f octahedron[] = {
        yp, xn, zp,
        yp, zp, xp,
        yp, xp, zn,
        yp, zn, xn,
        yn, zp, xn,
        yn, xp, zp,
        yn, zn, xp,
        yn, xn, zn,
    };

    vertexcount = 8*3;
    vertices = new Vector3f[vertexcount];
    for (int i = 0; i < vertexcount; i++) {
        vertices[i] = octahedron[i];
    }
    for (int d = 0; d < detail; d++) {
        int vc = vertexcount*4;
        Vector3f* newv = new Vector3f[vc];
        int j = 0;
        for (int i = 0; i < vertexcount; i += 3) {
            // Calculate midpoints of each side
            Vector3f a = (vertices[i+0] + vertices[i+1]) / 2;
            Vector3f b = (vertices[i+1] + vertices[i+2]) / 2;
            Vector3f c = (vertices[i+2] + vertices[i+0]) / 2;
            // Normalize them so that they'd be exactly 1 unit from center
            a.normalize();
            b.normalize();
            c.normalize();

            // Add new vertices
            newv[j++] = vertices[i+0];
            newv[j++] = a;
            newv[j++] = c;

            newv[j++] = c;
            newv[j++] = a;
            newv[j++] = b;

            newv[j++] = b;
            newv[j++] = a;
            newv[j++] = vertices[i+1];

            newv[j++] = b;
            newv[j++] = vertices[i+2];
            newv[j++] = c;
        }
        delete[] vertices;
        vertices = newv;
        vertexcount = vc;
    }

    // Calculate texcoords
    texcoords = new Vector2f[vertexcount];
    for (int i = 0; i < vertexcount; i += 3) {
//         qDebug() << "  Processing face" << i;
        for (int j = 0; j < 3; j++) {
            texcoords[i+j].x() = (atan2f(vertices[i+j].x(), vertices[i+j].z()) + M_PI) / (2*M_PI);
            texcoords[i+j].y() = (vertices[i+j].y() + 1) / 2;
        }
        for (int j = 0; j < 3; j++) {
            if (ei_isApprox(vertices[i+j].y(), 1.0f) || ei_isApprox(vertices[i+j].y(), -1.0f)) {
                texcoords[i+j].x() = (texcoords[i+0].x() + texcoords[i+1].x() + texcoords[i+2].x() - texcoords[i+j].x()) / 2;
            }
            if (ei_isApprox(vertices[i+j].x(), 0.0f) && vertices[i+j].z() < 0.0f) {
                float xsum = vertices[i+0].x() + vertices[i+1].x() + vertices[i+2].x();
                if (xsum > 0.0f) {
                    texcoords[i+j].x() = 1;
                } else {
                    texcoords[i+j].x() = 0;
                }
            }
        }
    }
}

void Shapes::createSphere(Batch* batch, int detail)
{
    int vertexcount = 0;
    Vector3f* vertices = 0;
    Vector2f* texcoords = 0;

    createSphereGeometry(vertexcount, vertices, texcoords, detail);

    batch->setVertices(vertices);
    batch->setTexcoords(texcoords);
    batch->setVertexCount(vertexcount);
//     batch->setPrimitiveType(GL_TRIANGLES);
    qDebug() << "Sphere mesh has" << vertexcount << "vertices (" << vertexcount/3 << "faces)";
}

Batch* Shapes::createSphere(int detail)
{
    Batch* b = new Batch();
    createSphere(b, detail);
    return b;
}



void Shapes::createCubeGeometry(int& vertexcount, Vector3f*& vertices, Vector2f*& texcoords)
{
    vertices = new Vector3f[6*4];
    texcoords = new Vector2f[6*4];
    vertexcount = 24;

    int i = 0;
    vertices[i++] = Vector3f(-1, -1, -1);
    vertices[i++] = Vector3f( 1, -1, -1);
    vertices[i++] = Vector3f( 1,  1, -1);
    vertices[i++] = Vector3f(-1,  1, -1);

    vertices[i++] = Vector3f(-1, -1,  1);
    vertices[i++] = Vector3f(-1,  1,  1);
    vertices[i++] = Vector3f( 1,  1,  1);
    vertices[i++] = Vector3f( 1, -1,  1);

    vertices[i++] = Vector3f( 1, -1, -1);
    vertices[i++] = Vector3f( 1, -1,  1);
    vertices[i++] = Vector3f( 1,  1,  1);
    vertices[i++] = Vector3f( 1,  1, -1);

    vertices[i++] = Vector3f(-1, -1, -1);
    vertices[i++] = Vector3f(-1,  1, -1);
    vertices[i++] = Vector3f(-1,  1,  1);
    vertices[i++] = Vector3f(-1, -1,  1);

    vertices[i++] = Vector3f(-1, -1, -1);
    vertices[i++] = Vector3f(-1, -1,  1);
    vertices[i++] = Vector3f( 1, -1,  1);
    vertices[i++] = Vector3f( 1, -1, -1);

    vertices[i++] = Vector3f(-1,  1, -1);
    vertices[i++] = Vector3f( 1,  1, -1);
    vertices[i++] = Vector3f( 1,  1,  1);
    vertices[i++] = Vector3f(-1,  1,  1);

    for (int j = 0; j < 6; j++) {
        texcoords[j*4 + 0] = Vector2f(0, 0);
        texcoords[j*4 + 1] = Vector2f(1, 0);
        texcoords[j*4 + 2] = Vector2f(1, 1);
        texcoords[j*4 + 3] = Vector2f(0, 1);
    }
}

void Shapes::createCube(Batch* batch)
{
    int vertexcount = 0;
    Vector3f* vertices = 0;
    Vector2f* texcoords = 0;

    createCubeGeometry(vertexcount, vertices, texcoords);

    batch->setVertices(vertices);
    batch->setTexcoords(texcoords);
    batch->setVertexCount(vertexcount);
    batch->setPrimitiveType(GL_QUADS);
}

Batch* Shapes::createCube()
{
    Batch* b = new Batch();
    createCube(b);
    return b;
}

}

