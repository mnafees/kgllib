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

#ifndef KGLLIB_HDRGLWIDGET_H
#define KGLLIB_HDRGLWIDGET_H

#include "glwidget.h"


namespace KGLLib
{
class RenderTarget;
class Program;
class Shader;


class KGLLIB_EXTRAS_EXPORT HdrGLWidget : public GLWidget
{
    Q_OBJECT
public:
    explicit HdrGLWidget(QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0);
    explicit HdrGLWidget(QGLContext* context, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0);
    explicit HdrGLWidget(const QGLFormat& format, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0);

    ~HdrGLWidget();

    /**
     * @return whether HDR rendering is currently active.
     **/
    bool hdrRenderingActive() const  { return hdrRenderingSupported() && mHdrRenderingActive; }
    /**
     * @return whether HDR rendering is supported on this hardware.
     **/
    bool hdrRenderingSupported() const  { return mHdrRenderingSupported; }

    /**
     * @return current exposure, used for tonemapping.
     * This is either set using @ref setExposure or calculated automatically
     *  when automatic exposure adjustment is enabled.
     **/
    float exposure() const  { return mExposure; }
    /**
     * @return whether automatic exposure adjustment is enabled.
     * See @ref setAutoExposure for more info.
     **/
    bool autoExposure() const  { return mAutoExposure; }

    float autoExposureSpeed() const  { return mAutoExposureSpeed; }
    float autoExposureTarget() const  { return mAutoExposureTarget; }
    float autoExposureMin() const  { return mAutoExposureMin; }
    float autoExposureMax() const  { return mAutoExposureMax; }

    bool bloomEnabled() const  { return mBloomEnabled; }

    bool bloomAfterTonemapping() const  { return mBloomAfterTonemapping; }

    int bloomDownsize() const  { return mBloomDownsize; }

    float bloomStrength() const  { return mBloomStrength; }

    float bloomRamp() const  { return mBloomRamp; }

    void render2DQuad(float width, float height) const;
    void setupOrthoProjection(int width, int height) const;
    void activateRenderTarget(RenderTarget* target) const;
    void deactivateRenderTarget(RenderTarget* target) const;

    /**
     * Sets the data path where HdrGLWidget will look for shaders.
     **/
    void setDataPath(const QString& path);

public Q_SLOTS:
    /**
     * Activates or deactivates HDR rendering. Note that if HDR rendering
     *  isn't supported on current hardware then it has no effect and HDR
     *  rendering will never be active.
     **/
    void setHdrRenderingActive(bool active)  { mHdrRenderingActive = active; }
    /**
     * Set current exposure, used for tonemapping. Default exposure is 1.0
     *
     * Note that this has no effect when automatic exposure adjustment has been
     *  enabled (see @ref setAutoExposure).
     **/
    void setExposure(float e)  { mExposure = e; }
    /**
     * Enables or disables automatic exposure adjustment.
     * When enabled, exposure is automatically calculated every frame after
     *  calling @ref renderScene.
     * Automatic exposure adjustment is disabled by default.
     **/
    void setAutoExposure(bool a)  { mAutoExposure = a; }

    void setAutoExposureSpeed(float s)  { mAutoExposureSpeed = s; }
    void setAutoExposureTarget(float t)  { mAutoExposureTarget = t; }
    void setAutoExposureRange(float a, float b)  { mAutoExposureMin = a; mAutoExposureMax = b; }

    void setBloomEnabled(bool e)  { mBloomEnabled = e; }
    void setBloomAfterTonemapping(bool e)  { mBloomAfterTonemapping = e; }
    void setBloomDownsize(int d)  { mBloomDownsize = d; }
    void setBloomStrength(float s);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void render();

    virtual void renderScene();

    virtual void hdrTonemapping();
    virtual void hdrBloom();

    virtual void calculateHdrExposure();

    virtual float* calculateBlurKernel(float sigma, int radius);
    virtual Program* generateBlurProgram(float sigma, int radius, bool horizontal);
    virtual Shader* generateBlurVertexShader(float sigma, int radius, bool horizontal);
    virtual Shader* generateBlurFragmentShader(float sigma, int radius, bool horizontal);

private:
    void init();
    void initGL();

    bool mHdrRenderingSupported;
    bool mHdrRenderingActive;

    RenderTarget* mSceneRenderTarget;
    RenderTarget* mLuminanceDetectionTarget;
    RenderTarget* mTonemappingTarget;
    RenderTarget* mBloomHTarget;
    RenderTarget* mBloomVTarget;
    QString mDataPath;
    Program* mTonemappingProgram;
    Program* mBloomHProgram;
    Program* mBloomVProgram;

    float mExposure;
    bool mAutoExposure;
    float mAutoExposureSpeed;
    float mAutoExposureTarget;
    float mAutoExposureMin;
    float mAutoExposureMax;
    bool mBloomEnabled;
    bool mBloomAfterTonemapping;
    int mBloomDownsize;
    float mBloomRamp;
    float mBloomStrength;
};

}

#endif

