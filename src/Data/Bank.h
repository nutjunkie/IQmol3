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
#include "DataFactory.h"


namespace IQmol {
namespace Data {

   // Class representing and managing a list of Data objects.   The load
   // functions makes use of the Data::Factory to create the required objects
   // based on the saved TypeIDs.
   // 
   // Note that the Bank has ownership of the objects and deletes them in the
   // dtor.  

   class Bank : public Base, public QList<Base*> {

	  public: 
         Bank() : m_deleteContents(true) { }
         ~Bank();
         IQmol::Data::Type::ID typeID() const { return IQmol::Data::Type::Bank; }

         Bank(Bank const& that) : Base(), QList<Base*>() { copy(that); }

         Bank& operator=(Bank const& that) {
            if (this != &that) copy(that);
            return *this;
         }

         /// Moves the data from that Bank to this Bank.  After a merge, that
         /// will have no data.
         void merge(Bank& that) {
            while (!that.isEmpty()) { append(that.takeFirst()); }
         }

         /// Sets whether or not to delete the contents of the Bank 
         /// when the destructor is called.  The default is to delete
         void setDeleteContents(bool tf) { m_deleteContents = tf; }

         template <class T>
         QList<T*> findData() {
            QList<T*> list;
            T* t(0);
            for (int i = 0; i < size(); ++i) {
                if ( (t = dynamic_cast<T*>(value(i))) ) list.append(t);
            }
            return list;
         }

         template <class T>
         QList<T*> takeData() {
            QList<T*> list(findData<T>());
            for (int i = 0; i < list.size(); ++i) {
                removeAll(list[i]);
            }
            return list;
         }

	 /// Used to remove and delete all Data objects of a given type from
          //the Bank
         template <class T>
         void deleteData() {
            QList<T*> list(findData<T>());
            for (int i = 0; i < list.size(); ++i) {
                removeAll(list[i]);
                delete list[i];
            }
         }

         void dump() const;

      private:
         bool m_deleteContents;
         //Bank(Bank const&);

         void copy(Bank const& that) {
            Base* base;
            for (int i = 0; i < that.size(); ++i) {
                base = Factory::instance().create(that[i]->typeID());
                *base = *that[i]; 
                append(base);
            }
         }
   };

} } // end namespace IQmol::Data
