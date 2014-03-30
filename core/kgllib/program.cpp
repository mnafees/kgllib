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

#include "program.h"

#include <shader.h>
#include "renderer.h"

#include <qlist.h>
#include <qstring.h>
#include <qhash.h>
#include <QtDebug>

namespace KGLLib
{

Program::Program()
{
    init();
}

Program::Program(const QList<Shader*>& shaders)
{
    init();
    addShaders(shaders);
    link();
}

Program::Program(const QString& vertexshaderfile, const QString& fragmentshaderfile)
{
    init();
    // Load both shaders, even if one is invalid. This way developer can see
    //  any errors/warnings for both shaders at once.
    VertexShader vs(vertexshaderfile);
    FragmentShader fs(fragmentshaderfile);
    if (!vs.isValid() || !fs.isValid()) {
        return;
    }
    addShader(&vs);
    addShader(&fs);
    link();
}

Program::~Program()
{
    glDeleteProgram(glId());
    delete[] mLinkLog;
    delete mUniformLocations;
    delete mAttributeLocations;
}

void Program::init()
{
    // Create program object
    mGLId = glCreateProgram();
    mValid = false;
    mLinkLog = 0;
    mUniformLocations = mAttributeLocations = 0;
}

void Program::addShader(Shader* shader)
{
    if (!shader->isValid()) {
        qCritical() << "Program::addShader(): Cannot add invalid shader";
        return;
    }
    // Attach the shader to the program
    glAttachShader(glId(), shader->glId());
}

void Program::addShaders(const QList<Shader*>& shaders)
{
    foreach (Shader* s, shaders) {
        addShader(s);
    }
}

bool Program::link()
{
    // Link the program
    glLinkProgram(glId());
    // Make sure it linked correctly
    GLint linked;
    glGetProgramiv(glId(), GL_LINK_STATUS, &linked);
    mValid = linked;
    // Get info log
    GLsizei logsize, logarraysize;
    glGetProgramiv(glId(), GL_INFO_LOG_LENGTH, &logarraysize);
    mLinkLog = new char[logarraysize];
    glGetProgramInfoLog(glId(), logarraysize, &logsize, mLinkLog);
    if (!mValid) {
        qCritical() << "Program::link(): Couldn't link program. Log follows:" << endl << mLinkLog;
    } else {
        mUniformLocations = new QHash<QString, int>;
        mAttributeLocations = new QHash<QString, int>;
    }
    if (!logsize) {
        delete[] mLinkLog;
        mLinkLog = 0;
    }
    return mValid;
}

void Program::bind() const
{
    renderer->bindProgram(this);
}

void Program::unbind() const
{
    renderer->bindProgram(0);
}

int Program::uniformLocation(const QString& name)
{
    return uniformLocation(name.toLatin1().data());
}

int Program::uniformLocation(const char* name)
{
    if (!mValid) {
        return -1;
    }
    if (!mUniformLocations->contains(name)) {
        int location = glGetUniformLocation(glId(), name);
        mUniformLocations->insert(name, location);
    }
    return mUniformLocations->value(name);
}

int Program::attributeLocation(const QString& name)
{
    return attributeLocation(name.toLatin1().data());
}

int Program::attributeLocation(const char* name)
{
    if (!mValid) {
        return -1;
    }
    if (!mAttributeLocations->contains(name)) {
        int location = glGetAttribLocation(glId(), name);
        mAttributeLocations->insert(name, location);
    }
    return mAttributeLocations->value(name);
}

void Program::invalidateLocations()
{
    mUniformLocations->clear();
    mAttributeLocations->clear();
}

bool Program::setUniform(const char* name, float value)
{
    int location = uniformLocation(name);
    if (location >= 0) {
        glUniform1f(location, value);
    }
    return (location >= 0);
}

bool Program::setUniform(const char* name, Eigen::Vector2f value)
{
    int location = uniformLocation(name);
    if (location >= 0) {
        glUniform2fv(location, 1, value.data());
    }
    return (location >= 0);
}

bool Program::setUniform(const char* name, Eigen::Vector3f value)
{
    int location = uniformLocation(name);
    if (location >= 0) {
        glUniform3fv(location, 1, value.data());
    }
    return (location >= 0);
}

bool Program::setUniform(const char* name, Eigen::Vector4f &value)
{
    int location = uniformLocation(name);
    if (location >= 0) {
        glUniform4fv(location, 1, value.data());
    }
    return (location >= 0);
}

bool Program::setUniform(const char* name, int value)
{
    int location = uniformLocation(name);
    if (location >= 0) {
        glUniform1i(location, value);
    }
    return (location >= 0);
}

}

