#pragma once
#include <cassert>
#include <memory>

#include <wayland-egl-core.h>

#include "internal.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

class EGLWindow {
  private:
    struct Deleter {
        auto operator()(wl_egl_window* const egl_window) -> void {
            wl_egl_window_destroy(egl_window);
        }
    };

    std::unique_ptr<wl_egl_window, Deleter> egl_window;

  public:
    auto native() -> wl_egl_window* {
        return egl_window.get();
    }

    auto resize(const int width, const int height, const int dx, const int dy) -> void {
        wl_egl_window_resize(egl_window.get(), width, height, dx, dy);
    }

    auto get_attached_size(int& width, int& height) -> void {
        wl_egl_window_get_attached_size(egl_window.get(), &width, &height);
    }

    EGLWindow(internal::SurfaceLike auto& surface, const int width, const int height) : egl_window(wl_egl_window_create(surface.native(), width, height)) {
        assert(egl_window);
    }
};

#ifdef TOWL_NS
}
#endif
