#include "Parser.h"
#include "Data/ProteinChain.h"


namespace IQmol {

namespace Data {
  class Pdb;
  class Gro;
  class Group;
  class Geometry;
  class ProteinChain;
  class Solvent;
  class Atom;
  
}

namespace Parser {


   // Parses a PDB data file.  
   class Gro: public Base {

      public:
         bool parse(TextStream&);
         bool parseChain(TextStream&);

      private:
         QString m_label,m_prefix,m_currentChain,m_currentGroup,m_aToZ,m_secStrucKey,m_topolLine,m_secStrucLine;
         int m_chainNumber,m_topolCount,m_secStrucIndex,m_grpNumber;
         float m_atomCharge;
         std::vector<QString> m_secStrucChain;
         std::vector<int>m_secStrucType,m_secStrucResStart,m_secStrucResStop;
         std::vector<float> m_chain_charges;
         Vec3 m_vCA;
         Vec3 m_vO;
         std::vector<QString> m_topologies;
         Data::ProteinChain* m_chain;
         Data::Solvent* m_solvent;
         Data::Group* m_group;
         Data::Atom* m_CA;
         Data::Atom* m_O;




         QMap<QString, Data::ProteinChain*> m_chains;
         QMap<QString, Data::Geometry*> m_geometries;

         
         bool parseATOM(QString const& line, Data::Group&, float atomcharge);
   };

} } // end namespace IQmol::Parser
