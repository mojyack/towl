#pragma once
#include <cassert>
#include <memory>

#include "common.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

template <class Glue>
concept PointerOnEnter = requires(Glue& m, SurfaceTag surface, double x, double y) {
                             m.on_enter(surface, x, y);
                         };

template <class Glue>
concept PointerOnLeave = requires(Glue& m, SurfaceTag surface) {
                             m.on_leave(surface);
                         };

template <class Glue>
concept PointerOnMotion = requires(Glue& m, double x, double y) {
                              m.on_motion(x, y);
                          };

template <class Glue>
concept PointerOnButton = requires(Glue& m, uint32_t button, uint32_t state) {
                              m.on_button(button, state);
                          };

template <class Glue>
concept PointerOnAxis = requires(Glue& m, uint32_t axis, double value) {
                            m.on_axis(axis, value);
                        };

template <class Glue>
concept PointerOnFrame = requires(Glue& m) {
                             m.on_frame();
                         };

template <class Glue>
concept PointerOnAxisSource = requires(Glue& m, uint32_t source) {
                                  m.on_axis_source(source);
                              };

template <class Glue>
concept PointerOnAxisStop = requires(Glue& m, uint32_t axis) {
                                m.on_axis_stop(axis);
                            };

template <class Glue>
concept PointerOnAxisDiscrete = requires(Glue& m, uint32_t axis, int32_t discrete) {
                                    m.on_axis_discrete(axis, discrete);
                                };

template <class Glue>
concept PointerOnAxisValue120 = requires(Glue& m, uint32_t axis, int32_t value120) {
                                    m.on_axis_value120(axis, value120);
                                };

template <class Glue>
concept PointerGlue =
    PointerOnEnter<Glue> ||
    PointerOnLeave<Glue> ||
    PointerOnMotion<Glue> ||
    PointerOnButton<Glue> ||
    PointerOnAxis<Glue> ||
    PointerOnFrame<Glue> ||
    PointerOnAxisSource<Glue> ||
    PointerOnAxisStop<Glue> ||
    PointerOnAxisDiscrete<Glue> ||
    IsEmpty<Glue>;

template <uint32_t version, PointerGlue PointerGlue>
class Pointer {
  private:
    struct Deleter {
        auto operator()(wl_pointer* const pointer) -> void {
            if(version >= WL_POINTER_RELEASE) {
                wl_pointer_release(pointer);
            } else {
                wl_pointer_destroy(pointer);
            }
        }
    };

    std::unique_ptr<wl_pointer, Deleter> pointer;

    static auto enter(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*serial*/, wl_surface* const surface, const wl_fixed_t x, const wl_fixed_t y) -> void {
        if constexpr(PointerOnEnter<PointerGlue>) {
            auto& self = *std::bit_cast<Pointer*>(data);
            self.glue->on_enter(std::bit_cast<SurfaceTag>(surface), x / 256, y / 256);
        }
    }

    static auto leave(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*serial*/, wl_surface* const surface) -> void {
        if constexpr(PointerOnLeave<PointerGlue>) {
            auto& self = *std::bit_cast<Pointer*>(data);
            self.glue->on_leave(std::bit_cast<SurfaceTag>(surface));
        }
    }

    static auto motion(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*time*/, const wl_fixed_t x, const wl_fixed_t y) -> void {
        if constexpr(PointerOnMotion<PointerGlue>) {
            auto& self = *std::bit_cast<Pointer*>(data);
            self.glue->on_motion(x / 256, y / 256);
        }
    }

    static auto button(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*serial*/, const uint32_t /*time*/, const uint32_t button, const uint32_t state) -> void {
        if constexpr(PointerOnButton<PointerGlue>) {
            auto& self = *std::bit_cast<Pointer*>(data);
            self.glue->on_button(button, state);
        }
    }

    static auto axis(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*time*/, const uint32_t axis, const wl_fixed_t value) -> void {
        if constexpr(PointerOnAxis<PointerGlue>) {
            auto& self = *std::bit_cast<Pointer*>(data);
            self.glue->on_axis(axis, value / 256);
        }
    }

    static auto frame(void* const data, wl_pointer* const /*pointer*/) -> void {
        if constexpr(PointerOnFrame<PointerGlue>) {
            auto& self = *std::bit_cast<Pointer*>(data);
            self.glue->on_frame();
        }
    }

    static auto axis_source(void* const data, wl_pointer* const /*pointer*/, const uint32_t axis_source) -> void {
        if constexpr(PointerOnAxisSource<PointerGlue>) {
            auto& self = *std::bit_cast<Pointer*>(data);
            self.glue->on_axis_source(axis_source);
        }
    }

    static auto axis_stop(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*time*/, const uint32_t axis) -> void {
        if constexpr(PointerOnAxisStop<PointerGlue>) {
            auto& self = *std::bit_cast<Pointer*>(data);
            self.glue->on_axis_stop(axis);
        }
    }

    static auto axis_descrete(void* const data, wl_pointer* const /*pointer*/, const uint32_t axis, const int32_t descrete) -> void {
        if constexpr(PointerOnAxisDiscrete<PointerGlue>) {
            auto& self = *std::bit_cast<Pointer*>(data);
            self.glue->on_axis_descrete(axis, descrete);
        }
    }

    static auto axis_value120(void* const data, wl_pointer* const /*pointer*/, const uint32_t axis, const int32_t value120) -> void {
        if constexpr(PointerOnAxisValue120<PointerGlue>) {
            auto& self = *std::bit_cast<Pointer*>(data);
            self.glue->on_axis_descrete(axis, value120);
        }
    }

    static inline wl_pointer_listener listener = {enter, leave, motion, button, axis, frame, axis_source, axis_stop, axis_descrete, axis_value120};

    PointerGlue* glue;

  public:
    Pointer(wl_pointer* const pointer, PointerGlue& glue) : pointer(pointer), glue(&glue) {
        static_assert(!(PointerOnEnter<PointerGlue> && version < WL_POINTER_ENTER_SINCE_VERSION));
        static_assert(!(PointerOnLeave<PointerGlue> && version < WL_POINTER_LEAVE_SINCE_VERSION));
        static_assert(!(PointerOnMotion<PointerGlue> && version < WL_POINTER_MOTION_SINCE_VERSION));
        static_assert(!(PointerOnButton<PointerGlue> && version < WL_POINTER_BUTTON_SINCE_VERSION));
        static_assert(!(PointerOnAxis<PointerGlue> && version < WL_POINTER_AXIS_SINCE_VERSION));
        static_assert(!(PointerOnFrame<PointerGlue> && version < WL_POINTER_FRAME_SINCE_VERSION));
        static_assert(!(PointerOnAxisSource<PointerGlue> && version < WL_POINTER_AXIS_SOURCE_SINCE_VERSION));
        static_assert(!(PointerOnAxisStop<PointerGlue> && version < WL_POINTER_AXIS_STOP_SINCE_VERSION));
        static_assert(!(PointerOnAxisDiscrete<PointerGlue> && version < WL_POINTER_AXIS_DISCRETE_SINCE_VERSION));
        static_assert(!(PointerOnAxisValue120<PointerGlue> && version < WL_POINTER_AXIS_VALUE120_SINCE_VERSION));

        assert(pointer);
        wl_pointer_add_listener(pointer, &listener, this);
    }
};

#ifdef TOWL_NS
}
#endif
