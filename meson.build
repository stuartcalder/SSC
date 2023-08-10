project('SSC', 'c', default_options: ['c_std=c17', 'optimization=3', 'buildtype=release', 'strip=true'])
# Project / Language
PROJECT  = 'SSC'
LANG = 'c'

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#Operating Systems & Compilers#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
UNIXLIKE_OPERATING_SYSTEMS  = ['openbsd', 'freebsd', 'netbsd', 'darwin', 'linux']
SUPPORTED_OPERATING_SYSTEMS = UNIXLIKE_OPERATING_SYSTEMS + ['windows']
GCC_COMPATIBLE_COMPILERS = ['gcc', 'clang']
SUPPORTED_COMPILERS = GCC_COMPATIBLE_COMPILERS + ['msvc']
GCC_COMPATIBLE_FLAGS = [
  '-fvisibility=hidden',
  '-fstrict-aliasing',
  '-Wall',
  '-funroll-loops'
]
compiler = meson.get_compiler(LANG)
os = host_machine.system() # What OS will the compiled code be running on?
if os == 'windows' and compiler.get_id() == 'msvc'
  _D = '/D'
else
  _D = '-D'
endif
build_os = build_machine.system() # What OS will be used during compilation?
if os != 'netbsd' and compiler.get_id() != 'clang'
  # Do not use link-time optimizations on NetBSD or when using the Clang compiler
  GCC_COMPATIBLE_FLAGS += '-flto'
endif
assert(os in SUPPORTED_OPERATING_SYSTEMS, 'You are trying to build ' + PROJECT + ' using an unsupported operating system!')
assert(compiler.get_id() in SUPPORTED_COMPILERS, 'You are trying to build ' + PROJECT + ' using an unsupported compiler!')

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#What are the library directories?#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
_LIB_DIRS = {
  'bsd':   ['/usr/lib', '/usr/local/lib'],
  'linux': ['/usr/lib64', '/usr/lib'],
  'windows': 'C:\lib'
}
if build_os in ['openbsd', 'freebsd', 'netbsd', 'darwin']
  lib_dir = _LIB_DIRS.get('bsd') # These BSDs share common library directories
elif build_os in ['linux', 'windows']
  lib_dir = _LIB_DIRS.get(build_os)
else
  assert(false, 'Unsupported OS.')
endif


#%%%%%%%%%%%%%%%%%%%%%%%%%#
#Where is the source code?#
#%%%%%%%%%%%%%%%%%%%%%%%%%#
src =  [
'Impl/CommandLineArg.c',
'Impl/Error.c',
'Impl/File.c',
'Impl/MemLock.c',
'Impl/MemMap.c',
'Impl/Operation.c',
'Impl/Print.c',
'Impl/Random.c',
'Impl/String.c',
'Impl/Swap.c',
'Impl/Terminal.c'
]
lib_deps     = []
lang_flags   = []
include_dirs = []

#%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#What are the debug options?#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%#
if not get_option('debug_build')
  lang_flags += _D + 'NDEBUG'
else
  lang_flags += _D + 'SSC_EXTERN_DEBUG'
  lang_flags += _D + 'YDEBUG'
endif

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#Static or Dynamic Linking for the SSC library?#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
if get_option('static')
  lang_flags += _D + 'SSC_EXTERN_STATIC_LIB'
else
  lang_flags += _D + 'SSC_EXTERN_BUILD_DYNAMIC_LIB'
endif

if os == 'darwin'
  lang_flags += _D + '__STDC_WANT_LIB_EXT1__=1' # MacOS Requires __STDC_WANT_LIB_EXT1__ == 1
endif

#%%%%%%%%%%%%%%%%%%%%%%#
#Memory Locking Options#
#%%%%%%%%%%%%%%%%%%%%%%#
if get_option('memlock') and os != 'openbsd' # Disable on OpenBSD since swap is encrypted.
  lang_flags += _D + 'SSC_EXTERN_MEMLOCK'
  if get_option('memlock_threadsafe')
	lang_flags += _D + 'SSC_EXTERN_MEMLOCK_THREADSAFE'
  endif
endif

_INCLUDE_DIRS = {
  'bsd': '/usr/local/include',
  'netbsd': '/usr/pkg/include',
  'linux': '/usr/include',
  'windows': 'C:\include'
}

# Where do we install the headers afterwards?
# Look here for headers during compilation as well
if os in ['openbsd', 'freebsd', 'darwin']
  include_install = _INCLUDE_DIRS.get('bsd')
  include_dirs   += _INCLUDE_DIRS.get('bsd')
