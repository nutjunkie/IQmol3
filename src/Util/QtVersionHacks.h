/**************************************************************************
**
** This file is part of QMsgBox
**
** QMsgBox is free software. You can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the license, or
** (at you option) any later version.
**
** QMsgBox is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with QMsgBox. If not, set <http://www.gnu.org/licenses/>.
**
** Copyleft (c) 2009 Gustavo Ribeiro Croscato
** Contact: croscato (at) gmail (dot) com
**
**************************************************************************/

#ifndef IQMOL_UTIL_QTVERSION_HACKS
#define IQMOL_UTIL_QTVERSION_HACKS

#include <QString>
#include <QtGlobal>

namespace IQmol {

#if QT_VERSION < 0x051500
#define IQmolSkipEmptyParts  QString::SkipEmptyParts
#else
#define IQmolSkipEmptyParts  Qt::SkipEmptyParts
#endif

}

#endif
