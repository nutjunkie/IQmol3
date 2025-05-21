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

#ifndef QMSGBOX_H
#define QMSGBOX_H

#include <QMessageBox>
#include <QApplication>

class QMsgBox: public QMessageBox {
	Q_OBJECT

public:
	static StandardButton warning(QWidget* parent, const QString& title,
		const QString& text, StandardButtons buttons = Ok,
		StandardButton defaultButton = NoButton);

	static StandardButton critical(QWidget* parent, const QString& title,
			const QString& text, StandardButtons buttons = Ok,
			StandardButton defaultButton = NoButton);

	static StandardButton information(QWidget* parent, const QString& title,
		const QString& text, StandardButtons buttons = Ok,
			StandardButton defaultButton = NoButton);

	static StandardButton question(QWidget* parent, const QString& title,
		const QString& text, StandardButtons buttons = Ok | Cancel,
			StandardButton defaultButton = NoButton);


	static StandardButton warning(
        const QString& text, 
        QWidget* parent = QApplication::activeWindow(),
        const QString& title = QString("IQmol"), 
        StandardButtons buttons = Ok,
		StandardButton defaultButton = NoButton)
    {
        return warning(parent, title, text, buttons, defaultButton);
    }

	static StandardButton information(
        const QString& text, 
        QWidget* parent = QApplication::activeWindow(),
        const QString& title = QString("IQmol"), 
        StandardButtons buttons = Ok,
		StandardButton defaultButton = NoButton)
    {
         return information(parent, title, text, buttons, defaultButton);
    }

	static StandardButton critical(
        const QString& text, 
        QWidget* parent = QApplication::activeWindow(),
        const QString& title = QString("IQmol"), 
        StandardButtons buttons = Ok,
		StandardButton defaultButton = NoButton)
    {
        return critical(parent, title, text, buttons, defaultButton);
    }

	static StandardButton question(
        const QString& text, 
        QWidget* parent = QApplication::activeWindow(),
        const QString& title = QString("IQmol"), 
        StandardButtons buttons = Ok,
		StandardButton defaultButton = NoButton)
    {
        return question(parent, title, text, buttons, defaultButton);
    }
};

#endif // QMSGBOX_H
