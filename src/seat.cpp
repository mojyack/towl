#include "seat.hpp"

namespace towl::impl {
auto AutoNativeSeatDeleter::operator()(wl_seat* const seat) -> void {
    if(version >= WL_SEAT_RELEASE_SINCE_VERSION) {
        wl_seat_release(seat);
    } else {
        wl_seat_destroy(seat);
    }
}
} // namespace towl::impl

namespace towl {
auto Seat::capabilities(void* const data, wl_seat* const /*seat*/, const uint32_t cap) -> void {
    auto& self = *std::bit_cast<Seat*>(data);
    if(self.keyboard_callbacks && cap & WL_SEAT_CAPABILITY_KEYBOARD) {
        self.keyboard.emplace(wl_seat_get_keyboard(self.seat.get()), self.binder->version, self.keyboard_callbacks);
    } else {
        self.keyboard.reset();
    }
    if(self.pointer_callbacks && cap & WL_SEAT_CAPABILITY_POINTER) {
        self.pointer.emplace(wl_seat_get_pointer(self.seat.get()), self.binder->version, self.pointer_callbacks);
    } else {
        self.pointer.reset();
    }
    if(self.touch_callbacks && cap & WL_SEAT_CAPABILITY_TOUCH) {
        self.touch.emplace(wl_seat_get_touch(self.seat.get()), self.binder->version, self.touch_callbacks);
    } else {
        self.touch.reset();
    }
}

Seat::Seat(void* const data, const uint32_t version, KeyboardCallbacks* const keyboard_callbacks, PointerCallbacks* const pointer_callbacks, TouchCallbacks* const touch_callbacks)
    : seat(std::bit_cast<wl_seat*>(data), {version}),
      keyboard_callbacks(keyboard_callbacks),
      pointer_callbacks(pointer_callbacks),
      touch_callbacks(touch_callbacks) {}

auto SeatBinder::get_interface_description() -> const wl_interface* {
    return &wl_seat_interface;
}

auto SeatBinder::create_interface(void* const data) -> std::unique_ptr<impl::Interface> {
    return std::unique_ptr<impl::Interface>(new Seat(data, version, keyboard_callbacks, pointer_callbacks, touch_callbacks));
}
} // namespace towl
