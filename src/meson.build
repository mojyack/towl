towl_files = files(
  'display.cpp',
  'interface.cpp',
  'registry.cpp',
  'compositor.cpp',
  'output.cpp',
  'seat.cpp',
  'keyboard.cpp',
  'pointer.cpp',
  'touch.cpp',
  'shell.cpp',
  'shm.cpp',
  'xdg-wm-base.cpp',
  'egl.cpp',
)

wayland_client    = dependency('wayland-client', version: '>=1.21')
wayland_cursor    = dependency('wayland-cursor')
wayland_egl       = dependency('wayland-egl')
wayland_protocols = dependency('wayland-protocols', version: '>=1.26')

wayland_scanner_dep = dependency('wayland-scanner', native: true)
wayland_scanner     = find_program(wayland_scanner_dep.get_variable(pkgconfig: 'wayland_scanner'), native: true)

protocol_dir = wayland_protocols.get_variable('pkgdatadir')
protocols = [
    [protocol_dir, 'stable/xdg-shell/xdg-shell.xml'],
]

protocol_files = []
protocol_headers = []
foreach p : protocols
    xml = join_paths(p)
    protocol_files += custom_target(
        xml.underscorify() + '_c',
        input: xml,
        output: '@BASENAME@.c',
        command: [wayland_scanner, 'public-code', '@INPUT@', '@OUTPUT@'],
    )
    protocol_headers += custom_target(
        xml.underscorify() + '_h',
        input: xml,
        output: '@BASENAME@.h',
        command: [wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@'],
    )
endforeach

towl_files += protocol_files + protocol_headers
towl_deps = [wayland_client, wayland_egl]

egl    = dependency('egl')
opengl = dependency('opengl')
coop   = dependency('coop', version : ['>=1.0.0', '<1.1.0'])

towl_egl_deps = [egl, opengl, coop]
