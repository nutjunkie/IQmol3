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
#include "Data/Vibronic.h"


using namespace qglviewer;

namespace IQmol {
namespace Layer {


Vibronic::Vibronic(Data::Vibronic const& vibronic) : Base("Vibronic"), 
   m_vibronic(vibronic), m_configurator(*this)
{
   m_configurator.load();
   setConfigurator(&m_configurator);
}


void Vibronic::configure()
{
   m_configurator.reset();
   m_configurator.display();
}


Data::VibronicSpectrumList const& Vibronic::spectra() const 
{ 
   return m_vibronic.spectra(); 
}


} } // end namespace IQmol::Layer
