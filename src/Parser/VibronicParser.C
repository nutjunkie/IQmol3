/*******************************************************************************
       
  Copyright (C) 2022 Andrew Gilbert
           
  This file is part of IQmol, a free molecular visualization program. See
  <http://iqmol.org> for more details.
       
  IQmol is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  IQmol is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.
      
  You should have received a copy of the GNU General Public License along
  with IQmol.  If not, see <http://www.gnu.org/licenses/>.  
   
********************************************************************************/

#include "VibronicParser.h"
#include "TextStream.h"
#include <QDir>
#include <QDebug>


namespace IQmol {
namespace Parser {


bool VibronicDir::parseFile(QString const& path)
{
   QDir dir(path);
   QFileInfoList fileList(dir.entryInfoList(QDir::Files | QDir::Readable));

   m_vibronic = new Data::Vibronic;

   QFileInfoList::iterator iter;
   for (iter = fileList.begin(); iter != fileList.end(); ++iter) {

       QFile file(iter->absoluteFilePath());

       if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
          TextStream textStream(&file);

          QString fileName(iter->fileName());
          qDebug() << "===> fileName: " << fileName;

          if (fileName.startsWith("Spec")) {
             parseSpectrumFile(textStream, fileName);

          }else if (fileName == "vibronic_spectra.log") {
             parseLogFile(textStream);

          }else if (fileName == "abs_spectra.dat") {
             parseDatFile(textStream);
          }

       }else {
          QString msg("Failed to open vibronic data file ");
          msg += iter->absoluteFilePath();
          m_errors += msg;
       }
   }

   // Need to check vibronic data is valid
   m_dataBank.append(m_vibronic);

   return true;
}


bool VibronicDir::parseSpectrumFile(TextStream& textStream, QString const& title)
{
   Data::VibronicSpectrum* vibronicSpectrum(new Data::VibronicSpectrum(title));
   QList<double> data;

   while (!textStream.atEnd()) {
      QStringList tokens(textStream.nextLineAsTokens());

      for (unsigned i = 0; i < tokens.length(); ++i) {
          bool ok;
          double x = tokens[i].toDouble(&ok); if (!ok) goto error;
          data.append(x);
      }
   }

   vibronicSpectrum->setData(data);

   m_vibronic->addSpectrum(vibronicSpectrum);
   return true;

   error:
      delete vibronicSpectrum;
      QString msg("Problem parsing vibronic spectrum file, line number ");
      m_errors.append(msg + QString::number(textStream.lineNumber()));
      return false;
}


bool VibronicDir::parseDatFile(TextStream& textStream)
{
   QList<double> fc, ht, fcht;
   double x;
   bool ok;

   // First line is a header
   QStringList tokens(textStream.nextLineAsTokens());
   if (tokens[0] != '#' || tokens.size() != 6) goto error;

   while (!textStream.atEnd()) {
      tokens = textStream.nextLineAsTokens();
      if (tokens.size() != 5) goto error;
      fc.append(  tokens[2].toDouble(&ok)); if (!ok) goto error;
      ht.append(  tokens[3].toDouble(&ok)); if (!ok) goto error;
      fcht.append(tokens[4].toDouble(&ok)); if (!ok) goto error;
   }

   m_vibronic->addSpectrum(new Data::VibronicSpectrum("FC",   fc));
   m_vibronic->addSpectrum(new Data::VibronicSpectrum("HT",   ht));
   m_vibronic->addSpectrum(new Data::VibronicSpectrum("FCHT", fcht));

   return true;

   error:
      QString msg("Problem parsing abs_spectra.dat file, line number ");
      m_errors.append(msg + QString::number(textStream.lineNumber()));
      return false;
}


bool VibronicDir::parseLogFile(TextStream& textStream)
{
   QStringList tokens;
   bool ok;

   while (!textStream.atEnd()) {
      QString line(textStream.nextLine());

      if (line.contains("Temperature(K)")) {
         tokens = TextStream::tokenize(line); 
         if (tokens.size() == 3) {
            double t = tokens[2].toDouble(&ok);  if (!ok) goto error;
            m_vibronic->setTemperature(t);
         }
      }


      if (line.contains("Frequency Domain")) {
         tokens = TextStream::tokenize(line); 
         if (tokens.size() == 10) {
            double min   = tokens[7].toDouble(&ok);  if (!ok) goto error;
            double max   = tokens[8].toDouble(&ok);  if (!ok) goto error;
            double delta = tokens[9].toDouble(&ok);  if (!ok) goto error;
            m_vibronic->setFrequencyDomain(min, max, delta);
         }
      }

      if (line.contains("Electronic Dipole Strength")) {
         tokens = TextStream::tokenize(line); 
         if (tokens.size() == 7) {
            double x = tokens[4].toDouble(&ok);  if (!ok) goto error;
            double y = tokens[5].toDouble(&ok);  if (!ok) goto error;
            double z = tokens[6].toDouble(&ok);  if (!ok) goto error;
            m_vibronic->setElectronicDipole(Vec3(x,y,z));
         }
      }

   }

   return true;

   error:
      QString msg("Problem readin Vibronic log file data, line number ");
      m_errors.append(msg + QString::number(textStream.lineNumber()));
      return false;
}

} } // end namespace IQmol::Parser
