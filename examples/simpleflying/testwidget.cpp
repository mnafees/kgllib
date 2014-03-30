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

#include "testwidget.h"

#include <kgllib/texture.h>
#include <kgllib/simpleterrain.h>
#include <kgllib/shader.h>
#include <kgllib/program.h>

#include <QTimer>
#include <QtDebug>
#include <QImage>
#include <QMouseEvent>
#include <QKeyEvent>

#include <Eigen/Geometry>

#include <iostream>
#include <cmath>

using namespace std;
using namespace Eigen;


static Vector4f fogcolor(0.6, 0.6, 1.0, 0.0);

TestWidget::TestWidget() : KGLLib::GLWidget()
{
    isPaused = false;
    targetGameSpeed = gameSpeed = 1.0;

    setFocusPolicy(Qt::StrongFocus);
}

void TestWidget::mousePressEvent(QMouseEvent* e)
{
    lastMousePos = e->pos();
}

void TestWidget::mouseMoveEvent(QMouseEvent* e)
{
    int dx = e->x() - lastMousePos.x();
    int dy = e->y() - lastMousePos.y();
    if (e->buttons() & Qt::LeftButton) {
        trackball.rotateX(dy/35.0f);
        trackball.rotateZ(dx/35.0f);
    }
    lastMousePos = e->pos();
}

void TestWidget::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_P) {
        isPaused = !isPaused;
    } else if (e->key() == Qt::Key_Plus) {
        targetGameSpeed += 0.1;
    } else if (e->key() == Qt::Key_Minus) {
        targetGameSpeed = qMax(0.0f, targetGameSpeed - 0.1f);
    } else {
        isKeyPressed.insert(e->key());
    }
}

void TestWidget::processInput()
{
    float elapsed = fpsCounter()->timeElapsed();
    if (isKeyPressed.contains(Qt::Key_W)) {
        trackball.rotateX(-10 * elapsed);
    } else if (isKeyPressed.contains(Qt::Key_S)) {
        trackball.rotateX( 10 * elapsed);
    } else if (isKeyPressed.contains(Qt::Key_A)) {
        trackball.rotateY( 10 * elapsed);
    } else if (isKeyPressed.contains(Qt::Key_D)) {
        trackball.rotateY(-10 * elapsed);
    } else if (isKeyPressed.contains(Qt::Key_Q)) {
        trackball.rotateZ( 10 * elapsed);
    } else if (isKeyPressed.contains(Qt::Key_E)) {
        trackball.rotateZ(-10 * elapsed);
    }
}

void TestWidget::keyReleaseEvent(QKeyEvent* e)
{
    isKeyPressed.remove(e->key());
}

void TestWidget::initializeGL()
{
    GLWidget::initializeGL();

    setClearColor(fogcolor);

    qDebug() << "loading qt4 tex";
    tex = new KGLLib::Texture(":/pilt.png");
    qDebug() << "loading grass tex";
    grass = new KGLLib::Texture(":/grass1.jpg");
    KGLLib::checkGLError("init3");

    qDebug() << "loeading terrain";
    terrain = new KGLLib::SimpleTerrain(":/height.png");
    terrain->setTileSize(10);
    terrain->setHeightRange(-200, 200);
    terrain->setTexture(grass);
    KGLLib::checkGLError("init4");

    if (!tex->isValid() || !grass->isValid() || !terrain->isValid()) {
        return setErrorText("Data couldn't be loaded");
    }

    qDebug() << "loading program";
    program = new KGLLib::Program(":/test.vert", ":/test.frag");
    KGLLib::checkGLError("init5.1");
    if (!program->isValid()) {
        return setErrorText("Data couldn't be loaded");
    }
    program->bind();
    program->setUniform("groundTexture", 0);
    program->setUniform("fogColor", fogcolor);
    program->unbind();
    qDebug() << "init done";
    KGLLib::checkGLError("init6");
    terrain->setProgram(program);

//     camera()->setPosition(Vector3f(terrain->width()*terrain->tileSize()/2, 0, terrain->height()*terrain->tileSize()/2));
    camera()->setDepthRange(1, 5000);
    camera()->setPosition(Vector3f(0, 20, 0));
    trackball.rotateY(180 + 45);
}

void TestWidget::processMovement()
{
    float elapsed = fpsCounter()->timeElapsed() * qMax(0.5f, targetGameSpeed);
    float targetspeed = isPaused ? 0.0f : targetGameSpeed;
    if (gameSpeed > targetspeed) {
        gameSpeed = qMax(targetspeed, gameSpeed - elapsed);
    } else if (gameSpeed < targetspeed) {
        gameSpeed = qMin(targetspeed, gameSpeed + elapsed);
    }

    Vector3f pos = camera()->position();
    pos += trackball.transform(Vector3f(0, 1, 0)) * 0.15 * gameSpeed;  // Lift
    pos += Vector3f(0, -1, 0) * 0.1 * gameSpeed;  // Gravity
    pos += trackball.transform(Vector3f(0, 0, -1)) * 0.1 * gameSpeed;  // Thrust
    camera()->setPosition(pos);
    camera()->setDirection(trackball.transform(Vector3f(0, 0, -1)));
    camera()->setUp(trackball.transform(Vector3f(0, 1, 0)));
}

void TestWidget::render()
{
    processInput();
    processMovement();

    camera()->applyView();

    tex->enable();
    glBegin(GL_QUADS);
    float y = -5;
        glTexCoord2f(0, 0); glVertex3f(-20, y, -50);
        glTexCoord2f(0, 1); glVertex3f(-20, y,  50);
        glTexCoord2f(1, 1); glVertex3f( 20, y,  50);
        glTexCoord2f(1, 0); glVertex3f( 20, y, -50);
    glEnd();
    tex->disable();

    Vector3f pos = camera()->position();
    Vector3f lightdir(2, 1, 1);
    lightdir.normalize();
    program->bind();
    program->setUniform("lightDir", lightdir);
    program->setUniform("cameraPos", pos);
    terrain->render();


    renderText(10, 25, QString("Pos: (%1; %2; %3)").arg(pos.x(), 0, 'f', 2).arg(pos.y(), 0, 'f', 2).arg(pos.z(), 0, 'f', 2));
    renderText(10, 35, QString("Elapsed: %1").arg(fpsCounter()->timeElapsed(), 0, 'f', 3));
    renderText(10, 45, QString("GameSpeed: %1, target: %2").arg(gameSpeed, 0, 'f', 3).arg(targetGameSpeed, 0, 'f', 3));

    QTimer::singleShot(0, this, SLOT(updateGL()));
}


#include "testwidget.moc"
