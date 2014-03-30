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

#ifndef LOGOS_H
#define LOGOS_H

#include <kgllib/glwidget.h>

namespace KGLLib
{
    class Texture;
    class Mesh;
}


class Logos : public KGLLib::GLWidget
{
    Q_OBJECT
public:
    Logos();
    ~Logos();

protected:
    void initializeGL();
    void render();

    KGLLib::Mesh* createLogo(KGLLib::Texture* tex);
    void renderLogo(KGLLib::Texture* tex);

private:
    KGLLib::Texture* mKDELogoTex;
    KGLLib::Texture* mQtLogoTex;
    float mRotation;
    KGLLib::Mesh* mKDELogoMesh;
    KGLLib::Mesh* mQtLogoMesh;
};

#endif
