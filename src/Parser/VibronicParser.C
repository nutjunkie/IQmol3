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
          QString baseName(iter->baseName());

          if (fileName.startsWith("Spec")) {
             parseSpectrumFile(textStream, baseName);

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

   // Need to take differences for FC component
   m_vibronic->finalize();
   
   // Need to check vibronic data is valid
   m_dataBank.append(m_vibronic);

   return true;
}


bool VibronicDir::parseSpectrumFile(TextStream& textStream, QString const& title)
{
   bool ok; 
   const QRegularExpression rx(QLatin1Literal("[^0-9]+"));
   const auto&& parts = title.split(rx, QString::SkipEmptyParts);
   int mode = parts.last().toInt(&ok); 

   Data::VibronicSpectrum::Theory theory;
   if (title.contains("_fcht_")) {
      theory = Data::VibronicSpectrum::FCHT;
   }else if (title.contains("_fc_")) {
      theory = Data::VibronicSpectrum::FC;
   }else if (title.contains("_ht_")) {
      theory = Data::VibronicSpectrum::HT;
   }

   QList<double> data;
   
   while (!textStream.atEnd()) {
      QStringList tokens(textStream.nextLineAsTokens());
      for (unsigned i = 0; i < tokens.length(); ++i) {
          bool ok;
          double x = tokens[i].toDouble(&ok); if (!ok) goto error;
          data.append(x);
      }
   }

   m_vibronic->addSpectrum(new Data::VibronicSpectrum(theory, mode-1, data));

   return true;

   error:
      QString msg("Problem parsing vibronic spectrum file, line number ");
      m_errors.append(msg + QString::number(textStream.lineNumber()));
      return false;
}


bool VibronicDir::parseDatFile(TextStream& textStream)
{
   QList<double> fc, ht, fcht;
   int mode(-1); // Used to indicate these are total spetra
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

   m_vibronic->addSpectrum(new Data::VibronicSpectrum(
      Data::VibronicSpectrum::FC,   mode, fc));
   m_vibronic->addSpectrum(new Data::VibronicSpectrum(
      Data::VibronicSpectrum::HT,   mode, ht));
   m_vibronic->addSpectrum(new Data::VibronicSpectrum(
      Data::VibronicSpectrum::FCHT, mode, fcht));

   return true;

   error:
      QString msg("Problem parsing abs_spectra.dat file, line number ");
      m_errors.append(msg + QString::number(textStream.lineNumber()));
      return false;
}


bool VibronicDir::parseLogFile(TextStream& textStream)
{
   QStringList tokens;
   unsigned nModes(0);
   bool ok;

   while (!textStream.atEnd()) {
      QString line(textStream.nextLine());

      if (line.contains("Number of Frequency")) {
         tokens = TextStream::tokenize(line); 
         if (tokens.size() == 5) {
            nModes = tokens[4].toInt(&ok);  if(!ok) goto error;
         }
      }

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

      if (line.contains("Frequency   Initial         Final")) {
         QList<double> initial;
         QList<double> final;
         double f;
         for (unsigned mode = 0; mode < nModes; ++mode) {
             tokens = textStream.nextLineAsTokens();
             f = tokens[1].toDouble(&ok);  if (!ok) goto error;
             initial.append(f);
             f = tokens[2].toDouble(&ok);  if (!ok) goto error;
             final.append(f);
             m_vibronic->setFrequencies(initial, final);
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
