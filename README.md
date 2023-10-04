# IQmol3
IQmol version 3

This is IQmol, a molecular builder and visualization package written by Andrew
Gilbert.  IQmol is able to build molecules, set up and submit input for Q-Chem
calculations, and analyse the output.  Analyses include display of molecular
surfaces (densities, molecular orbitals) and animations of frequencies and 
reaction pathways.  A user guide can be found in the doc directory.

For an up-to-date list of features and pre-compiled binaries, please visit the 
website:  http://iqmol.org

This is a rebase of the original code that migrates to CMake and updates several
of the external libraries, including them as submodules in an attempt to ease the
build process.

The source relies on several submodules, so to checkout the code use the recursive flag:

```
git clone --recursive https://github.com/nutjunkie/IQmol3.git
```

To compile, make sure that you QT installation can be found by cmake.  This
means that the CMAKE\_PREFIX\_PATH environment variable should include the
directory containing the Qt5Config.cmake file
```
export CMAKE_PREFIX_PATH=/directory/containing_Qt5Config.cmake
./configure
cd build
make
```
