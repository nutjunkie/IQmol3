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

#include "Data/Data.h"
#include "Data/Mesh.h"

#include <QList>
#include <functional>



namespace IQmol {

   namespace AtomicDensity {
      class Base;
   }

   namespace Layer {
      class Molecule;
      class ProteinChain;
   }

   namespace Data {
      class GridData;
      class ShellList;
      class MultipoleExpansionList;
   }

   namespace Property {

      class Base 
      {
         public: 
            Base(QString const& text, Data::Mesh const* mesh = 0) : 
               m_mesh(mesh), m_text(text) { }

            virtual ~Base() { }

            QString text() const { return m_text; }
            void setText(QString const& text) { m_text = text; }

            void setMesh(Data::Mesh* mesh) { m_mesh = mesh; }

            virtual Data::Mesh::VertexFunction const& evaluator() = 0;

            virtual bool isAvailable() const { return m_mesh != 0; }

         protected:
            Data::Mesh const*  m_mesh;
            Data::Mesh::VertexFunction m_vertexFunction;

         private:
            QString m_text;
      };


      // - - - - - - - - - - Indexed - - - - - - - - - -
      // Base class for scalar properties that are defined by an index

      class Indexed : public Base
      {
         public:
            Indexed(QString const& text, Data::Mesh const* mesh = 0) : 
               Base(text, mesh) { }

            Data::Mesh::VertexFunction const& evaluator();

         protected:
            virtual void update() { }
            QList<double> m_map;
              
         private:
            double map(Data::Mesh::Vertex const&);
      };


      // - - - - - - - - - - AtomicNumber - - - - - - - - - -

      class AtomicNumber : public Indexed 
      {
         public:
            AtomicNumber(Layer::Molecule* molecule) : 
               Indexed("Atomic number"), m_molecule(molecule) { }
           
         protected:
            void update();

         private:
            Layer::Molecule* m_molecule;
      };


      // - - - - - - - - - - MeshIndex - - - - - - - - - -

      class MeshIndex : public Indexed
      {
        public:
           MeshIndex(QString const& label) : Indexed(label) { }
      };



      // - - - - - - - - - - Residue - - - - - - - - - -

      class Residue : public Indexed 
      {
         public:
            Residue(Layer::ProteinChain* chain) : 
               Indexed("Residue"), m_chain(chain) { }
           
         protected:
            void update();

         private:
            Layer::ProteinChain* m_chain;
      };


      // - - - - - - - - - - Spatial - - - - - - - - - -
      // Base class for scalar properties that are defined by position

      class Spatial : public Base
      {
         public:
            Spatial(QString const& text, Data::Mesh const* mesh = 0) : 
               Base(text, mesh) { }

            Data::Mesh::VertexFunction const& evaluator();

            Function3D const& function3D() const { return m_function; }

         protected:
            virtual void update() { }
            Function3D m_function;

         private:
            double function(Data::Mesh::Vertex const&);
      };


      // - - - - - - - - - - RadialDistance - - - - - - - - - -
      class RadialDistance : public Spatial
      {
         public:
            RadialDistance(Data::Mesh const* mesh = 0);

         private:
            double distance(double const x, double const y, double const z) const;
      };


      // - - - - - - - - - - GridBased - - - - - - - - - -
      class GridBased : public Spatial
      {
         public:
            GridBased(QString const& type, Data::GridData const& grid);

         private:
            Data::GridData const& m_grid;
            double evaluate(double const x, double const y, double const z) const;
      };


      // - - - - - - - - - - PromoleculeDensity - - - - - - - - - -

      class PromoleculeDensity : public Spatial
      {
         public:
            PromoleculeDensity(QString const& label, QList<AtomicDensity::Base*> atoms,
               QList<qglviewer::Vec> coordinates);

            ~PromoleculeDensity();

            bool isAvailable() const override;

            void boundingBox(qglviewer::Vec& min, qglviewer::Vec& max) const; 

         private:
            static const double s_thresh;
            double rho(double const x, double const y, double const z) const;
            QList<AtomicDensity::Base*> m_atomicDensities;
            QList<qglviewer::Vec> m_coordinates;
      }; 


      // - - - - - - - - - - PointChargePotential - - - - - - - - - -
      class PointChargePotential : public Spatial
      {
         public:
            PointChargePotential(Data::Type::ID tupe, Layer::Molecule* molecule, 
               Data::Mesh const* mesh = 0);

         protected:
            virtual void update();

         private:  
            Data::Type::ID m_type;
            Layer::Molecule* m_molecule;
            double potential(double const x, double const y, double const z) const;
            QList<double> m_charges;
            QList<qglviewer::Vec> m_coordinates;
      };


      // - - - - - - - - - - PointChargePotential - - - - - - - - - -

      class MultipolePotential : public Spatial
      {
         public:
            MultipolePotential(QString const& type, unsigned const order, 
               Data::MultipoleExpansionList const& siteList);

         private:
            unsigned m_order;
            Data::MultipoleExpansionList const& m_siteList;
            double potential(double const x, double const y, double const z) const;
      };

      // - - - - - - - - - - ComplexPhase - - - - - - - - - -

      class ComplexPhase : public Spatial
      {
         public:
            ComplexPhase(
               QString const& type, 
               Data::ShellList const& shells,
               Vector const& realCoefficients, 
               Vector const& imagCoefficients);

         private:
            Vector m_realCoefficients;
            Vector m_imagCoefficients;
            Data::ShellList const& m_shellList;
            double eval(double const x, double const y, double const z) const;
      };



   } // end namespace Property

}  // end namespace IQmol
