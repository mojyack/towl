#include <vector>

#include <wayland-client.h>

#include "macros/assert.hpp"
#include "registry.hpp"

namespace towl {
auto Registry::global_callback(void* const data, wl_registry* const /*registry*/, const uint32_t id, const char* const interface, const uint32_t version) -> void {
    auto& self = *std::bit_cast<Registry*>(data);
    self.bind_interface(interface, version, id);
}

auto Registry::remove_callback(void* const data, wl_registry* const /*registry*/, const uint32_t id) -> void {
    auto& self = *std::bit_cast<Registry*>(data);
    self.unbind_interface(id);
}

auto Registry::bind_interface(const char* const name, const uint32_t version, const uint32_t id) -> void {
    for(const auto binder : binders) {
        const auto& desc             = *binder->get_interface_description();
        const auto  required_version = binder->version;
        if(std::strcmp(desc.name, name) != 0) {
            continue;
        }
        ensure(required_version <= version, "application requires version ", required_version, " of ", desc.name, ", but server provides version ", version, ".");
        const auto data = wl_registry_bind(registry.get(), id, &desc, required_version);
        binder->bind(data, id);
        break;
    }
}

auto Registry::unbind_interface(const uint32_t id) -> void {
    for(const auto binder : binders) {
        if(binder->unbind(id)) {
            break;
        }
    }
}

auto Registry::set_binders(std::vector<impl::InterfaceBinder*> binders) -> void {
    this->binders = std::move(binders);
}

Registry::Registry(wl_registry* const registry)
    : registry(registry) {
    wl_registry_add_listener(registry, &listener, this);
}
} // namespace towl
