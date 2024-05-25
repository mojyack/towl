#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <EGL/egl.h>

#define TOWL_NS towl
#include "towl/towl.hpp"

#include "util/assert.hpp"

using Compositor = towl::Compositor<4>;
using WMBase     = towl::WMBase<2>;

struct EGLObject {
    EGLDisplay display = nullptr;
    EGLConfig  config  = nullptr;
    EGLContext context = nullptr;

    EGLObject(towl::Display& wl_display) {
        display = eglGetDisplay(wl_display.native());
        dynamic_assert(display != EGL_NO_DISPLAY);

        auto major = EGLint(0);
        auto minor = EGLint(0);
        dynamic_assert(eglInitialize(display, &major, &minor) != EGL_FALSE);
        dynamic_assert((major == 1 && minor >= 4) || major >= 2);
        dynamic_assert(eglBindAPI(EGL_OPENGL_API) != EGL_FALSE);

        constexpr auto config_attribs = std::array<EGLint, 15>{EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                                               EGL_RED_SIZE, 8,
                                                               EGL_GREEN_SIZE, 8,
                                                               EGL_BLUE_SIZE, 8,
                                                               EGL_ALPHA_SIZE, 8,
                                                               EGL_SAMPLES, 4,
                                                               EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
                                                               EGL_NONE};

        auto num = EGLint(0);
        dynamic_assert(eglChooseConfig(display, config_attribs.data(), &config, 1, &num) != EGL_FALSE && num != 0);

        constexpr auto context_attribs = std::array<EGLint, 3>{EGL_CONTEXT_CLIENT_VERSION, 2,
                                                               EGL_NONE};

        context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs.data());
        dynamic_assert(context != EGL_NO_CONTEXT);
    }

    ~EGLObject() {
        dynamic_assert(eglDestroyContext(display, context) != EGL_FALSE);
        dynamic_assert(eglTerminate(display) != EGL_FALSE);
    }
};

auto main() -> int {
    // connect to display
    auto display = towl::Display();

    // bind interfaces
    auto registry = display.get_registry<towl::Empty, Compositor, WMBase>({});
    display.roundtrip();
    dynamic_assert(!registry.interface<Compositor>().empty() && !registry.interface<WMBase>().empty(), "wayland server doesn't provide necessary interfaces");

    // initialize egl
    auto egl = EGLObject(display);

    // get surface from compositor
    auto compositor = registry.interface<Compositor>()[0].get();
    auto surface    = compositor->create_surface(towl::Empty{});

    // create xdg_surface and xdg_toplevel
    auto wmbase       = registry.interface<WMBase>()[0].get();
    auto xdg_surface  = wmbase->create_xdg_surface(surface);
    auto xdg_toplevel = xdg_surface.create_xdg_toplevel(towl::Empty{});

    // then commit surface changes
    surface.commit();

    // create egl surface
    auto egl_window = towl::EGLWindow(surface, 800, 600);
    auto eglsurface = eglCreateWindowSurface(egl.display, egl.config, reinterpret_cast<EGLNativeWindowType>(egl_window.native()), nullptr);
    dynamic_assert(eglsurface != EGL_NO_SURFACE);

    // process configure events before drawing anything
    display.roundtrip();

    // fill screen
    dynamic_assert(eglMakeCurrent(egl.display, eglsurface, eglsurface, egl.context) != EGL_FALSE);
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    dynamic_assert(eglSwapBuffers(egl.display, eglsurface) != EGL_FALSE);

    // main loop
    while(display.dispatch()) {
    }
    return 0;
}
