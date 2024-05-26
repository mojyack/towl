#include "keyboard.hpp"

namespace towl::impl {
auto AutoNativeKeyboardDeleter::operator()(wl_keyboard* const keyboard) -> void {
    if(version >= WL_KEYBOARD_RELEASE_SINCE_VERSION) {
        wl_keyboard_release(keyboard);
    } else {
        wl_keyboard_destroy(keyboard);
    }
}
} // namespace towl::impl

namespace towl {
auto Keyboard::keymap(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t format, const int32_t fd, const uint32_t size) -> void {
    auto& self = *std::bit_cast<Keyboard*>(data);
    self.callbacks->on_wl_keyboard_keymap(format, fd, size);
}

auto Keyboard::enter(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, wl_surface* const surface, wl_array* const keys) -> void {
    auto& self = *std::bit_cast<Keyboard*>(data);
    self.callbacks->on_wl_keyboard_enter(surface, *keys);
}

auto Keyboard::leave(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, wl_surface* const surface) -> void {
    auto& self = *std::bit_cast<Keyboard*>(data);
    self.callbacks->on_wl_keyboard_leave(surface);
}

auto Keyboard::key(void* const data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, const uint32_t /*time*/, const uint32_t key, const uint32_t state) -> void {
    auto& self = *std::bit_cast<Keyboard*>(data);
    self.callbacks->on_wl_keyboard_key(key, state);
}

auto Keyboard::modifiers(void* data, wl_keyboard* const /*wl_keyboard*/, const uint32_t /*serial*/, const uint32_t mods_depressed, const uint32_t mods_latched, const uint32_t mods_locked, const uint32_t group) -> void {
    auto& self = *std::bit_cast<Keyboard*>(data);
    self.callbacks->on_wl_keyboard_modifiers(mods_depressed, mods_latched, mods_locked, group);
}

auto Keyboard::repeat_info(void* const data, wl_keyboard* const /*wl_keyboard*/, const int32_t rate, const int32_t delay) -> void {
    auto& self = *std::bit_cast<Keyboard*>(data);
    self.callbacks->on_wl_keyboard_repeat_info(rate, delay);
}

Keyboard::Keyboard(wl_keyboard* const keyboard, const uint32_t version, KeyboardCallbacks* const callbacks)
    : keyboard(keyboard, {version}),
      callbacks(callbacks){};
} // namespace towl
