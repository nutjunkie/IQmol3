#ifndef IQMOL_DATA_VIBRONIC_H
#define IQMOL_DATA_VIBRONIC_H
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

#include "Data/DataList.h"
#include "Math/Vector.h"
#include <QDebug>


namespace IQmol {
namespace Data {

   class VibronicSpectrum: public Base {

      friend class boost::serialization::access;

      public:
         Type::ID typeID() const { return Type::VibronicSpectrum; }

         VibronicSpectrum(QString const& title = QString(), 
            QList<double> const& data = QList<double>()) : m_title(title), m_data(data) { }

         void setData(QList<double> const&);

         void dump() const;

         QString const& title() const { return m_title; }

         void serialize(InputArchive& ar, unsigned int const /*version*/) 
         {
            ar & m_title;
            ar & m_data;
         }

         void serialize(OutputArchive& ar, unsigned int const /*version*/) 
         {
            ar & m_title;
            ar & m_data;
         }

      private:
         QString m_title;
         QList<double> m_data;
   };


   class VibronicSpectrumList: public List<VibronicSpectrum>
   {
      public:
        // Why is a fully qualified namespace required for Type with Qt >= 6.0
        IQmol::Data::Type::ID typeID() const { return IQmol::Data::Type::VibronicSpectrumList; }
   };


   class Vibronic: public Base {

      friend class boost::serialization::access;

      public:
         Type::ID typeID() const { return Type::Vibronic; }

         void setTemperature(double const t);

         void setElectronicEnergy(double const t);

         void setFrequencyDomain(double const min, double const max, double const delta); 

         void setElectronicDipole(Vec3 const& mu);

         void addSpectrum(VibronicSpectrum* spectrum);

         void dump() const;

         void serialize(InputArchive& ar, unsigned int const /*version*/) 
         {
            ar & m_temperature;
            ar & m_electronicEnergy;
            //ar & m_electronicDipole;
            ar & m_fmin;
            ar & m_fmax;
            ar & m_fdelta;
         }

         void serialize(OutputArchive& ar, unsigned int const /*version*/) 
         {
            ar & m_temperature;
            ar & m_electronicEnergy;
            //ar & m_electronicDipole;
            ar & m_fmin;
            ar & m_fmax;
            ar & m_fdelta;
         }

         VibronicSpectrumList const& spectra() const { return m_spectra; }

      private:
         double m_temperature;
         double m_electronicEnergy;
         double m_fmin, m_fmax, m_fdelta;
         Vec3   m_electronicDipole;
         VibronicSpectrumList m_spectra;
   };

} } // end namespace IQmol::Data

#endif
