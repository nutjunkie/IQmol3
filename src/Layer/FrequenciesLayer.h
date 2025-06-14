#pragma once
/*******************************************************************************

  Copyright (C) 2022-2025 Andrew Gilbert

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

#include "Layer.h"
#include "Viewer/Animator.h"
#include "Configurator/FrequenciesConfigurator.h"
#include "QGLViewer/vec.h"
#include "openbabel/math/vector3.h"
#include <vector>


//typedef std::vector<OpenBabel::vector3> Eigenvectors;

namespace OpenBabel {
   class OBVibrationData;
}

namespace IQmol {

namespace Data {
   class Frequencies;
   class VibrationalMode;
}

namespace Layer {

   class Mode;
   class Molecule;

   /// Data Layer representing the vibrational frequencies of a molecule.
   /// The individual Modes are children of this item.
   class Frequencies : public Base {

      Q_OBJECT 

      public:
         Frequencies(Data::Frequencies const&);

         void setMolecule(Molecule*);
         void setPlay(bool const play);
         void setPlay();
         void setLoop(bool const loop);
         void setSpeed(double const speed);
         void setScale(double const scale);

         double zpve() const;
         double maxFrequency() const;
         double maxIntensity() const;
         double maxRamanIntensity() const;
         bool   haveRaman() const;

         QList<double> frequencies() const;

      Q_SIGNALS:
         void pushAnimators(AnimatorList const&);
         void popAnimators(AnimatorList const&);
         void update();

      public Q_SLOTS:
         void configure();
         void setActiveMode(Mode const& mode);
         void playMode(Mode const& mode); 
         void playMode(int mode); 
         void clearActiveMode();
         void displayModeVector(bool on);

      private:
         Data::Frequencies const& m_frequencies;
         Configurator::Frequencies m_configurator;
         AnimatorList m_animatorList;
         bool   m_play;
         double m_loop;  // -1.0 => loop forever
         double m_speed;
         double m_scale;
         bool   m_displayModeVector;
         const  Mode*  m_activeMode;
         QList<Mode*> m_modeList;
         Molecule* m_molecule;
   };


   class Mode : public Base {

      Q_OBJECT

      public:
         Mode(Data::VibrationalMode const& mode);
         Data::VibrationalMode const& modeData() const { return m_mode; }
             
      Q_SIGNALS:
         void playMode(Mode const&);

      public Q_SLOTS:
         void configure() { playMode(*this); }

      private:
         Data::VibrationalMode const& m_mode;
   };

} } // end namespace IQmol::Layer
