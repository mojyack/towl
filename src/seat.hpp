#pragma once
#include <optional>

#include <wayland-client.h>

#include "interface.hpp"
#include "keyboard.hpp"
#include "pointer.hpp"
#include "touch.hpp"

namespace towl::impl {
struct AutoNativeSeatDeleter {
    uint32_t version;

    auto operator()(wl_seat* seat) -> void;
};

using AutoNativeSeat = std::unique_ptr<wl_seat, AutoNativeSeatDeleter>;
} // namespace towl::impl

namespace towl {
class Seat : public impl::Interface {
  private:
    impl::AutoNativeSeat    seat;
    std::optional<Keyboard> keyboard;
    std::optional<Pointer>  pointer;
    std::optional<Touch>    touch;
    KeyboardCallbacks*      keyboard_callbacks;
    PointerCallbacks*       pointer_callbacks;
    TouchCallbacks*         touch_callbacks;

    static auto capabilities(void* data, wl_seat* seat, uint32_t cap) -> void;
    static auto name(void* const /*data*/, wl_seat* const /*wl_seat*/, const char* const /*name*/) -> void {}

    static inline wl_seat_listener listener = {capabilities, name};

  public:
    Seat(void* data, uint32_t version, KeyboardCallbacks* keyboard_callbacks, PointerCallbacks* pointer_callbacks, TouchCallbacks* touch_callbacks);
};

// version = 1 ~ 8
struct SeatBinder : impl::InterfaceBinder {
    KeyboardCallbacks* keyboard_callbacks; // nullable
    PointerCallbacks*  pointer_callbacks;  // nullable
    TouchCallbacks*    touch_callbacks;    // nullable

    auto get_interface_description() -> const wl_interface* override;
    auto create_interface(void* data) -> std::unique_ptr<impl::Interface> override;

    SeatBinder(const uint32_t version, KeyboardCallbacks* keyboard_callbacks, PointerCallbacks* pointer_callbacks, TouchCallbacks* touch_callbacks)
        : InterfaceBinder(version),
          keyboard_callbacks(keyboard_callbacks),
          pointer_callbacks(pointer_callbacks),
          touch_callbacks(touch_callbacks) {}
};
} // namespace towl
