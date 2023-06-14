/*!
 *  \file Job.C 
 *
 *  \brief Non-inline member functions of the Job class, see Job.h for details.
 *   
 *  \author Andrew Gilbert
 *  \date November 2008
 */

#include "QuiJob.h"
#include "OptionDatabase.h"
#include "Option.h"
#include "RemSection.h"
#include "MoleculeSection.h"
#include "KeyValueSection.h"
#include "ExternalChargesSection.h"

#include <QDebug>

namespace Qui {



Job::Job() {
   m_remSection           = new RemSection();
   m_moleculeSection      = new MoleculeSection();
   m_solventSection       = new KeyValueSection("solvent",false);
   m_pcmSection           = new KeyValueSection("pcm",false);
   m_smxSection           = new KeyValueSection("smx",false);
   m_chemsolSection       = new KeyValueSection("chemsol",false);
   m_geomOptSection       = new KeyValueSection("geom_opt",false, false);

   m_sections["rem"]      = m_remSection;
   m_sections["molecule"] = m_moleculeSection;
   m_sections["solvent"]  = m_solventSection;
   m_sections["pcm"]      = m_pcmSection;
   m_sections["smx"]      = m_smxSection;
   m_sections["chemsol"]  = m_chemsolSection;
   m_sections["geom_opt"] = m_geomOptSection;

   // A hack to ensure the corresponding $blocks have something inside them
   // when they are printed.
   setOption("QUI_SOLVENT_DIELECTRIC", "78.39");
   setOption("QUI_PCM_THEORY", "CPCM");
   setOption("QUI_SMX_SOLVENT", "water");
   setOption("QUI_CHEMSOL_EFIELD", "Exact");
}


void Job::addSections(std::vector<KeywordSection*> sections) {
   std::vector<KeywordSection*>::iterator iter;
   for (iter = sections.begin(); iter != sections.end(); ++iter) {
       addSection(*iter);
   }
}


Job::~Job() {
   destroy();
}


void Job::destroy() 
{
   //qDebug() << "WARN: Job::Destroy() called";
   QMap<QString,KeywordSection*>::iterator iter;
   for (iter = m_sections.begin(); iter != m_sections.end(); ++iter) {
       delete iter.value();
   }
   m_sections.clear();
}


void Job::copy(Job const& that) 
{
   destroy();

   QMap<QString,KeywordSection*>::const_iterator iter;
   for (iter = that.m_sections.begin(); iter != that.m_sections.end(); ++iter) {
       addSection(iter.value()->clone());
   }

   // Make sure we have the necessary sections.
   QMap<QString,KeywordSection*>::iterator iter2;

   iter2 = m_sections.find("rem");
   if (iter2 == m_sections.end()) addSection(new RemSection());

   iter2 = m_sections.find("molecule");
   if (iter2 == m_sections.end()) addSection(new MoleculeSection());

}


//! Adds the given section to the Job object, deleting any existing section of
//! the same name
void Job::addSection(KeywordSection* section) {
   QString name(section->name());
   QMap<QString,KeywordSection*>::iterator iter(m_sections.find(name));

   if (iter != m_sections.end()) {
      delete iter.value();
      m_sections.erase(iter);
   }

   m_sections.insert(name, section);

   if (name == "rem") {
      m_remSection = dynamic_cast<RemSection*>(section);
   }else if (name == "molecule") {
      m_moleculeSection = dynamic_cast<MoleculeSection*>(section);
   }else if (name == "solvent") {
      m_solventSection = dynamic_cast<KeyValueSection*>(section);
   }else if (name == "pcm") {
      m_pcmSection = dynamic_cast<KeyValueSection*>(section);
   }else if (name == "smx") {
      m_smxSection = dynamic_cast<KeyValueSection*>(section);
   }else if (name == "chemsol") {
      m_chemsolSection = dynamic_cast<KeyValueSection*>(section);
   }
}
 

StringMap Job::getOptions()  
{
   StringMap opts;
   StringMap::const_iterator iter;

   if (m_remSection) opts = m_remSection->getOptions();

   // QMap::unite does not work here for some reason 
   QList<KeyValueSection*> sections;
   sections.append(m_solventSection);
   sections.append(m_pcmSection);
   sections.append(m_chemsolSection);
   sections.append(m_smxSection);

   for (int i = 0; i < sections.size(); ++i) {
       if (sections[i]) {
          StringMap const& map(sections[i]->getOptions());
          for (iter = map.begin(); iter != map.end(); ++iter) {
              opts.insert(iter.key(), iter.value());
          }
       }
   }

   return opts;
}


QString Job::getOption(QString const& name) 
{
   if (m_remSection) {
      return m_remSection->getOption(name);
   }else {
      return QString();
   }
}


bool Job::isReadCoordinates() 
{
   if (m_moleculeSection) {
      return  m_moleculeSection->isReadCoordinates();
   }else { 
      return false;
   }
}


void Job::setCharge(int value) 
{
   if (m_moleculeSection) m_moleculeSection->setCharge(value);
}


void Job::setMultiplicity(int value) 
{
   if (m_moleculeSection) m_moleculeSection->setMultiplicity(value);
}


void Job::setCoordinates(QString const& coords) 
{
   qDebug() << "set coordinates";
   if (m_moleculeSection) {
      qDebug() << " has molecule";
      m_moleculeSection->setCoordinates(coords);
   }
}


void Job::setCoordinatesFsm(QString const& coords) 
{
   qDebug() << "setCoordinatesFsm";
   if (m_moleculeSection) m_moleculeSection->setCoordinatesFsm(coords);
}


void Job::setConstraints(QString const& constraints) 
{
   addSection("opt", constraints);
}


void Job::setScanCoordinates(QString const& scan) 
{
   addSection("scan", scan);
}



void Job::setEfpFragments(QString const& efpFragments) 
{
   qDebug() << "adding efpFragments type" ;
   KeywordSection* efp = addSection("efp_fragments", efpFragments);
   efp->visible(!efpFragments.isEmpty());
}


void Job::setEfpParameters(QString const& efpParameters) 
{
   KeywordSection* efp = addSection("efp_parameters", efpParameters);
   efp->visible(!efpParameters.isEmpty());
}


void Job::setExternalCharges(QString const& charges) 
{
   if (charges.isEmpty()) return;
   ExternalChargesSection* externalCharges(new ExternalChargesSection(charges));
   addSection(externalCharges);
   externalCharges->visible(!charges.isEmpty());
}


void Job::setGenericSection(QString const& name, QString const& contents)
{
   KeywordSection* section = addSection(name, contents);
   section->visible(false);
}


void Job::setOption(QString const& name, QString const& value) 
{
   // Note these prefixes need to be stripped in KeyValueSection::setOption
   if (name.startsWith("QUI_SOLVENT_", Qt::CaseInsensitive)) {
      if (m_solventSection) m_solventSection->setOption(name, value);

   }else if (name.startsWith("QUI_PCM_", Qt::CaseInsensitive)) {
      if (m_pcmSection) m_pcmSection->setOption(name, value);

   }else if (name.startsWith("QUI_SMX_", Qt::CaseInsensitive)) {
      if (m_smxSection) m_smxSection->setOption(name, value);

   }else if (name.startsWith("QUI_CHEMSOL_", Qt::CaseInsensitive)) {
      if (m_chemsolSection) m_chemsolSection->setOption(name, value);

   }else if (name.startsWith("QUI_GEOM_OPT", Qt::CaseInsensitive)) {
      if (m_geomOptSection) m_geomOptSection->setOption(name, value);

   }else if (m_remSection) {
      m_remSection->setOption(name, value);
      if (name.toUpper() == "JOB_TYPE" && m_moleculeSection) {
         m_moleculeSection->setFsm(value.toLower().contains("freezing string"));
      }
   }
}


void Job::printOption(QString const& name, bool doPrint) 
{
   if (name.startsWith("QUI_SOLVENT")) {
      if (m_solventSection) m_solventSection->printOption(name, doPrint);

   }else if (name.startsWith("QUI_PCM")) {
      if (m_pcmSection) m_pcmSection->printOption(name, doPrint);

   }else if (name.startsWith("QUI_SMX", Qt::CaseInsensitive)) {
      if (m_smxSection) m_smxSection->printOption(name, doPrint);

   }else if (name.startsWith("QUI_CHEMSOL", Qt::CaseInsensitive)) {
      if (m_chemsolSection) m_chemsolSection->printOption(name, doPrint);

   }else if (name.startsWith("QUI_GEOM_OPT", Qt::CaseInsensitive)) {
      if (m_geomOptSection) m_geomOptSection->printOption(name, doPrint);

   }else {
      if (m_remSection) m_remSection->printOption(name, doPrint);
   }
}


KeywordSection* Job::addSection(QString const& name, QString const& value) 
{
   qDebug() << "adding section" ;
   KeywordSection* section(KeywordSection::Factory(name));
   section->read(value);
   addSection(section);
   return section;
}


QString Job::getComment() 
{
   KeywordSection* comment = getSection("comment");
   return comment ? comment->formatContents() : QString();
}


void Job::setComment(QString const& s) 
{
   KeywordSection* comment = getSection("comment");
   if (comment) {
      comment->read(s);
   }else {
      addSection("comment", s);
   }
   setOption("QUI_TITLE", s);
}



//! Note that this function return a null pointer if no KeywordSection of the
//! given name exists.
KeywordSection* Job::getSection(QString const& name) {
   QMap<QString,KeywordSection*>::iterator iter(m_sections.find(name));
   if (iter != m_sections.end()) {
      return m_sections[name];
   }else {
      return 0;
   }  
}


QString Job::getCoordinates() {
   if (m_moleculeSection) {
      return m_moleculeSection->getCoordinates();
   }else {
      return QString();
   }
}


int Job::getNumberOfAtoms() {
   if (m_moleculeSection) {
      return m_moleculeSection->getNumberOfAtoms();
   }else {
      return 0;
   }
}


void Job::printSection(QString const& name, bool isVisible) {
   if (isVisible && m_sections.count(name) == 0) {
      // if we should print a section then there should be one there.
      addSection(KeywordSection::Factory(name)); 
   }
   if (m_sections.count(name)) m_sections[name]->visible(isVisible);
}


QString Job::format(bool const preview) 
{
   QMap<QString,KeywordSection*>::iterator iter, 
      begin(m_sections.begin()), end(m_sections.end());

   QString s;

   // Ensure standard ordering of common sections
   iter = m_sections.find("comment");
   if (iter != end) s += iter.value()->format(preview);
   iter = m_sections.find("molecule");
   if (iter != end) s += iter.value()->format(preview);
   iter = m_sections.find("rem");
   if (iter != end) s += iter.value()->format(preview);

   for (iter = begin; iter != end; ++iter) {
	   QString name = iter.key();
	   if (name != "comment" && name != "molecule" && name != "rem") {
           s += iter.value()->format(preview);
	   }
   }

   return s;
}

} // end namespace Qui
