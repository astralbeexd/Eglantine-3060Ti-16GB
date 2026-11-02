// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>

namespace Vulkan {

constexpr std::uint64_t AccountHeapUsage(
    std::uint64_t driver_usage, std::uint64_t block_bytes, std::uint64_t allocation_bytes,
    std::uint64_t slack_limit = std::numeric_limits<std::uint64_t>::max()) {
    const auto unused = block_bytes - (std::min)(block_bytes, allocation_bytes);
    return driver_usage - (std::min)({driver_usage, unused, slack_limit});
}

} // namespace Vulkan
