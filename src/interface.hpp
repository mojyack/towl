#pragma once
#include <memory>
#include <vector>

#include <wayland-client.h>

namespace towl::impl {
struct InterfaceBinder;

class Interface {
  public:
    InterfaceBinder* binder;
    uint32_t         interface_id;

    virtual ~Interface() {}
};

struct InterfaceBinder {
    uint32_t                                version;
    std::vector<std::unique_ptr<Interface>> interfaces;

    virtual auto get_interface_description() -> const wl_interface*         = 0;
    virtual auto create_interface(void* data) -> std::unique_ptr<Interface> = 0;

    auto bind(void* data, uint32_t interface_id) -> void;
    auto unbind(uint32_t interface_id) -> bool;

    InterfaceBinder(InterfaceBinder&&) = default;
    InterfaceBinder(uint32_t version) : version(version) {}

    virtual ~InterfaceBinder() {}
};
} // namespace towl::impl
