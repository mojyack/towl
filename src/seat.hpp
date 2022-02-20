#pragma once
#include <cassert>
#include <memory>
#include <optional>

#include "common.hpp"
#include "internal.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

template <class Glue>
concept SeatPointerOnEnter = requires(Glue& m, SurfaceTag surface, double x, double y) {
    m.on_enter(surface, x, y);
};

template <class Glue>
concept SeatPointerOnLeave = requires(Glue& m, SurfaceTag surface) {
    m.on_leave(surface);
};

template <class Glue>
concept SeatPointerOnMotion = requires(Glue& m, double x, double y) {
    m.on_motion(x, y);
};

template <class Glue>
concept SeatPointerOnButton = requires(Glue& m, uint32_t button, uint32_t state) {
    m.on_button(button, state);
};

template <class Glue>
concept SeatPointerOnAxis = requires(Glue& m, uint32_t axis, double value) {
    m.on_axis(axis, value);
};

template <class Glue>
concept SeatPointerOnFrame = requires(Glue& m) {
    m.on_frame();
};

template <class Glue>
concept SeatPointerOnAxisSource = requires(Glue& m, uint32_t source) {
    m.on_axis_source(source);
};

template <class Glue>
concept SeatPointerOnAxisStop = requires(Glue& m, uint32_t axis) {
    m.on_axis_stop(axis);
};

template <class Glue>
concept SeatPointerOnAxisDiscrete = requires(Glue& m, uint32_t axis, int32_t discrete) {
    m.on_axis_discrete(axis, discrete);
};

template <class Glue>
concept SeatPointerGlue =
    SeatPointerOnEnter<Glue> ||
    SeatPointerOnLeave<Glue> ||
    SeatPointerOnMotion<Glue> ||
    SeatPointerOnButton<Glue> ||
    SeatPointerOnAxis<Glue> ||
    SeatPointerOnFrame<Glue> ||
    SeatPointerOnAxisSource<Glue> ||
    SeatPointerOnAxisStop<Glue> ||
    SeatPointerOnAxisDiscrete<Glue> ||
    IsEmpty<Glue>;

template <class Glue>
concept SeatKeyboardOnKeymap = requires(Glue& m, uint32_t format, int32_t fd, uint32_t size) {
    m.on_keymap(format, fd, size);
};

template <class Glue>
concept SeatKeyboardOnEnter = requires(Glue& m, SurfaceTag surface, const Array<uint32_t>& keys) {
    m.on_enter(surface, keys);
};

template <class Glue>
concept SeatKeyboardOnLeave = requires(Glue& m, SurfaceTag surface) {
    m.on_leave(surface);
};

template <class Glue>
concept SeatKeyboardOnKey = requires(Glue& m, uint32_t key, uint32_t state) {
    m.on_key(key, state);
};

template <class Glue>
concept SeatKeyboardOnModifiers = requires(Glue& m, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
    m.on_modifiers(mods_depressed, mods_latched, mods_locked, group);
};

template <class Glue>
concept SeatKeyboardOnRepeatInfo = requires(Glue& m, int32_t rate, int32_t delay) {
    m.on_repeat_info(rate, delay);
};

template <class Glue>
concept SeatKeyboardGlue =
    SeatKeyboardOnKeymap<Glue> ||
    SeatKeyboardOnEnter<Glue> ||
    SeatKeyboardOnLeave<Glue> ||
    SeatKeyboardOnKey<Glue> ||
    SeatKeyboardOnModifiers<Glue> ||
    SeatKeyboardOnRepeatInfo<Glue> ||
    IsEmpty<Glue>;

template <class Glue>
concept GlueWithSeatPointerGlue = requires(Glue& m) {
    SeatPointerGlue<decltype(m.pointer_glue)>;
};

template <class Glue>
concept GlueWithSeatKeyboardGlue = requires(Glue& m) {
    SeatKeyboardGlue<decltype(m.keyboard_glue)>;
};

