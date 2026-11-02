#include <chrono>
#include <iostream>
#include <iomanip>
#include <array>
#include <cstring>
#include <openssl/sha.h>
#include "core/file_sys/vfs/vfs_real.h"
#include "core/loader/deconstructed_rom_directory.h"

int main(int argc, char** argv) {
    if (argc < 2) return 2;
    FileSys::RealVfsFilesystem vfs;
    auto file = vfs.OpenFile(argv[1]);
    if (!file) return 3;
    auto start=std::chrono::steady_clock::now();
    Loader::AppLoader_DeconstructedRomDirectory loader(file);
    u64 id{};
    if (loader.ReadProgramId(id) != Loader::ResultStatus::Success || id == 0) return 4;
    std::string version;
    if (loader.ReadDisplayVersion(version) != Loader::ResultStatus::Success) return 5;
    std::cout << "metadata_ms=" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count() << '\n' << std::flush;
    if (argc >= 3) {
        FileSys::VirtualFile first, second;
        start=std::chrono::steady_clock::now();
        if (loader.ReadRomFS(first) != Loader::ResultStatus::Success || !first || first->GetSize() < 0x50) return 6;
        if (loader.ReadRomFS(second) != Loader::ResultStatus::Success || first != second) return 7;
        std::cout << "romfs_ms=" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count() << " bytes=" << first->GetSize() << " repeated_read_same_object=true\n";
        auto bytes=first->ReadBytes(0x50);
        if (bytes.size()!=0x50) return 8;
        std::array<u64,10> header{};
        std::memcpy(header.data(),bytes.data(),bytes.size());
        for (int i=1;i<9;i+=2) {
            if (header[i+1]>64*1024*1024 || header[i]>first->GetSize() || header[i+1]>first->GetSize()-header[i]) return 9;
            auto table=first->ReadBytes(header[i+1],header[i]);
            if (table.size()!=header[i+1]) return 10;
            bytes.insert(bytes.end(),table.begin(),table.end());
        }
        std::array<unsigned char,SHA256_DIGEST_LENGTH> digest{};
        SHA256(bytes.data(),bytes.size(),digest.data());
        std::cout << "index_sha256=";
        for (auto value : digest) std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned(value);
        std::cout << '\n';
    }
}

#define VMA_IMPLEMENTATION
#include "video_core/vulkan_common/vma.h"
