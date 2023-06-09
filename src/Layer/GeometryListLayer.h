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

#include "Layer.h"
#include "Data/GeometryList.h"


namespace IQmol {

namespace Configurator {
   class GeometryList;
}

   namespace Animator {
      class Base;
   }
   typedef QList<Animator::Base*> AnimatorList;

namespace Layer {

   class Molecule;

   class GeometryList : public Base {

      Q_OBJECT

      friend class Configurator::GeometryList;

      public:
         GeometryList(Data::GeometryList&);
         ~GeometryList();
         void setMolecule(Molecule*);

      Q_SIGNALS:
         void pushAnimators(AnimatorList const&);
         void popAnimators(AnimatorList const&);
         void update();

      public Q_SLOTS:
         void configure();
         void setCurrentGeometry(unsigned const index);
         void resetGeometry();
         void cloneLastGeometry();
         void generateConformers();

      protected:
         void setPlay(bool const play);
         void setLoop(bool const loop);
         void setBounce(bool const bounce);
         void setSpeed(double const speed);
         void setReperceiveBonds(bool const tf);

      private Q_SLOTS:
         void removeGeometry();

      private:
         void makeAnimators();
         void deleteAnimators();

         Molecule* m_molecule;
         Configurator::GeometryList* m_configurator;
         Data::GeometryList& m_geometryList;
         AnimatorList m_animatorList;

         unsigned m_defaultIndex;
         double m_speed;
         bool m_reperceiveBonds;
         bool m_bounce;
         bool m_loop;
         bool m_allowModifications;
   };

} } // end namespace IQmol::Layer
