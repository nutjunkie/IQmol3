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

#include "Data/Bank.h"
#include "Data/File.h"
#include "Util/QsLog.h"

#include "ParseFile.h"
#include "XyzParser.h"
#include "CubeParser.h"
#include "GdmaParser.h"
#include "IQmolParser.h"
#include "MeshParser.h"
#include "PovRayParser.h"
#include "QChemInputParser.h"
#include "QChemOutputParser.h"
#include "QChemPlotParser.h"
#include "EfpFragmentParser.h"
#include "ExternalChargesParser.h"
#include "FormattedCheckpointParser.h"
#include "OpenBabelParser.h"
#include "PdbParser.h"
#include "GroParser.h"
#include "VibronicParser.h"
#include "YamlParser.h"

#ifdef QARCHIVE
#include "ArchiveParser.h"
#endif

#include <QDir>


namespace IQmol {
namespace Parser {


ParseFile::ParseFile(QString const& filePath, QString const& filter)
{
   m_filePath = filePath;
   QFileInfo info(filePath);

   if (info.isDir()) {
      parseDirectory(filePath, filter);
   }else {
      m_name = info.completeBaseName();
      m_filePaths.append(filePath);
   }
}


void ParseFile::parseDirectory(QString const& filePath, QString const& filter)
{
   QDir dir(filePath);

   QStringList list;
   if (filter.isEmpty()) {
      m_name = dir.dirName();
      list << dir.dirName() + ".*";
   }else {
      m_name = filter;
      list << filter + ".*";
   }
   dir.setNameFilters(list);

   QDir::Filters filters(QDir::Files | QDir::Readable);
   m_filePaths << dir.entryList(filters);

   QStringList dirs(dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));
   QStringList::iterator iter;
   for (iter = dirs.begin(); iter != dirs.end(); ++iter) {
       QFileInfo fileInfo(filePath, *iter);
       m_filePaths << *iter;
   }

   if (m_filePaths.isEmpty()) {
      // look for the first output file
      list.clear();
      list << "*.out";
      dir.setNameFilters(list);
      m_filePaths << dir.entryList(filters);
      if (m_filePaths.isEmpty()) return;

      QFileInfo info(m_filePaths.first());
      m_name = info.completeBaseName();
      m_filePaths.clear();
      list.clear();
      
      list << m_name + ".*";
      dir.setNameFilters(list);
      m_filePaths << dir.entryList(filters);
      if (m_filePaths.isEmpty()) return;
   }

   qDebug() << "FilePath: " << m_filePath;
   qDebug() << "Files:    " << m_filePaths;

//  ---
   // This is a hack to ensure the .out file gets read before the other files
   //qDebug() << "Reversing directory file order";

   QStringList filePaths;
   for (int i = 0; i < m_filePaths.size(); ++i) {
       filePaths.prepend(m_filePaths[i]);
   }
   m_filePaths = filePaths;
//  ---

