#pragma once
// #include <cassert>
// #include <memory>

#include <wayland-egl-core.h>

#include "macros/autoptr.hpp"

namespace towl::impl {
declare_autoptr(NativeEGLWindow, wl_egl_window, wl_egl_window_destroy);
} // namespace towl::impl

namespace towl {
class EGLWindow {
  private:
    impl::AutoNativeEGLWindow egl_window;

  public:
    auto native() -> wl_egl_window*;
    auto resize(int width, int height, int dx, int dy) -> void;
    auto get_attached_size() const -> std::pair<int, int>;

    EGLWindow(wl_surface* const surface, const int width, const int height);
};
} // namespace towl
