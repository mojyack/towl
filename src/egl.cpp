#include "egl.hpp"
#include "util/assert.hpp"

namespace towl {
auto EGLWindow::native() -> wl_egl_window* {
    return egl_window.get();
}

auto EGLWindow::resize(const int width, const int height, const int dx, const int dy) -> void {
    wl_egl_window_resize(egl_window.get(), width, height, dx, dy);
}

auto EGLWindow::get_attached_size() const -> std::pair<int, int> {
    auto width  = 0;
    auto height = 0;
    wl_egl_window_get_attached_size(egl_window.get(), &width, &height);
    return {width, height};
}

EGLWindow::EGLWindow(wl_surface* const surface, const int width, const int height)
    : egl_window(wl_egl_window_create(surface, width, height)) {
    line_assert(egl_window.get() != NULL);
}
} // namespace towl
