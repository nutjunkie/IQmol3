#! /bin/bash

#  Ref: https://www.internalpointers.com/post/build-binary-deb-package-practical-guide

EXE=bin/IQmol

if ! [ -f $EXE ]; then
   echo "ERROR: Could not find $EXE executable"
   echo "Please run this script from the build directory"
   exit
fi


#------------------------------------------------------------------------

get_dependencies()
{
   libs=$(ldd $EXE | awk '{print $1}' | grep -v linux-vdso)

   packages=()

   for lib in "${libs[@]}"
   do
      packages+=$(dpkg -S $lib | awk '{print $1}' | uniq)
   done

   depends=""
   for pkg in ${packages[@]}
   do
      pkg="${pkg%%:*}"
      depends+=" $pkg,"
   done

   depends=$(echo $depends | sed 's/,$//')

   echo $depends
}


setup_deb_dir()
{
   name=iqmol
   version=$(grep IQMOL_VERSION ../src/version.h | tr -d '"v' | awk '{print $3}') 
   revision=1
   arch=$(arch)
   arch=amd64
   pkgdir=${name}_${version}-${revision}_${arch}

   mkdir -p $pkgdir/usr/local/bin
   cp $EXE $pkgdir/usr/local/bin/$name

   dir=$pkgdir/usr/share/$name
   mkdir -p $dir
   cp -R ../share/* $dir/
   cp ../doc/IQmolUserGuide.pdf $dir/IQmolUserGuide.pdf

   dir=$pkgdir/usr/share/$name/openbabel
   mkdir -p $dir
   cp -R ../modules/openbabel/data/* $dir/


   dir=$pkgdir/usr/share/applications
   mkdir -p $dir
   cp ../resources/iqmol.desktop $dir/iqmol.desktop

   dir=$pkgdir/usr/share/icons/hicolor/512x512/apps
   mkdir -p $dir
   cp ../resources/IQmol.png $dir/iqmol.png

   arch=${arch//[_]/-}
   deps=$(get_dependencies)

   mkdir -p $pkgdir/DEBIAN
   cat > $pkgdir/DEBIAN/control <<- EOF
Package: $name
Version: $version-$revision
Architecture: $arch
Maintainer: Andrew Gilbert
Description: A molecular builder and visualisation package.
Depends: $deps
EOF

   echo $pkgdir
}

dir=$(setup_deb_dir)
dpkg-deb --root-owner-group --build $dir
echo 
echo "Install the package using the following command:"
echo "  sudo dpkg -i $dir.deb"
echo "Remove the package with the following command:"
echo "  sudo apt-get remove iqmol"
