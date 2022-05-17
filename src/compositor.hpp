#pragma once
#include <cassert>
#include <functional>
#include <memory>

#include "common.hpp"
#include "internal.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

template <class Glue>
concept CompositorSurfaceEnter = requires(Glue& m, OutputTag output) {
    m.on_enter(output);
};

template <class Glue>
concept CompositorSurfaceLeave = requires(Glue& m, OutputTag output) {
    m.on_leave(output);
};

template <class Glue>
concept CompositorSurfaceFrame = requires(Glue& m) {
    m.on_frame();
};

template <class Glue>
concept CompositorSurfaceGlue =
    (CompositorSurfaceEnter<Glue> ||
     CompositorSurfaceLeave<Glue> ||
     CompositorSurfaceFrame<Glue> ||
     IsEmpty<Glue>)&&std::movable<Glue>;

// version = 1 ~ 4
template <uint32_t version>
class Compositor {
  public:
    template <CompositorSurfaceGlue SurfaceGlue>
    class Surface {
      private:
        struct Deleter {
            auto operator()(wl_surface* const surface) -> void {
                static_assert(version >= WL_COMPOSITOR_CREATE_SURFACE_SINCE_VERSION);
                wl_surface_destroy(surface);
            }
        };

        std::unique_ptr<wl_surface, Deleter> surface;

        struct CallbackDeleter {
            auto operator()(wl_callback* const callback) -> void {
                wl_callback_destroy(callback);
            }
        };

        std::unique_ptr<wl_callback, CallbackDeleter> frame;

        static auto enter(void* const data, wl_surface* const /*surface*/, wl_output* const output) -> void {
            if constexpr(CompositorSurfaceEnter<SurfaceGlue>) {
                auto& self = *reinterpret_cast<Surface*>(data);
                self.glue.on_enter(reinterpret_cast<OutputTag>(output));
            }
        }
        static auto leave(void* const data, wl_surface* const /*surface*/, wl_output* const output) -> void {
            if constexpr(CompositorSurfaceEnter<SurfaceGlue>) {
                auto& self = *reinterpret_cast<Surface*>(data);
                self.glue.on_leave(reinterpret_cast<OutputTag>(output));
            }
        }

        static inline wl_surface_listener listener = {enter, leave};

        [[no_unique_address]] std::conditional_t<!IsEmpty<SurfaceGlue>, SurfaceGlue, Empty> glue;

        static auto done(void* const data, wl_callback* const /*wl_callback*/, const uint32_t /*callback_data*/) -> void {
            if constexpr(CompositorSurfaceFrame<SurfaceGlue>) {
                auto& self = *reinterpret_cast<Surface*>(data);
                self.frame.reset();
                self.glue.on_frame();
            }
        }

        static inline wl_callback_listener frame_listener = {done};

      public:
        auto native() -> wl_surface* {
            return surface.get();
        }
        auto attach(internal::BufferLike auto& buffer, const int32_t x, const int32_t y) -> void {
            static_assert(version >= WL_SURFACE_ATTACH_SINCE_VERSION);
            wl_surface_attach(surface.get(), buffer.native(), x, y);
        }
        auto damage(const int32_t x, const int32_t y, const int32_t width, const int32_t height) -> void {
            static_assert(version >= WL_SURFACE_DAMAGE_BUFFER_SINCE_VERSION);
            wl_surface_damage_buffer(surface.get(), x, y, width, height);
        }
        auto commit() -> void {
            static_assert(version >= WL_SURFACE_COMMIT_SINCE_VERSION);
            wl_surface_commit(surface.get());
        }
        auto set_buffer_scale(const int32_t scale) -> void {
            static_assert(version >= WL_SURFACE_SET_BUFFER_SCALE_SINCE_VERSION);
            wl_surface_set_buffer_scale(surface.get(), scale);
        }
        auto set_frame() -> void {
            if constexpr(CompositorSurfaceFrame<SurfaceGlue>) {
                if(!frame) {
                    frame.reset(wl_surface_frame(surface.get()));
                    wl_callback_add_listener(frame.get(), &frame_listener, this);
                }
            }
        }
        auto as_tag() const -> SurfaceTag {
            return reinterpret_cast<size_t>(surface.get());
        }
        auto operator==(const SurfaceTag tag) const -> bool {
            return surface.get() == reinterpret_cast<wl_surface*>(tag);
        }
        Surface(wl_surface* const surface, SurfaceGlue&& glue) : surface(surface), glue(std::move(glue)) {
            static_assert(!(CompositorSurfaceEnter<SurfaceGlue> && version < WL_SURFACE_ENTER_SINCE_VERSION));
            static_assert(!(CompositorSurfaceLeave<SurfaceGlue> && version < WL_SURFACE_LEAVE_SINCE_VERSION));

            assert(surface);
            wl_surface_add_listener(surface, &listener, this);
        }
    };

  private:
    struct Deleter {
        auto operator()(wl_compositor* const compositor) -> void {
            wl_compositor_destroy(compositor);
        }
    };

    std::unique_ptr<wl_compositor, Deleter> compositor;
    uint32_t                                id;

  public:
    static auto info() -> internal::InterfaceInfo {
        return {"wl_compositor", version, &wl_compositor_interface};
    }
    auto interface_id() const -> uint32_t {
        return id;
    }
    template <CompositorSurfaceGlue Glue>
    auto create_surface(Glue&& glue) -> Surface<Glue> {
        static_assert(version >= WL_COMPOSITOR_CREATE_SURFACE_SINCE_VERSION);

        return {wl_compositor_create_surface(compositor.get()), std::move(glue)};
    }
    Compositor(void* const data, const uint32_t id) : compositor(reinterpret_cast<wl_compositor*>(data)), id(id) {}
};

#ifdef TOWL_NS
}
#endif
