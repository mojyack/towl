#pragma once
#include <wayland-client.h>

#include "interface.hpp"

namespace towl::impl {
struct AutoNativeOutputDeleter {
    uint32_t version;

    auto operator()(wl_output* output) -> void;
};

using AutoNativeOutput = std::unique_ptr<wl_output, AutoNativeOutputDeleter>;
} // namespace towl::impl

namespace towl {
class OutputCallbacks {
  public:
    virtual auto on_wl_output_scale(wl_output* /*output*/, uint32_t /*scale*/) -> void {}
    virtual ~OutputCallbacks(){};
};

class Output : public impl::Interface {
  private:
    impl::AutoNativeOutput output;
    OutputCallbacks*       callbacks;

    static auto geometry(void* /*data*/,
                         wl_output* /*wl_output*/,
                         int32_t /*x*/,
                         int32_t /*y*/,
                         int32_t /*physical_width*/,
                         int32_t /*physical_height*/,
                         int32_t /*subpixel*/,
                         const char* /*make*/,
                         const char* /*model*/,
                         int32_t /*transform*/) -> void {}

    static auto mode(void* /*data*/,
                     wl_output* /*wl_output*/,
                     uint32_t /*flags*/,
                     int32_t /*width*/,
                     int32_t /*height*/,
                     int32_t /*refresh*/) -> void {}

    static auto done(void* /*data*/,
                     wl_output* /*wl_output*/) -> void {}

    static auto scale(void* data,
                      wl_output* /*wl_output*/,
                      int32_t factor) -> void;

    static auto name(void* /*data*/,
                     wl_output* /*wl_output*/,
                     const char* /*name*/) -> void {}

    static auto description(void* /*data*/,
                            wl_output* /*wl_output*/,
                            const char* /*description*/) -> void {}

    static inline wl_output_listener listener = {geometry, mode, done, scale, name, description};

  public:
    Output(void* data, uint32_t version, OutputCallbacks* callbacks);
};

// version = 1 ~ 4
struct OutputBinder : impl::InterfaceBinder {
    OutputCallbacks* callbacks;

    auto get_interface_description() -> const wl_interface* override;
    auto create_interface(void* data) -> std::unique_ptr<impl::Interface> override;

    OutputBinder(const uint32_t version, OutputCallbacks* callbacks)
        : InterfaceBinder(version),
          callbacks(callbacks) {}
};
} // namespace towl
