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
auto Output::geometry(void* const   data, wl_output* const /*wl_output*/,
                      const int32_t x, const int32_t y,
                      const int32_t physical_width, const int32_t physical_height,
                      const int32_t     subpixel,
                      const char* const make, const char* const model,
                      const int32_t transform) -> void {
    auto& self = *std::bit_cast<Output*>(data);
    self.callbacks->on_wl_output_geometry(self.output.get(), x, y, physical_width, physical_height, subpixel, make, model, transform);
}

auto Output::mode(void* const    data, wl_output* const /*wl_output*/,
                  const uint32_t flags,
                  const int32_t width, const int32_t height,
                  const int32_t refresh) -> void {
    auto& self = *std::bit_cast<Output*>(data);
    self.callbacks->on_wl_output_mode(self.output.get(), flags, width, height, refresh);
}

auto Output::done(void* const data, wl_output* const /*wl_output*/) -> void {
    auto& self = *std::bit_cast<Output*>(data);
    self.callbacks->on_wl_output_done(self.output.get());
}

auto Output::scale(void* const data, wl_output* const /*wl_output*/, const int32_t factor) -> void {
    auto& self = *std::bit_cast<Output*>(data);
    self.callbacks->on_wl_output_scale(self.output.get(), factor);
}

auto Output::name(void* const data, wl_output* const /*wl_output*/, const char* const name) -> void {
    auto& self = *std::bit_cast<Output*>(data);
    self.callbacks->on_wl_output_name(self.output.get(), name);
}

auto Output::description(void* const data, wl_output* const /*wl_output*/, const char* const description) -> void {
    auto& self = *std::bit_cast<Output*>(data);
    self.callbacks->on_wl_output_description(self.output.get(), description);
}

Output::Output(void* const data, const uint32_t version, OutputCallbacks* const callbacks)
    : output(std::bit_cast<wl_output*>(data), {version}),
      callbacks(callbacks) {
    wl_output_add_listener(output.get(), &listener, this);
    callbacks->on_wl_output_created(output.get());
}

Output::~Output() {
    callbacks->on_wl_output_removed(output.get());
}

auto OutputBinder::get_interface_description() -> const wl_interface* {
    return &wl_output_interface;
}

auto OutputBinder::create_interface(void* const data) -> std::unique_ptr<impl::Interface> {
    return std::unique_ptr<impl::Interface>(new Output(data, version, callbacks));
}
} // namespace towl
