#pragma once
#include <atomic>
#include <cassert>

#include "internal.hpp"
#include "registry.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

class Display {
  public:
    class ReadIntent {
      private:
        wl_display* display;
        bool        done = false;

      public:
        auto read() -> void {
            if(done) {
                return;
            }
            wl_display_read_events(display);
            done = true;
        }

        auto cancel() -> void {
            if(!done) {
                wl_display_cancel_read(display);
            }
        }

        auto operator=(ReadIntent&) -> ReadIntent& = delete;

        ReadIntent(ReadIntent&) = delete;

        ReadIntent(wl_display* const display) : display(display) {}

        ~ReadIntent() {
            cancel();
        }
    };

  private:
    struct Deleter {
        auto operator()(wl_display* const display) -> void {
            wl_display_disconnect(display);
        }
    };

    std::unique_ptr<wl_display, Deleter> display;

    static auto done(void* const data, wl_callback* const callback, const uint32_t /*time*/) -> void {
        auto& flag = *reinterpret_cast<std::atomic_flag*>(data);
        flag.test_and_set();
        flag.notify_one();

        wl_callback_destroy(callback);
    }

    wl_callback_listener listener = {done};

  public:
    auto native() -> wl_display* {
        return display.get();
    }

    auto wait_sync() -> void {
        auto flag = std::atomic_flag(false);
        auto sync = wl_display_sync(display.get());
        wl_callback_add_listener(sync, &listener, &flag);
        flush();
        flag.wait(false);
    }

    auto get_fd() -> int {
        return wl_display_get_fd(display.get());
    }

    auto obtain_read_intent() -> ReadIntent {
        while(wl_display_prepare_read(display.get()) != 0) {
            wl_display_dispatch_pending(display.get());
        }
        return display.get();
    }

    auto roundtrip() -> bool {
        return wl_display_roundtrip(display.get()) != 0;
    }

    auto dispatch() -> bool {
        return wl_display_dispatch(display.get()) != -1;
    }

    auto dispatch_pending() -> bool {
        return wl_display_dispatch_pending(display.get()) != -1;
    }

    auto flush() -> void {
        wl_display_flush(display.get());
    }

    template <class GlueParameterPack, internal::Interface... Interfaces>
    auto get_registry(GlueParameterPack parameter_pack) -> Registry<GlueParameterPack, Interfaces...> {
        return Registry<GlueParameterPack, Interfaces...>(wl_display_get_registry(display.get()), std::move(parameter_pack));
    }

    Display() {
        display.reset(wl_display_connect(nullptr));
        assert(display);
    }
};

#ifdef TOWL_NS
}
#endif
