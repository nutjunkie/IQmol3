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

#include "Vibronic.h"
#include <QDebug>
#include "Util/QsLog.h"


namespace IQmol {
namespace Data {


const VibronicSpectrum::Theory constexpr VibronicSpectrum::AllTheories[3] = { FC, HT, FCHT };


VibronicSpectrum::VibronicSpectrum(Theory theory, int mode, QList<double> const& data) 
  : m_theory(theory), m_mode(mode), m_data(data), m_min(0.0), m_max(0.0) 
{
}


VibronicSpectrum& VibronicSpectrum::operator+=(VibronicSpectrum const& that)
{
   unsigned n(nPoints());
   if (n != that.nPoints()) {
      QLOG_WARN() << "Invalid operation on VibrationalSpectrum";
   }else {
      for (unsigned i(0); i < n; ++i) {
          m_data[i] += that.m_data[i];
      }
   }

   return *this;
}


// Note this does (that -= *this) rather than (*this -= that);
VibronicSpectrum& VibronicSpectrum::operator-=(VibronicSpectrum const& that)
{
   unsigned n(nPoints());
   if (n != that.nPoints()) {
      QLOG_WARN() << "Invalid operation on VibrationalSpectrum";
   }else {
      for (unsigned i(0); i < n; ++i) {
          m_data[i] = that.m_data[i] - m_data[i];
      }
   }

   return *this;
}


void VibronicSpectrum::finalize()
{
   unsigned const n(m_data.size());
   for (unsigned i = 0; i < n; ++i) {
       if (m_data[i] > m_max) m_max = m_data[i];
       if (m_data[i] < m_min) m_min = m_data[i];
   }
}



// ------------------- Vibronic -------------------

Vibronic::~Vibronic() 
{
   for (auto iter = m_fcSpectra.begin(); iter != m_fcSpectra.end(); ++iter) {
       delete iter.value();
   }

   for (auto iter = m_htSpectra.begin(); iter != m_htSpectra.end(); ++iter) {
       delete iter.value();
   }

   for (auto iter = m_fchtSpectra.begin(); iter != m_fchtSpectra.end(); ++iter) {
       delete iter.value();
   }
}
   

void Vibronic::dump() const 
{
   qDebug() << "Freqency domain: " << m_fmin << m_fmax << m_fdelta;
   qDebug() << "Temperature:     " << m_temperature;
   qDebug() << "Transition dipole" << m_electronicDipole.format();
}


void Vibronic::setTemperature(double const t) 
{
   m_temperature = t;
}


void Vibronic::setElectronicEnergy(double const e)
{
   m_electronicEnergy = e;
}


void Vibronic::setFrequencyDomain(double const min, double const max, double const delta) 
{
   m_fmin = min;
   m_fmax = max;
   m_fdelta = delta;
}


void Vibronic::setElectronicDipole(Vec3 const& mu) 
{
   m_electronicDipole = mu;
   m_electronicDipole.dump(); 
}


void Vibronic::setFrequencies(QList<double> const& initial, QList<double> const& fin)
{
   m_initialFrequencies = initial;
   m_finalFrequencies   = fin;
}


void Vibronic::addSpectrum(VibronicSpectrum* spectrum)
{
   switch (spectrum->theory()) {
      case VibronicSpectrum::FC:
         m_fcSpectra[spectrum->mode()] = spectrum;
         break;
      case VibronicSpectrum::HT:
         m_htSpectra[spectrum->mode()] = spectrum;
         break;
      case VibronicSpectrum::FCHT:
         m_fchtSpectra[spectrum->mode()] = spectrum;
         break;
   }
}


VibronicSpectrum const& Vibronic::spectrum(VibronicSpectrum::Theory theory, int i) const
{
  VibronicSpectrum* spectrum;

  switch (theory) {
      case VibronicSpectrum::FC:
         spectrum = m_fcSpectra[i];
         break;
      case VibronicSpectrum::HT:
         spectrum = m_htSpectra[i];
         break;
      case VibronicSpectrum::FCHT:
         spectrum = m_fchtSpectra[i];
         break;
   }  

   return *spectrum;
}


// This creates the difference spectra for the FC case and combined FC+HT to get FCHT
void Vibronic::finalize()
{
   unsigned mModes(nModes());
   unsigned mPoints(nPoints());
  
   VibronicSpectrum::Theory theory(VibronicSpectrum::FCHT);

   for (unsigned mode(0); mode < mModes; ++mode) {
       (*m_fcSpectra[mode]) -= (*m_fcSpectra[-1]);
       m_fchtSpectra[mode] = new VibronicSpectrum(theory, mode, m_fcSpectra[mode]->data());
       (*m_fchtSpectra[mode]) += (*m_htSpectra[mode]);

       m_fcSpectra[mode]->finalize();
       m_htSpectra[mode]->finalize();
       m_fchtSpectra[mode]->finalize();
   }

   m_fcSpectra[-1]->finalize();
   m_htSpectra[-1]->finalize();
   m_fchtSpectra[-1]->finalize();

   // TEMP
m_fcSpectra[-2] = new VibronicSpectrum(VibronicSpectrum::FC, -2, m_fcSpectra[0]->data());
m_htSpectra[-2] =  new VibronicSpectrum(VibronicSpectrum::HT, -2, m_htSpectra[0]->data());
m_fchtSpectra[-2] = new VibronicSpectrum(VibronicSpectrum::FCHT, -2, m_fchtSpectra[0]->data());

   for (unsigned mode(1); mode < mModes; ++mode) {
       (*m_fcSpectra[-2])   += (*m_fcSpectra[mode]);
       (*m_htSpectra[-2])   += (*m_htSpectra[mode]);
       (*m_fchtSpectra[-2]) += (*m_fchtSpectra[mode]);
   }

   m_fcSpectra[-2]->finalize();
   m_htSpectra[-2]->finalize();
   m_fchtSpectra[-2]->finalize();
}

} } // end namespace IQmol::Data
