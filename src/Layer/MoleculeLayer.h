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

#include "ComponentLayer.h"

#include "InfoLayer.h"
#include "AtomLayer.h"
#include "BondLayer.h"
//#include "ChargeLayer.h"

#include "EfpFragmentListLayer.h"
#include "MolecularSurfacesLayer.h"
#include "Configurator/MoleculeConfigurator.h"
#include "Configurator/SurfaceAnimatorDialog.h"

#include "Viewer/Animator.h"

#include <QMap>
#include <QFileInfo>

#include <functional>


class QUndoCommand;

namespace OpenBabel {
   class OBMol;
   class OBAtom;
   class OBBond;
}

namespace IQmol {

   QString const DefaultMoleculeName = "Untitled";

   namespace Process {
      class  JobInfo;
   }

   namespace Amber {
      class ParametrizeMoleculeDialog;
   }

   namespace Layer {

      class Isotopes;
      class Constraint;
      class Surface;
      class Group;
      class Charge;

      typedef QMap<OpenBabel::OBAtom*, Atom*>  AtomMap;
      typedef QMap<OpenBabel::OBBond*, Bond*>  BondMap;
      typedef QMap<OpenBabel::OBAtom*, Group*> GroupMap;
   
      // Container Layer for all things related to a molecule which is to
      // be treated quantum mechanically.

      // This is the main data structure for a molecule.  It contains the Primitve
      // GLObjects that visually represent the molecule along with Data Layers
      // containing information such as checkpoint files, cube files and output
      // files.
      class Molecule : public Component {
   
         Q_OBJECT
   
         friend class Configurator::Molecule;

         public:
            explicit Molecule(QObject* parent = 0);

            ~Molecule();

            double radius();
   
// Attempts to save the molecule, returning false if the operation
// was unsuccessful or canceled by the user.
// Can this be moved up the Layer heirarchy
bool save(bool prompt = false);
   
            // Appends the Layers in the DataList, but only if an existing Layer
            // of that kind does not exist already.
            void appendData(IQmol::Data::Bank&);
            void appendData(Layer::List&);

            bool sanityCheck();

            Process::JobInfo qchemJobInfo();
            void jobInfoChanged(Process::JobInfo const&);
   
            // Attempts to determine the best axis for the functional group when
            // converting an atom to a functional group (click on atom event)
            qglviewer::Vec getBuildAxis(Atom*);

            void minimizeEnergy(QString const& forcefield);
            void computeEnergy(QString const& forcefield);

            void symmetrize(double tolerance, bool updateCoordinates = true);

            static void toggleAutoDetectSymmetry() 
            { 
               s_autoDetectSymmetry = !s_autoDetectSymmetry; 
            }
   
            // Obtains a list of selected Primitives in the Molecule and removes
            // them from the lists.  The Primitives are not immediately deleted,
            // rather they are embedded in a RemovePrimtives Command so that the 
            // action can be undone.
            void deleteSelection();
            
            // Returns a list of the selected atoms and bonds.  If dangling bonds
            // is true then all selected bonds are included, otherwise only bonds
            // with both atoms selected are included.
            PrimitiveList getSelected(bool danglingBonds = false);
   
            /// Locates all atoms for which there exists a path to B without going through A.
            AtomList getContiguousFragment(Atom* A, Atom* B);
            Bond* getBond(Atom*, Atom*);
            BondList getBonds(Atom*);

            bool isModified() const { return m_modified; }
   
            /// Removes the specified Primitive(s) from the molecule, 
            /// but does not delete them. 
            void takePrimitives(PrimitiveList const&);
            void takePrimitive(Primitive*);
   
            /// Adds the specified Primitive(s) to the molecule.
            void appendPrimitives(PrimitiveList const&);
            void appendPrimitive(Primitive*);
   
            Constraint* findMatchingConstraint(AtomList const&);

            bool editConstraint();
            bool freezeAtomPositions();
            bool canAcceptConstraint(Constraint*);
            void addConstraint(Constraint*);

