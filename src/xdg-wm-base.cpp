#include "xdg-wm-base.hpp"
#include "macros/assert.hpp"

namespace towl {
auto XDGToplevel::configure(void* const data, xdg_toplevel* const /*toplevel*/, const int32_t width, const int32_t height, wl_array* const /*states*/) -> void {
    auto& self = *std::bit_cast<XDGToplevel*>(data);
    self.callbacks->on_xdg_toplevel_configure(width, height);
}

auto XDGToplevel::close(void* const data, xdg_toplevel* const /*toplevel*/) -> void {
    auto& self = *std::bit_cast<XDGToplevel*>(data);
    self.callbacks->on_xdg_toplevel_close();
}

auto XDGToplevel::set_title(const char* const title) -> void {
    xdg_toplevel_set_title(toplevel.get(), title);
}

auto XDGToplevel::init(XDGToplevelCallbacks* const callbacks) -> bool {
    ensure(toplevel != NULL);
    this->callbacks = callbacks;
    xdg_toplevel_add_listener(toplevel.get(), &listener, this);
    return true;
}

XDGToplevel::XDGToplevel(xdg_toplevel* const toplevel)
    : toplevel(toplevel) {
}

auto XDGSurface::configure(void* const /*data*/, xdg_surface* const surface, const uint32_t serial) -> void {
    // NOTE
    // ack should be called after rendering to the buffer is complete.
    // however, we did not find any problems with calling ack here and decided to call it here to increase responsiveness.
    xdg_surface_ack_configure(surface, serial);
}

auto XDGSurface::create_xdg_toplevel() -> XDGToplevel {
    return XDGToplevel(xdg_surface_get_toplevel(surface.get()));
}

auto XDGSurface::init() -> bool {
    ensure(surface != NULL);
    xdg_surface_add_listener(surface.get(), &listener, this);
    return true;
}

XDGSurface::XDGSurface(xdg_surface* const surface)
    : surface(surface) {
}

auto XDGWMBase::ping(void* const /*data*/, xdg_wm_base* const wm_base, const uint32_t serial) -> void {
    xdg_wm_base_pong(wm_base, serial);
}

auto XDGWMBase::create_xdg_surface(wl_surface* const surface) -> XDGSurface {
    return {xdg_wm_base_get_xdg_surface(wm_base.get(), surface)};
}

XDGWMBase::XDGWMBase(void* const data)
    : wm_base(std::bit_cast<xdg_wm_base*>(data)) {}

auto XDGWMBaseBinder::get_interface_description() -> const wl_interface* {
    return &xdg_wm_base_interface;
}

auto XDGWMBaseBinder::create_interface(void* const data) -> std::unique_ptr<impl::Interface> {
    return std::unique_ptr<impl::Interface>(new XDGWMBase(data));
}
} // namespace towl