   for (iter = m_filePaths.begin(); iter != m_filePaths.end(); ++iter) {
       (*iter).prepend(m_filePath + "/");
   }
}


// There is a problem in this routine if an attempt is made to parse 
// iqmol_povray.inc, but the file does not exist.  A crash occurs at
// the end of the for loop.
void ParseFile::run()
{
   Data::FileList* fileList = new Data::FileList();

   qDebug() << "File list in run()" << m_filePaths;

   bool addToFileList(true);
   QStringList::const_iterator file;
   for (file = m_filePaths.begin(); file != m_filePaths.end(); ++file) {
       QFileInfo info(*file);
       if (info.exists()) {
          QLOG_INFO() << "Parsing file: " << *file;
          parse(*file, addToFileList); //ignore return for the moment
          if (addToFileList) fileList->append(new Data::File(*file));
       }else {
          QLOG_WARN() << "File not found:" << *file;
       }
   }

   if (fileList->isEmpty()) {
      delete fileList;
   }else {
      m_dataBank.append(fileList);
   }
}


bool ParseFile::parse(QString const& filePath, bool& addToFileList)
{
   QElapsedTimer timer;
   timer.start();

   QFileInfo fileInfo(filePath);
   addToFileList = true;
   
   if (!fileInfo.exists()) {
      QString msg("File not found: ");
      msg += fileInfo.filePath();
      m_errorList.append(msg);
      //m_filePaths.removeAll(filePath);
      addToFileList = false;
      return false;
   }

   QString extension(fileInfo.suffix().toLower());
   Base* parser(0);

   // Intercept directories first
   if (fileInfo.isDir()) {
      addToFileList = false;
      if (fileInfo.filePath().endsWith("files")) {
         parser = new VibronicDir;
      }else {
         QLOG_WARN() << "No parser for directory" << fileInfo.filePath();
         return false;
      }
   } 

   if (extension == "run" || extension == "err" || extension == "bat") {
      return false;
   }

   if (extension == "xyz") {
      parser = new Xyz;
   }

   if (extension == "txt") {
      parser = new Xyz;
   }

   if (extension == "efp") {
      parser = new EfpFragment;
   }

   if (extension == "dma") {
      parser = new Gdma;
   }

   if (extension == "esp" || extension == "mo" || extension == "hf") {
      parser = new QChemPlot;
   }

   if (extension == "in"  || extension == "qcin"  || extension == "inp") {
      parser = new QChemInput;
   }

   if (extension == "out"  || extension == "qcout") {
      parser = new QChemOutput;
   }

   if (extension == "iqmol" || extension == "iqm") {
      parser = new IQmol;
   }

   if (extension == "cube" || extension == "cub") {
      parser = new Cube;
   }

   if (extension == "chg") {
      parser = new ExternalCharges;
   }

   if (extension == "fchk" || extension == "fck" || extension == "fch") {
      parser = new FormattedCheckpoint;
   }

   if (extension == "pdb") {
      parser = new Pdb;
   }

   if (extension == "gro"){
      parser = new Gro;
      QLOG_INFO() << "Using gro parser";
   }

   if (extension == "ply" || extension == "obj" || 
       extension == "stl" || extension == "off" ) {
       QLOG_DEBUG() << "Using Mesh parser";
      parser = new Mesh;
   }

   if (extension == "yaml" || extension == "cfg") {
      addToFileList = false;
      QLOG_DEBUG() << "Using Yaml parser";
      parser = new Yaml;
   }

#ifdef QARCHIVE
   if (extension == "h5") {
      QLOG_DEBUG() << "Using Archive parser";
      parser = new Archive;
   }
#endif
 
   if (extension == "inc" || extension == "pov") {
      addToFileList = false;
      QLOG_DEBUG() << "Using PovRay parser";
      parser = new PovRay;
   }
   
   if (!parser && OpenBabel::formatSupported(extension)) {
      // Only if we do not have a custom parser do we let Open Babel at it
      QLOG_DEBUG() << "Using OpenBabel parser";
      parser = new OpenBabel;
   }

   if (!parser) {
      QLOG_WARN() << "Failed to find parser for file:" << filePath 
                  << " extension " << extension;
      return false;
   }

   runParser(parser, filePath);
   delete parser;

   QLOG_INFO() << "File parsed in" << double(timer.elapsed()) /1000.0  << "s";

   return m_errorList.isEmpty();
}


void ParseFile::runParser(Base* parser, QString const& filePath)
{
   if (parser->parseFile(filePath)) {
      QLOG_INFO() << "File parsed successfully: " << filePath;
   }else {
      QStringList errors(parser->errors());
      QFileInfo info(filePath);
      QString msg("Error parsing file: ");
      msg += info.fileName();
      m_errorList.append(msg);
      m_errorList << errors;
   }

   Data::Bank& bank(parser->data());
   m_dataBank.merge(bank);
}

} } // end namespace IQmol::Parser
