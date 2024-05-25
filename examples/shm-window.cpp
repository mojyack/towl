#include <fcntl.h>
#include <sys/mman.h>

#define TOWL_NS towl
#include "towl/towl.hpp"

#include "util/fd.hpp"
#include "util/assert.hpp"

using Compositor = towl::Compositor<4>;
using WMBase     = towl::WMBase<2>;
using Shm        = towl::Shm<1>;

struct Image {
    const size_t size;
    const size_t width;
    const size_t height;
    Shm::ShmPool pool;
    Shm::Buffer  buffer;
    uint8_t*     data;

    auto fill(const uint32_t pattern) -> void {
        for(auto y = size_t(0); y < height; y += 1) {
            for(auto x = size_t(0); x < width; x += 1) {
                *(reinterpret_cast<uint32_t*>(data) + y * width + x) = pattern;
            }
        }
    }

    Image(Shm* shm, const FileDescriptor& unix_shm, const size_t width, const size_t height) : size(width * height * 4),
                                                                                               width(width),
                                                                                               height(height),
                                                                                               pool(shm->create_shm_pool(unix_shm.as_handle(), size)),
                                                                                               buffer(pool.create_buffer(0, width, height, width * 4, WL_SHM_FORMAT_ARGB8888)),
                                                                                               data(static_cast<uint8_t*>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, unix_shm.as_handle(), 0))) {
        dynamic_assert(data != MAP_FILE);
        dynamic_assert(ftruncate(unix_shm.as_handle(), size) >= 0);
    }

    ~Image() {
        munmap(data, size);
    }
};

auto main() -> int {
    // connect to display
    auto display = towl::Display();

    // bind interfaces
    auto registry = display.get_registry<towl::Empty, Compositor, WMBase, Shm>({});
    display.roundtrip();
    dynamic_assert(!registry.interface<Compositor>().empty() && !registry.interface<WMBase>().empty() && !registry.interface<Shm>().empty(), "wayland server doesn't provide necessary interfaces");

    // get surface from compositor
    auto compositor = registry.interface<Compositor>()[0].get();
    auto surface    = compositor->create_surface(towl::Empty{});

    // create xdg_surface and xdg_toplevel
    auto wmbase       = registry.interface<WMBase>()[0].get();
    auto xdg_surface  = wmbase->create_xdg_surface(surface);
    auto xdg_toplevel = xdg_surface.create_xdg_toplevel(towl::Empty{});

    // then commit surface changes
    surface.commit();

    // open shm
    constexpr auto shm_name   = "/towl-example";
    const auto     shm_handle = FileDescriptor(shm_open(shm_name, O_CREAT | O_RDWR | O_CLOEXEC, 0600));
    dynamic_assert(shm_handle.as_handle() >= 0, "failed to open shared memory");
    shm_unlink(shm_name);

    // create image with shm
    auto shm   = registry.interface<Shm>()[0].get();
    auto image = Image(shm, shm_handle, 800, 600);

    // process configure events before drawing anything
    display.roundtrip();

    // draw image and commit
    image.fill(0xFFFFFFFF);
    surface.attach(image.buffer, 0, 0);
    surface.damage(0, 0, image.width, image.height);
    surface.commit();

    // main loop
    while(display.dispatch()) {
    }
    return 0;
}
