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

#include "trackball.h"

#include "camera.h"

#include <math.h>

inline float deg2rad(float d)
{
    return d * M_PI/180.0f;
}

using namespace Eigen;

namespace KGLLib
{

TrackBall::TrackBall()
{
    mMatrix.setIdentity();
}

TrackBall::~TrackBall()
{
}

void TrackBall::rotate(float dx, float dy)
{
    rotateY(-dx/5);
    rotateX(-dy/5);
}

void TrackBall::rotateX(float degrees)
{
    rotate(degrees, Vector3f(1, 0, 0));
}

void TrackBall::rotateY(float degrees)
{
    rotate(degrees, Vector3f(0, 1, 0));
}

void TrackBall::rotateZ(float degrees)
{
    rotate(degrees, Vector3f(0, 0, 1));
}

void TrackBall::rotate(float degrees, Eigen::Vector3f axis)
{
    mMatrix.rotate(Eigen::AngleAxisf(deg2rad(degrees), axis));
}

Eigen::Vector3f TrackBall::transform(const Eigen::Vector3f& v) const
{
    return mMatrix * v;
}

Camera TrackBall::transform(const Camera& c)
{
    Camera cam(c);
    cam.setPosition(transform(c.position()));
    cam.setLookAt(transform(c.lookAt()));
    cam.setUp(transform(c.up()));
    return cam;
}

Vector3f TrackBall::xAxis() const
{
    return mMatrix.linear().row(0);
}

Vector3f TrackBall::yAxis() const
{
    return mMatrix.linear().row(1);
}

Vector3f TrackBall::zAxis() const
{
    return mMatrix.linear().row(2);
}



}

