#pragma once
#include <wayland-client.h>

#include "interface.hpp"
#include "macros/autoptr.hpp"

namespace towl::impl {
declare_autoptr(NativeCompositor, wl_compositor, wl_compositor_destroy);
declare_autoptr(NativeSurface, wl_surface, wl_surface_destroy);
declare_autoptr(NativeCallback, wl_callback, wl_callback_destroy);
} // namespace towl::impl

namespace towl {
class SurfaceCallbacks {
  public:
    virtual auto on_wl_surface_enter(wl_output* /*output*/) -> void {}
    virtual auto on_wl_surface_leave(wl_output* /*output*/) -> void {}
    virtual auto on_wl_surface_preferred_buffer_scale(int32_t /*factor*/) -> void {}
    virtual auto on_wl_surface_frame() -> void {}
    ~SurfaceCallbacks() {}
};

class Surface {
  private:
    impl::AutoNativeSurface  surface;
    impl::AutoNativeCallback frame;
    SurfaceCallbacks*        callbacks;

    static auto enter(void* data, wl_surface* surface, wl_output* output) -> void;
    static auto leave(void* data, wl_surface* surface, wl_output* output) -> void;
    static auto preferred_buffer_scale(void* data, wl_surface* surface, int32_t factor) -> void;
    static auto preferred_buffer_transform(void* /*data*/, wl_surface* /*surface*/, uint32_t /*transform*/) -> void{};

    static inline wl_surface_listener listener = {enter, leave, preferred_buffer_scale, preferred_buffer_transform};

    static auto done(void* data, wl_callback* wl_callback, uint32_t callback_data) -> void;

    static inline wl_callback_listener frame_listener = {done};

  public:
    auto native() -> wl_surface*;
    auto attach(wl_buffer* buffer, int32_t x, int32_t y) -> void;
    auto damage(int32_t x, int32_t y, int32_t width, int32_t height) -> void;
    auto commit() -> void;
    auto set_buffer_scale(int32_t scale) -> void;
    auto set_frame() -> void;

    Surface(wl_surface* surface, SurfaceCallbacks* callbacks);
};

class Compositor : public impl::Interface {
  private:
    impl::AutoNativeCompositor compositor;

  public:
    auto create_surface(SurfaceCallbacks* callbacks) -> Surface;

    Compositor(void* data);
};

// version = 1 ~ 4
struct CompositorBinder : impl::InterfaceBinder {
    uint32_t interface_version;

    auto get_interface_description() -> const wl_interface* override;
    auto create_interface(void* data) -> std::unique_ptr<impl::Interface> override;

    CompositorBinder(const uint32_t version)
        : InterfaceBinder(version) {}
};
}; // namespace towl
