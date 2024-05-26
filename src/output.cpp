#include "output.hpp"

namespace towl::impl {
auto AutoNativeOutputDeleter::operator()(wl_output* const output) -> void {
    if(version >= WL_OUTPUT_RELEASE_SINCE_VERSION) {
        wl_output_release(output);
    } else {
        wl_output_destroy(output);
    }
}
} // namespace towl::impl

namespace towl {
auto Output::scale(void* const data, wl_output* const /*wl_output*/, const int32_t factor) -> void {
    auto& self = *std::bit_cast<Output*>(data);
    self.callbacks->on_wl_output_scale(self.output.get(), factor);
}

Output::Output(void* const data, const uint32_t version, OutputCallbacks* const callbacks)
    : callbacks(callbacks),
      output(std::bit_cast<wl_output*>(data), {version}) {
    wl_output_add_listener(output.get(), &listener, this);
}

auto OutputBinder::get_interface_description() -> const wl_interface* {
    return &wl_output_interface;
}

auto OutputBinder::create_interface(void* const data) -> std::unique_ptr<impl::Interface> {
    return std::unique_ptr<impl::Interface>(new Output(data, version, callbacks));
}
} // namespace towl
