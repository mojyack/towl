#include "shell.hpp"
#include "util/assert.hpp"

namespace towl {
auto ShellSurface::ping(void* const /*data*/, wl_shell_surface* const shell_surface, const uint32_t serial) -> void {
    wl_shell_surface_pong(shell_surface, serial);
}

auto ShellSurface::configure(void* const data, wl_shell_surface* const /*shell_surface*/, const uint32_t /*edges*/, const int32_t width, const int32_t height) -> void {
    auto& self = *std::bit_cast<ShellSurface*>(data);
    self.callbacks->on_wl_shell_surface_configure(width, height);
}

ShellSurface::ShellSurface(wl_shell_surface* const shell_surface, const bool toplevel, ShellSurfaceCallbacks* callbacks)
    : shell_surface(shell_surface),
      callbacks(callbacks) {
    dynamic_assert(shell_surface != NULL);
    wl_shell_surface_add_listener(shell_surface, &listener, this);
    if(toplevel) {
        wl_shell_surface_set_toplevel(shell_surface);
    }
}

auto Shell::create_surface(wl_surface* const surface, const bool toplevel, ShellSurfaceCallbacks* const callbacks) -> ShellSurface {
    return ShellSurface(wl_shell_get_shell_surface(shell.get(), surface), toplevel, callbacks);
}

Shell::Shell(void* const data)
    : shell(std::bit_cast<wl_shell*>(data)) {}

auto ShellBinder::get_interface_description() -> const wl_interface* {
    return &wl_shell_interface;
}

auto ShellBinder::create_interface(void* const data) -> std::unique_ptr<impl::Interface> {
    return std::unique_ptr<impl::Interface>(new Shell(data));
}
} // namespace towl
