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

#include "demoglwidget.h"

#include <kgllib/fpscounter.h>
#include <kgllib/textrenderer.h>

#include <QTimer>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QtDebug>

#include <Eigen/Geometry>

using namespace Eigen;


GLWidget::GLWidget(QWidget* parent) : KGLLib::GLWidget(parent)
{
    mInputFontSize = 14;
    mDrawingLine = false;
    mMousePressed = false;
    mBoomProgress = 2;
    mFramesRendered = 0;
    mAutoUpdate = false;

    setMinimumSize(400, 400);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setFocus();
    setMouseTracking(true);
    mSize = QSize(100, 100);
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
    qDebug() << "GLWidget::initializeGL()";
    KGLLib::GLWidget::initializeGL();

    setClearColor(Vector4f(0.6, 0.8, 0.7, 0.0));
    glDisable(GL_DEPTH_TEST);
}

void GLWidget::resizeGL(int width, int height)
{
    mSize = QSize(width, height);
    KGLLib::GLWidget::resizeGL(width, height);
}

void GLWidget::render()
{
    mFramesRendered++;

    // Set up ortho projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width(), height(), 0, 0, 1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(1.0, 1.0, 1.0, 0.2);
    glRecti(5, 5, mSize.width() - 5, mSize.height() - 5);
    glDisable(GL_BLEND);

    // Render the lines
    glColor3f(1.0, 1.0, 0.0);
    glLineWidth(2.0);
    glBegin(GL_LINES);
    foreach (const QLine& line, mLines) {
        glVertex2i(line.x1(), line.y1());
        glVertex2i(line.x2(), line.y2());
    }
    if (mDrawingLine) {
        glVertex2i(mLastClickPos.x(), mLastClickPos.y());
        glVertex2i(mMousePos.x(), mMousePos.y());
    }
    glEnd();


    // Render text
    textRenderer()->begin(this);
    textRenderer()->draw(20, 20, "You can resize text using the mouse wheel");

    QFont inputFont("Sans-Serif", mInputFontSize);
    if (mInputString.isEmpty()) {
        glColor3f(0.4, 0.4, 0.4);
        textRenderer()->draw(10, 50, "Type something to see it here", inputFont);
    } else {
        glColor3f(0.2, 0.2, 0.2);
        textRenderer()->draw(10, 50, mInputString, inputFont);
    }

    if (mMousePressed) {
        textRenderer()->draw(10, 300, "Mouse is pressed");
    }

    if (mBoomProgress < 1) {
        int fontsize = (int)(mBoomProgress*100);
        glColor4f(1.0, 1.0, 0.0, 0.3 * (1-mBoomProgress));
        textRenderer()->draw(200-2*fontsize, 200-fontsize/2, "BOOM", QFont("Serif", fontsize));
        mBoomProgress += 0.05;
    }

    glColor4f(0.8, 0.8, 1.0, 1.0);
    textRenderer()->draw(10, height() - 30, QString("Frames rendered: %1").arg(mFramesRendered));

    textRenderer()->end();

    if (mAutoUpdate || mBoomProgress < 1) {
        // Repaint for animation
        QTimer::singleShot(20, this, SLOT(update()));
    }
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
    mInputString += event->text();
    event->accept();
    update();
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
    if (event->delta() > 0) {
        mInputFontSize = qMin(30, mInputFontSize+1);
    } else {
        mInputFontSize = qMax(3, mInputFontSize-1);
    }
    event->accept();
    update();
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    qDebug() << "GLWidget::mouseDoubleClickEvent";
    mBoomProgress = 0;
    update();
    event->accept();
    hideAndDebug();
    QTimer::singleShot(2000, this, SLOT(showAndDebug()));
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "GLWidget::mousePressEvent";
    mMousePressed = true;
    event->accept();

    if (mDrawingLine) {
        mLines.append(QLine(mLastClickPos, event->pos()));
    } else {
        mLastClickPos = event->pos();
    }
    mDrawingLine = !mDrawingLine;
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    mMousePressed = false;
    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
    mMousePos = event->pos();
    if (mDrawingLine) {
        update();
    }
}

void GLWidget::setAutoUpdate(bool up)
{
    mAutoUpdate = up;
    if (mAutoUpdate) {
        update();
    }
}

void GLWidget::showAndDebug()
{
    qDebug() << "GLWidget::showAndDebug()";
    show();
    qDebug() << "GLWidget::showAndDebug() DONE";
}

void GLWidget::hideAndDebug()
{
    qDebug() << "GLWidget::hideAndDebug()";
    hide();
    qDebug() << "GLWidget::hideAndDebug() DONE";
}
