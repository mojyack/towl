#pragma once
#include <cassert>
#include <functional>
#include <memory>

#include "internal.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

// version = 1
template <uint32_t version>
class Shell {
  public:
    class ShellSurface {
      private:
        struct Deleter {
            auto operator()(wl_shell_surface* const shell_surface) -> void {
                wl_shell_surface_destroy(shell_surface);
            }
        };

        std::unique_ptr<wl_shell_surface, Deleter> shell_surface;

        std::function<void(int32_t, int32_t)> on_configure;

        static auto ping(void* const /*data*/, wl_shell_surface* const shell_surface, const uint32_t serial) -> void {
            wl_shell_surface_pong(shell_surface, serial);
        }
        static auto configure(void* const data, wl_shell_surface* const /*shell_surface*/, const uint32_t /*edges*/, const int32_t width, const int32_t height) -> void {
            auto& self = *reinterpret_cast<ShellSurface*>(data);
            if(self.on_configure) {
                self.on_configure(width, height);
            }
        }
        static auto popup_done(void* /*data*/, struct wl_shell_surface* /*shell_surface*/) -> void {}

        static inline wl_shell_surface_listener listener = {ping, configure, popup_done};

      public:
        auto set_configure_callback(std::function<void(int32_t, int32_t)> callback) {
            on_configure = callback;
        }
        ShellSurface(wl_shell_surface* const shell_surface, const bool toplevel) : shell_surface(shell_surface) {
            assert(shell_surface);
            wl_shell_surface_add_listener(shell_surface, &listener, this);
            if(toplevel) {
                wl_shell_surface_set_toplevel(shell_surface);
            }
        }
    };

  private:
    struct Deleter {
        auto operator()(wl_shell* const shell) -> void {
            wl_shell_destroy(shell);
        }
    };

    std::unique_ptr<wl_shell, Deleter> shell;
    uint32_t                           id;

  public:
    static auto info() -> internal::InterfaceInfo {
        return {"wl_shell", version, &wl_shell_interface};
    }
    auto interface_id() const -> uint32_t {
        return id;
    }
    auto create_shell_surface(internal::SurfaceLike auto& surface, const bool toplevel = true) -> ShellSurface {
        static_assert(version >= WL_SHELL_GET_SHELL_SURFACE_SINCE_VERSION);
        return {wl_shell_get_shell_surface(shell, surface.native()), toplevel};
    }
    Shell(void* const data, const uint32_t id) : shell(reinterpret_cast<wl_shell*>(data)), id(id) {}
};

#ifdef TOWL_NS
}
#endif
