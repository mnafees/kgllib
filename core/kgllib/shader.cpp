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

#include "shader.h"

#include <QFile>
#include <QtDebug>

namespace KGLLib
{

Shader::Shader(GLenum type)
{
    init(type);
}

Shader::Shader(GLenum type, const QString& filename)
{
    init(type);

    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly)) {
        qCritical() << "Shader::Shader(QString): Can't open file" << filename << "for reading";
        return;
    }
    setSource(f.readAll());
    compile();
}

Shader::~Shader()
{
    glDeleteShader(glId());
    delete[] mCompileLog;
}

void Shader::init(GLenum type)
{
    mType = type;
    mValid = false;
    mCompiled = false;
    mCompileLog = false;
    mGLId = glCreateShader(mType);
}

void Shader::setSource(const QString& source)
{
    setSource(source.toLatin1());
}

void Shader::setSource(const QByteArray& source)
{
    if (isCompiled()) {
        qCritical() << "Shader::setSource(): Can't set source for compiled shaders";
        return;
    }
    const char* src = source.data();
    glShaderSource(glId(), 1, &src, NULL);
}

bool Shader::compile()
{
    if (isCompiled()) {
        qCritical() << "Shader::compile(): Can't compile a shader twice";
        return false;
    }
    // Compile the shader
    glCompileShader(glId());
    mCompiled = true;
    // Make sure it compiled correctly
    GLint compiled;
    glGetShaderiv(glId(), GL_COMPILE_STATUS, &compiled);
    mValid = compiled;
    // Get info log
    GLsizei logsize, logarraysize;
    glGetShaderiv(glId(), GL_INFO_LOG_LENGTH, &logarraysize);
    mCompileLog = new char[logarraysize];
    glGetShaderInfoLog(glId(), logarraysize, &logsize, mCompileLog);
    // Output the log if compilation failed
    if (!mValid) {
        qCritical() << "Shader::compile(): Couldn't compile shader. Log follows:" << endl << mCompileLog;
    }
    if (!logsize) {
        delete[] mCompileLog;
        mCompileLog = 0;
    }
    return mValid;
 }

VertexShader::VertexShader() : Shader(GL_VERTEX_SHADER)
{
}

VertexShader::VertexShader(const QString& filename) : Shader(GL_VERTEX_SHADER, filename)
{
}

FragmentShader::FragmentShader() : Shader(GL_FRAGMENT_SHADER)
{
}

FragmentShader::FragmentShader(const QString& filename) : Shader(GL_FRAGMENT_SHADER, filename)
{
}


}

