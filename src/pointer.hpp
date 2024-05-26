#pragma once
#include <memory>

#include <wayland-client.h>

namespace towl::impl {
struct AutoNativePointerDeleter {
    uint32_t version;

    auto operator()(wl_pointer* pointer) -> void;
};

using AutoNativePointer = std::unique_ptr<wl_pointer, AutoNativePointerDeleter>;
} // namespace towl::impl

namespace towl {
class PointerCallbacks {
  public:
    virtual auto on_wl_pointer_enter(wl_surface* /*surface*/, double /*x*/, double /*y*/) -> void {}
    virtual auto on_wl_pointer_motion(double /*x*/, double /*y*/) -> void {}
    virtual auto on_wl_pointer_leave(wl_surface* /*surface*/) -> void {}
    virtual auto on_wl_pointer_button(uint32_t /*button*/, uint32_t /*state*/) -> void {}
    virtual auto on_wl_pointer_axis(uint32_t /*axis*/, double /*value*/) -> void {}
    virtual auto on_wl_pointer_frame() -> void {}
    virtual auto on_wl_pointer_axis_source(uint32_t /*source*/) -> void {}
    virtual auto on_wl_pointer_axis_stop(uint32_t /*axis*/) -> void {}
    virtual auto on_wl_pointer_axis_discrete(uint32_t /*axis*/, int32_t /*discrete*/) -> void {}
    virtual auto on_wl_pointer_axis_value120(uint32_t /*axis*/, int32_t /*value120*/) -> void {}
    virtual ~PointerCallbacks(){};
};

class Pointer {
  private:
    impl::AutoNativePointer pointer;
    PointerCallbacks*       callbacks;

    static auto enter(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t y) -> void;
    static auto leave(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface) -> void;
    static auto motion(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y) -> void;
    static auto button(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) -> void;
    static auto axis(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value) -> void;
    static auto frame(void* data, wl_pointer* pointer) -> void;
    static auto axis_source(void* data, wl_pointer* pointer, uint32_t axis_source) -> void;
    static auto axis_stop(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis) -> void;
    static auto axis_descrete(void* data, wl_pointer* pointer, uint32_t axis, int32_t descrete) -> void;
    static auto axis_value120(void* data, wl_pointer* pointer, uint32_t axis, int32_t value120) -> void;
    static auto axis_relative_direction(void* /*data*/, wl_pointer* /*pointer*/, uint32_t /*axis*/, uint32_t /*direction*/) -> void {}

    static inline wl_pointer_listener listener = {enter, leave, motion, button, axis, frame, axis_source, axis_stop, axis_descrete, axis_value120, axis_relative_direction};

  public:
    Pointer(wl_pointer* pointer, uint32_t version, PointerCallbacks* callbacks);
};
} // namespace towl
