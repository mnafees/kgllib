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

#ifndef KGLLIB_EXPORT_H
#define KGLLIB_EXPORT_H

#include <QtGlobal>


#ifndef KGLLIB_EXPORT
# if defined(MAKE_KGLLIB_LIB)
#  define KGLLIB_EXPORT Q_DECL_EXPORT
# else
#  define KGLLIB_EXPORT Q_DECL_IMPORT
# endif
#endif

#ifndef KGLLIB_EXTRAS_EXPORT
# if defined(MAKE_KGLLIB_EXTRAS_LIB)
#  define KGLLIB_EXTRAS_EXPORT Q_DECL_EXPORT
# else
#  define KGLLIB_EXTRAS_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif
