#include <windows.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <utility>
#include "common/host_memory.h"

int main() {
    constexpr size_t size = 64ULL << 20;
    constexpr size_t page = 0x10000;
    DWORD handles_before{};
    GetProcessHandleCount(GetCurrentProcess(), &handles_before);
    for (int i = 0; i < 24; ++i) {
        void* backing{};
        {
            Common::HostMemory memory(size, 1ULL << 30);
            assert(memory.VirtualBasePointer() != nullptr);
            backing = memory.BackingBasePointer();
            memory.Map(0, 0, page, Common::MemoryPermission::ReadWrite, false);
            memory.Map(page, 0, page, Common::MemoryPermission::ReadWrite, false);
            memory.BackingBasePointer()[0] = 0x5a;
            assert(memory.VirtualBasePointer()[0] == 0x5a);
            assert(memory.VirtualBasePointer()[page] == 0x5a);
            memory.VirtualBasePointer()[page + 7] = 0x42;
            assert(memory.BackingBasePointer()[7] == 0x42);
            if (i % 2 == 0) {
                memory.Unmap(0, page, false);
                memory.Unmap(page, page, false);
            } else {
                Common::HostMemory moved{std::move(memory)};
                assert(memory.BackingBasePointer() == nullptr);
                assert(moved.VirtualBasePointer()[page + 7] == 0x42);
                // Destroy while both alias views are still mapped.
            }
        }
        MEMORY_BASIC_INFORMATION info{};
        assert(VirtualQuery(backing, &info, sizeof(info)) != 0);
        assert(info.State == MEM_FREE);
        {
            // An address space larger than Windows supports forces Init's cleanup and fallback.
            Common::HostMemory memory(size, 1ULL << 60);
            assert(memory.VirtualBasePointer() == nullptr);
            backing = memory.BackingBasePointer();
            assert(backing != nullptr);
            memory.BackingBasePointer()[0] = 0x33;
            memory.BackingBasePointer()[size - 1] = 0x77;
        }
        assert(VirtualQuery(backing, &info, sizeof(info)) != 0);
        if (info.State != MEM_FREE) {
            std::cerr << "FAIL: fallback allocation remains committed after destruction\n";
            return 2;
        }
    }
    for (int i = 0; i < 24; ++i) {
        void* backing{};
        void* replaced{};
        {
            Common::HostMemory source(size, 1ULL << 60);
            backing = source.BackingBasePointer();
            Common::HostMemory moved{std::move(source)};
            assert(source.BackingBasePointer() == nullptr);
            Common::HostMemory target(size, 1ULL << 30);
            replaced = target.BackingBasePointer();
            target = std::move(moved);
            assert(moved.BackingBasePointer() == nullptr);
            MEMORY_BASIC_INFORMATION info{};
            assert(VirtualQuery(replaced, &info, sizeof(info)) != 0);
            assert(info.State == MEM_FREE);
            target.BackingBasePointer()[0] = 0x55;
            target = std::move(target);
            assert(target.BackingBasePointer()[0] == 0x55);
        }
        MEMORY_BASIC_INFORMATION info{};
        assert(VirtualQuery(backing, &info, sizeof(info)) != 0);
        assert(info.State == MEM_FREE);
    }
    DWORD handles_after{};
    GetProcessHandleCount(GetCurrentProcess(), &handles_after);
    assert(handles_after <= handles_before + 2);
    std::cout << "PASS: 24 fastmem alias cycles and 24 forced-fallback release cycles, 24 move/replace cycles; handles "
              << handles_before << " -> " << handles_after << '\n';
}
