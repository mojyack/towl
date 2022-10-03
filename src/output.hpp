#pragma once
#include <memory>
#include <optional>
#include <string>

#include "common.hpp"
#include "internal.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

template <class Glue>
concept OutputOnScale = requires(Glue& m, OutputTag output, int32_t scale) {
    m.on_scale(output, scale);
};

template <class Glue>
concept OutputGlue = (OutputOnScale<Glue> || IsEmpty<Glue>)&&std::movable<Glue>;

// version = 1 ~ 4
template <uint32_t version, OutputGlue OutputGlue>
class Output {
  private:
    struct Deleter {
        auto operator()(wl_output* const output) -> void {
            if(version >= WL_OUTPUT_RELEASE_SINCE_VERSION) {
                wl_output_release(output);
            } else {
                wl_output_destroy(output);
            }
        }
    };

    std::unique_ptr<wl_output, Deleter> output;
    uint32_t                            id;

    static auto geometry(void* const /*data*/, wl_output* const /*wl_output*/, const int32_t /*x*/, const int32_t /*y*/, const int32_t /*physical_width*/, const int32_t /*physical_height*/, const int32_t /*subpixel*/, const char* const /*make*/, const char* const /*model*/, const int32_t /*transform*/) {}

    static auto mode(void* const /*data*/, wl_output* const /*wl_output*/, const uint32_t /*flags*/, const int32_t /*width*/, const int32_t /*height*/, const int32_t /*refresh*/) {}

    static auto done(void* const /*data*/, wl_output* const /*wl_output*/) {}

    static auto scale(void* const data, wl_output* const /*wl_output*/, const int32_t factor) {
        auto& self          = *reinterpret_cast<Output*>(data);
        self.scaling_factor = factor;
        if constexpr(OutputOnScale<OutputGlue>) {
            self.glue.on_scale(reinterpret_cast<OutputTag>(self.output.get()), factor);
        }
    }

    static auto name(void* const /*data*/, wl_output* const /*wl_output*/, const char* const /*name*/) {}

    static auto description(void* const /*data*/, wl_output* /*wl_output*/, const char* const /*description*/) {}

    static inline wl_output_listener listener = {geometry, mode, done, scale, name, description};

    int32_t scaling_factor = 1;

    [[no_unique_address]] std::conditional_t<!IsEmpty<OutputGlue>, OutputGlue, Empty> glue;

  public:
    static auto info() -> internal::InterfaceInfo {
        return {"wl_output", version, &wl_output_interface};
    }

    auto interface_id() const -> uint32_t {
        return id;
    }

    auto as_tag() const -> OutputTag {
        return reinterpret_cast<size_t>(output.get());
    }

    static auto from_tag(const OutputTag output) -> Output& {
        return *reinterpret_cast<Output*>(wl_output_get_user_data(reinterpret_cast<wl_output*>(output)));
    }

    auto get_scale() const -> int32_t {
        return scaling_factor;
    }

    auto operator==(const OutputTag tag) const -> bool {
        return output.get() == reinterpret_cast<wl_output*>(tag);
    }

    Output(void* const data, const uint32_t id, OutputGlue&& glue) : output(reinterpret_cast<wl_output*>(data)), id(id), glue(std::move(glue)) {
        // static_assert(!(OutputOnGeometry<OutputGlue> && version < WL_OUTPUT_GEOMETRY_SINCE_VERSION));
        // static_assert(!(OutputOnMode<OutputGlue> && version < WL_OUTPUT_MODE_SINCE_VERSION));
        // static_assert(!(OutputOnDone<OutputGlue> && version < WL_OUTPUT_DONE_SINCE_VERSION));
        static_assert(!(OutputOnScale<OutputGlue> && version < WL_OUTPUT_SCALE_SINCE_VERSION));
        // static_assert(!(OutputOnName<OutputGlue> && version < WL_OUTPUT_NAME_SINCE_VERSION));
        // static_assert(!(OutputOnDescription<OutputGlue> && version < WL_OUTPUT_DESCRIPTION_SINCE_VERSION));

        wl_output_add_listener(output.get(), &listener, this);
        wl_output_set_user_data(output.get(), this);
    }
};

#ifdef TOWL_NS
}
#endif
