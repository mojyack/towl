#include "touch.hpp"

namespace towl::impl {
auto AutoNativeTouchDeleter::operator()(wl_touch* const touch) -> void {
    if(version >= WL_TOUCH_RELEASE_SINCE_VERSION) {
        wl_touch_release(touch);
    } else {
        wl_touch_destroy(touch);
    }
}
} // namespace towl::impl

namespace towl {
auto Touch::down(void* const data, wl_touch* const /*wl_touch*/, const uint32_t /*serial*/, const uint32_t /*time*/, wl_surface* const surface, const int32_t id, const wl_fixed_t x, const wl_fixed_t y) -> void {
    auto& self = *std::bit_cast<Touch*>(data);
    self.callbacks->on_wl_touch_down(surface, id, wl_fixed_to_double(x), wl_fixed_to_double(y));
}

auto Touch::up(void* const data, wl_touch* const /*wl_touch*/, const uint32_t /*serial*/, const uint32_t /*time*/, const int32_t id) -> void {
    auto& self = *std::bit_cast<Touch*>(data);
    self.callbacks->on_wl_touch_up(id);
}

auto Touch::motion(void* const data, wl_touch* const /*wl_touch*/, const uint32_t /*time*/, const int32_t id, const wl_fixed_t x, const wl_fixed_t y) -> void {
    auto& self = *std::bit_cast<Touch*>(data);
    self.callbacks->on_wl_touch_motion(id, wl_fixed_to_double(x), wl_fixed_to_double(y));
}

auto Touch::frame(void* const data, wl_touch* const /*wl_touch*/) -> void {
    auto& self = *std::bit_cast<Touch*>(data);
    self.callbacks->on_wl_touch_frame();
}

Touch::Touch(wl_touch* const touch, const uint32_t version, TouchCallbacks* const callbacks)
    : touch(touch, {version}),
      callbacks(callbacks){};
} // namespace towl
