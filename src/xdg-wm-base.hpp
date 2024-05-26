#pragma once
#include <xdg-shell.h>

#include "interface.hpp"
#include "macros/autoptr.hpp"

namespace towl::impl {
declare_autoptr(NativeXDGToplevel, xdg_toplevel, xdg_toplevel_destroy);
declare_autoptr(NativeXDGSurface, xdg_surface, xdg_surface_destroy);
declare_autoptr(NativeXDGWMBase, xdg_wm_base, xdg_wm_base_destroy);
} // namespace towl::impl

namespace towl {
class XDGToplevelCallbacks {
  public:
    virtual auto on_xdg_toplevel_configure(int /*width*/, int /*height*/) -> void {}
    virtual auto on_xdg_toplevel_close() -> void {}
};

class XDGToplevel {
  private:
    impl::AutoNativeXDGToplevel toplevel;
    XDGToplevelCallbacks*       callbacks;

    static auto configure(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states) -> void;
    static auto close(void* data, xdg_toplevel* toplevel) -> void;
    static auto bounds(void* const /*data*/, xdg_toplevel* const /*toplevel*/, const int32_t /*width*/, const int32_t /*height*/) -> void {}
    static auto capabilities(void* const /*data*/, xdg_toplevel* const /*toplevel*/, wl_array* const /*capabilities*/) -> void{};

    static inline xdg_toplevel_listener listener = {configure, close, bounds, capabilities};

  public:
    auto set_title(const char* title) -> void;

    XDGToplevel(xdg_toplevel* const toplevel, XDGToplevelCallbacks* const callbacks);
};

class XDGSurface {
  private:
    impl::AutoNativeXDGSurface surface;

    static auto configure(void* data, xdg_surface* surface, uint32_t serial) -> void;

    static inline xdg_surface_listener listener = {configure};

  public:
    auto create_xdg_toplevel(XDGToplevelCallbacks* callbacks) -> XDGToplevel;

    XDGSurface(xdg_surface* const surface);
};

class XDGWMBase : public impl::Interface {
  private:
    impl::AutoNativeXDGWMBase wm_base;

    static auto ping(void* data, xdg_wm_base* wm_base, uint32_t serial) -> void;

    static inline xdg_wm_base_listener listener = {ping};

  public:
    auto create_xdg_surface(wl_surface* const surface) -> XDGSurface;

    XDGWMBase(void* data);
};

// version = 1 ~ 5
struct XDGWMBaseBinder : impl::InterfaceBinder {
    uint32_t interface_version;

    auto get_interface_description() -> const wl_interface* override;
    auto create_interface(void* data) -> std::unique_ptr<impl::Interface> override;

    XDGWMBaseBinder(const uint32_t version)
        : InterfaceBinder(version) {}
};
} // namespace towl
