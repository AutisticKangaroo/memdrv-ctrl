#include <memdrv-ctrl/memdrv-ctrl.hpp>

#include <Windows.h>
#include <winternl.h>

#pragma comment(lib, "ntdll.lib")

#define SYSCALL_MAGIC 0xDEADBEEF

namespace memdrv {
    enum class dispatch_id : uint32_t {
        map_physical = 0,
        unmap_physical,
        copy_virtual_memory
    };

    struct syscall_info {
        uint32_t pad_0;
        uint32_t pad_1;
        uint32_t magic;
        uint32_t syscall;
        uint64_t arguments;
        uint64_t success;
    };

    struct map_physical_packet_t {
        uint32_t pid;
        uint64_t address;
        uint64_t size;
        uint64_t view;
    };

    struct unmap_physical_packet_t {
        uint32_t pid;
        uint64_t view;
    };

    struct copy_virtual_memory_packet_t {
        uint32_t source_pid;
        uint64_t target_pid;

        uint64_t source_address;
        uint64_t target_address;
        uint64_t size;
    };

    HANDLE handle_;

    bool initialize() {
        UNICODE_STRING device_name;
        OBJECT_ATTRIBUTES attributes;

        RtlInitUnicodeString(&device_name, L"\\Device\\Beep");

        InitializeObjectAttributes(&attributes, &device_name, 0, nullptr, nullptr);

        IO_STATUS_BLOCK status;
        if (!NT_SUCCESS(NtCreateFile(&handle_, 0x3, &attributes, &status, nullptr, 0, 0x3, 0x3, 0, nullptr, 0)))
            return false;

        return handle_ != nullptr;
    }

    bool call_driver(dispatch_id id, const void* parameter) {
        syscall_info data {};

        data.pad_0 = 100;
        data.pad_1 = 100;

        data.magic = SYSCALL_MAGIC;
        data.syscall = (uint32_t) id;
        data.arguments = (uint64_t) parameter;

        bool success = false;

        data.success = (uint64_t) &success;

        (void) DeviceIoControl(
            handle_,
            CTL_CODE(FILE_DEVICE_BEEP, 0, METHOD_BUFFERED, FILE_SPECIAL_ACCESS),
            &data,
            sizeof(data),
            &data,
            sizeof(data),
            nullptr,
            nullptr
        );

        return success;
    }

    bool map_physical(uint32_t pid, uint64_t address, size_t size, uint64_t* view) {
        map_physical_packet_t packet { };

        packet.pid = pid;
        packet.address = address;
        packet.size = size;

        if (!call_driver(dispatch_id::map_physical, &packet)) {
            *view = 0;

            return false;
        }

        *view = packet.view;

        return true;
    }

    bool unmap_physical(uint32_t pid, uint64_t view) {
        unmap_physical_packet_t packet { };

        packet.pid = pid;
        packet.view = view;

        return call_driver(dispatch_id::unmap_physical, &packet);
    }

    bool copy_virtual_memory(uint32_t source_pid, uint64_t source_address, uint32_t target_pid, uint64_t target_address, size_t size) {
        copy_virtual_memory_packet_t packet { };

        packet.source_pid = source_pid;
        packet.target_pid = target_pid;

        packet.source_address = source_address;
        packet.target_address = target_address;

        packet.size = size;

        return call_driver(dispatch_id::copy_virtual_memory, &packet);
    }
}