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

#ifndef KGLLIB_HDRGLWIDGETCONTROL_H
#define KGLLIB_HDRGLWIDGETCONTROL_H

#include "kgllib.h"

#include <QWidget>


namespace KGLLib
{
class HdrGLWidget;

class KGLLIB_EXTRAS_EXPORT HdrGLWidgetControl : public QWidget
{
    Q_OBJECT
public:
    explicit HdrGLWidgetControl(HdrGLWidget* widget, QWidget* parent = 0);
    virtual ~HdrGLWidgetControl();

public Q_SLOTS:
    void updateSettings();

protected Q_SLOTS:
    void setBloomDownsize(int ds);
    void setBloomStrength(double s);
    void setExposure(double exp);
    void setExposureAutoSpeed(double s);
    void setExposureAutoTarget(double t);
    void setAutoExposure(bool a);

private:
    class Private;
    Private* const d;

    HdrGLWidget* mGLWidget;
};

}

#endif
