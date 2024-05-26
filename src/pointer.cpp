#include "pointer.hpp"

namespace towl::impl {
auto AutoNativePointerDeleter::operator()(wl_pointer* const pointer) -> void {
    if(version >= WL_POINTER_RELEASE_SINCE_VERSION) {
        wl_pointer_release(pointer);
    } else {
        wl_pointer_destroy(pointer);
    }
}
} // namespace towl::impl

namespace towl {
auto Pointer::enter(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*serial*/, wl_surface* const surface, const wl_fixed_t x, const wl_fixed_t y) -> void {
    auto& self = *std::bit_cast<Pointer*>(data);
    self.callbacks->on_wl_pointer_enter(surface, wl_fixed_to_double(x), wl_fixed_to_double(y));
}

auto Pointer::leave(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*serial*/, wl_surface* const surface) -> void {
    auto& self = *std::bit_cast<Pointer*>(data);
    self.callbacks->on_wl_pointer_leave(surface);
}

auto Pointer::motion(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*time*/, const wl_fixed_t x, const wl_fixed_t y) -> void {
    auto& self = *std::bit_cast<Pointer*>(data);
    self.callbacks->on_wl_pointer_motion(wl_fixed_to_double(x), wl_fixed_to_double(y));
}

auto Pointer::button(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*serial*/, const uint32_t /*time*/, const uint32_t button, const uint32_t state) -> void {
    auto& self = *std::bit_cast<Pointer*>(data);
    self.callbacks->on_wl_pointer_button(button, state);
}

auto Pointer::axis(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*time*/, const uint32_t axis, const wl_fixed_t value) -> void {
    auto& self = *std::bit_cast<Pointer*>(data);
    self.callbacks->on_wl_pointer_axis(axis, wl_fixed_to_double(value));
}

auto Pointer::frame(void* const data, wl_pointer* const /*pointer*/) -> void {
    auto& self = *std::bit_cast<Pointer*>(data);
    self.callbacks->on_wl_pointer_frame();
}

auto Pointer::axis_source(void* const data, wl_pointer* const /*pointer*/, const uint32_t axis_source) -> void {
    auto& self = *std::bit_cast<Pointer*>(data);
    self.callbacks->on_wl_pointer_axis_source(axis_source);
}

auto Pointer::axis_stop(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*time*/, const uint32_t axis) -> void {
    auto& self = *std::bit_cast<Pointer*>(data);
    self.callbacks->on_wl_pointer_axis_stop(axis);
}

auto Pointer::axis_descrete(void* const data, wl_pointer* const /*pointer*/, const uint32_t axis, const int32_t descrete) -> void {
    auto& self = *std::bit_cast<Pointer*>(data);
    self.callbacks->on_wl_pointer_axis_discrete(axis, descrete);
}

auto Pointer::axis_value120(void* const data, wl_pointer* const /*pointer*/, const uint32_t axis, const int32_t value120) -> void {
    auto& self = *std::bit_cast<Pointer*>(data);
    self.callbacks->on_wl_pointer_axis_value120(axis, value120);
}

Pointer::Pointer(wl_pointer* const pointer, const uint32_t version, PointerCallbacks* const callbacks)
    : pointer(pointer, {version}),
      callbacks(callbacks){};
} // namespace towl
