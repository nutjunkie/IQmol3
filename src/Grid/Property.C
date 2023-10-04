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

#include "Property.h"
#include "Data/GridData.h"
#include "Data/AtomicDensity.h"
#include "Data/MultipoleExpansion.h"
#include "Util/Constants.h"
#include "Layer/MoleculeLayer.h"
#include "Layer/ProteinChainLayer.h"



namespace IQmol {

   namespace Property {

      // - - - - - - - - - - Indexed - - - - - - - - - -

      Data::Mesh::VertexFunction const& Indexed::evaluator() 
      {
         update();
         m_vertexFunction = std::bind(&Indexed::map, this, std::placeholders::_1);
         return m_vertexFunction;
      }


      double Indexed::map(Data::Mesh::Vertex const& vertex)
      {
         if (m_mesh == 0) return 0.0; 
         int idx(m_mesh->indexFieldValue(vertex));
         return (idx < m_map.size()) ? m_map[idx] : 0.0;
      }
      

      // - - - - - - - - - - AtomicNumber - - - - - - - - - -

      void AtomicNumber::update()
      {
         m_map.clear();
         if (m_molecule == 0) return;

         QList<int> atomicNumbers(m_molecule->atomicNumbers());
         for (auto Z : atomicNumbers) m_map.append(double(Z));
      }


      // - - - - - - - - - - Residue - - - - - - - - - -

      void Residue::update()
      {
         m_map.clear();
         if (m_chain == 0) return;

         QList<Data::AminoAcid_t> residues(m_chain->residueList());

         for (auto residue : residues) {
             int r = static_cast<int>(residue);
             m_map.append(double(r));
         }
      }


      // - - - - - - - - - - Spatial - - - - - - - - - -

      Data::Mesh::VertexFunction const& Spatial::evaluator() 
      {
         update();
         m_vertexFunction = std::bind(&Spatial::function, this, std::placeholders::_1);
         return m_vertexFunction;
      }

      double Spatial::function(Data::Mesh::Vertex const& vertex)
      {
         if (m_mesh == 0) return 0.0; 
         Data::Mesh::Point p(m_mesh->vertex(vertex));
         return m_function(p[0],p[1],p[2]);
      }
 

      // - - - - - - - - - - RadialDistance - - - - - - - - - -

