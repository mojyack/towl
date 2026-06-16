#include "layer-shell.hpp"
#include "macros/assert.hpp"

namespace towl {
auto LayerSurface::configure(void* const data, zwlr_layer_surface_v1* const surface, const uint32_t serial, const uint32_t width, const uint32_t height) -> void {
    zwlr_layer_surface_v1_ack_configure(surface, serial);
    auto& self = *std::bit_cast<LayerSurface*>(data);
    self.callbacks->on_zwlr_layer_surface_configure(width, height);
}

auto LayerSurface::closed(void* const data, zwlr_layer_surface_v1* const /*surface*/) -> void {
    auto& self = *std::bit_cast<LayerSurface*>(data);
    self.callbacks->on_zwlr_layer_surface_closed();
}

auto LayerSurface::set_size(const uint32_t width, const uint32_t height) -> void {
    zwlr_layer_surface_v1_set_size(surface.get(), width, height);
}

auto LayerSurface::set_anchor(const uint32_t anchor) -> void {
    zwlr_layer_surface_v1_set_anchor(surface.get(), anchor);
}

auto LayerSurface::set_exclusive_zone(const int32_t zone) -> void {
    zwlr_layer_surface_v1_set_exclusive_zone(surface.get(), zone);
}

auto LayerSurface::set_margin(const int32_t top, const int32_t right, const int32_t bottom, const int32_t left) -> void {
    zwlr_layer_surface_v1_set_margin(surface.get(), top, right, bottom, left);
}

auto LayerSurface::init(LayerSurfaceCallbacks* const callbacks) -> bool {
    ensure(surface != NULL);
    this->callbacks = callbacks;
    zwlr_layer_surface_v1_add_listener(surface.get(), &listener, this);
    return true;
}

LayerSurface::LayerSurface(zwlr_layer_surface_v1* const surface)
    : surface(surface) {
}

auto LayerShell::create_layer_surface(wl_surface* const surface, wl_output* const output, const uint32_t layer, const char* const namespace_) -> LayerSurface {
    return LayerSurface(zwlr_layer_shell_v1_get_layer_surface(shell.get(), surface, output, layer, namespace_));
}

LayerShell::LayerShell(void* const data)
    : shell(std::bit_cast<zwlr_layer_shell_v1*>(data)) {}

auto LayerShellBinder::get_interface_description() -> const wl_interface* {
    return &zwlr_layer_shell_v1_interface;
}

auto LayerShellBinder::create_interface(void* const data) -> std::unique_ptr<impl::Interface> {
    return std::unique_ptr<impl::Interface>(new LayerShell(data));
}
} // namespace towl
