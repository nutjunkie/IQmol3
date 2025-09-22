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

#include "Data.h"
#include "QGLViewer/vec.h"


namespace IQmol {
namespace Data {

   class Constraint : public Base {

      public:
         Type::ID typeID() const { return Type::Constraint; }

         Constraint() : m_value(0.0) { }
         virtual ~Constraint();

         QList<unsigned> const& atomIndices() const { return m_atomIndices; }

         void setValue(double const value) { m_value = value; }
         double value() const;

         virtual void dump() const;

      protected:
         QList<unsigned> m_atomIndices;
         double m_value;
   };



   class PositionConstraint : public Constraint {


      public:
         Type::ID typeID() const { return Type::PositionConstraint; }

         PositionConstraint() { }

         PositionConstraint(unsigned const atomIndex, qglviewer::Vec const& position);

         qglviewer::Vec const& position() const { return m_position; }

         void dump() const;

      private:
         unsigned m_atomIndex;
         qglviewer::Vec m_position;
   };



   class DistanceConstraint : public Constraint {

      public:
         Type::ID typeID() const { return Type::DistanceConstraint; }

         DistanceConstraint() { }

         DistanceConstraint(unsigned const atomIndexA, unsigned const atomIndexB,
            double const distance);

         void dump() const;
   };


   class AngleConstraint : public Constraint {

      public:
         Type::ID typeID() const { return Type::AngleConstraint; }

         AngleConstraint() { }

         AngleConstraint(unsigned const atomIndexA, unsigned const atomIndexB,
            unsigned const atomIndexC, double const angle);

         void dump() const;
   };


   class TorsionConstraint : public Constraint {

      public:
         Type::ID typeID() const { return Type::TorsionConstraint; }

         TorsionConstraint() { }

         TorsionConstraint(unsigned const atomIndexA, unsigned const atomIndexB,
            unsigned const atomIndexC, unsigned const atomIndexD, double const torsion);

         void dump() const;
   };


   class FrozenAtomsConstraint : public Constraint {


      public:
         Type::ID typeID() const { return Type::FrozenAtomsConstraint; }

         FrozenAtomsConstraint() { }

         FrozenAtomsConstraint(QList<unsigned> const& atomIndices, 
           QList<qglviewer::Vec> const& position);

         QList<qglviewer::Vec> const& positions() const { return m_positions; }

         void dump() const;

      private:
         QList<unsigned> m_atomIndices;
         QList<qglviewer::Vec> m_positions;
   };


} } // end namespace IQmol::Data