      RadialDistance::RadialDistance(Data::Mesh const* mesh) 
       : Spatial("Radial distance", mesh)
      { 
         m_function = std::bind(&RadialDistance::distance, this, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
      }

      double RadialDistance::distance(double const x, double const y, double const z) const
      {
          return std::sqrt(x*x + y*y + z*z);
      }


      // - - - - - - - - - - GridBased - - - - - - - - - -

      GridBased::GridBased(QString const& type, Data::GridData const& grid)
       : Spatial(type), m_grid(grid)
      {
         m_function = std::bind(&GridBased::evaluate, this, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
      }

      double GridBased::evaluate(double const x, double const y, double const z) const
      {
         return m_grid.interpolate(x, y, z);
      }


      // - - - - - - - - - - PromoleculeDensity - - - - - - - - - -

      double const PromoleculeDensity::s_thresh = 0.0001;

      PromoleculeDensity::PromoleculeDensity(QString const& label, 
         QList<AtomicDensity::Base*> atoms, QList<qglviewer::Vec> coordinates)
          : Spatial(label), m_atomicDensities(atoms), m_coordinates(coordinates)
      {
         m_function = std::bind(&PromoleculeDensity::rho, this,  
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
      }

      PromoleculeDensity::~PromoleculeDensity()
      {
         QSet<AtomicDensity::Base*> uniqueAtoms;
         for (auto iter : m_atomicDensities)  uniqueAtoms.insert(iter);
         for (auto atom : uniqueAtoms) delete atom;
      }

      bool PromoleculeDensity::isAvailable() const
      {
         bool available(true);

         for (auto const& iter : m_atomicDensities) {
             available = available && (*iter).isAvailable();
         }

         return available;
      }

      void PromoleculeDensity::boundingBox(qglviewer::Vec& min, qglviewer::Vec& max) const
      {
         if (m_coordinates.isEmpty()) {
            min.setValue(0.0, 0.0, 0.0);
            max.setValue(0.0, 0.0, 0.0);
            return;
         }

         min = m_coordinates[0];
         max = m_coordinates[0];
         qglviewer::Vec v;

         for (int i = 0; i < m_atomicDensities.size(); ++i) {
             double r(m_atomicDensities[i]->computeSignificantRadius(s_thresh));
             qglviewer::Vec v(m_coordinates[i]);
             min.x = std::min(double(min.x), v.x-r);
             min.y = std::min(double(min.y), v.y-r);
             min.z = std::min(double(min.z), v.z-r);
             max.x = std::max(double(max.x), v.x+r);
             max.y = std::max(double(max.y), v.y+r);
             max.z = std::max(double(max.z), v.z+r);
        }
      }

      double PromoleculeDensity::rho(double const x, double const y, double const z) const
      {
         double density(0.0);
         qglviewer::Vec p(x,y,z);

         for (int i = 0; i < m_atomicDensities.size(); ++i) {
             density +=  m_atomicDensities[i]->density(p - m_coordinates[i]);
         }

         return density;
      }


      // - - - - - - - - - - PointChargePotential - - - - - - - - - -

      PointChargePotential::PointChargePotential(Data::Type::ID type,
         Layer::Molecule* molecule, Data::Mesh const* mesh) 
          : Spatial(Data::Type::chargeToString(type), mesh), m_type(type), 
            m_molecule(molecule)
      { 
         m_function = std::bind(&PointChargePotential::potential, this, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
      }

      void PointChargePotential::update()
      {
         m_charges = m_molecule->atomicCharges(m_type);
         m_coordinates = m_molecule->coordinates();
      }

      double PointChargePotential::potential(double const x, double const y, double const z)
         const
      {
         double esp(0.0);
         double d;
         qglviewer::Vec pos{x, y, z}; 

         for (int i = 0; i < m_charges.size(); ++i) {
             d = (m_coordinates[i] - pos).norm();
             esp += m_charges[i]/d;
         }   
         return esp * Constants::BohrToAngstrom;
      }


      // - - - - - - - - - - MultipolePotential - - - - - - - - - -

      // !!!! Note that the evaluator call should be updating the positions and
      // orientations of the multipoles before returning.  The fact that it does
      // not means that if a DMA is loaded and the molecule is rotated before
      // evaluating the ESP, it will be incorrect.  I have left this class without
      // the update to the coordinates so that it is more obviously wrong until I
      // figure out how to rotate things properly.

      MultipolePotential::MultipolePotential(QString const& type, unsigned const order,
        Data::MultipoleExpansionList const& siteList) : Spatial(type), m_order(order),
        m_siteList(siteList)
      {
         m_function = std::bind(&MultipolePotential::potential, this, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
      }

      double MultipolePotential::potential(double const x, double const y, double const z)
        const
      {
         double esp(0.0);
         double tmp, R2, s, ir1, ir2, ir3, ir5, ir7;
         qglviewer::Vec pos(x, y, z); 
         qglviewer::Vec R;

         Data::MultipoleExpansionList::const_iterator site;

         for (site = m_siteList.begin(); site != m_siteList.end(); ++site) {
             R   = pos-(*site)->position();
             R  *= Constants::AngstromToBohr;
             R2  = R.squaredNorm();
             ir1 = 1.0/R.norm();
             ir2 = ir1*ir1;
             ir3 = ir1*ir2;
             ir5 = ir3*ir2;
             ir7 = ir5*ir2;
        
             if (m_order >= 0) { // charge
                esp += (*site)->moment(Data::MultipoleExpansion::Q) * ir1;
             }
             if (m_order >= 1) { // dipole
                tmp  = (*site)->moment(Data::MultipoleExpansion::X) * R.x;
                tmp += (*site)->moment(Data::MultipoleExpansion::Y) * R.y;
                tmp += (*site)->moment(Data::MultipoleExpansion::Z) * R.z;
                esp += tmp * ir3;
             }
             if (m_order >= 2) { // quadrupole
                tmp  = (*site)->moment(Data::MultipoleExpansion::XX) * (3.0*R.x*R.x - R2);
                tmp += (*site)->moment(Data::MultipoleExpansion::YY) * (3.0*R.y*R.y - R2);
                tmp += (*site)->moment(Data::MultipoleExpansion::ZZ) * (3.0*R.z*R.z - R2);
                tmp += (*site)->moment(Data::MultipoleExpansion::XY) * (3.0*R.x*R.y);
                tmp += (*site)->moment(Data::MultipoleExpansion::XZ) * (3.0*R.x*R.z);
                tmp += (*site)->moment(Data::MultipoleExpansion::YZ) * (3.0*R.y*R.z);
                esp += 0.5*tmp*ir5;
             }   
             if (m_order >= 3) { // octopole
                tmp  = (*site)->moment(Data::MultipoleExpansion::XYZ) * (30.0*R.x*R.y*R.z);
                s    = 5.0*R.x*R.x;
                tmp += (*site)->moment(Data::MultipoleExpansion::XXX) *     R.x*(s - 3.0*R2);
                tmp += (*site)->moment(Data::MultipoleExpansion::XXY) * 3.0*R.y*(s -     R2);
                tmp += (*site)->moment(Data::MultipoleExpansion::XXZ) * 3.0*R.z*(s -     R2);
                s    = 5.0*R.y*R.y;
                tmp += (*site)->moment(Data::MultipoleExpansion::XYY) * 3.0*R.x*(s -     R2);
                tmp += (*site)->moment(Data::MultipoleExpansion::YYY) *     R.y*(s - 3.0*R2);
                tmp += (*site)->moment(Data::MultipoleExpansion::YYZ) * 3.0*R.z*(s -     R2);
                s    = 5.0*R.z*R.z;
                tmp += (*site)->moment(Data::MultipoleExpansion::XZZ) * 3.0*R.x*(s -     R2);
                tmp += (*site)->moment(Data::MultipoleExpansion::YZZ) * 3.0*R.y*(s -     R2);
                tmp += (*site)->moment(Data::MultipoleExpansion::ZZZ) *     R.z*(s - 3.0*R2);
                esp += 0.5*tmp*ir7;
             }
         }   
        
         return esp;
      }

   } // end namespace Property

} // end namespace IQmol
