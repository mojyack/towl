#pragma once
#include <wayland-client.h>

#include "interface.hpp"
#include "macros/autoptr.hpp"

namespace towl::impl {
declare_autoptr(NativeShell, wl_shell, wl_shell_destroy);
declare_autoptr(NativeShellSurface, wl_shell_surface, wl_shell_surface_destroy);
} // namespace towl::impl

namespace towl {
class ShellSurfaceCallbacks {
  public:
    virtual auto on_wl_shell_surface_configure(uint32_t /*width*/, uint32_t /*height*/) -> void {}
};

class ShellSurface {
  private:
    impl::AutoNativeShellSurface shell_surface;
    ShellSurfaceCallbacks*       callbacks;

    static auto ping(void* data, wl_shell_surface* shell_surface, uint32_t serial) -> void;
    static auto configure(void* data, wl_shell_surface* shell_surface, const uint32_t edges, const int32_t width, const int32_t height) -> void;
    static auto popup_done(void* /*data*/, struct wl_shell_surface* /*shell_surface*/) -> void {}

    static inline wl_shell_surface_listener listener = {ping, configure, popup_done};

  public:
    ShellSurface(wl_shell_surface* const shell_surface, const bool toplevel, ShellSurfaceCallbacks* callbacks);
};

class Shell : public impl::Interface {
  private:
    impl::AutoNativeShell shell;

  public:
    auto create_surface(wl_surface* surface, bool toplevel, ShellSurfaceCallbacks* callbacks) -> ShellSurface;

    Shell(void* data);
};

// version = 1
struct ShellBinder : impl::InterfaceBinder {
    auto get_interface_description() -> const wl_interface* override;
    auto create_interface(void* data) -> std::unique_ptr<impl::Interface> override;

    ShellBinder(const uint32_t version)
        : InterfaceBinder(version) {}
};
} // namespace towl
