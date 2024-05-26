#pragma once
#include <vector>

#include <wayland-client.h>

#include "interface.hpp"
#include "macros/autoptr.hpp"

namespace towl::impl {
declare_autoptr(NativeRegistry, wl_registry, wl_registry_destroy);
} // namespace towl::impl

namespace towl {
class Registry {
  private:
    impl::AutoNativeRegistry            registry;
    std::vector<impl::InterfaceBinder*> binders;

    static auto global_callback(void* data, wl_registry* registry, uint32_t id, const char* interface, uint32_t version) -> void;
    static auto remove_callback(void* data, wl_registry* registry, uint32_t id) -> void;

    static inline wl_registry_listener listener = {global_callback, remove_callback};

    auto bind_interface(const char* const name, const uint32_t version, const uint32_t id) -> void;
    auto unbind_interface(const uint32_t id) -> void;

  public:
    auto set_binders(std::vector<impl::InterfaceBinder*> binders) -> void;

    Registry(wl_registry* const registry);
};
} // namespace towl
