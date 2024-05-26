#pragma once
#include <memory>

#include <wayland-client.h>

#include "array.hpp"

namespace towl::impl {
struct AutoNativeKeyboardDeleter {
    uint32_t version;

    auto operator()(wl_keyboard* keyboard) -> void;
};

using AutoNativeKeyboard = std::unique_ptr<wl_keyboard, AutoNativeKeyboardDeleter>;
} // namespace towl::impl

namespace towl {
class KeyboardCallbacks {
  public:
    virtual auto on_wl_keyboard_keymap(uint32_t /*format*/, int32_t /*fd*/, uint32_t /*size*/) -> void {}
    virtual auto on_wl_keyboard_enter(wl_surface* /*surface*/, const Array<uint32_t>& /*keys*/) -> void {}
    virtual auto on_wl_keyboard_leave(wl_surface* /*surface*/) -> void {}
    virtual auto on_wl_keyboard_key(uint32_t /*key*/, uint32_t /*state*/) -> void {}
    virtual auto on_wl_keyboard_modifiers(uint32_t /*mods_depressed*/, uint32_t /*mods_latched*/, uint32_t /*mods_locked*/, uint32_t /*group*/) -> void {}
    virtual auto on_wl_keyboard_repeat_info(int32_t /*rate*/, int32_t /*delay*/) -> void {}
    virtual ~KeyboardCallbacks(){};
};

class Keyboard {
  private:
    impl::AutoNativeKeyboard keyboard;
    KeyboardCallbacks*       callbacks;

    static auto keymap(void* data, wl_keyboard* wl_keyboard, uint32_t format, int32_t fd, uint32_t size) -> void;
    static auto enter(void* data, wl_keyboard* wl_keyboard, uint32_t serial, wl_surface* surface, wl_array* keys) -> void;
    static auto leave(void* data, wl_keyboard* wl_keyboard, uint32_t serial, wl_surface* surface) -> void;
    static auto key(void* data, wl_keyboard* wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) -> void;
    static auto modifiers(void* data, wl_keyboard* wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) -> void;
    static auto repeat_info(void* data, wl_keyboard* wl_keyboard, int32_t rate, int32_t delay) -> void;

    static inline wl_keyboard_listener listener = {keymap, enter, leave, key, modifiers, repeat_info};

  public:
    Keyboard(wl_keyboard* keyboard, uint32_t version, KeyboardCallbacks* callbacks);
};
} // namespace towl
