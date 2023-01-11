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

         enum Theory { FC = 0, HT, FCHT };
         static const Theory AllTheories[3];

         VibronicSpectrum(Theory theory = FC, int mode = -1, 
            QList<double> const& data = QList<double>());

         VibronicSpectrum& operator+=(VibronicSpectrum const& that);

         VibronicSpectrum& operator-=(VibronicSpectrum const& that);

         QList<double> const& data() const { return m_data; }

         int mode() const { return m_mode; }

         Theory theory() const { return m_theory; }

         double min() const { return m_min; }

         double max() const { return m_max; }

         unsigned nPoints() const { return m_data.size(); }

         void finalize();

         void dump() const { }

         void serialize(InputArchive& ar, unsigned int const /*version*/) 
         {
            ar & m_data;
            ar & m_min;
            ar & m_max;
            ar & m_mode;
         }

         void serialize(OutputArchive& ar, unsigned int const /*version*/) 
         {
            ar & m_data;
            ar & m_min;
            ar & m_max;
            ar & m_mode;
         }

      private:
         Theory m_theory;
         double m_max;
         double m_min;
         int m_mode;
         QList<double> m_data;
   };


   class Vibronic: public Base {

      friend class boost::serialization::access;

      public:

         Type::ID typeID() const { return Type::Vibronic; }

         ~Vibronic();

         void setTemperature(double const t);

         void setElectronicEnergy(double const t);
         double electronicEnergy() const { return m_electronicEnergy; }

         void setElectronicDipole(Vec3 const& mu);

         void addSpectrum(VibronicSpectrum* spectrum);

         void setFrequencies(QList<double> const& initial, QList<double> const& fin);


         void setFrequencyDomain(double const min, double const max, double const delta); 

         double frequencyDomainMin() const { return m_fmin; }

         double frequencyDomainMax() const { return m_fmax; }

         double frequencyDomainDelta() const { return m_fdelta; }


         unsigned nPoints() const { return m_fcSpectra[-1]->nPoints(); };

         unsigned nModes() const { return m_initialFrequencies.size(); }

         QList<double> const& initialFrequencies() const { return m_initialFrequencies; }

         QList<double> const& finalFrequencies() const { return m_finalFrequencies; }

         VibronicSpectrum const& spectrum(VibronicSpectrum::Theory theory, int i) const;

         void dump() const;

         void finalize();

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

         QList<double> m_initialFrequencies;
         QList<double> m_finalFrequencies;

         QMap<int, VibronicSpectrum*> m_fcSpectra;
         QMap<int, VibronicSpectrum*> m_htSpectra;
         QMap<int, VibronicSpectrum*> m_fchtSpectra;
   };

} } // end namespace IQmol::Data

#endif
