#include "shm.hpp"
#include "macros/assert.hpp"

namespace towl {
auto Buffer::native() -> wl_buffer* {
    return buffer.get();
}

Buffer::Buffer(wl_buffer* const buffer) : buffer(buffer) {
    ASSERT(buffer != NULL);
}

auto ShmPool::create_buffer(const int32_t offset, const int32_t width, const int32_t height, const int32_t stride, const uint32_t format) -> Buffer {
    return wl_shm_pool_create_buffer(shm_pool.get(), offset, width, height, stride, format);
}

ShmPool::ShmPool(wl_shm_pool* const shm_pool) : shm_pool(shm_pool) {
    ASSERT(shm_pool != NULL);
}

auto Shm::create_shm_pool(const int posix_shm, const size_t size) -> ShmPool {
    return wl_shm_create_pool(shm.get(), posix_shm, size);
}

Shm::Shm(void* const data)
    : shm(std::bit_cast<wl_shm*>(data)) {}

auto ShmBinder::get_interface_description() -> const wl_interface* {
    return &wl_shm_interface;
}

auto ShmBinder::create_interface(void* const data) -> std::unique_ptr<impl::Interface> {
    return std::unique_ptr<impl::Interface>(new Shm(data));
}
} // namespace towl
