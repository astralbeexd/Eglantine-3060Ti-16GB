#!/usr/bin/env bash
set -euo pipefail

if [[ "${MSYSTEM:-}" != UCRT64 ]]; then
    printf '%s\n' 'Abre la terminal MSYS2 UCRT64 para ejecutar este script.' >&2
    exit 1
fi

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
trap 'status=$?; printf "\nLa compilación se detuvo (código %s). Conserva el último error mostrado.\n" "$status" >&2' ERR

printf '%s\n' 'Si MSYS2 actualiza su runtime y cierra esta terminal, vuelve a abrir UCRT64 y ejecuta este script otra vez.'

pacman -Syu --needed --noconfirm
pacman -S --needed --noconfirm \
    git make autoconf libtool automake-wrapper patch perl ninja \
    mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-clang \
    mingw-w64-ucrt-x86_64-lld mingw-w64-ucrt-x86_64-llvm-tools \
    mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-python-pip mingw-w64-ucrt-x86_64-ffmpeg \
    mingw-w64-ucrt-x86_64-sdl3 \
    mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-qt6-charts \
    mingw-w64-ucrt-x86_64-qt6-tools mingw-w64-ucrt-x86_64-qt6-translations \
    mingw-w64-ucrt-x86_64-qt6-svg mingw-w64-ucrt-x86_64-openssl \
    mingw-w64-ucrt-x86_64-boost mingw-w64-ucrt-x86_64-fmt \
    mingw-w64-ucrt-x86_64-lz4 mingw-w64-ucrt-x86_64-nlohmann-json \
    mingw-w64-ucrt-x86_64-zlib mingw-w64-ucrt-x86_64-zstd \
    mingw-w64-ucrt-x86_64-libiconv mingw-w64-ucrt-x86_64-vulkan-memory-allocator \
    mingw-w64-ucrt-x86_64-vulkan-devel mingw-w64-ucrt-x86_64-glslang \
    mingw-w64-ucrt-x86_64-enet mingw-w64-ucrt-x86_64-opus \
    mingw-w64-ucrt-x86_64-libusb mingw-w64-ucrt-x86_64-unordered_dense \
    mingw-w64-ucrt-x86_64-jbigkit mingw-w64-ucrt-x86_64-xz \
    mingw-w64-ucrt-x86_64-libdeflate mingw-w64-ucrt-x86_64-bzip2

base=6bde8c20d1f7c020a920e59a989c03822a08445d
if [[ ! -e Eglantine ]]; then
    git clone https://github.com/Djihads80/Eglantine.git Eglantine
    git -C Eglantine checkout --detach "$base"
elif [[ ! -d Eglantine/.git ]] || [[ "$(git -C Eglantine rev-parse HEAD)" != "$base" ]]; then
    printf '%s\n' 'La carpeta Eglantine existente no corresponde a la base esperada; se conserva sin modificar.' >&2
    exit 1
fi
if git -C Eglantine apply --reverse --check ../3060ti-mejoras-v2-incremental.patch 2>/dev/null; then
    printf '%s\n' 'Los parches de esta versión ya están aplicados.'
else
for patch_file in 3060ti-memory-experimental.patch 3060ti-mejoras-v2-incremental.patch; do
    if git -C Eglantine apply --reverse --check "../$patch_file" 2>/dev/null; then
        printf '%s\n' "El parche $patch_file ya está aplicado."
    else
        git -C Eglantine apply --check "../$patch_file"
        git -C Eglantine apply "../$patch_file"
    fi
done
fi

if ! git -C Eglantine apply --reverse --check ../3060ti-security-surface.patch 2>/dev/null; then
    git -C Eglantine apply --check ../3060ti-security-surface.patch
    git -C Eglantine apply ../3060ti-security-surface.patch
fi

clang++ -std=c++20 -O2 -march=znver3 -Wall -Wextra -Werror \
    -I Eglantine/src/video_core/vulkan_common \
    tests/heap_accounting_test.cpp -o tests/heap_accounting_test.exe
