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

#include "Job.h"
#include "QMsgBox.h"
#include "QChemOutputParser.h"

#include <QDir>


namespace IQmol {
namespace Process {


void SetQChemFileNames(Job* job)
{
   QString basename(job->get<QString>("BaseName"));
   job->set("BaseName",basename);
   job->set("InputFileName",basename+".inp");
   job->set("OutputFileName",basename+".out");
   job->set("AuxFileName",basename+".fchk");
   job->set("RunFileName",basename+".run");
}


void FilterQChemFields(Job* job)
{
   QStringList filterFields = { 
       "InputString", 
       "Coordinates", 
       "Constraints", 
       "CoordinatesFsm", 
       "ScanCoordinates",
       "OnsagerRadius",
       "EfpParameters",
       "EfpFragments",
       "ExternalCharges",
       "Charge",
       "Multiplicity",
       "NumElectrons", 
       "RunFileName", 
       "BatchFileName", 
       "MoleculePointer" 
   };  

   for (auto field : filterFields) job->remove(field);
}


void CleanUpQChem(Job* job) 
{
   QDir dir (job->get<QString>("LocalWorkingDirectory"));
   if (!dir.exists()) {
      QMsgBox::warning(0, "IQmol", QString("Unable to find results for") + job->jobName());
      return;
   }

   // Rename Http files
   QString oldName("input"); 
   QString newName(job->get<QString>("InputFileName"));
   if (dir.exists(oldName)) {
      if (dir.exists(newName)) dir.remove(newName);
      dir.rename(oldName, newName);
   }

   oldName = "output";
   newName = job->get<QString>("OutputFileName");
   if (dir.exists(oldName)) {
      if (dir.exists(newName)) dir.remove(newName);
      dir.rename(oldName, newName);
   }

   oldName = "input.FChk";
   newName = job->get<QString>("AuxFileName");
   if (dir.exists(oldName) && oldName != newName) {
      if (dir.exists(newName)) dir.remove(newName);
      dir.rename(oldName, newName);
   }

   oldName = "input.fchk";
   if (dir.exists(oldName) && oldName != newName) {
      if (dir.exists(newName)) dir.remove(newName);
      dir.rename(oldName, newName);
   }

   oldName = job->get<QString>("InputFileName") + ".fchk";
   if (dir.exists(oldName) && oldName != newName) {
      if (dir.exists(newName)) dir.remove(newName);
      dir.rename(oldName, newName);
   }

   if (dir.exists("pathtable")) dir.remove("pathtable");

   // Check for errors and update the run time
   QString output(job->getLocalFilePath("OutputFileName"));
   QStringList errors(Parser::QChemOutput::parseForErrors(output));

   if (!errors.isEmpty()) {
      QString time(errors.takeLast());
      time.remove("Time: ");
      bool ok(false);
      unsigned t(time.toUInt(&ok));
      if (ok) job->resetTimer(t);
   }

   QLOG_DEBUG() << "Errors in file:" << errors;

   if (!errors.isEmpty()) {
      job->setStatus(Job::Error, errors.join("\n")); 
   }
}

} } // end namespaces IQmol::Process
