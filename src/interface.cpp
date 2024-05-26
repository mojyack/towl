#include "interface.hpp"

namespace towl::impl {
auto InterfaceBinder::bind(void* data, uint32_t interface_id) -> void {
    auto& interface = interfaces.emplace_back(create_interface(data));

    interface->binder       = this;
    interface->interface_id = interface_id;
}

auto InterfaceBinder::unbind(const uint32_t interface_id) -> bool {
    for(auto i = interfaces.begin(); i != interfaces.end(); i += 1) {
        const auto interface = i->get();
        if(interface->interface_id == interface_id) {
            interfaces.erase(i);
            return true;
        }
    }
    return false;
}
} // namespace towl::impl
