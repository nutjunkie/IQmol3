#pragma once
/*******************************************************************************

  Copyright (C) 2023 Andrew Gilbert

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

#include "MacroMolecule.h"
#include <QVector>


namespace IQmol {
namespace Data {

   // Data class representing a single chain of a protein
   class ProteinChain : public MacroMolecule 
   {
      public:
         ProteinChain(QString const& label) : MacroMolecule(label) { }

         Type::ID typeID() const { return Type::ProteinChain; }

         unsigned nResidues() const { return m_groups.size(); }
         double const* cao() const { return &m_caoPositions[0]; }
         int const*  secondaryStructure() const { return &m_secondaryStructure[0]; }

         void appendAlphaCarbon(qglviewer::Vec const& v) 
         {
            m_alphaCarbons.append(v);
         }

         void appendPeptideOxygen(qglviewer::Vec const& v) 
         {
            m_peptideOxygens.append(v);
         }

         bool setSecondaryStructure(QVector<int> const& secondaryStructure)
         {
            unsigned nRes(nResidues());
            if (m_alphaCarbons.size() != nRes) return false;
            if (m_peptideOxygens.size() != nRes) return false;
            if (secondaryStructure.size() != nRes) return false;

            m_secondaryStructure = secondaryStructure.toStdVector();

            // TMP for recreating the data structures required for cartoons
            for (size_t i(0); i < nRes; ++i) {
                addResidue(m_alphaCarbons[i], m_peptideOxygens[i]);
            }
            return true;
         }

      private:
         inline void addResidue(qglviewer::Vec const& posCA, qglviewer::Vec const& posO)
         {
             m_caoPositions.push_back(posCA.x);
             m_caoPositions.push_back(posCA.y);
             m_caoPositions.push_back(posCA.z);
             m_caoPositions.push_back(posO.x);
             m_caoPositions.push_back(posO.y);
             m_caoPositions.push_back(posO.z);
         }

         // Data for generataing the cartoon representation
         std::vector<double> m_caoPositions;
         std::vector<int>  m_secondaryStructure;

         QVector<qglviewer::Vec> m_alphaCarbons;
         QVector<qglviewer::Vec> m_peptideOxygens;
   };

} } // end namespace IQmol::Data
