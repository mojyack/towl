#pragma once
#include <cassert>
#include <memory>

#include "common.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

template <class Glue>
concept TouchOnDown = requires(Glue& m, SurfaceTag surface, const uint32_t id, double x, double y) {
                          m.on_down(surface, id, x, y);
                      };

template <class Glue>
concept TouchOnUp = requires(Glue& m, const uint32_t id) {
                        m.on_up(id);
                    };

template <class Glue>
concept TouchOnMotion = requires(Glue& m, const uint32_t id, double x, double y) {
                            m.on_motion(id, x, y);
                        };

template <class Glue>
concept TouchOnFrame = requires(Glue& m) {
                           m.on_frame();
                       };

template <class Glue>
concept TouchGlue =
    TouchOnDown<Glue> ||
    TouchOnUp<Glue> ||
    TouchOnMotion<Glue> ||
    TouchOnFrame<Glue> ||
    IsEmpty<Glue>;

template <uint32_t version, TouchGlue TouchGlue>
class Touch {
  private:
    struct Deleter {
        auto operator()(wl_touch* const touch) -> void {
            if(version >= WL_TOUCH_RELEASE) {
                wl_touch_release(touch);
            } else {
                wl_touch_destroy(touch);
            }
        }
    };

    std::unique_ptr<wl_touch, Deleter> touch;

    static auto down(void* const data, wl_touch* const /*wl_touch*/, const uint32_t /*serial*/, const uint32_t /*time*/, wl_surface* const surface, const int32_t id, const wl_fixed_t x, const wl_fixed_t y) -> void {
        if constexpr(TouchOnDown<TouchGlue>) {
            auto& self = *std::bit_cast<Touch*>(data);
            self.glue->on_down(std::bit_cast<SurfaceTag>(surface), id, x / 256.0, y / 256.0);
        }
    }

    static auto up(void* const data, wl_touch* const /*wl_touch*/, const uint32_t /*serial*/, const uint32_t /*time*/, const int32_t id) -> void {
        if constexpr(TouchOnUp<TouchGlue>) {
            auto& self = *std::bit_cast<Touch*>(data);
            self.glue->on_up(id);
        }
    }

    static auto motion(void* const data, wl_touch* const /*wl_touch*/, const uint32_t /*time*/, const int32_t id, const wl_fixed_t x, const wl_fixed_t y) -> void {
        if constexpr(TouchOnMotion<TouchGlue>) {
            auto& self = *std::bit_cast<Touch*>(data);
            self.glue->on_motion(id, x / 256.0, y / 256.0);
        }
    }

    static auto frame(void* const data, wl_touch* const /*wl_touch*/) -> void {
        if constexpr(TouchOnFrame<TouchGlue>) {
            auto& self = *std::bit_cast<Touch*>(data);
            self.glue->on_frame();
        }
    }

    static auto cancel(void* const data, wl_touch* const wl_touch) -> void {
        // not implemented
    }

    static auto shape(void* const data, wl_touch* const wl_touch, const int32_t id, const wl_fixed_t major, const wl_fixed_t minor) -> void {
        // not implemented
    }

    static auto orientation(void* const data, wl_touch* const wl_touch, const int32_t id, const wl_fixed_t orientation) -> void {
        // not implemented
    }

    static inline wl_touch_listener listener = {.down = down, .up = up, .motion = motion, .frame = frame, .cancel = cancel, .shape = shape, .orientation = orientation};

    TouchGlue* glue;

  public:
    Touch(wl_touch* const touch, TouchGlue& glue) : touch(touch), glue(&glue) {
        static_assert(!(TouchOnDown<TouchGlue> && version < WL_TOUCH_DOWN_SINCE_VERSION));
        static_assert(!(TouchOnUp<TouchGlue> && version < WL_TOUCH_UP_SINCE_VERSION));
        static_assert(!(TouchOnMotion<TouchGlue> && version < WL_TOUCH_MOTION_SINCE_VERSION));
        static_assert(!(TouchOnFrame<TouchGlue> && version < WL_TOUCH_FRAME_SINCE_VERSION));

        assert(touch);
        wl_touch_add_listener(touch, &listener, this);
    }
};

#ifdef TOWL_NS
}
#endif
