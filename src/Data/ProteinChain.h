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
#include "Residue.h"
#include "Math/Vec.h"
#include <QVector>


namespace IQmol {
namespace Data {

   // Data class representing a single chain of a protein
   class ProteinChain : public MacroMolecule 
   {
      public:
         ProteinChain(QString const& chainId) 
          : MacroMolecule(QString("Chain " + chainId))
         { 
            qDebug() << "Attempting to set chain Id to" << chainId;
            QString const letters("ABCDEFGHIJKLMNOPQRTSUVWXYZ");
            m_chainIndex = letters.indexOf(chainId);
            qDebug() << "set to" << m_chainIndex;
         }

         Type::ID typeID() const { return Type::ProteinChain; }

         int chainIndex() const { return m_chainIndex; }

         unsigned nResidues() const { return m_groups.size(); }

         QList<AminoAcid_t> residueList() const 
         {
            QList<AminoAcid_t> list;

            for (auto group : m_groups) {
                Residue* res = dynamic_cast<Residue*>(group);
                if (res) list.append(res->type());
            }

            return list;
         }

         QVector<Math::Vec3> const& alphaCarbons() const { return m_alphaCarbons; }

         QVector<Math::Vec3> const& peptideOxygens() const { return m_peptideOxygens; }

         QVector<SecondaryStructure> const& 
            secondaryStructures() const { return m_secondaryStructure; }
    
         void appendAlphaCarbon(Math::Vec3 const& v) { m_alphaCarbons.append(v); }

         void appendPeptideOxygen(Math::Vec3 const& v) { m_peptideOxygens.append(v); }

         void appendSecondaryStructure(SecondaryStructure sstype){m_secondaryStructure.append(sstype); }

         bool setSecondaryStructure(
             QVector<SecondaryStructure> const& secondaryStructure)
         {
            unsigned nRes(nResidues());
            if (m_alphaCarbons.size() != nRes) return false;
            if (m_peptideOxygens.size() != nRes) return false;
            if (secondaryStructure.size() != nRes) return false;

            m_secondaryStructure = secondaryStructure;
            return true;
         }

      private:
         int m_chainIndex;
         QVector<SecondaryStructure> m_secondaryStructure;
         QVector<Math::Vec3> m_alphaCarbons;
         QVector<Math::Vec3> m_peptideOxygens;
   };

} } // end namespace IQmol::Data
