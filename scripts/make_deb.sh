#! /bin/bash

   # Based on the tutorial at:
   # https://www.internalpointers.com/post/build-binary-deb-package-practical-guide
   # run from the build directory

   name=iqmol
   version=$(grep IQMOL_VERSION ../src/version.h | tr -d '"v' | awk '{print $3}') 
   revision=1
   arch=amd64
   package=${name}_${version}-${revision}_${arch}

   mkdir -p $package/DEBIAN
   mkdir -p $package/usr/local/bin
   mkdir -p $package/usr/share/$name

   cp IQmol $package/usr/local/bin/iqmol
   cp -R ../share/* $package/usr/share/$name/
   cp -R ../doc/IQmolUserGuide.pdf $package/usr/share/$name/

   ctrl=$package/DEBIAN/control
   arch=${arch//[_]/-}


cat > $ctrl <<- EOF
Package: $name
Version: $version-$revision
Architecture: $arch
Maintainer: Andrew Gilbert
Description: A molecular builder and visualisation package.
Depends: openbabel, libqt5printsupport5, libqt5sql5, libqt5network5, libqt5xml5, libqt5widgets5, libqt5gui5
EOF

   dpkg-deb --build --root-owner-group $package

