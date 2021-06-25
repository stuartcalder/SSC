project('Base', 'c', default_options: ['c_std=c17', 'optimization=3', 'buildtype=release', 'strip=true'])
# Project / Language
_PROJECT  = 'Base'
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
static = get_option('static')
if os != 'netbsd' and compiler.get_id() != 'clang'
  _GCC_COMPATIBLE_FLAGS += '-flto'
endif
assert(os in _SUPPORTED_OPERATING_SYSTEMS,
  'You are trying to build ' + _PROJECT + ' using an unsupported operating system!')
assert(compiler.get_id() in _SUPPORTED_COMPILERS,
  'You are trying to build ' + _PROJECT + ' using an unsupported compiler!')
# Compiler Constants
if os == 'windows' and compiler.get_id() == 'msvc'
	_D_PREFIX = '/D'
else
	_D_PREFIX = '-D'
endif
# Source Code Files
src =  [
'impl/args.c',
'impl/errors.c',
'impl/files.c',
'impl/mem.c',
'impl/mlock.c',
'impl/mmap.c',
'impl/operations.c',
'impl/print.c',
'impl/random.c',
'impl/strings.c',
'impl/swap.c',
'impl/term.c'
]
lib_deps     = []
lang_flags   = []
include_dirs = []
# Debug Build?
if not get_option('debug_build')
  lang_flags += _D_PREFIX + 'NDEBUG'
else
  lang_flags += _D_PREFIX + 'BASE_EXTERN_DEBUG'
  lang_flags += _D_PREFIX + 'YDEBUG'
endif
# We're building Base
if static
  lang_flags += _D_PREFIX + 'BASE_EXTERN_STATIC_LIB'
else
  lang_flags += _D_PREFIX + 'BASE_EXTERN_BUILD_DYNAMIC_LIB'
endif
# MacOS Requires __STDC_WANT_LIB_EXT1__ == 1
if os == 'darwin'
  lang_flags += _D_PREFIX + '__STDC_WANT_LIB_EXT1__=1'
endif
# Unixlike options
if os in _UNIXLIKE_OPERATING_SYSTEMS
  if compiler.get_id() in _GCC_COMPATIBLE_COMPILERS # GCC, Clang
    if get_option('native_optimize') # -march=native
      lang_flags += '-march=native'
    endif
    lang_flags += _GCC_COMPATIBLE_FLAGS
  endif
  if get_option('lua') # Lua support
    lang_flags += _D_PREFIX + 'BASE_EXTERN_LUA'
	src += [
	  'impl/lua.c',		  # luaopen_Base
	  'impl/lua/mmap.c',  # luaopen_Base_MMap
	  'impl/lua/procs.c', # luaopen_Base_Procs
	  'impl/lua/sbuffer.c'# luaopen_Base_SBuffer
	]
    src += 'impl/lua.c'
    lib_deps += compiler.find_library('lua5.4')
  endif
endif
# MLock options
if get_option('mlock') and os != 'openbsd'
  lang_flags += _D_PREFIX + 'BASE_EXTERN_MLOCK'
  if get_option('mlock_threadsafe')
	lang_flags += _D_PREFIX + 'BASE_EXTERN_MLOCK_THREADSAFE'
  endif
endif

# On {Open,Free}BSD and MacOS...
if os in ['openbsd', 'freebsd', 'darwin']
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
# Install the base folder into the system header directory
install_subdir('../Base', install_dir: include_install, exclude_directories: ['builddir', '.git', 'impl'], exclude_files: ['.gitignore', 'meson.build', 'README.md'])

if static
  if os == 'windows'
    static_library('Base', sources: src, dependencies: lib_deps, c_args: lang_flags, include_directories: include_dirs,
      name_suffix: 'lib', name_prefix: '', install: true, install_dir: 'C:/lib')
  else
    static_library('Base', sources: src, dependencies: lib_deps, c_args: lang_flags, include_directories: include_dirs, install: true)
  endif
else
  if os == 'windows'
    shared_library('Base', sources: src, dependencies: lib_deps, c_args: lang_flags, include_directories: include_dirs,
      name_suffix: 'dll', name_prefix: '', install: true, install_dir: 'C:/lib')
  else
    shared_library('Base', sources: src, dependencies: lib_deps, c_args: lang_flags, include_directories: include_dirs, install: true)
  endif
endif
