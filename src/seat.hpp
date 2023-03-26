#pragma once
#include <cassert>
#include <memory>
#include <optional>

#include "common.hpp"
#include "internal.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

// version = 1 ~ 8
template <uint32_t version, KeyboardGlue KeyboardGlue, PointerGlue PointerGlue>
class Seat {
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

    static auto capabilities(void* const data, wl_seat* const /*seat*/, const uint32_t cap) -> void {
        auto& self = *std::bit_cast<Seat*>(data);
        if constexpr(!IsEmpty<KeyboardGlue>) {
            if(cap & WL_SEAT_CAPABILITY_KEYBOARD) {
                static_assert(version >= WL_SEAT_GET_KEYBOARD_SINCE_VERSION);
                self.keyboard.emplace(wl_seat_get_keyboard(self.seat.get()), self.keyboard_glue);
            } else {
                self.keyboard.reset();
            }
        }
        if constexpr(!IsEmpty<PointerGlue>) {
            if(cap & WL_SEAT_CAPABILITY_POINTER) {
                static_assert(version >= WL_SEAT_GET_POINTER_SINCE_VERSION);
                self.pointer.emplace(wl_seat_get_pointer(self.seat.get()), self.pointer_glue);
            } else {
                self.pointer.reset();
            }
        }
    }

    static auto name(void* const /*data*/, wl_seat* const /*wl_seat*/, const char* const /*name*/) -> void {}

    static inline wl_seat_listener listener = {capabilities, name};

    uint32_t id;

    using KeyboardOpt = std::optional<Keyboard<version, KeyboardGlue>>;
    using PointerOpt  = std::optional<Pointer<version, PointerGlue>>;

    [[no_unique_address]] KeyboardGlue                                                   keyboard_glue;
    [[no_unique_address]] PointerGlue                                                    pointer_glue;
    [[no_unique_address]] std::conditional_t<!IsEmpty<KeyboardGlue>, KeyboardOpt, Empty> keyboard;
    [[no_unique_address]] std::conditional_t<!IsEmpty<PointerGlue>, PointerOpt, Empty>   pointer;

  public:
    static auto info() -> internal::InterfaceInfo {
        return {"wl_seat", version, &wl_seat_interface};
    }

    auto interface_id() const -> uint32_t {
        return id;
    }

    Seat(void* const data, const uint32_t id, auto glue_param)
        : seat(std::bit_cast<wl_seat*>(data)),
          id(id),
          keyboard_glue(glue_param),
          pointer_glue(glue_param) {
        static_assert(version >= WL_SEAT_CAPABILITIES_SINCE_VERSION);
        static_assert(version >= WL_SEAT_NAME_SINCE_VERSION);

        wl_seat_add_listener(seat.get(), &listener, this);
    }
};

#ifdef TOWL_NS
}
#endif
