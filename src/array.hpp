#pragma once
#include <bit>

#include <wayland-client.h>

namespace towl {
template <class T>
struct Array {
    size_t size;
    T*     data;

    Array(const wl_array& array)
        : size(array.size / sizeof(T)),
          data(std::bit_cast<T*>(array.data)) {}
};

} // namespace towl
