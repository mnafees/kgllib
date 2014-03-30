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

#include <fpscounter.h>

namespace KGLLib
{

FPSCounter::FPSCounter()
{
    mFPS = 0;
    mFrames = -1;
    resetTimeElapsed();
    mTotalTimeElapsed = 0;
}

void FPSCounter::resetTimeElapsed()
{
    mTimeElapsed = 0;
    mTotalTimeElapsed += mLastTime.elapsed() / 1000.0f;
    mLastTime.restart();
}

void FPSCounter::nextFrame()
{
    mFrames++;
    mTimeElapsed = mLastTime.restart() / 1000.0f;
    mTotalTimeElapsed += mTimeElapsed;
    if (mFrames == 0) {
        mTime.start();
    } else if (mTime.elapsed() > 1000) {
        mFPS = mFrames / (mTime.elapsed() / 1000.0f);
        mTime.restart();
        mFrames = 0;
    }
}

QString FPSCounter::fpsString() const
{
    if (fps() > 100) {
        return QString("%1").arg((int)fps());
    } else {
        return QString("%1").arg(fps(), 0, 'g', 3);
    }
}

}

