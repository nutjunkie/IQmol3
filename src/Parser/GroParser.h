#include "Parser.h"


namespace IQmol {

namespace Data {
  class Pdb;
  class Gro;
  class Group;
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

         bool parseATOM(QString const& line, Data::Group&);
         bool parseCOMPND(QString const& line);
         bool parseCartoon(TextStream&);
         int  parseGRO(char const* pdbFilePath, Data::Gro *P , char *options);
   };

} } // end namespace IQmol::Parser
