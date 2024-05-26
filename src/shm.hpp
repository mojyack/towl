#pragma once
#include <wayland-client.h>

#include "interface.hpp"
#include "macros/autoptr.hpp"

namespace towl::impl {
declare_autoptr(NativeBuffer, wl_buffer, wl_buffer_destroy);
declare_autoptr(NativeShmPool, wl_shm_pool, wl_shm_pool_destroy);
declare_autoptr(NativeShm, wl_shm, wl_shm_destroy);
} // namespace towl::impl

namespace towl {
class Buffer {
  private:
    impl::AutoNativeBuffer buffer;

  public:
    auto native() -> wl_buffer*;

    Buffer(wl_buffer* buffer);
};

class ShmPool {
  private:
    impl::AutoNativeShmPool shm_pool;

  public:
    auto create_buffer(int32_t offset, int32_t width, int32_t height, int32_t stride, uint32_t format) -> Buffer;

    ShmPool(wl_shm_pool* const shm_pool);
};

class Shm : public impl::Interface {
  private:
    impl::AutoNativeShm shm;

  public:
    auto create_shm_pool(int posix_shm, size_t size) -> ShmPool;

    Shm(void* data);
};

// version = 1
struct ShmBinder : impl::InterfaceBinder {
    auto get_interface_description() -> const wl_interface* override;
    auto create_interface(void* data) -> std::unique_ptr<impl::Interface> override;

    ShmBinder(const uint32_t version)
        : InterfaceBinder(version) {}
};
} // namespace towl
