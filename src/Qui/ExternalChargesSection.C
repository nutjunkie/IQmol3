/*!
 *  \file ExternalChargesSection.C 
 *
 *  \author Andrew Gilbert
 *  \date February 2008
 */

#include "ExternalChargesSection.h"
#include "Qui.h"

#include <QFile>
#include <QtDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QRegularExpression>


namespace Qui {

void ExternalChargesSection::processContents() 
{
   int numberOfCharges = m_contents.count(QRegularExpression("\\n"));

   // This is a hack switch to undo the charge truncation
   if (numberOfCharges > 0) {
 //if (numberOfCharges < 10) {
      m_truncatedData = m_contents;

   }else {
      int n(0);

      for (int i = 0; i < 5; ++i) {
         n = m_contents.indexOf("\n",n+1);
      }
      m_truncatedData = m_contents.left(n+1);

      m_truncatedData += "< ... +";
      m_truncatedData += QString::number(numberOfCharges-5);
      m_truncatedData += " additional charges ... >";
   }
}


void ExternalChargesSection::read(QString const& data) 
{
    m_contents = data.trimmed();
    if (m_contents.isEmpty()) m_visible = false;
    processContents();
}


ExternalChargesSection* ExternalChargesSection::clone() const 
{
   return new ExternalChargesSection(m_contents, m_visible);
}


QString ExternalChargesSection::previewContents() const 
{
   return m_truncatedData.isEmpty() ? m_truncatedData: m_truncatedData + "\n";
}

} // end namespace Qui

