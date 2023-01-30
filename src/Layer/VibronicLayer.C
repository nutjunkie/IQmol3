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

#include "VibronicLayer.h"
#include "FrequenciesLayer.h"
#include "Data/Vibronic.h"
#include "Util/QsLog.h"


using namespace qglviewer;

namespace IQmol {
namespace Layer {


Vibronic::Vibronic(Data::Vibronic const& vibronic) : Base("Vibronic"), 
   m_vibronic(vibronic), m_configurator(*this), m_initialFrequencies(0), m_finalFrequencies(0)
{
   m_configurator.load();
   setConfigurator(&m_configurator);
}


void Vibronic::configure()
{
   m_configurator.reset();
   m_configurator.display();
}


void Vibronic::setFrequencyLayers(QList<Layer::Frequencies*> const& frequencyLayers)
{
   if (frequencyLayers.size() != 2) {
      QLOG_WARN() << "Invalid number of Frquency Layers passed to Vibronic Layer" 
                  << frequencyLayers.size();
   }

   QList<double> const& f0(m_vibronic.initialFrequencies());
   QList<double> const& f1(m_vibronic.finalFrequencies());

   QList<double> const F0(frequencyLayers.first()->frequencies());
   QList<double> const F1(frequencyLayers.last()->frequencies());

   int n(f0.size());
   if (f1.size() != n || F0.size() != n || F1.size() != n) {
      QLOG_WARN() << "Invalid frequency data in Vibronic Layer";
      return;
   }

   double d0(0), d1(0);
   for (int i = 0; i < n; ++i) {
       d0 += std::abs(f0[i]-F0[i]);
       d1 += std::abs(f0[i]-F1[i]);
   }


   QLOG_INFO() << "d0/d1 values" << d0 << d1;

   if (d0 < d1) {
      m_initialFrequencies = frequencyLayers.first();
      m_finalFrequencies   = frequencyLayers.last();
   }else {
      m_initialFrequencies = frequencyLayers.last();
      m_finalFrequencies   = frequencyLayers.first();
   }

   connectFinalFrequencies(true);
}


void Vibronic::connectInitialFrequencies(bool)
{
   playMode(-1);
   disconnect(this, SIGNAL(playMode(int)));
   if (m_initialFrequencies) {
      connect(this, SIGNAL(playMode(int)), m_initialFrequencies, SLOT(playMode(int)));
   }
}


void Vibronic::connectFinalFrequencies(bool)
{
   playMode(-1);
   disconnect(this, SIGNAL(playMode(int)));
   if (m_finalFrequencies) {
      connect(this, SIGNAL(playMode(int)), m_finalFrequencies, SLOT(playMode(int)));
   }
}

} } // end namespace IQmol::Layer
