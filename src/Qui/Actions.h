#ifndef QUI_ACTIONS_H
#define QUI_ACTIONS_H

/*!
 *  \file Actions.h 
 *
 *  \brief Declarations for custom Action functions.
 *   
 *  \author Andrew Gilbert
 *  \date August 2008
 */


#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include "Logic.h"

class QToolBox;

namespace Qui {


inline Action Disable(QWidget* widget) 
{
   return std::bind(&QWidget::setEnabled, widget, false);
}

inline Action Enable(QWidget* widget) 
{
   return std::bind(&QWidget::setEnabled, widget, true);
}


inline Action Hide(QWidget* widget) 
{
   return std::bind(&QWidget::hide, widget);
}

inline Action Show(QWidget* widget) 
{
   return std::bind(&QWidget::show, widget);
}


inline Action SetValue(QSpinBox* widget, int value ) 
{
   return std::bind(&QSpinBox::setValue, widget, value);
}


inline Action SetLabel(QLabel* label, QString const& text) 
{
   return std::bind(&QLabel::setText, label, text);
}



void RemoveToolBoxPages(QToolBox* toolBox, QStringList const& pageNames);

void AddToolBoxPage(QToolBox* toolBox, QWidget* page, QString const& pageName);


inline Action RemovePage(QToolBox* toolBox, QString const& pageName)
{
   QStringList pageNames;
   pageNames << pageName;
   return std::bind(&RemoveToolBoxPages, toolBox, pageNames);
}


inline Action RemovePages(QToolBox* toolBox, QStringList const& pageNames)
{
   return std::bind(&RemoveToolBoxPages, toolBox, pageNames);
}


inline Action AddPage(QToolBox* toolBox, QWidget* page, QString const& pageName)
{
   return std::bind(&AddToolBoxPage, toolBox, page, pageName);
}

} // end namespace Qui
#endif
