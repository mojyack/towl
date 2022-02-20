#pragma once
#include <wayland-client.h>

#ifdef TOWL_NS
namespace TOWL_NS {
#endif

using SurfaceTag = size_t;
using OutputTag  = size_t;

constexpr size_t nulltag = 0;

template <class T>
struct Array {
    size_t size;
    T*     data;

    Array(const wl_array& array) : size(array.size / sizeof(T)), data(reinterpret_cast<T*>(array.data)) {}
};

struct Empty {};

template <class T>
concept IsEmpty = std::is_same_v<T, Empty>;

#ifdef TOWL_NS
}
#endif
