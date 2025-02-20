project('towl', ['cpp', 'c'], version : '2.0.0', default_options : ['warning_level=3', 'werror=false', 'cpp_std=c++23'])
add_project_arguments('-Wfatal-errors', language: 'cpp')
add_project_arguments('-Wno-error', language: 'cpp')

subdir('src')
executable('shm-window', towl_files + 'examples/shm-window.cpp', dependencies: towl_deps)
executable('egl-window', towl_files + 'examples/egl-window.cpp', dependencies: towl_deps + towl_egl_deps)
