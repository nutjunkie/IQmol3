#include "Parser.h"
//#include "Data/PdbData.h"


namespace IQmol {

namespace Data {
  class Pdb;
  class Gro;
  class Group;
  class Geometry;
  class ProteinChain;
}

namespace Parser {

   // Parses a PDB data file.  
   class Gro: public Base {

      public:
         bool parse(TextStream&);

      private:
         QString m_label;

         QMap<QString, Data::ProteinChain*> m_chains;
         QMap<QString, Data::Geometry*> m_geometries;

         bool parseATOM(QString const& line, Data::Group&);
   };

} } // end namespace IQmol::Parser
