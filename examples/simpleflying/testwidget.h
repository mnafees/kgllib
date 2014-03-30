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

#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <kgllib/glwidget.h>
#include <kgllib/fpscounter.h>
#include <kgllib/camera.h>
#include <kgllib/trackball.h>

#include <QTime>
#include <QSet>

namespace KGLLib
{
    class Texture;
    class SimpleTerrain;
    class Program;
}


class TestWidget : public KGLLib::GLWidget
{
    Q_OBJECT

public:
    TestWidget();

protected:
    void initializeGL();
    void render();

    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void keyReleaseEvent(QKeyEvent* e);

    void processInput();
    void processMovement();

private:
    KGLLib::TrackBall trackball;
    KGLLib::Texture* tex;
    KGLLib::SimpleTerrain* terrain;
    KGLLib::Texture* grass;
    KGLLib::Program* program;

    QPoint lastMousePos;

    QSet<int> isKeyPressed;
    bool isPaused;
    float gameSpeed;
    float targetGameSpeed;
};

#endif
