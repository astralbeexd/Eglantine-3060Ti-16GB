// SPDX-License-Identifier: GPL-3.0-or-later
#include "heap_accounting.h"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>

int main() {
    constexpr std::uint64_t MiB = 1024 * 1024;
    using Vulkan::AccountHeapUsage;
    // The normal policy discounts all free space; the opt-in policy bounds that discount.
    assert(AccountHeapUsage(7200 * MiB, 7000 * MiB, 6000 * MiB) == 6200 * MiB);
    assert(AccountHeapUsage(7200 * MiB, 7000 * MiB, 6000 * MiB, 256 * MiB) == 6944 * MiB);
    assert(AccountHeapUsage(7200 * MiB, 7000 * MiB, 6900 * MiB, 256 * MiB) == 7100 * MiB);
    // Empty, inconsistent and stale snapshots must not wrap the unsigned counters.
    assert(AccountHeapUsage(0, 0, 0, 256 * MiB) == 0);
    assert(AccountHeapUsage(100, 1000, 0, 256 * MiB) == 0);
    assert(AccountHeapUsage(100, 1000, 1001, 256 * MiB) == 100);
    assert(AccountHeapUsage(100, 0, 0, 256 * MiB) == 100);
    constexpr auto max = std::numeric_limits<std::uint64_t>::max();
    assert(AccountHeapUsage(max, max, 0, 256 * MiB) == max - 256 * MiB);
    assert(AccountHeapUsage(max, 0, max, 256 * MiB) == max);
    for (std::uint64_t usage = 0; usage < 128; ++usage) {
        for (std::uint64_t blocks = 0; blocks < 128; ++blocks) {
            for (std::uint64_t allocated = 0; allocated < 128; ++allocated) {
                const auto measured = AccountHeapUsage(usage, blocks, allocated, 16);
                assert(measured <= usage);
                assert(usage - measured <= 16);
            }
        }
    }
    std::cout << "PASS: boundary cases and 2097152 accounting combinations\n";
}
