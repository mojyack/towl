#include "compositor.hpp"
#include "macros/assert.hpp"
#include "util/assert.hpp"

namespace towl {
auto Surface::enter(void* const data, wl_surface* const /*surface*/, wl_output* const output) -> void {
    auto& self = *std::bit_cast<Surface*>(data);
    self.callbacks->on_wl_surface_enter(output);
}

auto Surface::leave(void* const data, wl_surface* const /*surface*/, wl_output* const output) -> void {
    auto& self = *std::bit_cast<Surface*>(data);
    self.callbacks->on_wl_surface_leave(output);
}

auto Surface::preferred_buffer_scale(void* data, wl_surface* /*surface*/, const int32_t factor) -> void {
    auto& self = *std::bit_cast<Surface*>(data);
    self.callbacks->on_wl_surface_preferred_buffer_scale(factor);
}

auto Surface::done(void* const data, wl_callback* const /*wl_callback*/, const uint32_t /*callback_data*/) -> void {
    auto& self = *std::bit_cast<Surface*>(data);
    self.frame.reset();
    self.callbacks->on_wl_surface_frame();
}

auto Surface::native() -> wl_surface* {
    return surface.get();
}

auto Surface::attach(wl_buffer* const buffer, const int32_t x, const int32_t y) -> void {
    wl_surface_attach(surface.get(), buffer, x, y);
}

auto Surface::damage(const int32_t x, const int32_t y, const int32_t width, const int32_t height) -> void {
    wl_surface_damage_buffer(surface.get(), x, y, width, height);
}

auto Surface::commit() -> void {
    wl_surface_commit(surface.get());
}

auto Surface::set_buffer_scale(const int32_t scale) -> void {
    wl_surface_set_buffer_scale(surface.get(), scale);
}

auto Surface::set_frame() -> void {
    if(!frame) {
        frame.reset(wl_surface_frame(surface.get()));
        wl_callback_add_listener(frame.get(), &frame_listener, this);
    }
}

Surface::Surface(wl_surface* const surface, SurfaceCallbacks* const callbacks)
    : surface(surface),
      callbacks(callbacks) {
    DYN_ASSERT(surface);
    wl_surface_add_listener(surface, &listener, this);
}

auto Compositor::create_surface(SurfaceCallbacks* callbacks) -> Surface {
    return {wl_compositor_create_surface(compositor.get()), callbacks};
}

Compositor::Compositor(void* const data)
    : compositor(std::bit_cast<wl_compositor*>(data)) {}

auto CompositorBinder::get_interface_description() -> const wl_interface* {
    return &wl_compositor_interface;
}

auto CompositorBinder::create_interface(void* const data) -> std::unique_ptr<impl::Interface> {
    return std::unique_ptr<impl::Interface>(new Compositor(data));
}
} // namespace towl
