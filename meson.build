project('shim',
	'c',
	default_options: ['c_std=c18', 'optimization=3', 'buildtype=release', 'strip=true'])
# Project / Language
_PROJECT  = 'shim'
_LANG = 'c'
# Operating Systems & Compilers
_UNIXLIKE_OPERATING_SYSTEMS  = ['openbsd', 'freebsd', 'netbsd', 'darwin', 'linux']
_SUPPORTED_OPERATING_SYSTEMS = _UNIXLIKE_OPERATING_SYSTEMS + ['windows']
_GCC_COMPATIBLE_COMPILERS    = ['gcc', 'clang']
_SUPPORTED_COMPILERS         = _GCC_COMPATIBLE_COMPILERS + ['msvc']
_GCC_COMPATIBLE_FLAGS = [
  '-fvisibility=hidden',
  '-fstrict-aliasing',
  '-Wall',
  '-funroll-loops'
]
compiler = meson.get_compiler(_LANG)
os = host_machine.system()
if os != 'netbsd' and compiler.get_id() != 'clang'
  _GCC_COMPATIBLE_FLAGS += '-flto'
endif
assert (os in _SUPPORTED_OPERATING_SYSTEMS, 'You are trying to build ' + _PROJECT + ' using an unsupported operating system!')
assert (compiler.get_id() in _SUPPORTED_COMPILERS, 'You are trying to build ' + _PROJECT + ' using an unsupported compiler!')
# Compiler Constants
if os == 'windows' and compiler.get_id() == 'msvc'
	_D_PREFIX = '/D'
else
	_D_PREFIX = '-D'
endif
# Source Code Files
src = [
  'impl/operations.c',
  'impl/files.c',
  'impl/map.c',
  'impl/args.c',
  'impl/term.c',
  'impl/print.c',
  'impl/strings.c',
  'impl/mlock.c'
  ]
lib_deps     = []
lang_flags   = []
include_dirs = []
# We're building Shim
_flag = _D_PREFIX + 'SHIM_EXT_BUILD'
lang_flags += _flag
# OSX Requires __STDC_WANT_LIB_EXT1__ == 1
if os == 'darwin'
  _flag = _D_PREFIX + '__STDC_WANT_LIB_EXT1__=1'
  lang_flags += _flag
endif
# Unixlike options
if os in _UNIXLIKE_OPERATING_SYSTEMS
  if get_option('native_optimize')
    lang_flags += '-march=native'
  endif
  if compiler.get_id() in _GCC_COMPATIBLE_COMPILERS
    lang_flags += _GCC_COMPATIBLE_FLAGS
  endif
endif

# On {Open,Free}BSD and OSX...
if os == 'openbsd' or os == 'freebsd' or os == 'darwin'
  include_install = '/usr/local/include'
  lib_deps += compiler.find_library('ncurses')
# On NetBSD...
elif os == 'netbsd'
  include_install = '/usr/local/include'
  include_dirs += '/usr/pkg/include'
  lib_deps += compiler.find_library('ncurses')
# On GNU/Linux...
elif os == 'linux'
  include_install = '/usr/include'
  lib_deps += compiler.find_library('ncurses')
  lib_deps += compiler.find_library('tinfo')
# On Windows...
elif os == 'windows'
  lib_deps += compiler.find_library('bcrypt')
  include_install = 'C:/include'
endif

include_dirs += '..'
# Install the shim folder into the system header directory
install_subdir('../shim', install_dir: include_install, exclude_directories: ['builddir', '.git', 'impl'], exclude_files: ['.gitignore', 'meson.build', 'README.md'])

if os == 'windows'
  shared_library('shim', sources: src, dependencies: lib_deps,
		 c_args: lang_flags, include_directories: include_dirs,
		 name_suffix: 'dll', name_prefix: '',
		 install: true, install_dir: 'C:/lib')
else
  shared_library('shim', sources: src, dependencies: lib_deps,
		 c_args: lang_flags, include_directories: include_dirs,
		 install: true)
endif
