#pragma once
#include "interface.hpp"
#include "macros/autoptr.hpp"

#define namespace namespace_
#include <wlr-layer-shell-unstable-v1.h>
#undef namespace

namespace towl::impl {
declare_autoptr(NativeLayerShell, zwlr_layer_shell_v1, zwlr_layer_shell_v1_destroy);
declare_autoptr(NativeLayerSurface, zwlr_layer_surface_v1, zwlr_layer_surface_v1_destroy);
} // namespace towl::impl

namespace towl {
class LayerSurfaceCallbacks {
  public:
    virtual auto on_zwlr_layer_surface_configure(uint32_t /*width*/, uint32_t /*height*/) -> void {}
    virtual auto on_zwlr_layer_surface_closed() -> void {}
    virtual ~LayerSurfaceCallbacks() {}
};

class LayerSurface {
  private:
    impl::AutoNativeLayerSurface surface;
    LayerSurfaceCallbacks*       callbacks;

    static auto configure(void* data, zwlr_layer_surface_v1* surface, uint32_t serial, uint32_t width, uint32_t height) -> void;
    static auto closed(void* data, zwlr_layer_surface_v1* surface) -> void;

    static inline zwlr_layer_surface_v1_listener listener = {configure, closed};

  public:
    auto set_size(uint32_t width, uint32_t height) -> void;
    auto set_anchor(uint32_t anchor) -> void;
    auto set_exclusive_zone(int32_t zone) -> void;
    auto set_margin(int32_t top, int32_t right, int32_t bottom, int32_t left) -> void;
    auto init(LayerSurfaceCallbacks* callbacks) -> bool;

    LayerSurface() = default;
    LayerSurface(zwlr_layer_surface_v1* const surface);
};

class LayerShell : public impl::Interface {
  private:
    impl::AutoNativeLayerShell shell;

  public:
    auto create_layer_surface(wl_surface* surface, wl_output* output, uint32_t layer, const char* namespace_) -> LayerSurface;

    LayerShell(void* data);
};

// version = 1 ~ 4
struct LayerShellBinder : impl::InterfaceBinder {
    auto get_interface_description() -> const wl_interface* override;
    auto create_interface(void* data) -> std::unique_ptr<impl::Interface> override;

    LayerShellBinder(const uint32_t version)
        : InterfaceBinder(version) {}
};
} // namespace towl
