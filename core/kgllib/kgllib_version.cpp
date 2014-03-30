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

#include "kgllib_version.h"

namespace KGLLib
{

unsigned int version()
{
    return KGLLIB_VERSION;
}

unsigned int versionMajor()
{
    return KGLLIB_VERSION_MAJOR;
}

unsigned int versionMinor()
{
    return KGLLIB_VERSION_MINOR;
}

unsigned int versionRelease()
{
    return KGLLIB_VERSION_RELEASE;
}

bool isVersion(unsigned int a, unsigned int b, unsigned int c)
{
    return KGLLIB_IS_VERSION(a, b, c);
}

const char* versionString()
{
    return KGLLIB_VERSION_STRING;
}

}
