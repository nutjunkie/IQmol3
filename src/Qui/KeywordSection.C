/*!
 *  \file KeywordSection.C 
 *
 *  \brief Non-inline member functions of the KeywordSection class, see
 *  KeywordSection.h for details.
 *   
 *  \author Andrew Gilbert
 *  \date January 2008
 */

#include "RemSection.h"
#include "OptSection.h"
#include "MoleculeSection.h"
#include "ExternalChargesSection.h"
#include "LJParametersSection.h"
#include "KeyValueSection.h"


namespace Qui {

// Factory - we use this to generate a derived KeywordSection based on its
// name.  Note that if you add additional specialized KeywordSections, this
// Factory needs to be made aware of them.
KeywordSection* KeywordSection::Factory(QString const& type) 
{
   QString t(type.toLower());

   if (t == "molecule") {
      return new MoleculeSection();
   }else if (t == "rem") {
      return new RemSection();
   }else if (t == "opt") {
      return new OptSection();
   }else if (t == "external_charges") {
      return new ExternalChargesSection();
   }else if (t == "lj_parameters") {
      return new LJParametersSection();
   }else if (t == "pcm") {
      return new KeyValueSection("pcm");
   }else if (t == "solvent") {
      return new KeyValueSection("solvent");
   }else if (t == "smx") {
      return new KeyValueSection("smx");
   }else if (t == "chemsol") {
      return new KeyValueSection("chemsol");
   }else if (t == "geom_opt") {
      return new KeyValueSection("geom_opt");
   }else {
      return new KeywordSection(t);
   }
}

} // end namespace Qui
