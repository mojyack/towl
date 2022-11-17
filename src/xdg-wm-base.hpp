#pragma once
#include <cassert>
#include <memory>

#include "common.hpp"
#include "internal.hpp"
#include "protocols/xdg-shell.h"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

template <class Glue>
concept WMBaseXDGToplevelOnConfigure = requires(Glue& m, int32_t width, int32_t height) {
                                           { m.on_configure(width, height) };
                                       };

template <class Glue>
concept WMBaseXDGToplevelOnClose = requires(Glue& m) {
                                       { m.on_close() };
                                   };

template <class Glue>
concept WMBaseXDGToplevelGlue =
    (WMBaseXDGToplevelOnConfigure<Glue> ||
     WMBaseXDGToplevelOnClose<Glue> ||
     IsEmpty<Glue>) &&
    std::movable<Glue>;

// version = 1 ~ 5
template <uint32_t version>
class WMBase {
  public:
    template <WMBaseXDGToplevelGlue XDGToplevelGlue>
    class XDGToplevel {
      private:
        struct Deleter {
            auto operator()(xdg_toplevel* const toplevel) -> void {
                static_assert(version >= XDG_TOPLEVEL_DESTROY_SINCE_VERSION);
                xdg_toplevel_destroy(toplevel);
            }
        };

        std::unique_ptr<xdg_toplevel, Deleter> toplevel;

        static auto configure(void* const data, xdg_toplevel* const /*toplevel*/, const int32_t width, const int32_t height, wl_array* const /*states*/) -> void {
            if constexpr(WMBaseXDGToplevelOnConfigure<XDGToplevelGlue>) {
                if((width != 0 && height != 0)) {
                    auto& self = *reinterpret_cast<XDGToplevel*>(data);
                    self.glue.on_configure(width, height);
                }
            }
        }

        static auto close(void* const data, xdg_toplevel* const /*toplevel*/) -> void {
            if constexpr(WMBaseXDGToplevelOnClose<XDGToplevelGlue>) {
                auto& self = *reinterpret_cast<XDGToplevel*>(data);
                self.glue.on_close();
            }
        }

        static auto bounds(void* const /*data*/, xdg_toplevel* const /*toplevel*/, const int32_t /*width*/, const int32_t /*height*/) -> void {}

        static auto capabilities(void* const /*data*/, xdg_toplevel* const /*toplevel*/, wl_array* const /*capabilities*/) -> void{};

        static inline xdg_toplevel_listener listener = {configure, close, bounds, capabilities};

        [[no_unique_address]] XDGToplevelGlue glue;

      public:
        auto set_title(const char* const title) -> void {
            static_assert(version >= XDG_TOPLEVEL_SET_TITLE_SINCE_VERSION);
            xdg_toplevel_set_title(toplevel.get(), title);
        }

        XDGToplevel(xdg_toplevel* const toplevel, XDGToplevelGlue glue) : toplevel(toplevel), glue(std::move(glue)) {
            static_assert(!(WMBaseXDGToplevelOnConfigure<XDGToplevelGlue> && version < XDG_TOPLEVEL_CONFIGURE_SINCE_VERSION));
            static_assert(!(WMBaseXDGToplevelOnClose<XDGToplevelGlue> && version < XDG_TOPLEVEL_CLOSE_SINCE_VERSION));

            assert(toplevel);
            if constexpr(WMBaseXDGToplevelGlue<XDGToplevelGlue>) {
                xdg_toplevel_add_listener(toplevel, &listener, this);
            }
        }
    };

    class XDGSurface {
      private:
        struct Deleter {
            auto operator()(xdg_surface* const surface) -> void {
                xdg_surface_destroy(surface);
            }
        };

        std::unique_ptr<xdg_surface, Deleter> surface;

        static auto configure(void* const data, xdg_surface* const surface, const uint32_t serial) -> void {
            auto& self = *reinterpret_cast<XDGSurface*>(data);

            self.configured = true;

            static_assert(version >= XDG_SURFACE_ACK_CONFIGURE_SINCE_VERSION);
            xdg_surface_ack_configure(surface, serial);
        }

        static inline xdg_surface_listener listener = {configure};

        bool configured = false;

      public:
        template <WMBaseXDGToplevelGlue Glue>
        auto create_xdg_toplevel(Glue&& glue) -> XDGToplevel<Glue> {
            static_assert(version >= XDG_SURFACE_GET_TOPLEVEL_SINCE_VERSION);
            return XDGToplevel<Glue>(xdg_surface_get_toplevel(surface.get()), std::move(glue));
        }

        auto is_configured() const -> bool {
            return configured;
        }

        XDGSurface(xdg_surface* const surface) : surface(surface) {
            static_assert(version >= XDG_TOPLEVEL_CONFIGURE_SINCE_VERSION);

            assert(surface);
            xdg_surface_add_listener(surface, &listener, this);
        }
    };

  private:
    struct Deleter {
        auto operator()(xdg_wm_base* const wm_base) -> void {
            static_assert(version >= XDG_WM_BASE_DESTROY_SINCE_VERSION);
            xdg_wm_base_destroy(wm_base);
        }
    };

    std::unique_ptr<xdg_wm_base, Deleter> wm_base;

    static auto ping(void* const /*data*/, xdg_wm_base* const wm_base, const uint32_t serial) -> void {
        static_assert(version >= XDG_WM_BASE_PONG_SINCE_VERSION);
        xdg_wm_base_pong(wm_base, serial);
    }

    static inline xdg_wm_base_listener listener = {ping};

    uint32_t id;

  public:
    static auto info() -> internal::InterfaceInfo {
        return {"xdg_wm_base", version, &xdg_wm_base_interface};
    }

    auto interface_id() const -> uint32_t {
        return id;
    }

    auto create_xdg_surface(internal::SurfaceLike auto& surface) -> XDGSurface {
        static_assert(version >= XDG_WM_BASE_GET_XDG_SURFACE_SINCE_VERSION);
        return {xdg_wm_base_get_xdg_surface(wm_base.get(), surface.native())};
    }

    WMBase(void* const data, const uint32_t id) : wm_base(reinterpret_cast<xdg_wm_base*>(data)), id(id) {
        static_assert(version >= XDG_WM_BASE_PING_SINCE_VERSION);
        xdg_wm_base_add_listener(wm_base.get(), &listener, this);
    }
};

#ifdef TOWL_NS
}
#endif