			// These simpy add or remove the Constraint layer to the model view
			// and are used by the undo commands.
            void addConstraintLayer(Constraint*);
            void removeConstraintLayer(Constraint*);

//            void addIsotopes(Isotopes*);
            void clearIsotopes();

            // Returns true if isotopes are actually added
            bool editIsotopes();
   
            /// Converts the Molecule to an XYZ format and uses OpenBabel to parse this.  
            /// Useful for, e.g., reperceiving bonds.
            QString coordinatesAsString(bool const selectedOnly = false);
            QString coordinatesAsStringFsm();
            QStringList coordinatesForCubeFile();

            QString isotopesAsString();
   
            /// Assigns the atom indices based on the ordering selected by 
            /// the user via the reorderIndex variable in the Atom class.
            void updateAtomOrder();

            Atom* createAtom(unsigned int const Z, qglviewer::Vec const& position);
            Bond* createBond(Atom* begin, Atom* end, int const order = 1);
            Charge* createCharge(double const q, qglviewer::Vec const& position);
   
            QList<qglviewer::Vec> coordinates();
            QList<int> atomicNumbers();
            QList<QString> atomicSymbols();

            QList<double> atomicCharges(Data::Type::ID type);
            void setGeometry(IQmol::Data::Geometry&);

            // There must be a better way of doing this, but this is used
            // in Layer::GeometryList for de
            bool isCurrentGeometry(Data::Geometry const* geometry) const 
            { 
               return m_currentGeometry == geometry;
            }
   
            void setReperceiveBondsForAnimation(bool tf) 
            {
                 m_reperceiveBondsForAnimation = tf;
            }

//            unsigned maxAtomicNumber() { return m_maxAtomicNumber; }

            void   setMullikenDecompositions(Matrix const& M);
            double mullikenDecomposition(int const a, int const b) const;
            bool   hasMullikenDecompositions() const;

            int totalCharge() const;
            int multiplicity() const;
   
         Q_SIGNALS:
            void multiplicityAvailable(unsigned);
            void chargeAvailable(int);
            void pointGroupAvailable(Data::PointGroup const&);
            void energyAvailable(double const, Info::EnergyUnit, QString const&);
            void dipoleAvailable(qglviewer::Vec const& dipole, bool const estimated);
            void radiusAvailable(double const radius);
            void centerOfNuclearChargeAvailable(qglviewer::Vec const&);
            void parameterFileAvailable(QString const&);
 
         public Q_SLOTS:
            void groupSelection();
            void ungroupSelection();
            void applyConstraint(Constraint*);
            void constraintUpdated();
            void removeConstraint();
            void selectAll();
            void selectAtoms(QList<int> const& indices);
            void createGeometryList();

            void openSurfaceAnimator();

            /// Passes the remove signal on so that the ViewerModel can deal with it
            void removeMolecule() { Component::removeMolecule(this); }
            void detectSymmetry();
            void autoDetectSymmetry();
            void invalidateSymmetry();
            void saveToCurrentGeometry();

            void addHydrogens();
            void updateInfo();
            void reindexAtomsAndBonds();
            void reperceiveBonds(bool postCmd);
            void reperceiveBondsSlot() { reperceiveBonds(true); }

            void reperceiveBondsForAnimation();
   
            /// Writes the molecule to the specified file.  The format is
            /// determined from the file extension and a Parser::IOError 
            /// exception is thrown if there are any problems.
            void writeToFile(QString const& filePath);

         protected:
            void updateAtomScale(double const scale);
            void updateSmallerHydrogens(bool smallerHydrogens);
            void updateHideHydrogens(bool hideHydrogens);
            void updateBondScale(double const scale);
            void updateChargeScale(double const scale);
            void updateDrawMode(Primitive::DrawMode const);
   