template <class Glue>
concept SeatGlue =
    (GlueWithSeatPointerGlue<Glue> ||
     GlueWithSeatKeyboardGlue<Glue> ||
     IsEmpty<Glue>)&&std::movable<Glue>;

// version = 1 ~ 7
template <uint32_t version, SeatGlue SeatGlue>
class Seat {
  public:
    template <SeatPointerGlue PointerGlue>
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
            if constexpr(SeatPointerOnEnter<PointerGlue>) {
                auto& self = *reinterpret_cast<Pointer*>(data);
                self.glue->on_enter(reinterpret_cast<SurfaceTag>(surface), x / 256, y / 256);
            }
        }
        static auto leave(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*serial*/, wl_surface* const surface) -> void {
            if constexpr(SeatPointerOnLeave<PointerGlue>) {
                auto& self = *reinterpret_cast<Pointer*>(data);
                self.glue->on_leave(reinterpret_cast<SurfaceTag>(surface));
            }
        }
        static auto motion(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*time*/, const wl_fixed_t x, const wl_fixed_t y) -> void {
            if constexpr(SeatPointerOnMotion<PointerGlue>) {
                auto& self = *reinterpret_cast<Pointer*>(data);
                self.glue->on_motion(x / 256, y / 256);
            }
        }
        static auto button(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*serial*/, const uint32_t /*time*/, const uint32_t button, const uint32_t state) -> void {
            if constexpr(SeatPointerOnButton<PointerGlue>) {
                auto& self = *reinterpret_cast<Pointer*>(data);
                self.glue->on_button(button, state);
            }
        }
        static auto axis(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*time*/, const uint32_t axis, const wl_fixed_t value) -> void {
            if constexpr(SeatPointerOnAxis<PointerGlue>) {
                auto& self = *reinterpret_cast<Pointer*>(data);
                self.glue->on_axis(axis, value / 256);
            }
        }
        static auto frame(void* const data, wl_pointer* const /*pointer*/) -> void {
            if constexpr(SeatPointerOnFrame<PointerGlue>) {
                auto& self = *reinterpret_cast<Pointer*>(data);
                self.glue->on_frame();
            }
        }
        static auto axis_source(void* const data, wl_pointer* const /*pointer*/, const uint32_t axis_source) -> void {
            if constexpr(SeatPointerOnAxisSource<PointerGlue>) {
                auto& self = *reinterpret_cast<Pointer*>(data);
                self.glue->on_axis_source(axis_source);
            }
        }
        static auto axis_stop(void* const data, wl_pointer* const /*pointer*/, const uint32_t /*time*/, const uint32_t axis) -> void {
            if constexpr(SeatPointerOnAxisStop<PointerGlue>) {
                auto& self = *reinterpret_cast<Pointer*>(data);
                self.glue->on_axis_stop(axis);
            }
        }
        static auto axis_descrete(void* const data, wl_pointer* const /*pointer*/, const uint32_t axis, const int32_t descrete) -> void {
            if constexpr(SeatPointerOnAxisDiscrete<PointerGlue>) {
                auto& self = *reinterpret_cast<Pointer*>(data);
                self.glue->on_axis_descrete(axis, descrete);
            }
        }

        static inline wl_pointer_listener listener = {enter, leave, motion, button, axis, frame, axis_source, axis_stop, axis_descrete};

        [[no_unique_address]] typename std::conditional<!IsEmpty<PointerGlue>, PointerGlue*, Empty>::type glue;

      public:
        Pointer(wl_pointer* const pointer, PointerGlue& glue) : pointer(pointer), glue(&glue) {
            static_assert(!(SeatPointerOnEnter<PointerGlue> && version < WL_POINTER_ENTER_SINCE_VERSION));
            static_assert(!(SeatPointerOnLeave<PointerGlue> && version < WL_POINTER_LEAVE_SINCE_VERSION));
            static_assert(!(SeatPointerOnMotion<PointerGlue> && version < WL_POINTER_MOTION_SINCE_VERSION));
            static_assert(!(SeatPointerOnButton<PointerGlue> && version < WL_POINTER_BUTTON_SINCE_VERSION));
            static_assert(!(SeatPointerOnAxis<PointerGlue> && version < WL_POINTER_AXIS_SINCE_VERSION));
            static_assert(!(SeatPointerOnFrame<PointerGlue> && version < WL_POINTER_FRAME_SINCE_VERSION));
            static_assert(!(SeatPointerOnAxisSource<PointerGlue> && version < WL_POINTER_AXIS_SOURCE_SINCE_VERSION));
            static_assert(!(SeatPointerOnAxisStop<PointerGlue> && version < WL_POINTER_AXIS_STOP_SINCE_VERSION));
            static_assert(!(SeatPointerOnAxisDiscrete<PointerGlue> && version < WL_POINTER_AXIS_DISCRETE_SINCE_VERSION));

            assert(pointer);
            wl_pointer_add_listener(pointer, &listener, this);
        }
    };

    template <SeatKeyboardGlue KeyboardGlue>
    class Keyboard {
      private:
        struct Deleter {
            auto operator()(wl_keyboard* const keyboard) -> void {
                if(version >= WL_KEYBOARD_RELEASE) {
                    wl_keyboard_release(keyboard);
                } else {
                    wl_keyboard_destroy(keyboard);
                }
            }
        };

        std::unique_ptr<wl_keyboard, Deleter> keyboard;

        static auto keymap(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t format, const int32_t fd, const uint32_t size) -> void {
            if constexpr(SeatKeyboardOnKeymap<KeyboardGlue>) {
                auto& self = *reinterpret_cast<Keyboard*>(data);
                self.glue->on_keymap(format, fd, size);
            }
        }
        static auto enter(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, wl_surface* const surface, wl_array* const keys) -> void {
            if constexpr(SeatKeyboardOnEnter<KeyboardGlue>) {
                auto& self = *reinterpret_cast<Keyboard*>(data);
                self.glue->on_enter(reinterpret_cast<SurfaceTag>(surface), Array<uint32_t>(*keys));
            }
        }
        static auto leave(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, wl_surface* const surface) -> void {
            if constexpr(SeatKeyboardOnLeave<KeyboardGlue>) {
                auto& self = *reinterpret_cast<Keyboard*>(data);
                self.glue->on_leave(reinterpret_cast<SurfaceTag>(surface));
            }
        }
        static auto key(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, const uint32_t /*time*/, const uint32_t key, const uint32_t state) -> void {
            if constexpr(SeatKeyboardOnKey<KeyboardGlue>) {
                auto& self = *reinterpret_cast<Keyboard*>(data);
                self.glue->on_key(key, state);
            }
        }
        static auto modifiers(void* data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, const uint32_t mods_depressed, const uint32_t mods_latched, uint32_t mods_locked, uint32_t group) -> void {
            if constexpr(SeatKeyboardOnModifiers<KeyboardGlue>) {
                auto& self = *reinterpret_cast<Keyboard*>(data);
                self.glue->on_modifiers(mods_depressed, mods_latched, mods_locked, group);
            }
        }
        static auto repeat_info(void* const data, wl_keyboard* const /*wl_keyboard*/, const int32_t rate, const int32_t delay) {
            if constexpr(SeatKeyboardOnRepeatInfo<KeyboardGlue>) {
                auto& self = *reinterpret_cast<Keyboard*>(data);
                self.glue->on_repeat_info(rate, delay);
            }
        }

        static inline wl_keyboard_listener listener = {keymap, enter, leave, key, modifiers, repeat_info};

        [[no_unique_address]] typename std::conditional<!IsEmpty<KeyboardGlue>, KeyboardGlue*, Empty>::type glue;

      public:
        Keyboard(wl_keyboard* const keyboard, KeyboardGlue& glue) : keyboard(keyboard), glue(&glue) {
            static_assert(!(SeatKeyboardOnKeymap<KeyboardGlue> && version < WL_KEYBOARD_KEYMAP_SINCE_VERSION));
            static_assert(!(SeatKeyboardOnEnter<KeyboardGlue> && version < WL_KEYBOARD_ENTER_SINCE_VERSION));
            static_assert(!(SeatKeyboardOnLeave<KeyboardGlue> && version < WL_KEYBOARD_LEAVE_SINCE_VERSION));
            static_assert(!(SeatKeyboardOnKey<KeyboardGlue> && version < WL_KEYBOARD_KEY_SINCE_VERSION));
            static_assert(!(SeatKeyboardOnModifiers<KeyboardGlue> && version < WL_KEYBOARD_MODIFIERS_SINCE_VERSION));
            static_assert(!(SeatKeyboardOnRepeatInfo<KeyboardGlue> && version < WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION));

            assert(keyboard);
            wl_keyboard_add_listener(keyboard, &listener, this);
        }
    };

  private:
    struct Deleter {
        auto operator()(wl_seat* seat) -> void {
            if(version >= WL_SEAT_RELEASE_SINCE_VERSION) {
                wl_seat_release(seat);
            } else {
                wl_seat_destroy(seat);
            }
        }
    };

    std::unique_ptr<wl_seat, Deleter> seat;
    uint32_t                          id;

    [[no_unique_address]] SeatGlue glue;
    using PointerGlue  = std::conditional_t<GlueWithSeatPointerGlue<SeatGlue>, decltype(glue.pointer_glue), Empty>;
    using KeyboardGlue = std::conditional_t<GlueWithSeatKeyboardGlue<SeatGlue>, decltype(glue.keyboard_glue), Empty>;
    [[no_unique_address]] std::conditional_t<GlueWithSeatPointerGlue<SeatGlue>, std::optional<Pointer<PointerGlue>>, Empty>    pointer;
    [[no_unique_address]] std::conditional_t<GlueWithSeatKeyboardGlue<SeatGlue>, std::optional<Keyboard<KeyboardGlue>>, Empty> keyboard;

    static auto capabilities(void* const data, wl_seat* const /*seat*/, const uint32_t cap) -> void {
        auto& self = *reinterpret_cast<Seat*>(data);
        if constexpr(SeatPointerGlue<PointerGlue>) {
            if(cap & WL_SEAT_CAPABILITY_POINTER) {
                static_assert(version >= WL_SEAT_GET_POINTER_SINCE_VERSION);
                if constexpr(GlueWithSeatPointerGlue<SeatGlue>) {
                    self.pointer.emplace(wl_seat_get_pointer(self.seat.get()), self.glue.pointer_glue);
                } else {
                    self.pointer.emplace(wl_seat_get_pointer(self.seat.get()), {});
                }
            } else {
                self.pointer.reset();
            }
        }
        if constexpr(SeatKeyboardGlue<KeyboardGlue>) {
            if(cap & WL_SEAT_CAPABILITY_KEYBOARD) {
                static_assert(version >= WL_SEAT_GET_KEYBOARD_SINCE_VERSION);
                if constexpr(GlueWithSeatKeyboardGlue<SeatGlue>) {
                    self.keyboard.emplace(wl_seat_get_keyboard(self.seat.get()), self.glue.keyboard_glue);
                } else {
                    self.keyboard.emplace(wl_seat_get_keyboard(self.seat.get()), {});
                }
            } else {
                self.keyboard.reset();
            }
        }
    }

    static auto name(void* const /*data*/, wl_seat* const /*wl_seat*/, const char* const /*name*/) -> void {}

    wl_seat_listener listener = {capabilities, name};

  public:
    static auto info() -> internal::InterfaceInfo {
        return {"wl_seat", version, &wl_seat_interface};
    }
    auto interface_id() const -> uint32_t {
        return id;
    }
    Seat(void* const data, const uint32_t id, SeatGlue&& glue) : seat(reinterpret_cast<wl_seat*>(data)), id(id), glue(glue) {
        static_assert(version >= WL_SEAT_CAPABILITIES_SINCE_VERSION);
        static_assert(version >= WL_SEAT_NAME_SINCE_VERSION);

        wl_seat_add_listener(seat.get(), &listener, this);
    }
};

#ifdef TOWL_NS
}
#endif
