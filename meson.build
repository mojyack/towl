project('towl', ['cpp', 'c'], version : '1.0.5', default_options : ['warning_level=3', 'werror=true', 'cpp_std=c++20'])

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
        install: true,
        install_dir: join_paths(get_option('includedir'), 'towl/protocols'),
    )
endforeach

subdir('src')
install_headers(towl_headers, subdir: 'towl')
towl = library(
    'towl',
    [protocol_files + protocol_headers],
    dependencies: [wayland_client, wayland_egl],
    install: true
)

pkgconfig = import('pkgconfig')
cpp = meson.get_compiler('cpp')
pkgconfig.generate(
    libraries: [towl],
    requires: [wayland_client, wayland_egl],
    name: 'towl',
    filebase: 'towl',
    description: 'libtowl'
)
