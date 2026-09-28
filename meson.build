project(
  'poor_base',
  version: '0.1.0',
  license: 'MIT',
  meson_version: '>= 0.54.0',
)

install_subdir(
  'include',
  install_dir: get_option('includedir'),
  strip_directory: true,
)

poor_base_dep = declare_dependency(
  include_directories: include_directories('include'),
)
meson.override_dependency('poor_base', poor_base_dep)
if meson.is_cross_build()
  meson.override_dependency('poor_base', poor_base_dep, native: true)
endif

import('pkgconfig').generate(
  libraries: poor_base_dep,
  name: 'poor_base',
  description: 'Header-only library for C with macros for type-generic printing and advanced array operations',
  install_dir: get_option('datadir') / 'pkgconfig',
)
