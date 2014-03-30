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

#include "mainwidget.h"

#include "demoglwidget.h"
#include <kgllib/widgetproxy.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QtDebug>


MainWidget::MainWidget()
{
    const bool useProxy = true;

    GLWidget* gl = new GLWidget(this);
    KGLLib::WidgetProxy* proxy = 0;
    if (useProxy) {
        proxy = new KGLLib::WidgetProxy(gl, this);
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("This is the upper widget", this));

    if (useProxy) {
        layout->addWidget(proxy);
    } else {
        layout->addWidget(gl);
    }

    QCheckBox* cb = new QCheckBox("Enable continuous updates", this);
    cb->setChecked(false);
    connect(cb, SIGNAL(toggled(bool)), gl, SLOT(setAutoUpdate(bool)));
    layout->addWidget(cb);
    layout->addWidget(new QLabel("Statusbar", this));

    if (useProxy) {
        QWidget* w = proxy->createWindow("Embedded window");
        w->move(20, 270);
        w->layout()->addWidget(new QLineEdit(this));
        QPushButton* pb = new QPushButton("Create new toplevel GL widget", this);
        connect(pb, SIGNAL(clicked()), this, SLOT(createToplevelGLWidget()));
        layout->addWidget(pb);
        w->layout()->addWidget(pb);
        proxy->addWidget(w);
    }
}

MainWidget::~MainWidget()
{
}

void MainWidget::createToplevelGLWidget()
{
    // Create a toplevel GLWidget
    GLWidget* gl = new GLWidget(0);
    // Create a proxy for the gl widget. Note that this could be done inside
    //  GLWidget, so the client (that creates the GLWidget) needn't know
    //  anything about the WidgetProxy.
    KGLLib::WidgetProxy* proxy = new KGLLib::WidgetProxy(gl, 0);
    gl->show();

    QWidget* hintWin = proxy->createWindow("Hint");
    hintWin->layout()->addWidget(new QLabel("This is another toplevel GLwidget,\n with embedded proxy", hintWin));
    hintWin->move(10, 100);
    proxy->addWidget(hintWin);
    // This will crash on app shutdown, but thiago said it's probably internal
    //  Qt issue.
}


#include "mainwidget.moc"
