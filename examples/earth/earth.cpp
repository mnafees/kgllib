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

#include "earth.h"

#include <kgllib/texture.h>
#include <kgllib/fpscounter.h>
#include <kgllib/mesh.h>
#include <kgllib/camera.h>
#include <kgllib/textrenderer.h>
#include <kgllib/shapes.h>

#include <QTimer>
#include <QMouseEvent>
#include <QtDebug>

#include <Eigen/Array>
#include <Eigen/Geometry>
#include <math.h>

using namespace KGLLib;
using namespace Eigen;


Earth::Earth() : GLWidget()
{
    mDiffuseTex = 0;
    mMesh = mStarsMesh = 0;
    mRotation = 0;
}

Earth::~Earth()
{
    delete mDiffuseTex;
    delete mMesh;
    delete mStarsMesh;
}

void Earth::mousePressEvent(QMouseEvent* e)
{
    lastMousePos = e->pos();
}

void Earth::mouseMoveEvent(QMouseEvent* e)
{
    int dx = e->x() - lastMousePos.x();
    int dy = e->y() - lastMousePos.y();
    if (e->buttons() & Qt::LeftButton) {
        trackball.rotate(dx, dy);
    }
    lastMousePos = e->pos();
}

void Earth::initializeGL()
{
    GLWidget::initializeGL();

    mDiffuseTex = new Texture(":/data/earth.png");
    if (!mDiffuseTex->isValid()) {
        return setErrorText("Error: data files weren't found");
    }

    mDiffuseTex->setWrapMode(GL_CLAMP);
    mMesh = new Mesh();
    Shapes::createSphere(mMesh, 6);
    mMesh->setTexture(mDiffuseTex);
    glEnable(GL_CULL_FACE);

    mStarsMesh = createStarMesh(2000);
}

void Earth::render()
{
    trackball.transform(*camera()).applyView();

    // Rotate 20 degrees/sec
    mRotation += fpsCounter()->timeElapsed() * 20;
    glRotatef(mRotation, 0, 1, 0);
    mStarsMesh->render();
    glColor4f(1, 1, 1, 1);
    mMesh->render();

    int ty = 17;
    Vector3f pos = trackball.transform(Vector3f(3, 0, 0));
    textRenderer()->begin(this);
    textRenderer()->draw(5, ty += 12, QString("pos: (%1, %2, %3)").arg(pos.x()).arg(pos.y()).arg(pos.z()));
    // Show hint during the first 5 seconds, then fade it out
    if (fpsCounter()->totalTimeElapsed() < 6.0f) {
        glColor4f(1, 1, 1, qMax(0.0f, 6 - fpsCounter()->totalTimeElapsed()));
        textRenderer()->draw(5, height() - 30, "Hint: you can move the Earth using the mouse");
    }
    textRenderer()->end();
    // Repaint for animation
    QTimer::singleShot(20, this, SLOT(update()));
}

Mesh* Earth::createStarMesh(int stars)
{
    Vector3f* vertices = new Vector3f[stars];
    Vector3f* colors = new Vector3f[stars];

    for (int i = 0; i < stars; i++) {
        vertices[i] = Vector3f::Random().normalized();
        vertices[i] *= 50;
        float c = 0.1f + rand()/(float)RAND_MAX * 0.6;
        colors[i].x() = colors[i].y() = colors[i].z() = c;
    }

    Mesh* m = new Mesh();
    m->setVertices(vertices);
    m->setColors(colors);
    m->setVertexCount(stars);
    m->setPrimitiveType(GL_POINTS);
    return m;
}