         private Q_SLOTS:
            void dumpData() { m_bank.dump(); }
            void setAtomicCharges(Data::Type::ID type);
            void updateAtomicCharges();
            void generateConformersDialog();
            void generateConformers();
            void parametrizeMoleculeDialog();
            void saveAs() { save(true); }

         private:
            static bool s_autoDetectSymmetry;

            QString constraintsAsString();
            QString scanCoordinatesAsString();
            QString efpFragmentsAsString();
            QString efpParametersAsString();
            QString externalChargesAsString();

            template <class T>
            QList<double> atomicCharges();

            qglviewer::Vec dipoleFromPointCharges();

            QList<double> zeroCharges();
            QList<double> gasteigerCharges();
   
            /// Translates the coordinates of all the atoms so that the constraint is satisfied.
            void applyPositionConstraint(Constraint*);
   
            /// Adjusts the coordinates of the Molecule so that the constraint is satisfied.
            void applyDistanceConstraint(Constraint*);
   
            /// Adjusts the coordinates of the Molecule so that the constraint is satisfied.
            void applyAngleConstraint(Constraint*);
   
            /// Adjusts the coordinates of the Molecule so that the constraint is satisfied.
            void applyTorsionConstraint(Constraint*);
   
            /// Used when a ring constraint is encountered.  In this case the
            /// atoms are not moved so that the constraint is exactly satisfied,
            /// rather we pass the constraint to the minmizer to sort things out.
            void applyRingConstraint();
   
            /// Note that these create new OBMol objects which must be deleted
            OpenBabel::OBMol* toOBMol(AtomMap*, BondMap*, GroupMap* = 0);
   
            /// Loads the molecular information from an OBMol object.  If existing
            /// Atom and Bond Maps are specified, only the additional primitives
            /// (e.g. added hydrogens) are returned, otherwise a complete list of
            /// added primtives is returned.  The returned primitive list can be
            /// used for UndoCommands.  Note that this does not load all the data
            /// in the OBMol (e.g. frequency data), for that, Parser::OpenBabel
            /// should be used.
   		    PrimitiveList fromOBMol(OpenBabel::OBMol*, AtomMap* = 0, BondMap* = 0, 
               GroupMap* = 0);

            template <class T>
            void update(std::function<void(T&)>);

            void clearData();
   
            void initProperties();
            void deleteProperties();

            void saveToGeometry(Data::Geometry&);
   
            /// State variable that determines how the Primitives are drawn (e.g.
            /// CPK or wireframe)
            Primitive::DrawMode m_drawMode;
   
            // Determines by how much the respective radii should be scaled
            // from their default value.
            double m_atomScale;
            double m_bondScale;
            double m_chargeScale;
   
            /// Determines if the hydrogen atoms are drawn smaller in the CPK model
            bool m_smallerHydrogens;
            bool m_hideHydrogens;


            bool m_modified;
            bool m_reperceiveBondsForAnimation;
   
            Configurator::Molecule m_configurator;
            IQmol::SurfaceAnimatorDialog  m_surfaceAnimator;
            Amber::ParametrizeMoleculeDialog* m_parametrizeMolecule;

            Layer::Info      m_info;
            Layer::Container m_atomList;
            Layer::Container m_bondList;
            Layer::Container m_chargesList;
            Layer::Container m_fileList;
            Layer::Container m_constraintList;
            Layer::Container m_isotopesList;
            Layer::Container m_scanList;
            Layer::Container m_groupList;
   
            Layer::EfpFragmentList m_efpFragmentList;
            Layer::MolecularSurfaces m_molecularSurfaces;
   
            Data::Bank m_bank;

            Data::Geometry* m_currentGeometry;
            Data::Type::ID m_chargeType;
            QAction* m_atomicChargesMenu;
//            unsigned m_maxAtomicNumber;
            QAction* m_addGeometryMenu;;

            Matrix m_mullikenDecompositions;
      };
   
   } // end namespace Layer



   typedef QList<Layer::Molecule*> MoleculeList;

} // end namespace IQmol
