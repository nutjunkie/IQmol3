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


namespace IQmol {
namespace Data {

void VibronicData::dump() const 
{
   qDebug() << "Freqency domain: " << m_fmin << m_fmax << m_fdelta;
   qDebug() << "Temperature:     " << m_temperature;
   qDebug() << "Transition dipole" << m_electronicDipole.format();
}


void VibronicData::setTemperature(double const t) 
{
   m_temperature = t;
}

void VibronicData::setFrequencyDomain(double const min, double const max, double const delta) 
{
   m_fmin = min;
   m_fmax = max;
   m_fdelta = delta;
}

void VibronicData::setElectronicDipole(Vec3 const& mu) 
{
   m_electronicDipole = mu;
   m_electronicDipole.dump(); 
}


// ------------------------------ VibronicSpectrum ------------------------------ 

void VibronicSpectrum::dump() const 
{
}


void VibronicSpectrum::setData(VibronicData const& data)
{
   m_vibronic = data;
}



void VibronicSpectrum::setData(QList<double> const& data)
{
   m_data = data;
}

} } // end namespace IQmol::Data
