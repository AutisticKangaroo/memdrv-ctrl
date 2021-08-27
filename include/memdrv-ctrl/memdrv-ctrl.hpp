#ifndef MEMDRV_CTRL_MEMDRV_CTRL_HPP_
#define MEMDRV_CTRL_MEMDRV_CTRL_HPP_

#include <cstdint>

namespace memdrv {
    bool initialize();

    bool loaded();

    bool map_physical(uint32_t pid, uint64_t address, size_t size, uint64_t* view);
    bool unmap_physical(uint32_t pid, uint64_t view);

    bool copy_virtual_memory(uint32_t source_pid, uint64_t source_address, uint32_t target_pid, uint64_t target_address, size_t size);
}

#endif