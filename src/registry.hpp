#pragma once
#include <array>
#include <cassert>
#include <concepts>
#include <cstring>
#include <sstream>
#include <vector>

#include "internal.hpp"

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

template <class T>
concept GlueParameter = requires(T& m) {
    typename T::Interface;
};

template <GlueParameter... Parameters>
struct GlueParameterPack {
    std::tuple<Parameters...> value;
};

template <class GlueParameterPack, internal::Interface... Interfaces>
class Registry {
  public:
    template <class Interface, class Value>
    struct GlueInitializer : Value {
        using Type = Interface;
    };

  private:
    struct Deleter {
        auto operator()(wl_registry* const registry) -> void {
            wl_registry_destroy(registry);
        }
    };

    std::unique_ptr<wl_registry, Deleter> registry;
    GlueParameterPack                     parameter_pack;

    using InterfaceArrays = std::tuple<std::vector<Interfaces>...>;
    InterfaceArrays interface_arrays;

    static auto global_callback(void* const data, wl_registry* const /*registry*/, const uint32_t id, const char* const interface, const uint32_t version) -> void {
        auto& self = *reinterpret_cast<Registry*>(data);
        self.bind_interface(interface, version, id);
    }
    static auto remove_callback(void* const data, wl_registry* const /*registry*/, const uint32_t id) -> void {
        auto& self = *reinterpret_cast<Registry*>(data);
        self.unbind_interface(id);
    }

    static inline wl_registry_listener listener = {global_callback, remove_callback};

    template <size_t n>
    auto emplace_with_initializer(auto& interface_array, const internal::InterfaceInfo& info, const uint32_t id) -> void {
        if constexpr(n < std::tuple_size_v<decltype(parameter_pack.value)>) {
            using Interface      = typename std::remove_reference_t<decltype(interface_array)>::value_type;
            auto& initializer    = std::get<n>(parameter_pack.value);
            using InitializerFor = typename std::remove_reference_t<decltype(initializer)>::Interface;
            if constexpr(std::is_same_v<InitializerFor, Interface>) {
                interface_array.emplace_back(wl_registry_bind(registry.get(), id, info.structure, info.version), id, initializer);
            } else {
                emplace_with_initializer<n + 1>(interface_array, info, id);
            }
        } else {
            interface_array.emplace_back(wl_registry_bind(registry.get(), id, info.structure, info.version), id);
        }
    }

    template <size_t n = 0>
    auto bind_interface(const char* const name, const uint32_t version, const uint32_t id) -> void {
        if constexpr(n < std::tuple_size_v<InterfaceArrays>) {
            auto& interface_array = std::get<n>(interface_arrays);
            using Interface       = typename std::remove_reference_t<decltype(interface_array)>::value_type;
            const auto info       = Interface::info();
            if(std::strcmp(info.name, name) != 0) {
                bind_interface<n + 1>(name, version, id);
                return;
            }
            if(info.version > version) {
                panic("application requires version ", info.version, " of ", info.name, ", but server provides version ", version, ".");
            }
            emplace_with_initializer<0>(interface_array, info, id);
        }
    }

    template <size_t n = 0>
    auto unbind_interface(const uint32_t id) -> void {
        if constexpr(n < std::tuple_size_v<InterfaceArrays>) {
            auto& interface_array = std::get<n>(interface_arrays);
            for(auto i = interface_array.begin(); i < interface_array.end(); i += 1) {
                if(id == i->interface_id()) {
                    interface_array.erase(i);
                    return;
                }
            }
            unbind_interface<n + 1>(id);
        }
    }

    template <size_t n, class Interface>
    auto find_interface_array() -> std::vector<Interface>& {
        if constexpr(n < std::tuple_size_v<InterfaceArrays>) {
            auto& interface_array = std::get<n>(interface_arrays);
            using Current         = typename std::remove_reference_t<decltype(interface_array)>::value_type;
            if constexpr(std::is_same_v<Current, Interface>) {
                return interface_array;
            } else {
                return find_interface_array<n + 1, Interface>();
            }
        } else {
            static_assert(n - n, "invalid type");
        }
    }

    template <size_t n>
    auto ensure_interfaces() -> bool {
        if constexpr(n < std::tuple_size_v<InterfaceArrays>) {
            const auto& interface_array = std::get<n>(interface_arrays);
            if(interface_array.empty()) {
                return false;
            }
            return ensure_interfaces<n + 1>();
        } else {
            return true;
        }
    }

    template <class... Args>
    [[noreturn]] auto panic(Args... args) -> void {
        auto ss = std::stringstream();
        (ss << ... << args) << std::endl;
        throw std::runtime_error(ss.str());
    }

  public:
    template <class T>
    auto interface() -> std::vector<T>& {
        return find_interface_array<0, T>();
    }
    auto ensure_interfaces() -> bool {
        return ensure_interfaces<0>();
    }
    Registry(wl_registry* const registry, GlueParameterPack&& parameter_pack) : registry(registry), parameter_pack(parameter_pack) {
        wl_registry_add_listener(registry, &listener, this);
    }
};

#ifdef TOWL_NS
}
#endif
