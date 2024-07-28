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

#include "Bank.h"
#include "Atom.h"

#include <QDebug>


namespace IQmol {
namespace Data {

   class AtomicProperty;

   /// Data structure representing an atom.
   class QmAtom : public Atom {

      friend class boost::serialization::access;

      public:
         QmAtom(unsigned const Z = 0, QString const& label = QString()) 
          :  Atom(Z,label) { }
         QmAtom(QString const& symbol, QString const& label = QString()) 
          : Atom(symbol,label) { };

         Type::ID typeID() const { return Type::QmAtom; }

		 /// Returns a reference to the requested AtomicProperty.  If the
		 /// property does not exist, then a new one is created and added to 
		 /// the list.
         template <class P>
         P& getProperty() 
         {
            P* p(0);
            Bank::iterator iter;
            for (iter = m_properties.begin(); iter != m_properties.end(); ++iter) {
                if ( (p = dynamic_cast<P*>(*iter)) ) break; 
            }
            if (p == 0) {
               p = new P();
               p->setDefault(m_atomicNumber);
               m_properties.append(p);
            }
            return *p;
         }

         template <class P>
         bool hasProperty() const
         {
            P* p(0);
            Bank::const_iterator iter;
            for (iter = m_properties.begin(); iter != m_properties.end(); ++iter) {
                if ( (p = dynamic_cast<P*>(*iter)) ) return true; 
            }
            return false;
         }

         template <class P>
         QString getLabel() { return getProperty<P>().label(); }
 
         void serialize(InputArchive& ar, unsigned const version = 0) 
         {
            privateSerialize(ar, version);
         }

         void serialize(OutputArchive& ar, unsigned const version = 0) 
         {
            privateSerialize(ar, version);
         }

         void dump() const
         {
            qDebug() << "Atom dump for Z =" << m_atomicNumber;
            m_properties.dump();
         }

      private:
         template <class Archive>
         void privateSerialize(Archive& ar, unsigned const) 
         {
            ar & m_atomicNumber;
            m_properties.serialize(ar);
         }
         Bank m_properties;
   };


   class QmAtomList : public List<QmAtom> 
   {
      public:
         // Why is a fully qualified namespace required for Type with Qt >= 6.0
         IQmol::Data::Type::ID typeID() const { return IQmol::Data::Type::QmAtomList; }
   };

} } // end namespace IQmol::Data

