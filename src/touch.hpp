#pragma once
#include <memory>

#include <wayland-client.h>

namespace towl::impl {
struct AutoNativeTouchDeleter {
    uint32_t version;

    auto operator()(wl_touch* pointer) -> void;
};

using AutoNativeTouch = std::unique_ptr<wl_touch, AutoNativeTouchDeleter>;
} // namespace towl::impl

namespace towl {
class TouchCallbacks {
  public:
    virtual auto on_wl_touch_down(wl_surface* /*surface*/, uint32_t /*id*/, double /*x*/, double /*y*/) -> void {}
    virtual auto on_wl_touch_motion(uint32_t /*id*/, double /*x*/, double /*y*/) -> void {}
    virtual auto on_wl_touch_up(uint32_t /*id*/) -> void {}
    virtual auto on_wl_touch_frame() -> void {}
};

class Touch {
  private:
    impl::AutoNativeTouch touch;
    TouchCallbacks*       callbacks;

    static auto down(void* data, wl_touch* wl_touch, uint32_t serial, uint32_t time, wl_surface* surface, int32_t id, wl_fixed_t x, wl_fixed_t y) -> void;
    static auto up(void* data, wl_touch* wl_touch, uint32_t serial, uint32_t time, int32_t id) -> void;
    static auto motion(void* data, wl_touch* wl_touch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y) -> void;
    static auto frame(void* data, wl_touch* wl_touch) -> void;
    static auto cancel(void* /*data*/, wl_touch* /*wl_touch*/) -> void {}
    static auto shape(void* /*data*/, wl_touch* /*wl_touch*/, int32_t /*id*/, wl_fixed_t /*major*/, wl_fixed_t /*minor*/) -> void {}
    static auto orientation(void* /*data*/, wl_touch* /*wl_touch*/, int32_t /*id*/, wl_fixed_t /*orientation*/) -> void {}

    static inline wl_touch_listener listener = {down, up, motion, frame, cancel, shape, orientation};

  public:
    Touch(wl_touch* touch, uint32_t version, TouchCallbacks* callbacks);
};
} // namespace towl
