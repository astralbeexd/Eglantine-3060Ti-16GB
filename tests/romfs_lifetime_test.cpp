#include <iostream>
#include <atomic>
#include <cstdlib>
#include <cstddef>
#include <new>
#include "core/file_sys/fsmitm_romfsbuild.h"
#include "core/file_sys/vfs/vfs_vector.h"

namespace {
std::atomic<std::size_t> live_bytes{};
struct alignas(std::max_align_t) Allocation { std::size_t size; };
}
void* operator new(std::size_t size) {
    if (size > std::size_t(-1)-sizeof(Allocation)) throw std::bad_alloc();
    auto p=static_cast<Allocation*>(std::malloc(sizeof(Allocation)+size));
    if (!p) throw std::bad_alloc();
    p->size=size;live_bytes+=size;return p+1;
}
void operator delete(void* memory) noexcept {
    if (!memory) return;
    auto p=static_cast<Allocation*>(memory)-1;
    live_bytes-=p->size;std::free(p);
}
void operator delete(void* memory,std::size_t) noexcept { ::operator delete(memory); }
void* operator new[](std::size_t size) { return ::operator new(size); }
void operator delete[](void* memory) noexcept { ::operator delete(memory); }
void operator delete[](void* memory,std::size_t) noexcept { ::operator delete(memory); }

bool roundtrip() {
    using namespace FileSys;
    auto source=std::make_shared<VectorVfsFile>(std::vector<u8>{1,2,3},"sample");
    std::weak_ptr<VfsFile> observed=source;
    auto directory=std::make_shared<VectorVfsDirectory>(std::vector<VirtualFile>{source});
    std::vector<std::pair<u64,VirtualFile>> parts;
    {
        RomFSBuildContext context(directory,nullptr);
        parts=context.Build();
    }
    source.reset();
    directory.reset();
    if (observed.expired()) return false;
    parts.clear();
    return observed.expired();
}
int main() {
    if (!roundtrip()) return 2;
    auto baseline=live_bytes.load();
    for (int i=0;i<64;++i) if (!roundtrip()) return 3;
    auto retained=live_bytes.load()-baseline;
    std::cout << "source_released=true metadata_retained_bytes_after_64_builds=" << retained << '\n';
    return retained == 0 ? 0 : 1;
}
