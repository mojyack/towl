#include "display.hpp"
#include "util/assert.hpp"

namespace towl {
auto DisplayReadIntent::read() -> void {
    if(done) {
        return;
    }
    wl_display_read_events(display);
    done = true;
}

auto DisplayReadIntent::cancel() -> void {
    if(!done) {
        wl_display_cancel_read(display);
    }
}

DisplayReadIntent::DisplayReadIntent(wl_display* const display) : display(display) {}

DisplayReadIntent::~DisplayReadIntent() {
    cancel();
}

auto Display::done(void* const data, wl_callback* const callback, const uint32_t /*time*/) -> void {
    auto& event = *std::bit_cast<coop::Event*>(data);
    event.notify();
    wl_callback_destroy(callback);
}

auto Display::native() -> wl_display* {
    return display.get();
}

auto Display::wait_sync() -> coop::Async<void> {
    auto event = coop::Event();
    auto sync  = wl_display_sync(display.get());
    wl_callback_add_listener(sync, &listener, &event);
    flush();
    co_await event;
}

auto Display::get_fd() -> int {
    return wl_display_get_fd(display.get());
}

auto Display::obtain_read_intent() -> DisplayReadIntent {
    while(wl_display_prepare_read(display.get()) != 0) {
        wl_display_dispatch_pending(display.get());
    }
    return display.get();
}

auto Display::roundtrip() -> bool {
    return wl_display_roundtrip(display.get()) != 0;
}

auto Display::dispatch() -> bool {
    return wl_display_dispatch(display.get()) != -1;
}

auto Display::dispatch_pending() -> bool {
    return wl_display_dispatch_pending(display.get()) != -1;
}

auto Display::flush() -> void {
    wl_display_flush(display.get());
}

auto Display::get_registry() -> wl_registry* {
    return wl_display_get_registry(display.get());
}

Display::Display() {
    display.reset(wl_display_connect(nullptr));
    line_assert(display != NULL);
}
}; // namespace towl
