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

#include "logos.h"

#include <kgllib/texture.h>
#include <kgllib/fpscounter.h>
#include <kgllib/mesh.h>

#include <QTimer>

#include <Eigen/Geometry>

using namespace KGLLib;
using namespace Eigen;


// Vertex and texcoord arrays for a logo. We'll create a Mesh object using
// those arrays
static Vector3f logoShapeVertices[] =
    { Vector3f(-1.0f, -1.0f, 0.0f), Vector3f( 1.0f, -1.0f, 0.0f),
      Vector3f( 1.0f,  1.0f, 0.0f), Vector3f(-1.0f,  1.0f, 0.0f) };
static Vector2f logoShapeTexcoords[] =
    { Vector2f(0.0f, 0.0f), Vector2f(1.0f, 0.0f), Vector2f(1.0f, 1.0f), Vector2f(0.0f, 1.0f) };
static int logoShapeVertexCount = 4;


Logos::Logos() : GLWidget()
{
    mRotation = 0;
    mKDELogoTex = mQtLogoTex = 0;
    mKDELogoMesh = mQtLogoMesh = 0;
}

Logos::~Logos()
{
    delete mKDELogoTex;
    delete mQtLogoTex;
    delete mKDELogoMesh;
    delete mQtLogoMesh;
}

void Logos::initializeGL()
{
    GLWidget::initializeGL();

    // Load logo textures
    mKDELogoTex = new Texture(":/kde.png");
    mQtLogoTex = new Texture(":/qt.png");
    // If either of the logos couldn't be loaded then show an error
    if (!mKDELogoTex->isValid() || !mQtLogoTex->isValid()) {
        return setErrorText("Data couldn't be loaded");
    }
    // We don't want the textures to repeat (tile), so set a GL_CLAMP wrap mode
    //  for them.
    mKDELogoTex->setWrapMode(GL_CLAMP);
    mQtLogoTex->setWrapMode(GL_CLAMP);

    // This cuts away surfaces that doesn't face the camera (called backfacing
    //  surfaces). This way we can render both logos at the same place without
    //  worrying about them intersecting with each other
    glEnable(GL_CULL_FACE);

    // Enable blending so that transparent parts of the logo blend with the background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use yellow background
    setClearColor(Vector4f(1.0, 1.0, 0.0, 0.0));

    // Create the logo meshes
    mKDELogoMesh = createLogo(mKDELogoTex);
    mQtLogoMesh = createLogo(mQtLogoTex);
}

Mesh* Logos::createLogo(Texture* tex)
{
    // Create new mesh, using the vertex and texcoord arrays defined at the
    // beginning of the file.
    Mesh* logo = new Mesh();
    logo->setVertexCount(logoShapeVertexCount);
    logo->setVertices(logoShapeVertices);
    logo->setTexcoords(logoShapeTexcoords);
    logo->setPrimitiveType(GL_QUADS);
    logo->setTexture(tex);
    return logo;
}

void Logos::render()
{
    // Rotate 20 degrees/sec
    mRotation += fpsCounter()->timeElapsed() * 20;
    glEnable(GL_BLEND);

    // Rotate around y-axis and draw KDE logo
    glRotatef(mRotation, 0, 1, 0);
    // Both lines do the same
    // renderLogo(mKDELogoTex);
    mKDELogoMesh->render();

    // Rotate another 180 degrees and draw Qt logo (so that it will be facing the other way
    glRotatef(180, 0, 1, 0);
    // Both lines do the same
    // renderLogo(mQtLogoTex);
    mQtLogoMesh->render();

    // Repaint for animation
    QTimer::singleShot(20, this, SLOT(update()));
}

void Logos::renderLogo(KGLLib::Texture* tex)
{
    // This both enables texturing as well as binds the texture.
    tex->enable();
    // Now render the logo
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1, -1, 0);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1, -1, 0);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1,  1, 0);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1,  1, 0);
    glEnd();
    // And finally disable texturing.
    tex->disable();
}


#include "logos.moc"
