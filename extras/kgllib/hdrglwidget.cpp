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

#include "hdrglwidget.h"

#include "rendertarget.h"
#include "program.h"
#include "shader.h"
#include "texture.h"
#include "fpscounter.h"
#include <Eigen/Core>

#include <QFile>
#include <QDebug>

using namespace Eigen;

namespace KGLLib
{

HdrGLWidget::HdrGLWidget(QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f) :
    GLWidget(parent, shareWidget, f)
{
    init();
}

HdrGLWidget::HdrGLWidget(QGLContext* context, QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f) :
    GLWidget(context, parent, shareWidget, f)
{
    init();
}

HdrGLWidget::HdrGLWidget(const QGLFormat& format, QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f) :
    GLWidget(format, parent, shareWidget, f)
{
    init();
}

HdrGLWidget::~HdrGLWidget()
{
    delete mLuminanceDetectionTarget;
    delete mSceneRenderTarget;
}

void HdrGLWidget::init()
{
    mSceneRenderTarget = 0;
    mLuminanceDetectionTarget = 0;
    mTonemappingTarget = 0;
    mBloomHTarget = 0;
    mBloomVTarget = 0;
    mBloomVProgram = 0;
    mBloomHProgram = 0;
    mTonemappingProgram = 0;

    mHdrRenderingSupported = false;
    mDataPath = '.';

    mHdrRenderingActive = true;
    mExposure = 1.0f;
    mAutoExposure = false;
    mAutoExposureTarget = 0.3;
    mAutoExposureSpeed = 0.5;
    mAutoExposureMin = 0.25;
    mAutoExposureMax = 4.0;
    mBloomEnabled = true;
    mBloomAfterTonemapping = false;
    mBloomDownsize = 2;
    mBloomStrength = 1.0;
}

void HdrGLWidget::setDataPath(const QString& path)
{
    mDataPath = path;
}

void HdrGLWidget::setBloomStrength(float s)
{
    mBloomStrength = s;

    if (mBloomVProgram) {
        mBloomVProgram->bind();
        mBloomVProgram->setUniform("strength", mBloomStrength);
        mBloomVProgram->unbind();
    }
}

void HdrGLWidget::initializeGL()
{
    GLWidget::initializeGL();

    initGL();
}

void HdrGLWidget::initGL()
{
    // Check for hardware support
    mHdrRenderingSupported = false;
    if (!GLEW_VERSION_2_0) {
        qDebug() << "No OpenGL 2.0";
        return;
    }

    QString missing;
    if (!RenderTarget::isSupported()) {
        qDebug() << "Rendertargets aren't supported";
        return;
    }
    if (!GLEW_ARB_texture_float) {
        qDebug() << "Floating-point textures aren't supported";
        return;
    }

    if (GLEW_ARB_color_buffer_float) {
        // TODO: this doesn't seem to be supported on ATI cards - would be nice
        //  if there was another rendering path for those (e.g. downscale
        //  using shaders)
        glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
        glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
        glClampColorARB(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);
    }

    // Load data
    // Tonemapping program
    mTonemappingProgram = new Program(mDataPath + "/tonemapping.vert", mDataPath + "/tonemapping.frag");
    if (!mTonemappingProgram->isValid()) {
        qDebug() << "Couldn't load tonemapping program";
        delete mTonemappingProgram;
        return;
    }
    mTonemappingProgram->bind();
    mTonemappingProgram->setUniform("sceneTexture", 0);
    mTonemappingProgram->unbind();

    mBloomHProgram = generateBlurProgram(3.0, 6, true);
    mBloomVProgram = generateBlurProgram(3.0, 6, false);

    // Everything succeeded, HDR rendering is supported
    mHdrRenderingSupported = true;
}

void HdrGLWidget::resizeGL(int width, int height)
{
    GLWidget::resizeGL(width, height);

    // Delete all current render targets, they'll be recreated lazyly
    delete mSceneRenderTarget;
    mSceneRenderTarget = 0;
    delete mTonemappingTarget;
    mTonemappingTarget = 0;
    delete mBloomHTarget;
    mBloomHTarget = 0;
    delete mBloomVTarget;
    mBloomVTarget = 0;
}

void HdrGLWidget::render()
{
    // If HDR rendering isn't active, just render the scene as usual and return
    if (!hdrRenderingActive()) {
        renderScene();
        return;
    }

    // Render the scene
    if (!mSceneRenderTarget || mSceneRenderTarget->size() != size()) {
        delete mSceneRenderTarget;
        mSceneRenderTarget = new RenderTarget(width(), height(), true, GL_RGBA16F_ARB);
    }
    activateRenderTarget(mSceneRenderTarget);
    if (automaticClear()) {
        setClearColor(clearColor());
        if (context()->format().depth()) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } else {
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }

    renderScene();
    deactivateRenderTarget(mSceneRenderTarget);

    // Generate mipmaps for scene target if necessary
   if (mAutoExposure || (mBloomEnabled && !mBloomAfterTonemapping && mBloomDownsize > 1)) {
        // Generate mipmaps for the scene texture and set mipmapping filter
        mSceneRenderTarget->texture()->enable();
        glGenerateMipmapEXT(GL_TEXTURE_2D);
        mSceneRenderTarget->texture()->setFilter(GL_LINEAR_MIPMAP_LINEAR);
        mSceneRenderTarget->texture()->disable();
   }

    // Calculate exposure if necessary
    if (mAutoExposure) {
        calculateHdrExposure();
    }

    if (mBloomEnabled && !mBloomAfterTonemapping) {
        // Do the bloom before tonemapping
        hdrBloom();
    }

    // Do the tonemapping
    if (mBloomEnabled && mBloomAfterTonemapping) {
        // When we do bloom after tonemapping, then tonemapping's results need
        //  to go to their own rendertarget
        if (!mTonemappingTarget || mTonemappingTarget->size() != size()) {
            delete mTonemappingTarget;
            mTonemappingTarget = new RenderTarget(width(), height(), false, GL_RGBA16F_ARB);
        }
        activateRenderTarget(mTonemappingTarget);
        hdrTonemapping();
        deactivateRenderTarget(mTonemappingTarget);

        // Copy tonemapping's results onto screen
        mTonemappingTarget->texture()->enable();
        setupOrthoProjection(width(), height());
        render2DQuad(width(), height());

        if (mBloomDownsize > 1) {
            // Also generate mipmaps for tonemapping target, they'll be used by
            //  bloom
            glGenerateMipmapEXT(GL_TEXTURE_2D);
            mTonemappingTarget->texture()->setFilter(GL_LINEAR_MIPMAP_LINEAR);
        }
        mTonemappingTarget->texture()->disable();

        // We'll use additive blending to combine results of tonemapping and bloom
        hdrBloom();
    } else {
        // No bloom here, just tonemapping and results go directly onto the screen.
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        hdrTonemapping();
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void HdrGLWidget::calculateHdrExposure()
{
    // Create luminance detection rendertarget, if it hasn't been created yet
    const int luminanceDetectionTargetSize = 8;
    if (!mLuminanceDetectionTarget) {
        mLuminanceDetectionTarget = new RenderTarget(luminanceDetectionTargetSize, luminanceDetectionTargetSize,
                                                     false, GL_RGBA16F_ARB);
    }

    activateRenderTarget(mLuminanceDetectionTarget);
    // Render scene onto the small scene target
    mSceneRenderTarget->texture()->enable();
    setupOrthoProjection(luminanceDetectionTargetSize, luminanceDetectionTargetSize);
    render2DQuad(luminanceDetectionTargetSize, luminanceDetectionTargetSize);
    mSceneRenderTarget->texture()->disable();

    // Read from FBO's color texture
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    // Read FBO's contents into a buffer
    float* buffer = new float[luminanceDetectionTargetSize * luminanceDetectionTargetSize * 4];
    glReadPixels(0, 0, luminanceDetectionTargetSize, luminanceDetectionTargetSize, GL_RGBA, GL_FLOAT, buffer);

    // Determine average luminance of the scene
    Vector3d sum(0, 0, 0);
    for (int i = 0; i < luminanceDetectionTargetSize * luminanceDetectionTargetSize; i++) {
        sum += Vector3d(buffer[i*4 + 0], buffer[i*4 + 1], buffer[i*4 + 2]);
    }
    Vector3d avg = sum / (luminanceDetectionTargetSize * luminanceDetectionTargetSize);
    float avgluminance = avg.dot(Vector3d(0.30, 0.59, 0.11));
    // Set exposure
    float targetexposure = mAutoExposureTarget / avgluminance;
    float oldc = powf(mAutoExposureSpeed, fpsCounter()->timeElapsed());
    float e = exposure() * oldc + targetexposure * (1-oldc);
    setExposure(qBound(mAutoExposureMin, e, mAutoExposureMax));

    // Clean up
    delete[] buffer;
    deactivateRenderTarget(mLuminanceDetectionTarget);
}

void HdrGLWidget::hdrTonemapping()
{
    // Bind scene texture
    glActiveTexture(GL_TEXTURE0);
    mSceneRenderTarget->texture()->enable();
    // Bind tonemapping shader
    mTonemappingProgram->bind();
    mTonemappingProgram->setUniform("exposure", mExposure);
    // Render a quad
    setupOrthoProjection(width(), height());
    render2DQuad(width(), height());
    // Unbind everything
    mTonemappingProgram->unbind();
    glActiveTexture(GL_TEXTURE0);
    mSceneRenderTarget->texture()->disable();
}

void HdrGLWidget::hdrBloom()
{
    // The source rendertarget which we'll blur
    RenderTarget* sourceTarget = mBloomAfterTonemapping ? mTonemappingTarget : mSceneRenderTarget;

    // Recreate bloom target if necessary
    int blurw = width()/mBloomDownsize;
    int blurh = height()/mBloomDownsize;
    if (!mBloomHTarget || mBloomHTarget->size() != QSize(blurw, blurh)) {
        delete mBloomHTarget;
        mBloomHTarget = new RenderTarget(blurw, blurh, false, GL_RGBA16F_ARB);
        // Use linear filtering
        mBloomHTarget->texture()->bind();
        mBloomHTarget->texture()->setFilter(GL_LINEAR);
        mBloomHTarget->texture()->disable();
        // Update texture size variables in programs
        mBloomHProgram->bind();
        mBloomHProgram->setUniform("inverseTextureSize", Vector2f(1.0f / blurw, 1.0f / blurh));
        mBloomHProgram->unbind();
        mBloomVProgram->bind();
        mBloomVProgram->setUniform("inverseTextureSize", Vector2f(1.0f / blurw, 1.0f / blurh));
        mBloomVProgram->unbind();
    }

    // First the horizontal blur pass
    activateRenderTarget(mBloomHTarget);
    // Bind scene texture
    glActiveTexture(GL_TEXTURE0);
    sourceTarget->texture()->enable();
    // Bind tonemapping shader
    mBloomHProgram->bind();
    if (!mBloomAfterTonemapping) {
        const float rampAfterTonemapping = 0.7;
        mBloomRamp = -log(1 - rampAfterTonemapping) / exposure();
    } else {
        mBloomRamp = 0.8;
    }
    mBloomHProgram->setUniform("ramp", mBloomRamp);
    // Render a quad
    setupOrthoProjection(blurw, blurh);
    render2DQuad(blurw, blurh);
    // Unbind everything
    mBloomHProgram->unbind();
    glActiveTexture(GL_TEXTURE0);
    sourceTarget->texture()->disable();
    deactivateRenderTarget(mBloomHTarget);


    // Then the vertical blur pass, results of which go directly onto screen
    //  (with additive blending)
    if (mBloomDownsize != 1) {
        // When bloom texture is downsized, we can't render directly onto
        //  screen, so we must use another rendertarget
        if (!mBloomVTarget || mBloomVTarget->size() != QSize(blurw, blurh)) {
            delete mBloomVTarget;
            mBloomVTarget = new RenderTarget(blurw, blurh, false, GL_RGBA16F_ARB);
            // Use linear filtering
            mBloomVTarget->texture()->bind();
            mBloomVTarget->texture()->setFilter(GL_LINEAR);
            mBloomVTarget->texture()->disable();
        }
        activateRenderTarget(mBloomVTarget);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
    }
    // Bind scene texture
    glActiveTexture(GL_TEXTURE0);
    mBloomHTarget->texture()->enable();
    // Bind tonemapping shader
    mBloomVProgram->bind();
    mBloomVProgram->setUniform("strength", mBloomStrength);
    // Render a quad
    setupOrthoProjection(blurw, blurh);
    render2DQuad(blurw, blurh);
    // Unbind everything
    mBloomVProgram->unbind();
    glActiveTexture(GL_TEXTURE0);
    mBloomHTarget->texture()->disable();

    if (mBloomDownsize != 1) {
        deactivateRenderTarget(mBloomVTarget);
        // Copy blur results onto screen
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        mBloomVTarget->texture()->enable();
        setupOrthoProjection(width(), height());
        render2DQuad(width(), height());
        mBloomVTarget->texture()->disable();
    }
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void HdrGLWidget::renderScene()
{
}

void HdrGLWidget::render2DQuad(float width, float height) const
{
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, 0.0);
        glTexCoord2f(1.0f, 0.0);
        glVertex2f(width, 0.0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(width, height);
        glTexCoord2f(0.0, 1.0f);
        glVertex2f(0.0, height);
    glEnd();
}

void HdrGLWidget::setupOrthoProjection(int width, int height) const
{
    // Set up ortho projection, clear buffers
    glViewport(0, 0, width, height);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // This is same as  gluOrtho2D(0, width, 0, height);
    glOrtho(0, width, 0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void HdrGLWidget::activateRenderTarget(RenderTarget* target) const
{
    // Enable the rendertarget
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    target->enable();

    // Load viewport
    glViewport(0, 0, target->texture()->width(), target->texture()->height());
}

void HdrGLWidget::deactivateRenderTarget(RenderTarget* target) const
{
    target->disable();
    glPopAttrib();
}

float* HdrGLWidget::calculateBlurKernel(float sigma, int radius)
{
    float* kernel = new float[radius+1];
    kernel[0] = 1.0;
    float sum = 1.0;
    for (int i = 1; i <= radius; i++) {
        kernel[i] = exp(-(i*i) / (2*sigma*sigma));
        sum += kernel[i];
    }
    for (int i = 0; i <= radius; i++) {
        kernel[i] = kernel[i] / sum;
    }

    return kernel;
}

Program* HdrGLWidget::generateBlurProgram(float sigma, int radius, bool horizontal)
{
    Shader* vert = generateBlurVertexShader(sigma, radius, horizontal);
    Shader* frag = generateBlurFragmentShader(sigma, radius, horizontal);
    if (!vert || !frag) {
        return 0;
    }
    QList<Shader*> shaders;
    shaders << vert << frag;
    Program* prog = new Program(shaders);
    if (!prog->isValid()) {
        qCritical() << "Invalid program";
        delete prog;
        return 0;
    }
    prog->bind();
    prog->setUniform("ramp", horizontal ? 0.8f : 0.0f);
    prog->setUniform("inputTexture", 0);
    prog->setUniform("strength", horizontal ? 1.0f : mBloomStrength);
    prog->unbind();
    return prog;
}

Shader* HdrGLWidget::generateBlurVertexShader(float sigma, int radius, bool horizontal)
{
    Shader* s = new VertexShader(mDataPath + "/blur.vert");
    if (!s->isValid()) {
        qCritical() << "Invalid shader";
        delete s;
        return 0;
    }
    return s;
}

Shader* HdrGLWidget::generateBlurFragmentShader(float sigma, int radius, bool horizontal)
{
    QFile f(mDataPath + "/blur.frag");
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "Can't open blur.frag for reading";
        return 0;
    }
    QByteArray source;
    if (horizontal) {
        source += "#define BLURDIRECTION vec2(1.0, 0.0)\n";
    } else {
        source += "#define BLURDIRECTION vec2(0.0, 1.0)\n";
    }
    source += "#define BLURCODE ";
    float* kernel = calculateBlurKernel(sigma, radius);
    for (int r = -radius; r <= radius; r++) {
        source += QString(" \\\n    result += sampleAtOffset(%1.0) * %2;").arg(r).arg(kernel[qAbs(r)]).toAscii();
    }
    delete[] kernel;
    source += "\n\n";
    source += f.readAll();
//     qDebug() << "Shader source is:\n" << source;

    FragmentShader* shader = new FragmentShader();
    shader->setSource(source);
    if (!shader->compile()) {
        qCritical() << "Shader failed to compile";
        delete shader;
        return 0;
    }
    return shader;
}

}

#include "hdrglwidget.moc"
