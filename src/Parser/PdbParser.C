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

#include "PdbParser.h"
#include "CPdbParser.h"
#include "Data/CMesh.h"
#include "Layer/Cartoon.h"
#include "TextStream.h"

#include "Grid/MeshDecimator.h"

#include "Data/GridSize.h"
#include "Data/MacroMolecule.h"
#include "Data/Group.h"
#include "Data/Atom.h"
#include "Data/Mesh.h"

#include <QDebug>


namespace IQmol {
namespace Parser {


bool Pdb::parse(TextStream& textStream)
{
   bool ok(true);
   
   QMap<QString, Data::MacroMolecule*> chains;

   Data::MacroMolecule* chain(0);
   Data::Group* group(0);

   QString line, key;
   QString currentChain;
   QString currentGroup;

   while (!textStream.atEnd()) {
      line = textStream.nextLine();
      key  = line;

      key.resize(6);
      key = key.trimmed().toUpper();

      if (key == "ATOM" || key == "HETATM") {
         QString s;

         // Chain Id
         s = line.mid(21, 1);
         if (s != currentChain) {
            currentChain = s;
            if (chains.contains(currentChain)) {
               chain = chains[currentChain];
            }else {
               chain = new Data::MacroMolecule(currentChain);
               chains.insert(currentChain, chain);
            }
         }

         s = line.mid(22, 4); // Res seq
         if (s != currentGroup) {
            currentGroup = s;
            group = new Data::Group(s.trimmed() + " " +line.mid(17, 3).trimmed());
            chain->append(group);
         }

         ok = ok && parseATOM(line, *group);
         if (!ok) goto error;

      }else if (key == "COMPND") {
         parseCOMPND(line);
      } 
      
   }

   if (!chains.isEmpty()) {
      QList<Data::MacroMolecule*> list(chains.values());
      for (auto chain : chains.values()) m_dataBank.append(chain);
   } 
   
   // This must go last as it rewinds the TextStream!
   ok = parseCartoon(textStream);

   return ok;

   error:
      QString msg("Error parsing PDB file around line number ");
      msg +=  QString::number(textStream.lineNumber());
      msg +=  "\n" + line;
      m_errors.append(msg);
      return false;
}


bool Pdb::parseCOMPND(QString const& line)
{
  if (line.contains("MOLECULE")) {
     QString s(line);
     s.remove(0,20); 
     m_label += s;
  }   
  return true;
}


/* http://www.wwpdb.org/documentation/file-format-content/format33/sect9.html#ATOM

COLUMNS        DATA  TYPE    FIELD        DEFINITION
-------------------------------------------------------------------------------------
 1 -  6        Record name   "ATOM  "
 7 - 11        Integer       serial       Atom  serial number.
13 - 16        Atom          name         Atom name.
17 - 17        Character     altLoc       Alternate location indicator.
18 - 20        Residue name  resName      Residue name.
22 - 22        Character     chainID      Chain identifier.
23 - 26        Integer       resSeq       Residue sequence number.
27 - 30        AChar         iCode        Code for insertion of residues.
31 - 38        Real(8.3)     x            Orthogonal coordinates for X in Angstroms.
39 - 46        Real(8.3)     y            Orthogonal coordinates for Y in Angstroms.
47 - 54        Real(8.3)     z            Orthogonal coordinates for Z in Angstroms.
55 - 60        Real(6.2)     occupancy    Occupancy.
61 - 66        Real(6.2)     tempFactor   Temperature  factor.
77 - 78        LString(2)    element      Element symbol, right-justified.
79 - 80        LString(2)    charge       Charge  on the atom.
*/


bool Pdb::parseATOM(QString const& line, Data::Group& group)
{
   bool ok(true);

   QString key(line.mid(0,5));
   QString index(line.mid(6,10));

   double x = line.mid(30, 8).toFloat(&ok);
   if (!ok) return false;

   double y = line.mid(38, 8).toFloat(&ok);
   if (!ok) return false;

   double z = line.mid(46, 8).toFloat(&ok);
   if (!ok) return false;

   QString label(line.mid(12, 4).trimmed());
   QString sym(line.mid(76, 2).trimmed());
   
   Data::Atom* atom(new Data::Atom(sym, label));
   group.addAtom(atom, qglviewer::Vec(x,y,z));

   return ok;
}


std::vector<cpdb::Mesh> computeCartoonMesh(int nbChain, int *nbResPerChain, 
   float *CA_OPositions, char *ssTypePerRes) 
{
    std::vector<cpdb::Mesh> meshes(nbChain);
    if (nbChain <= 0) {
        return meshes;
    }

#if USEOMP
    #pragma omp parallel for num_threads(nbChain)
    for (int chainId = 0; chainId < nbChain; chainId++) {
        cpdb::Mesh m = cpdb::createChainMesh(chainId, nbResPerChain, CA_OPositions, ssTypePerRes);
        meshes[omp_get_thread_num()] = m;
    }
#else
    for (int chainId = 0; chainId < nbChain; chainId++) {
        cpdb::Mesh m = cpdb::createChainMesh(chainId, nbResPerChain, CA_OPositions, ssTypePerRes);
        meshes[chainId] = m;
    }
#endif

    return meshes;
}


Data::MeshList fromCpdb(std::vector<cpdb::Mesh> const& meshes) 
{
   Data::MeshList meshList;

   for (int i = 0; i < meshes.size(); i++) {
       Data::Mesh* mesh(new Data::Mesh);
       std::vector<v3> verts  = meshes[i].vertices;
       std::vector<v3> colors = meshes[i].colors;
       std::vector<Data::Mesh::Vertex> vertices;

       for (int j = 0; j < verts.size(); j++) {
           Data::Mesh::Vertex v(mesh->addVertex(verts[j].x, verts[j].y, verts[j].z));
           vertices.push_back(v);
           //colors[j].x, colors[j].y, colors[j].z);
       }   

       qDebug() << "Number of vertices" << vertices.size();

       std::vector<int> tri = meshes[i].triangles;
       for (unsigned j = 0; j < tri.size(); j += 3) {
           unsigned j0(tri[j  ]);
           unsigned j1(tri[j+1]);
           unsigned j2(tri[j+2]);

           // Avoid adding degenerate faces
           if (j0 != j1 && j0 != j2 && j1 != j2) {
              Data::Mesh::Vertex v0(vertices[j0]);
              Data::Mesh::Vertex v1(vertices[j1]);
              Data::Mesh::Vertex v2(vertices[j2]);
              mesh->addFace(v0, v1, v2);
           }
       }   
       mesh->computeFaceNormals();
       mesh->computeVertexNormals();

       double delta(Data::GridSize::stepSize(2));
       MeshDecimator decimator(*mesh);
       if (!decimator.decimate(delta)) {
          qDebug() <<"Mesh decimation failed: " + decimator.error();
       }
       meshList.append(mesh);
   }   

   return meshList;
}


bool Pdb::parseCartoon(TextStream& textStream)
{
   textStream.rewind(); 
   qDebug() << "In the cartoon PDB parser";

   Data::pdb *P;
   P = cpdb::initPDB();

   std::string str(m_filePath.toStdString());
   const char* fname(str.c_str());

   cpdb::parsePDB(fname, P, (char *)"");

   std::vector<int> nbResPerChain(P->size);
   std::vector<float> CAOPos;
   std::vector<char> allSS;

   for (int chainId = 0; chainId < P->size; chainId++) {
       Data::chain *C = &P->chains[chainId];
       nbResPerChain[chainId] = C->size;
       for (int r = 0; r < C->size; r++) {
           Data::residue *R = &C->residues[r];
           Data::atom *CA = cpdb::getAtom(*R, (char *)"CA");
           Data::atom *O = cpdb::getAtom(*R, (char *)"O");
           char ss = R->ss;

           if (CA == NULL || O == NULL) {
               QString msg("CA or O not found in chain ");
               msg + C->id + " residue " + R->type + "_"  + R->id;
               m_errors.append(msg);
               return false;
           }   
           CAOPos.push_back(CA->coor.x);
           CAOPos.push_back(CA->coor.y);
           CAOPos.push_back(CA->coor.z);
           CAOPos.push_back(O->coor.x);
           CAOPos.push_back(O->coor.y);
           CAOPos.push_back(O->coor.z);
           allSS.push_back(ss);
       }   
   }   

   std::vector<cpdb::Mesh> meshes = 
      computeCartoonMesh(P->size, &nbResPerChain[0], &CAOPos[0], &allSS[0]);

   Data::MeshList list(fromCpdb(meshes));
   for (auto mesh : list) m_dataBank.append(mesh);

   return true;
}


} } // end namespace IQmol::Parser
