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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <kgllib/glwidget.h>

namespace KGLLib
{
    class Texture;
    class Mesh;
}


class GLWidget : public KGLLib::GLWidget
{
    Q_OBJECT
public:
    GLWidget(QWidget* parent);
    ~GLWidget();

public slots:
    void setAutoUpdate(bool update);

    void showAndDebug();
    void hideAndDebug();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void render();

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

private:
    QString mInputString;
    int mInputFontSize;
    QSize mSize;

    QList<QLine> mLines;
    QPoint mLastClickPos;
    bool mDrawingLine;
    QPoint mMousePos;
    bool mMousePressed;

    float mBoomProgress;

    int mFramesRendered;
    bool mAutoUpdate;
};

#endif
