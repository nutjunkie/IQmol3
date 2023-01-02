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


   class VibronicData: public Base {

      friend class boost::serialization::access;

      public:
         Type::ID typeID() const { return Type::VibronicData; }

         void setTemperature(double const t);

         void setElectronicEnergy(double const t);

         void setFrequencyDomain(double const min, double const max, double const delta); 

         void setElectronicDipole(Vec3 const& mu);

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

      private:
         double m_temperature;
         double m_electronicEnergy;
         double m_fmin, m_fmax, m_fdelta;
         Vec3   m_electronicDipole;
   };


   class VibronicSpectrum: public Base {

      friend class boost::serialization::access;

      public:
         Type::ID typeID() const { return Type::VibronicSpectrum; }

         void setData(QList<double> const&);

         void setData(VibronicData const&);

         void dump() const;

         void serialize(InputArchive& ar, unsigned int const /*version*/) 
         {
            ar & m_vibronic;
            ar & m_data;
         }

         void serialize(OutputArchive& ar, unsigned int const /*version*/) 
         {
            ar & m_vibronic;
            ar & m_data;
         }

      private:
         VibronicData m_vibronic;
         QList<double> m_data;
   };


   class VibronicSpectrumList: public List<VibronicSpectrum>
   {
      public:
        // Why is a fully qualified namespace required for Type with Qt >= 6.0
        IQmol::Data::Type::ID typeID() const { return IQmol::Data::Type::VibronicSpectrumList; }
   };

} } // end namespace IQmol::Data

#endif
