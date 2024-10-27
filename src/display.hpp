#pragma once
#include <coop/event.hpp>
#include <coop/runner.hpp>
#include <wayland-client.h>

#include "macros/autoptr.hpp"

namespace towl::impl {
declare_autoptr(NativeDisplay, wl_display, wl_display_disconnect);
} // namespace towl::impl

namespace towl {
class DisplayReadIntent {
  private:
    wl_display* display;
    bool        done = false;

  public:
    auto read() -> void;
    auto cancel() -> void;

    auto operator=(DisplayReadIntent&) -> DisplayReadIntent& = delete;

    DisplayReadIntent(DisplayReadIntent&) = delete;
    DisplayReadIntent(wl_display* display);
    ~DisplayReadIntent();
};

class Display {
  private:
    impl::AutoNativeDisplay display;

    static auto done(void* const data, wl_callback* const callback, const uint32_t time) -> void;

    static inline wl_callback_listener listener = {done};

  public:
    auto native() -> wl_display*;
    auto wait_sync() -> coop::Async<void>;
    auto get_fd() -> int;
    auto obtain_read_intent() -> DisplayReadIntent;
    auto roundtrip() -> bool;
    auto dispatch() -> bool;
    auto dispatch_pending() -> bool;
    auto flush() -> void;
    auto get_registry() -> wl_registry*;

    Display();
};
} // namespace towl
