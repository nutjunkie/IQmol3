# IQmol3
IQmol version 3

This is IQmol, a molecular builder and visualization package written by Andrew
Gilbert.  IQmol is able to build molecules, set up and submit input for Q-Chem
calculations, and analyse the output.  Analyses include display of molecular
surfaces (densites, molecular orbitals) and animations of frequencies and 
reaction pathways.  A user guide can be found in the doc directory.

For an up-to-date list of features and pre-compiled binaries, please visit the 
website:  http://iqmol.org

This is a rebase of the original code that migrates to CMake and updates several
of the external libraries, including them as submodules in an attempt to ease the
build process.
