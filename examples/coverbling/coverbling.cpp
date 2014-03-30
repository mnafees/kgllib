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

#include "coverbling.h"

#include <QString>
#include <QDir>
#include <QtDebug>
#include <QKeyEvent>
#include <QTimer>

#include <kgllib/texture.h>
#include <kgllib/camera.h>
#include <kgllib/fpscounter.h>

using namespace std;
using namespace Eigen;
using namespace KGLLib;


// Use custom QGLFormat to use multisampling (antialiasing)
CoverBling::CoverBling() : GLWidget(QGLFormat(QGL::SampleBuffers))
{
    current = currentTarget = 0;
    setFocusPolicy(Qt::StrongFocus);
}

CoverBling::~CoverBling()
{
    // Delete cover textures
    for (int i = 0; i < covers.count(); i++) {
        delete covers[i].tex;
    }
}

void CoverBling::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Right) {
        currentTarget = qMin(currentTarget + 1.0f, covers.count()-1.0f);
    } else if (e->key() == Qt::Key_Left) {
        currentTarget = qMax(currentTarget - 1.0f, 0.0f);
    } else {
        return;
    }
    // Reset the timer so we can get proper elapsed time in paintGL()
    fpsCounter()->resetTimeElapsed();
    // Schedule the widget for repaint
    update();
}

void CoverBling::initializeGL()
{
    // Call superclass's initialization method. Among other things it
    // initializes KGLLib.
    GLWidget::initializeGL();

    // Try to find some covers
    QString coverpath = QDir::homePath() + "/.kde/share/apps/amarok/albumcovers/large/";
    QDir dir(coverpath);
    QStringList entries = dir.entryList(QDir::Files);
    qDebug() << "Found" << entries.count() << "covers";
    if (entries.count() == 0) {
        // If there are no covers then set the error text to inform the user
        // about it. The error text will be rendered by GLWidget. It will also
        // be the only thing that will be rendered, our render() method will
        // not be called when the error text is set.
        setErrorText("No covers found.\n"
                "You need to run Amarok and let it download some album covers before you can use this demo.");
        return;
    }
    // Load at most 20 covers
    int maxi = qMin(20, entries.count());
    covers.reserve(maxi);
    for (int i = 0; i < maxi; i++) {
        Cover c;
        // Cut a 1-pixel border from the image to reduce border artefacts
        QImage img = QImage(coverpath + entries[i]);
        img = img.copy(1, 1, img.width()-2, img.height()-2);
        // Create new Texture object using the loaded and cut image
        c.tex = new Texture(img);
        // Set texture's wrap mode to CLAMP. This ensures that texture won't
        // "wrap" at the borders which could create artefacts.
        c.tex->setWrapMode(GL_CLAMP);
        covers.append(c);
    }

    // Set up the camera
    camera()->setPosition(Vector3f(0, 0.5, 2));
    camera()->setLookAt(Vector3f(0, 0.5, 0));
}

void CoverBling::render()
{
    if (!covers.count()) {
        return;
    }

    // Animate current target, 200ms per transition
    float diff = fpsCounter()->timeElapsed() / 0.2f;
    if (current < currentTarget) {
        current = qMin(currentTarget, current + diff);
    } else if (current > currentTarget) {
        current = qMax(currentTarget, current - diff);
    }

    // First render the covers "normally"
    renderCovers(false);

    // Then render reflections. For this, just flip the y-axis and draw them
    // again (although slightly differently).
    glScalef(1.0, -1.0, 1.0);
    renderCovers(true);

    // If we're in the middle of animation, schedule a repaint.
    if (current != currentTarget) {
        QTimer::singleShot(20, this, SLOT(update()));
    }
}

void CoverBling::renderCovers(bool reflection)
{
    for (int i = 0; i < covers.count(); i++) {
        // Distance between this cover and the currently highlighted one
        float d = i-current;

        // Save current modelview matrix
        glPushMatrix();
        // Move and rotate the cover, depending on it's distance from the
        //  currently highlighted one.
        float xtrans = d/3 + ((d > 0.0f) ? qMin(d/2, 1.0f) : qMax(d/2, -1.0f));
        float rot = -60 * ((d > 0.0f) ? qMin(d, 1.0f) : qMax(d, -1.0f));
        glTranslatef(xtrans, 0, 0);
        glRotatef(rot, 0, 1, 0);

        // Render it
        renderCover(covers[i], reflection);

        // Restore the modelview matrix
        glPopMatrix();
    }
}

void CoverBling::renderCover(const Cover& c, bool reflection)
{
    // Calculate alphas of the bottom and upper edges of the cover. This is used
    // to make the reflection fade out.
    float color1 = reflection ? 0.3 : 1.0;
    float color2 = reflection ? 0.0 : 1.0;

    // Enable te cover texture. This both enables texturing and binds the
    // texture (so that it will be actually used for rendering).
    c.tex->enable();
    // Render the cover. We could also use a Mesh object to do the rendering,
    // but for simple objects it's sometimes easier to do it manually.
    // The cover will be a simple quad, so use GL_QUADS mode.
    glBegin(GL_QUADS);
        // Bottom edge
        glColor3f(color1, color1, color1);
        glTexCoord2f(0, 0); glVertex3f(-0.5, 0.0, 0);
        glTexCoord2f(1, 0); glVertex3f( 0.5, 0.0, 0);
        // Upper edge
        glColor3f(color2, color2, color2);
        glTexCoord2f(1, 1); glVertex3f( 0.5,  1.0, 0);
        glTexCoord2f(0, 1); glVertex3f(-0.5,  1.0, 0);
    glEnd();
}
