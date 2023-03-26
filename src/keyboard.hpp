#pragma once
#include <cassert>
#include <memory>

#include "common.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

template <class Glue>
concept KeyboardOnKeymap = requires(Glue& m, uint32_t format, int32_t fd, uint32_t size) {
                               m.on_keymap(format, fd, size);
                           };

template <class Glue>
concept KeyboardOnEnter = requires(Glue& m, SurfaceTag surface, const Array<uint32_t>& keys) {
                              m.on_enter(surface, keys);
                          };

template <class Glue>
concept KeyboardOnLeave = requires(Glue& m, SurfaceTag surface) {
                              m.on_leave(surface);
                          };

template <class Glue>
concept KeyboardOnKey = requires(Glue& m, uint32_t key, uint32_t state) {
                            m.on_key(key, state);
                        };

template <class Glue>
concept KeyboardOnModifiers = requires(Glue& m, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
                                  m.on_modifiers(mods_depressed, mods_latched, mods_locked, group);
                              };

template <class Glue>
concept KeyboardOnRepeatInfo = requires(Glue& m, int32_t rate, int32_t delay) {
                                   m.on_repeat_info(rate, delay);
                               };

template <class Glue>
concept KeyboardGlue =
    KeyboardOnKeymap<Glue> ||
    KeyboardOnEnter<Glue> ||
    KeyboardOnLeave<Glue> ||
    KeyboardOnKey<Glue> ||
    KeyboardOnModifiers<Glue> ||
    KeyboardOnRepeatInfo<Glue> ||
    IsEmpty<Glue>;

template <uint32_t version, KeyboardGlue KeyboardGlue>
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
        if constexpr(KeyboardOnKeymap<KeyboardGlue>) {
            auto& self = *std::bit_cast<Keyboard*>(data);
            self.glue->on_keymap(format, fd, size);
        }
    }

    static auto enter(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, wl_surface* const surface, wl_array* const keys) -> void {
        if constexpr(KeyboardOnEnter<KeyboardGlue>) {
            auto& self = *std::bit_cast<Keyboard*>(data);
            self.glue->on_enter(std::bit_cast<SurfaceTag>(surface), Array<uint32_t>(*keys));
        }
    }

    static auto leave(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, wl_surface* const surface) -> void {
        if constexpr(KeyboardOnLeave<KeyboardGlue>) {
            auto& self = *std::bit_cast<Keyboard*>(data);
            self.glue->on_leave(std::bit_cast<SurfaceTag>(surface));
        }
    }

    static auto key(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, const uint32_t /*time*/, const uint32_t key, const uint32_t state) -> void {
        if constexpr(KeyboardOnKey<KeyboardGlue>) {
            auto& self = *std::bit_cast<Keyboard*>(data);
            self.glue->on_key(key, state);
        }
    }

    static auto modifiers(void* data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, const uint32_t mods_depressed, const uint32_t mods_latched, uint32_t mods_locked, uint32_t group) -> void {
        if constexpr(KeyboardOnModifiers<KeyboardGlue>) {
            auto& self = *std::bit_cast<Keyboard*>(data);
            self.glue->on_modifiers(mods_depressed, mods_latched, mods_locked, group);
        }
    }

    static auto repeat_info(void* const data, wl_keyboard* const /*wl_keyboard*/, const int32_t rate, const int32_t delay) {
        if constexpr(KeyboardOnRepeatInfo<KeyboardGlue>) {
            auto& self = *std::bit_cast<Keyboard*>(data);
            self.glue->on_repeat_info(rate, delay);
        }
    }

    static inline wl_keyboard_listener listener = {keymap, enter, leave, key, modifiers, repeat_info};

    KeyboardGlue* glue;

  public:
    Keyboard(wl_keyboard* const keyboard, KeyboardGlue& glue) : keyboard(keyboard), glue(&glue) {
        static_assert(!(KeyboardOnKeymap<KeyboardGlue> && version < WL_KEYBOARD_KEYMAP_SINCE_VERSION));
        static_assert(!(KeyboardOnEnter<KeyboardGlue> && version < WL_KEYBOARD_ENTER_SINCE_VERSION));
        static_assert(!(KeyboardOnLeave<KeyboardGlue> && version < WL_KEYBOARD_LEAVE_SINCE_VERSION));
        static_assert(!(KeyboardOnKey<KeyboardGlue> && version < WL_KEYBOARD_KEY_SINCE_VERSION));
        static_assert(!(KeyboardOnModifiers<KeyboardGlue> && version < WL_KEYBOARD_MODIFIERS_SINCE_VERSION));
        static_assert(!(KeyboardOnRepeatInfo<KeyboardGlue> && version < WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION));

        assert(keyboard);
        wl_keyboard_add_listener(keyboard, &listener, this);
    }
};

#ifdef TOWL_NS
}
#endif