elif os == 'netbsd'
  include_install = _INCLUDE_DIRS.get('bsd')
  include_dirs   += _INCLUDE_DIRS.get('netbsd')
else
  include_install = _INCLUDE_DIRS.get(build_os)
  include_dirs   += _INCLUDE_DIRS.get(build_os)
endif

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#Unixlike dependencies and flags#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
if os in UNIXLIKE_OPERATING_SYSTEMS
  # All the supported Unixlikes require ncurses for terminal support
  lib_deps += compiler.find_library('ncurses', dirs: lib_dir)
  # Linux also requires tinfo
  if os == 'linux'
    lib_deps += compiler.find_library('tinfo', dirs: lib_dir)
  endif
  # Add GCC-specific options when we're using a GCC-compatible compiler
  if compiler.get_id() in GCC_COMPATIBLE_COMPILERS
    if get_option('native_optimize')
      lang_flags += '-march=native'
    endif
    lang_flags += GCC_COMPATIBLE_FLAGS
  endif
  #%%%%%%%%%%%#
  #Lua support#
  #%%%%%%%%%%%#
  if get_option('lua')
    lang_flags += _D + 'SSC_EXTERN_LUA'
    src += [
      'Impl/Lua/Lua.c',         # luaopen_SSC
      'Impl/Lua/MemMap.c',      # luaopen_SSC_MemMap
      'Impl/Lua/Proc.c',        # luaopen_SSC_Proc
      'Impl/Lua/SecureBuffer.c' # luaopen_SSC_SecureBuffer
    ]
    # Try to link Lua 5.4, but fallback to Lua 5.3 if we can't find 5.4
    lua_dep = compiler.find_library('lua5.4', required: false, static: false, dirs: lib_dir)
    if lua_dep.found()
      lib_deps += lua_dep
      lang_flags += _D + 'SSC_EXTERN_LUA_5_4'
    else
      lib_deps += compiler.find_library('lua5.3', static: false, dirs: lib_dir)
      lang_flags += _D + 'SSC_EXTERN_LUA_5_3'
    endif
  endif
endif
# Windows requires that we link bcrypt
if os == 'windows'
  lib_deps += compiler.find_library('bcrypt')
endif

# Allow manually specifying endianness
if get_option('endian') != 'auto'
  if   get_option('endian') == 'little'
    lang_flags += _D + 'SSC_EXTERN_ENDIAN=SSC_ENDIAN_LITTLE'
  elif get_option('endian') == 'big'
    lang_flags += _D + 'SSC_EXTERN_ENDIAN=SSC_ENDIAN_BIG'
  endif
endif

include_dirs += '..'
# Install the SSC folder into the system header directory
install_subdir('../SSC', install_dir: include_install, exclude_directories: ['builddir', '.git', 'Impl'], exclude_files: ['.gitignore', 'meson.build', 'README.md', 'cross_file.txt', 'meson_options.txt', 'LICENSE'])
_INC_DIRS = {
  'windows': 'C:\lib'
}
if build_os != os
  is_cross = true
else
  is_cross = false
endif

#%%%%%%%%%%%%#
# Static SSC #
#%%%%%%%%%%%%#
if get_option('static')
  if os == 'windows'
    if is_cross
      static_library('SSC', sources: src, dependencies: lib_deps, c_args: lang_flags, name_suffix: 'lib', name_prefix: '', install: false)
    else
      static_library('SSC', sources: src, dependencies: lib_deps, c_args: lang_flags, include_directories: include_dirs,
        name_suffix: 'lib', name_prefix: '', install: true, install_dir: 'C:\lib')
    endif
  else
    if is_cross
      static_library('SSC', sources: src, dependencies: lib_deps, c_args: lang_flags, install: false)
    else
      static_library('SSC', sources: src, dependencies: lib_deps, c_args: lang_flags, include_directories: include_dirs, install: true)
    endif
  endif
#%%%%%%%%%%%%%#
# Dynamic SSC #
#%%%%%%%%%%%%%#
else
  if os == 'windows'
    if is_cross
      shared_library('SSC', sources: src, dependencies: lib_deps, c_args: lang_flags, name_suffix: 'dll', name_prefix: '', install: false)
    else
      shared_library('SSC', sources: src, dependencies: lib_deps, c_args: lang_flags, include_directories: include_dirs,
        name_suffix: 'dll', name_prefix: '', install: true, install_dir: 'C:\lib')
    endif
  else
    if is_cross
      shared_library('SSC', sources: src, dependencies: lib_deps, c_args: lang_flags, install: false)
    else
      shared_library('SSC', sources: src, dependencies: lib_deps, c_args: lang_flags, include_directories: include_dirs, install: true)
    endif
  endif
endif