./tests/heap_accounting_test.exe

# An existing CMake compiler snapshot can override -D cache entries for these tools.
llvm_ar=$(cygpath -m "$(command -v llvm-ar)")
llvm_ranlib=$(cygpath -m "$(command -v llvm-ranlib)")
for compiler_info in Eglantine/build/3060ti/CMakeFiles/*/CMake{C,CXX,ASM}Compiler.cmake; do
    [[ -f "$compiler_info" ]] || continue
    sed -i -E \
        -e "s|^set\\(CMAKE_AR .*|set(CMAKE_AR \"$llvm_ar\")|" \
        -e "s|^set\\(CMAKE_RANLIB .*|set(CMAKE_RANLIB \"$llvm_ranlib\")|" \
        -e "s#^set\\(CMAKE_(C|CXX)_COMPILER_AR .*#set(CMAKE_\\1_COMPILER_AR \"$llvm_ar\")#" \
        -e "s#^set\\(CMAKE_(C|CXX)_COMPILER_RANLIB .*#set(CMAKE_\\1_COMPILER_RANLIB \"$llvm_ranlib\")#" \
        "$compiler_info"
done

if ! git -C Eglantine apply --reverse --check ../3060ti-romfs-loading.patch 2>/dev/null; then
    git -C Eglantine apply --check ../3060ti-romfs-loading.patch
    git -C Eglantine apply ../3060ti-romfs-loading.patch
fi

cmake -S Eglantine -B Eglantine/build/3060ti -G Ninja \
    -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DYUZU_TESTS=OFF \
    -DENABLE_QT=ON -DYUZU_CMD=ON -DYUZU_ROOM=OFF \
    -DUSE_DISCORD_PRESENCE=OFF -DENABLE_UPDATE_CHECKER=OFF \
    -DEGLANTINE_NX2_PGO=OFF \
    -DHTTPLIB_USE_NON_BLOCKING_GETADDRINFO=OFF \
    -DEGLANTINE_RENDERER_LTO=ON \
    -DYUZU_USE_BUNDLED_QT=OFF -DYUZU_USE_BUNDLED_FFMPEG=OFF \
    -DYUZU_USE_EXTERNAL_FFMPEG=OFF -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld \
    "-DCMAKE_AR=$(cygpath -m "$(command -v llvm-ar)")" \
    "-DCMAKE_RANLIB=$(cygpath -m "$(command -v llvm-ranlib)")" \
    "-DCMAKE_C_COMPILER_AR=$llvm_ar" "-DCMAKE_CXX_COMPILER_AR=$llvm_ar" \
    "-DCMAKE_C_COMPILER_RANLIB=$llvm_ranlib" "-DCMAKE_CXX_COMPILER_RANLIB=$llvm_ranlib" \
    '-DCMAKE_C_FLAGS=-march=znver3 -mtune=znver3' \
    '-DCMAKE_CXX_FLAGS=-march=znver3 -mtune=znver3'
cmake --build Eglantine/build/3060ti --target yuzu yuzu-cmd --parallel 2
clang++ -std=c++20 -O2 -UNDEBUG -I Eglantine/src \
    tests/host_memory_windows_test.cpp Eglantine/build/3060ti/src/common/libcommon.a \
    -lfmt -lzstd -lws2_32 -lbcrypt -lboost_context-mt -lole32 -luuid -lshell32 -lntdll \
    -o tests/host_memory_windows_test.exe
./tests/host_memory_windows_test.exe
clang++ -std=c++20 -O2 -UNDEBUG -I Eglantine/src \
    tests/thread_name_test.cpp Eglantine/build/3060ti/src/common/libcommon.a \
    -lfmt -lzstd -lws2_32 -lbcrypt -lboost_context-mt -lole32 -luuid -lshell32 -lntdll \
    -o tests/thread_name_test.exe
./tests/thread_name_test.exe
printf '%s\n' 'Compilación terminada: Eglantine/build/3060ti/bin/eglantine.exe'
