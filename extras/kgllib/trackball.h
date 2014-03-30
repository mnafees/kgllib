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

#ifndef KGLLIB_TRACKBALL_H
#define KGLLIB_TRACKBALL_H


#include "kgllib.h"

#include <Eigen/Geometry>

namespace KGLLib
{
class Camera;

class KGLLIB_EXTRAS_EXPORT TrackBall
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    TrackBall();
    virtual ~TrackBall();

    virtual void rotate(float dx, float dy);

    Eigen::Vector3f transform(const Eigen::Vector3f& v) const;
    Camera transform(const Camera& c);

    Eigen::Vector3f xAxis() const;
    Eigen::Vector3f yAxis() const;
    Eigen::Vector3f zAxis() const;

    void rotateX(float degrees);
    void rotateY(float degrees);
    void rotateZ(float degrees);

    void rotate(float degrees, Eigen::Vector3f axis);

protected:
    Eigen::Transform3f mMatrix;
};

}

#endif
