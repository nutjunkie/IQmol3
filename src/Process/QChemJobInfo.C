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

#include "QChemJobInfo.h"
#include "QsLog.h"


namespace IQmol {
namespace Process {


QVariantList QChemJobInfo::toQVariantList() const
{
   QVariantList list;
   list << QVariant(m_data["LocalWorkingDirectory"]);
   list << QVariant(m_data["RemoteWorkingDirectory"]);
   list << QVariant(m_data["InputString"]);
   list << QVariant(m_charge);
   list << QVariant(m_multiplicity);
   list << QVariant(m_localFilesExist);
   list << JobInfo::toQVariantList();

   return list;
}


bool QChemJobInfo::fromQVariantList(QVariantList const& list)
{
   bool ok(list.size() >= 6);            if (!ok) return false;

   ok = list[0].canConvert<QString>();   if (!ok) return false;
   m_data.insert("LocalWorkingDirectory",  list[0].toString());

   ok = list[1].canConvert<QString>();   if (!ok) return false;
   m_data.insert("RemoteWorkingDirectory", list[1].toString());

   ok = list[2].canConvert<QString>();   if (!ok) return false;
   m_data.insert("InputString",            list[2].toString());
             
   ok = list[3].canConvert<int>();       if (!ok) return false;
   m_multiplicity = list[3].toInt();
             
   ok = list[4].canConvert<int>();       if (!ok) return false;
   m_multiplicity = list[4].toInt();
             
   ok = list[5].canConvert<bool>();      if (!ok) return false;
   m_localFilesExist = list[5].toInt();
             
   return JobInfo::fromQVariantList(list.mid(6)); 
}


void QChemJobInfo::set(QString const key, QString const& value) 
{
   qDebug() << "Setting QChemJobInfo field" << key << "to" << value;
   m_data[key] = value;
}


void QChemJobInfo::set(QString const key, int const& value)
{

   if(key == "Charge"){       
      m_charge = value;
   } else if(key == "Multiplicity"){ 
      m_multiplicity = value;  
   } else if (key == " NElectrons"){
      m_nElectrons = value;  
   }else{
      m_data[key] = QString::number(value);
   }            
}


// Obviously this will return rubbish if a non-file field is specified
QString QChemJobInfo::getLocalFilePath(QString const key) const
{
   QString path(get("LocalWorkingDirectory"));
   if (!path.endsWith("/")) path += "/";
   path += get(key);
   return path;
}


// Obviously this will return rubbish if a non-file field is specified
QString QChemJobInfo::getRemoteFilePath(QString const key) const
{
   QString path(get("RemoteWorkingDirectory"));
   if (!path.endsWith("/")) path += "/";
   path += get(key);
   return path;
}

QString QChemJobInfo::get(QString const key) const
{ 
   QString value(baseName());
  
   if (key == "InputFileName"){
         value += ".inp";
   } else if (key == "OutputFileName"){
      value += ".out";
   } else if (key == "AuxFileName"){
      value += ".FChk";
   } else if (key == "EspFileName"){
      value += ".esp";
   } else if (key == "MoFileName"){
      value += ".mo";
   } else if (key == "DensityFileName"){
      value += ".hf";
   } else if (key == "RunFileName"){
      value += ".run";
   } else if (key == "BatchFileName"){
      value += ".bat";
   } else if (key == "ErrorFileName"){
      value += ".err";
   }else{
      qDebug() << "trying key" << key ;
      value = m_data[key];
      qDebug() << "returning value" << value;
   }

   return value;
}

int QChemJobInfo::getInt(QString const key) const
{
   
   if(key == "Charge"){       
      return m_charge;  
   } else if(key == "Multiplicity"){ 
      return  m_multiplicity;
   } else if (key == " NElectrons"){
      return m_nElectrons;  
   }else{
      return 0;
   }            
}


/*
QString QChemJobInfo::get(Field const field) const
{ 
   QString value(baseName());

   switch (field) {
      case InputFileName:
         value += ".inp";
         break;
      case OutputFileName:
         value += ".out";
         break;
      case AuxFileName:
         value += ".FChk";
         break;
      case EspFileName:
         value += ".esp";
         break;
      case MoFileName:
         value += ".mo";
         break;
      case DensityFileName:
         value += ".hf";
         break;
      case RunFileName:
         value += ".run";
         break;
      case BatchFileName:
         value += ".bat";
         break;
      case ErrorFileName:
         value += ".err";
         break;

      default:
         value = m_data[field];
         break;
   }

   return value;
}
*/

QStringList QChemJobInfo::outputFiles() const
{
   QStringList files;
   files.append(get("LocalWorkingDirectory") + "/" + get("OutputFileName"));
   files.append(get("LocalWorkingDirectory") + "/" + get("AuxFileName"));
   return files;
}


void QChemJobInfo::dump() const
{
   QLOG_DEBUG() << "QChemJobInfo info:";
   QLOG_DEBUG() << "   InputFileName         " << get("InputFileName");
   QLOG_DEBUG() << "   OutputFileName        " << get("OutputFileName");
   QLOG_DEBUG() << "   AuxFileName           " << get("AuxFileName");
   QLOG_DEBUG() << "   RunFileName           " << get("RunFileName");
   QLOG_DEBUG() << "   LocalWorkingDirectory " << get("LocalWorkingDirectory");
   QLOG_DEBUG() << "   RemoteWorkingDirectory" << get("RemoteWorkingDirectory");
   QLOG_DEBUG() << "   Molecule pointer      " << m_moleculePointer;
   JobInfo::dump();
}


void QChemJobInfo::copy(QChemJobInfo const& that)
{
   JobInfo::copy(that);
   m_data              = that.m_data;
   m_charge            = that.m_charge;
   m_multiplicity      = that.m_multiplicity;
   m_nElectrons        = that.m_nElectrons;
   m_localFilesExist   = that.m_localFilesExist;
   m_promptOnOverwrite = that.m_promptOnOverwrite;
   m_efpOnlyJob        = that.m_efpOnlyJob;
   m_moleculePointer   = that.m_moleculePointer;
}

} } // end namespace IQmol::Process

