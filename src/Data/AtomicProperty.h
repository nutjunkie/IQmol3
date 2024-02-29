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


namespace IQmol {
namespace Data {

   /// Base class for properties of atoms.  This can include things like
   /// charges, masses, chemical shifts and DMA expansions. 
   class AtomicProperty : public Base 
   {
      public:
         virtual Type::ID typeID() const { return Type::AtomicProperty; }
         virtual void setDefault(int const /* Z */) { }
         virtual QString label() const = 0;
   };


   class AtomicNumber : public AtomicProperty 
   {

      friend class boost::serialization::access;

      public:
         virtual Type::ID typeID() const { return Type::AtomicNumber; }

         AtomicNumber(int const Z = 0) { setDefault(Z); }
         void setDefault(int const Z) { m_atomicNumber = Z; }

         QString label() const { return QString::number(m_atomicNumber); }
         int value() const { return m_atomicNumber; }
         void setValue(int const Z) { m_atomicNumber = Z; }
         void dump() const { };

         void serialize(InputArchive& ar, unsigned int const version = 0) {
            Q_UNUSED(version);
            ar & m_atomicNumber;
         }
         void serialize(OutputArchive& ar, unsigned int const version = 0) {
            Q_UNUSED(version);
            ar & m_atomicNumber;
         }

      private:
         int m_atomicNumber;
   };


   class StringProperty : public AtomicProperty 
   {
      friend class boost::serialization::access;

      public:
         StringProperty() : m_value("") { }

         QString label() const { return m_value; }
         QString value() const { return m_value; }
         void setValue(QString const& value) { m_value = value; }

         void serialize(InputArchive& ar, unsigned int const version = 0) {
            Q_UNUSED(version);
            ar & m_value;
         }

         void serialize(OutputArchive& ar, unsigned int const version = 0) {
            Q_UNUSED(version);
            ar & m_value;
         }

      protected:
         QString m_value;
   };


   class AtomicSymbol : public StringProperty 
   {
      public:
         virtual Type::ID typeID() const { return Type::AtomicSymbol; }
         AtomicSymbol(int const Z = 0) { setDefault(Z); }
         void setDefault(int const Z);
   };


   class AtomicLabel : public StringProperty 
   {
      public:
         virtual Type::ID typeID() const { return Type::AtomicLabel; }
   };



   class ScalarProperty : public AtomicProperty 
   {
      friend class boost::serialization::access;

      public:
         ScalarProperty() : m_value(0.0) { }

         QString label() const { return QString::number(m_value, 'f', 3); }
         double value() const { return m_value; }
         void setValue(double const value) { m_value = value; }
         void dump() const;

         void serialize(InputArchive& ar, unsigned int const version = 0) {
            Q_UNUSED(version);
            ar & m_value;
         }

         void serialize(OutputArchive& ar, unsigned int const version = 0) {
            Q_UNUSED(version);
            ar & m_value;
         }

      protected:
         double m_value;
   };


   class Mass: public ScalarProperty 
   {
      public:
         virtual Type::ID typeID() const { return Type::Mass; }
         void setDefault(int const Z);
   };


   class VdwRadius : public ScalarProperty 
   {
      public:
         virtual Type::ID typeID() const { return Type::VdwRadius; }
         void setDefault(int const Z);
   };


   class SpinDensity : public ScalarProperty 
   {
      public:
         virtual Type::ID typeID() const { return Type::SpinDensity; }
   };


   class AtomicCharge : public ScalarProperty 
   {
      public:
         virtual Type::ID typeID() const { return Type::AtomicCharge; }
   };


   class GasteigerCharge : public AtomicCharge 
   {
      public:
         virtual Type::ID typeID() const { return Type::GasteigerCharge; }
   };

 
   class MullikenCharge : public AtomicCharge 
   {
      public:
         virtual Type::ID typeID() const { return Type::MullikenCharge; }
   };


   class MultipoleDerivedCharge : public AtomicCharge 
   {
      public:
         virtual Type::ID typeID() const { return Type::MultipoleDerivedCharge; }
   };


   class ChelpgCharge : public AtomicCharge 
   {
      public:
         virtual Type::ID typeID() const { return Type::ChelpgCharge; }
   };


   class HirshfeldCharge : public AtomicCharge 
   {
      public:
         virtual Type::ID typeID() const { return Type::HirshfeldCharge; }
   };


   class LowdinCharge : public AtomicCharge 
   {
      public:
         virtual Type::ID typeID() const { return Type::LowdinCharge; }
   };


   class Cm5Charge : public AtomicCharge 
   {
      public:
         virtual Type::ID typeID() const { return Type::Cm5Charge; }
   };


   class NaturalCharge : public AtomicCharge 
   {
      public:
         virtual Type::ID typeID() const { return Type::NaturalCharge; }
   };


   class MerzKollmanEspCharge : public AtomicCharge 
   {
      public:
         virtual Type::ID typeID() const { return Type::MerzKollmanEspCharge; }
   };


   class MerzKollmanRespCharge : public AtomicCharge 
   {
      public:
         virtual Type::ID typeID() const { return Type::MerzKollmanRespCharge; }
   };


   class NmrShielding : public ScalarProperty 
   {
      public:
         virtual Type::ID typeID() const { return Type::NmrShielding; }
   };


   class NmrShift : public ScalarProperty 
   {
      public:
         virtual Type::ID typeID() const { return Type::NmrShift; }
   };


   class AtomColor : public AtomicProperty 
   {
      friend class boost::serialization::access;

      public:
         virtual Type::ID typeID() const { return Type::AtomColor; }

         void setDefault(int const Z);
         QString label() const { return QString(); }
         const double* asArray() const { return m_color; }
         QColor get() const;
         void set(double const red, double const green, double const blue);
         void dump() const;

         void serialize(InputArchive& ar, unsigned int const version = 0) {
            Q_UNUSED(version);
            ar & m_color;
         }

         void serialize(OutputArchive& ar, unsigned int const version = 0) {
            Q_UNUSED(version);
            ar & m_color;
         }

      protected:
         double m_color[4];
   };

} } // end namespace IQmol::Data
