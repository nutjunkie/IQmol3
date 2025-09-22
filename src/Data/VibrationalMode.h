#pragma once
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

#include "DataList.h"
#include "QGLViewer/vec.h"


namespace IQmol {
namespace Data {

   /// Data class representing molecule with a particular geometry.  
   class VibrationalMode : public Base {

      public:
         VibrationalMode(double const frequency = 0.0, double const intensity = 0.0,
            bool irActive = false, bool ramanActive = false, 
            QList<qglviewer::Vec> eigenvector = QList<qglviewer::Vec>()) : 
            m_frequency(frequency), m_intensity(intensity), m_irActive(irActive),
            m_ramanActive(ramanActive), m_eigenvector(eigenvector) { }

         Type::ID typeID() const { return Type::VibrationalMode; }
         
         double frequency() const { return m_frequency; }
         double intensity() const { return m_intensity; }
         double ramanIntensity() const { return m_ramanIntensity; }
         bool   irActive() const { return m_irActive; }
         bool   ramanActive() const { return m_ramanActive; }
         QList<qglviewer::Vec> const&  eigenvector() const { return m_eigenvector; }

         void setFreqeuncy(double const frequency) { m_frequency = frequency; }
         void setIntensity(double const intensity) { m_intensity = intensity; }
         void setIrActive(bool const tf) { m_irActive = tf; }
         void setRamanActive(bool const tf) { m_ramanActive = tf; }
         void setRamanIntensity(double const intensity) { m_ramanIntensity = intensity; }
         void appendDirectionVector(qglviewer::Vec const& vec) { m_eigenvector.append(vec); }

         bool operator<(VibrationalMode const& that) {
            return (m_frequency < that.m_frequency);
         }

         void dump() const;

      private:
         double m_frequency;
         double m_intensity;
         bool   m_irActive;
         bool   m_ramanActive;
         double m_ramanIntensity;
         QList<qglviewer::Vec> m_eigenvector;
   };

   class VibrationalModeList : public List<VibrationalMode> {
      public:
         Type::ID typeID() const { return Type::VibrationalModeList; }
   };

} } // end namespace IQmol::Data
