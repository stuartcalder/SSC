project('Base', 'c', default_options: ['c_std=c17', 'optimization=3', 'buildtype=release', 'strip=true'])
# Project / Language
PROJECT  = 'Base'
LANG = 'c'

# Operating Systems & Compilers
UNIXLIKE_OPERATING_SYSTEMS  = ['openbsd', 'freebsd', 'netbsd', 'darwin', 'linux']
SUPPORTED_OPERATING_SYSTEMS = UNIXLIKE_OPERATING_SYSTEMS + ['windows']
GCC_COMPATIBLE_COMPILERS		= ['gcc', 'clang']
SUPPORTED_COMPILERS			= GCC_COMPATIBLE_COMPILERS + ['msvc']
GCC_COMPATIBLE_FLAGS = [
  '-fvisibility=hidden',
  '-fstrict-aliasing',
  '-Wall',
  '-funroll-loops'
]

compiler = meson.get_compiler(LANG)
os = host_machine.system()
if os != 'netbsd' and compiler.get_id() != 'clang'
  GCC_COMPATIBLE_FLAGS += '-flto'
endif
assert(os in SUPPORTED_OPERATING_SYSTEMS,
  'You are trying to build ' + PROJECT + ' using an unsupported operating system!')
assert(compiler.get_id() in SUPPORTED_COMPILERS,
  'You are trying to build ' + PROJECT + ' using an unsupported compiler!')

lib_dirs = {
  'bsd':   ['/usr/lib', '/usr/local/lib'],
  'linux': ['/usr/lib', '/usr/lib64'],
  'windows': ['C:\lib']
}

if os in ['openbsd', 'freebsd', 'netbsd', 'darwin']
  lib_dir = lib_dirs.get('bsd')
elif os in ['linux', 'windows']
  lib_dir = lib_dirs.get(os)
else
  assert(false, 'Unsupported OS.')
endif


# Compiler Constants
if os == 'windows' and compiler.get_id() == 'msvc'
	_D = '/D'
else
	_D = '-D'
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
  lang_flags += _D + 'NDEBUG'
else
  lang_flags += _D + 'BASE_EXTERN_DEBUG'
  lang_flags += _D + 'YDEBUG'
endif
# We're building Base
if get_option('static')
  lang_flags += _D + 'BASE_EXTERN_STATIC_LIB'
else
  lang_flags += _D + 'BASE_EXTERN_BUILD_DYNAMIC_LIB'
endif
# MacOS Requires __STDC_WANT_LIB_EXT1__ == 1
if os == 'darwin'
  lang_flags += _D + '__STDC_WANT_LIB_EXT1__=1'
endif
# MLock options
if get_option('mlock') and os != 'openbsd'
  lang_flags += _D + 'BASE_EXTERN_MLOCK'
  if get_option('mlock_threadsafe')
	lang_flags += _D + 'BASE_EXTERN_MLOCK_THREADSAFE'
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
# Unixlike options
if os in UNIXLIKE_OPERATING_SYSTEMS
  if compiler.get_id() in GCC_COMPATIBLE_COMPILERS # GCC, Clang
    if get_option('native_optimize') # -march=native
      lang_flags += '-march=native'
    endif
    lang_flags += GCC_COMPATIBLE_FLAGS
  endif
  if get_option('lua') # Lua support
    lang_flags += _D + 'BASE_EXTERN_LUA'
	src += [
	  'impl/lua/lua.c',   # luaopen_Base
	  'impl/lua/mmap.c',  # luaopen_Base_MMap
	  'impl/lua/procs.c', # luaopen_Base_Procs
	  'impl/lua/sbuffer.c'# luaopen_Base_SBuffer
	]
    lang_flags += _D + 'BASE_EXTERN_LUA_MAJOR=5'
    lua_dep = compiler.find_library('lua5.4', required: false, static: false, dirs: lib_dir)
    if lua_dep.found() # We are using Lua5.4
      lib_deps += lua_dep
      lang_flags += _D + 'BASE_EXTERN_LUA_MINOR=4'
    else # We will try using Lua5.3
      lib_deps += compiler.find_library('lua5.3', static: false, dirs: lib_dir)
      lang_flags += _D + 'BASE_EXTERN_LUA_MINOR=3'
    endif
  endif # ! if get_option('lua')
endif

include_dirs += '..'
# Install the base folder into the system header directory
install_subdir('../Base', install_dir: include_install, exclude_directories: ['builddir', '.git', 'impl'], exclude_files: ['.gitignore', 'meson.build', 'README.md'])

if get_option('static')
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
