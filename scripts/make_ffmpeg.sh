#! /bin/bash

#
#  Complete process for building a statically linked ffmpeg binary
#

export CC=/opt/homebrew/bin/gcc-13
export CXX=/opt/homebrew/bin/g++-13


build_zlib()
{
   echo "Building zlib"
   local cwd=$PWD
   git clone --depth 1 https://github.com/madler/zlib.git

   cd zlib
   ./configure --static --prefix=$extlibs
   make -j$(sysctl -n hw.ncpu) install
   cd $cwd
}


build_x264()
{
   echo "Building libx264"
   local cwd=$PWD
   git clone --depth 1 https://code.videolan.org/videolan/x264.git
   
   cd x264
   ./configure --prefix=$extlibs --enable-static --disable-asm --disable-opencl --disable-cli
   make -j$(sysctl -n hw.ncpu) install
   cd $cwd
}


patch_config()
{
   echo "Patching configure script"
   FILE=configure
   if [[ "$(uname)" == "Darwin" ]]; then
      # macOS requires empty string after -i
      sed -i '' 's|^for LATOMIC in "-latomic" ""; do|for LATOMIC in "" "-latomic" ; do|' "$FILE"
   else
      # Linux sed works with just -i
      sed -i 's|^for LATOMIC in "-latomic" ""; do|for LATOMIC in "" "-latomic" ; do|' "$FILE"
   fi
}


build_ffmpeg()
{
   echo "Building ffmpeg"
   local cwd=$PWD
   git clone --depth 1 https://github.com/FFmpeg/FFmpeg.git 
 
   cd FFmpeg
   patch_config
   ./configure --cc=$CC --cxx=$CXX --prefix=$extlibs --pkg-config-flags="--static" \
               --extra-cflags="-I$extlibs/include" --extra-ldflags="-L$extlibs/lib" \
               --disable-everything --disable-shared --enable-static --enable-protocol=file \
               --enable-gpl --enable-decoder=mjpeg --enable-demuxer=image2 --enable-muxer=mp4 \
               --enable-muxer=avi --enable-encoder=libx264 --enable-encoder=mpeg4 \
               --enable-filter=scale --enable-libx264 --disable-xlib --disable-iconv 
   make -j$(sysctl -n hw.ncpu) install
   cd $cdw
}


extlibs=$PWD/extlibs
mkdir -p $extlibs

echo "Building zlib"
build_zlib
build_x264
build_ffmpeg

echo ""
echo "Checking static linking:"

if [[ "$(uname)" == "Darwin" ]]; then
   otool -L $extlibs/bin/ffmpeg
else
   ldd $extlibs/bin/ffmpeg
fi
