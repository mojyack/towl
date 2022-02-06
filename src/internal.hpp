#pragma once
#include <concepts>

#include <wayland-client.h>

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

namespace internal {
struct InterfaceInfo {
    const char*         name;
    const uint32_t      version;
    const wl_interface* structure;
};

template <class T>
concept Interface = requires(const T& m) {
    { T::info() } -> std::same_as<InterfaceInfo>;
    { m.interface_id() } -> std::same_as<uint32_t>;
};

template <class T>
concept BufferLike = requires(T& m) {
    { m.native() } -> std::same_as<wl_buffer*>;
};

template <class T>
concept SurfaceLike = requires(T& m) {
    { m.native() } -> std::same_as<wl_surface*>;
};
} // namespace internal

#ifdef TOWL_NS
}
#endif
