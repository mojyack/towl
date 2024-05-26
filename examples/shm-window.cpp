#include <fcntl.h>
#include <sys/mman.h>

#include "towl/towl.hpp"

#include "macros/assert.hpp"
#include "util/assert.hpp"
#include "util/fd.hpp"

struct Image {
    const size_t  size;
    const size_t  width;
    const size_t  height;
    towl::ShmPool pool;
    towl::Buffer  buffer;
    uint8_t*      data;

    auto fill(const uint32_t pattern) -> void {
        for(auto y = size_t(0); y < height; y += 1) {
            for(auto x = size_t(0); x < width; x += 1) {
                *(reinterpret_cast<uint32_t*>(data) + y * width + x) = pattern;
            }
        }
    }

    Image(towl::Shm* shm, const FileDescriptor& unix_shm, const size_t width, const size_t height)
        : size(width * height * 4),
          width(width),
          height(height),
          pool(shm->create_shm_pool(unix_shm.as_handle(), size)),
          buffer(pool.create_buffer(0, width, height, width * 4, WL_SHM_FORMAT_ARGB8888)),
          data(static_cast<uint8_t*>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, unix_shm.as_handle(), 0))) {
        DYN_ASSERT(data != MAP_FILE);
        DYN_ASSERT(ftruncate(unix_shm.as_handle(), size) >= 0);
    }

    ~Image() {
        munmap(data, size);
    }
};

auto main() -> int {
    // connect to display
    auto display = towl::Display();

    // bind interfaces
    auto registry           = display.get_registry();
    auto compositor_binder  = towl::CompositorBinder(4);
    auto xdg_wm_base_binder = towl::XDGWMBaseBinder(2);
    auto shm_binder         = towl::ShmBinder(1);
    registry.set_binders({&compositor_binder, &xdg_wm_base_binder, &shm_binder});
    display.roundtrip();
    DYN_ASSERT(!compositor_binder.interfaces.empty());
    DYN_ASSERT(!xdg_wm_base_binder.interfaces.empty());
    DYN_ASSERT(!shm_binder.interfaces.empty());

    // get surface from compositor
    auto surface_callbacks = towl::SurfaceCallbacks();
    auto compositor        = std::bit_cast<towl::Compositor*>(compositor_binder.interfaces[0].get());
    auto surface           = compositor->create_surface(&surface_callbacks);

    // create xdg_surface and xdg_toplevel
    auto wmbase                 = std::bit_cast<towl::XDGWMBase*>(xdg_wm_base_binder.interfaces[0].get());
    auto xdg_surface            = wmbase->create_xdg_surface(surface.native());
    auto xdg_toplevel_callbacks = towl::XDGToplevelCallbacks();
    auto xdg_toplevel           = xdg_surface.create_xdg_toplevel(&xdg_toplevel_callbacks);

    // then commit surface changes
    surface.commit();

    // open shm
    constexpr auto shm_name   = "/towl-example";
    const auto     shm_handle = FileDescriptor(shm_open(shm_name, O_CREAT | O_RDWR | O_CLOEXEC, 0600));
    dynamic_assert(shm_handle.as_handle() >= 0, "failed to open shared memory");
    shm_unlink(shm_name);

    // create image with shm
    auto shm   = std::bit_cast<towl::Shm*>(shm_binder.interfaces[0].get());
    auto image = Image(shm, shm_handle, 800, 600);

    // process configure events before drawing anything
    display.roundtrip();

    // draw image and commit
    image.fill(0xFFFFFFFF);
    surface.attach(image.buffer.native(), 0, 0);
    surface.damage(0, 0, image.width, image.height);
    surface.commit();

    // main loop
    while(display.dispatch()) {
    }
    return 0;
}
