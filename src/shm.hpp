#pragma once
#include <cassert>
#include <memory>

#include "internal.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

// version = 1
template <uint32_t version>
class Shm {
  public:
    class Buffer {
      private:
        struct Deleter {
            auto operator()(wl_buffer* const buffer) -> void {
                static_assert(version >= WL_BUFFER_DESTROY_SINCE_VERSION);
                wl_buffer_destroy(buffer);
            }
        };

        std::unique_ptr<wl_buffer, Deleter> buffer;

      public:
        auto native() -> wl_buffer* {
            return buffer.get();
        }
        Buffer(wl_buffer* const buffer) : buffer(buffer) {
            assert(buffer);
        }
    };

    class ShmPool {
      private:
        struct Deleter {
            auto operator()(wl_shm_pool* const shm_pool) -> void {
                static_assert(version >= WL_SHM_POOL_DESTROY_SINCE_VERSION);
                wl_shm_pool_destroy(shm_pool);
            }
        };
        std::unique_ptr<wl_shm_pool, Deleter> shm_pool;

      public:
        auto create_buffer(const int32_t offset, const int32_t width, const int32_t height, const int32_t stride, const uint32_t format) -> Buffer {
            static_assert(version >= WL_SHM_POOL_CREATE_BUFFER_SINCE_VERSION);
            return wl_shm_pool_create_buffer(shm_pool.get(), offset, width, height, stride, format);
        }
        ShmPool(wl_shm_pool* const shm_pool) : shm_pool(shm_pool) {
            assert(shm_pool);
        }
    };

  private:
    struct Deleter {
        auto operator()(wl_shm* const shm) -> void {
            wl_shm_destroy(shm);
        }
    };

    std::unique_ptr<wl_shm, Deleter> shm;
    uint32_t                         id;

  public:
    static auto info() -> internal::InterfaceInfo {
        return {"wl_shm", version, &wl_shm_interface};
    }
    auto interface_id() const -> uint32_t {
        return id;
    }
    auto create_shm_pool(const int posix_shm, const size_t size) -> ShmPool {
        static_assert(version >= WL_SHM_CREATE_POOL_SINCE_VERSION);
        return wl_shm_create_pool(shm.get(), posix_shm, size);
    }
    Shm(void* const data, const uint32_t id) : shm(reinterpret_cast<wl_shm*>(data)), id(id) {}
};

#ifdef TOWL_NS
}
#endif
