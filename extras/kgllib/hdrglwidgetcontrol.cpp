/*
 * Copyright 2008  Rivo Laks <rivolaks@hot.ee>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "hdrglwidgetcontrol.h"

#include "ui_hdrglwidgetcontrol.h"
#include "hdrglwidget.h"

namespace KGLLib
{

class HdrGLWidgetControl::Private
{
public:
    Ui::HdrGLWidgetControl ui;
};

HdrGLWidgetControl::HdrGLWidgetControl(HdrGLWidget* glwidget, QWidget* parent) :
        QWidget(parent),
        d(new Private())
{
    mGLWidget = glwidget;

    d->ui.setupUi(this);
    connect(d->ui.enableHdr, SIGNAL(toggled(bool)), mGLWidget, SLOT(setHdrRenderingActive(bool)));
    connect(d->ui.bloom, SIGNAL(toggled(bool)), mGLWidget, SLOT(setBloomEnabled(bool)));
    connect(d->ui.bloomAfterTonemapping, SIGNAL(toggled(bool)), mGLWidget, SLOT(setBloomAfterTonemapping(bool)));
    connect(d->ui.bloomDownsize, SIGNAL(valueChanged(int)), this, SLOT(setBloomDownsize(int)));
    connect(d->ui.bloomStrength, SIGNAL(valueChanged(double)), this, SLOT(setBloomStrength(double)));
    connect(d->ui.exposureAuto, SIGNAL(toggled(bool)), this, SLOT(setAutoExposure(bool)));
    connect(d->ui.exposure, SIGNAL(valueChanged(double)), this, SLOT(setExposure(double)));
    connect(d->ui.exposureAutoSpeed, SIGNAL(valueChanged(double)), this, SLOT(setExposureAutoSpeed(double)));
    connect(d->ui.exposureAutoTarget, SIGNAL(valueChanged(double)), this, SLOT(setExposureAutoTarget(double)));

    updateSettings();
}

HdrGLWidgetControl::~HdrGLWidgetControl()
{
}

void HdrGLWidgetControl::updateSettings()
{
    d->ui.enableHdr->setChecked(mGLWidget->hdrRenderingActive());
    d->ui.bloom->setChecked(mGLWidget->bloomEnabled());
    d->ui.bloomAfterTonemapping->setChecked(mGLWidget->bloomAfterTonemapping());
    for (int i = 0; i <= d->ui.bloomDownsize->maximum(); i++) {
        if (mGLWidget->bloomDownsize() == 1 << i) {
            d->ui.bloomDownsize->setValue(i);
            break;
        }
    }
    d->ui.bloomStrength->setValue(mGLWidget->bloomStrength());
    d->ui.exposureAuto->setChecked(mGLWidget->autoExposure());
    d->ui.exposure->setValue(mGLWidget->exposure());
    d->ui.exposureAutoSpeed->setValue(mGLWidget->autoExposureSpeed());
    d->ui.exposureAutoTarget->setValue(mGLWidget->autoExposureTarget());
}

void HdrGLWidgetControl::setAutoExposure(bool a)
{
    mGLWidget->setAutoExposure(a);
    d->ui.exposureOptions->setCurrentIndex(a ? 1 : 0);
}

void HdrGLWidgetControl::setBloomDownsize(int ds)
{
    mGLWidget->setBloomDownsize(1 << ds);
}

void HdrGLWidgetControl::setBloomStrength(double s)
{
    mGLWidget->setBloomStrength(s);
}

void HdrGLWidgetControl::setExposure(double exp)
{
    mGLWidget->setExposure(exp);
}

void HdrGLWidgetControl::setExposureAutoSpeed(double s)
{
    mGLWidget->setAutoExposureSpeed(s);
}

void HdrGLWidgetControl::setExposureAutoTarget(double t)
{
    mGLWidget->setAutoExposureTarget(t);
}

}

#include "hdrglwidgetcontrol.moc"
