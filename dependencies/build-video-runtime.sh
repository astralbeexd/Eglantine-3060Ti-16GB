#!/usr/bin/env bash
set -euo pipefail
mkdir -p security-runtime/sources security-runtime/install
cd security-runtime
curl --fail --location --retry 2 https://ffmpeg.org/releases/ffmpeg-9.0.2.tar.xz -o ffmpeg-9.0.2.tar.xz
echo '8c3850283eb25fa026482078a04051e0be17347b09ef81a0849bec15a96e002e  ffmpeg-9.0.2.tar.xz' | sha256sum --check
tar -xJf ffmpeg-9.0.2.tar.xz -C sources
cd sources/ffmpeg-9.0.2
patch --dry-run -p1 < ../../../dependencies/patches/ffmpeg-hdr-cve-2026-107676.patch
patch -p1 < ../../../dependencies/patches/ffmpeg-hdr-cve-2026-107676.patch
./configure --prefix="$(cygpath -am ../../install)" \
  --target-os=mingw32 --arch=x86_64 --cc=gcc --cxx=g++ \
  --disable-autodetect --disable-everything --disable-programs --disable-doc \
  --disable-debug --disable-static --enable-shared --disable-network \
  --disable-avdevice --disable-avfilter --disable-avformat \
  --enable-avcodec --enable-avutil --enable-swresample --enable-swscale \
  --enable-libdav1d --enable-ffnvcodec --enable-cuda --enable-d3d11va --enable-dxva2 \
  --enable-decoder=h264,hevc,vp8,vp9,av1,libdav1d \
  --enable-parser=h264,hevc,vp8,vp9,av1 \
  --enable-hwaccel=h264_nvdec,hevc_nvdec,vp8_nvdec,vp9_nvdec,av1_nvdec,h264_d3d11va,hevc_d3d11va,vp9_d3d11va,av1_d3d11va,h264_dxva2,hevc_dxva2,vp9_dxva2
make -j2
make install
cp config.h ffbuild/config.mak ../../
make clean
cd ../..
pacman -Q > installed-packages.txt
find install/bin -type f -name '*.dll' -exec sha256sum '{}' + > SHA256SUMS.txt
