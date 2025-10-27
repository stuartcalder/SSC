cmake_minimum_required(VERSION 3.16)

project(SSC C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -s")

# ---------- OS & Compiler detection ----------
set(UNIXLIKE_OPERATING_SYSTEMS openbsd freebsd netbsd darwin linux)
set(SUPPORTED_OPERATING_SYSTEMS ${UNIXLIKE_OPERATING_SYSTEMS} windows)
set(GCC_COMPATIBLE_COMPILERS GNU Clang)
set(SUPPORTED_COMPILERS ${GCC_COMPATIBLE_COMPILERS} MSVC)
set(GCC_COMPATIBLE_FLAGS -fvisibility=hidden -fstrict-aliasing -Wall -funroll-loops)

if (WIN32 AND MSVC)
    set(_D "/D")
else()
    set(_D "-D")
endif()

# Host system
string(TOLOWER "${CMAKE_SYSTEM_NAME}" OS_LOWER)
if (NOT OS_LOWER IN_LIST SUPPORTED_OPERATING_SYSTEMS)
    message(FATAL_ERROR "Unsupported OS: ${CMAKE_SYSTEM_NAME}")
endif()
if (NOT CMAKE_C_COMPILER_ID IN_LIST SUPPORTED_COMPILERS)
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_C_COMPILER_ID}")
endif()

if (NOT OS_LOWER STREQUAL "netbsd" AND NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
    list(APPEND GCC_COMPATIBLE_FLAGS -flto)
endif()

# ---------- Library directories ----------
set(_LIB_DIRS_bsd /usr/lib /usr/local/lib)
set(_LIB_DIRS_linux /usr/lib64 /usr/lib)
set(_LIB_DIRS_windows "C:/lib")

if (OS_LOWER STREQUAL "openbsd" OR OS_LOWER STREQUAL "freebsd" OR
    OS_LOWER STREQUAL "netbsd" OR OS_LOWER STREQUAL "darwin")
    set(lib_dir ${_LIB_DIRS_bsd})
    set(lib_dir_install /usr/lib)
elseif(OS_LOWER STREQUAL "linux")
    set(lib_dir ${_LIB_DIRS_linux})
    set(lib_dir_install /usr/lib64)
elseif(WIN32)
    set(lib_dir ${_LIB_DIRS_windows})
    set(lib_dir_install ${lib_dir})
else()
    message(FATAL_ERROR "Unsupported OS for library dirs.")
endif()

# ---------- Sources ----------
set(SRC
    Impl/CommandLineArg.c
    Impl/Error.c
    Impl/File.c
    Impl/MemLock.c
    Impl/MemMap.c
    Impl/Memory.c
    Impl/Operation.c
    Impl/Process.c
    Impl/Print.c
    Impl/Random.c
    Impl/SSC_String.c
    Impl/Swap.c
    Impl/Terminal.c
)

set(LANG_FLAGS "")
set(INCLUDE_DIRS "")

# ---------- Debug options ----------
if (NOT DEBUG_BUILD)
    list(APPEND LANG_FLAGS "${_D}NDEBUG")
else()
    list(APPEND LANG_FLAGS "${_D}SSC_EXTERN_DEBUG" "${_D}YDEBUG")
endif()

# ---------- Static/Dynamic build selection ----------
option(BUILD_STATIC "Build static library" OFF)
if (BUILD_STATIC)
    list(APPEND LANG_FLAGS "${_D}SSC_EXTERN_STATIC_LIB")
else()
    list(APPEND LANG_FLAGS "${_D}SSC_EXTERN_BUILD_DYNAMIC_LIB")
endif()

if (APPLE)
    list(APPEND LANG_FLAGS "${_D}__STDC_WANT_LIB_EXT1__=1")
endif()

# ---------- Memory lock options ----------
option(MEMLOCK "Enable memory locking" OFF)
option(MEMLOCK_THREADSAFE "Threadsafe memory locking" OFF)
if (MEMLOCK AND NOT OS_LOWER STREQUAL "openbsd")
    list(APPEND LANG_FLAGS "${_D}SSC_EXTERN_MEMLOCK")
    if (MEMLOCK_THREADSAFE)
        list(APPEND LANG_FLAGS "${_D}SSC_EXTERN_MEMLOCK_THREADSAFE")
    endif()
endif()

# ---------- Include directories ----------
set(_INCLUDE_DIRS_bsd /usr/local/include)
set(_INCLUDE_DIRS_netbsd /usr/pkg/include)
set(_INCLUDE_DIRS_linux /usr/include)
set(_INCLUDE_DIRS_windows "C:/include")

if (OS_LOWER STREQUAL "openbsd" OR OS_LOWER STREQUAL "freebsd" OR OS_LOWER STREQUAL "darwin")
    set(include_install ${_INCLUDE_DIRS_bsd})
    list(APPEND INCLUDE_DIRS ${_INCLUDE_DIRS_bsd})
elseif(OS_LOWER STREQUAL "netbsd")
    set(include_install ${_INCLUDE_DIRS_bsd})
    list(APPEND INCLUDE_DIRS ${_INCLUDE_DIRS_netbsd})
elseif(OS_LOWER STREQUAL "linux")
    set(include_install ${_INCLUDE_DIRS_linux})
    list(APPEND INCLUDE_DIRS ${_INCLUDE_DIRS_linux})
elseif(WIN32)
    set(include_install ${_INCLUDE_DIRS_windows})
    list(APPEND INCLUDE_DIRS ${_INCLUDE_DIRS_windows})
endif()

# ---------- Dependencies ----------
set(LIB_DEPS "")
if (OS_LOWER IN_LIST UNIXLIKE_OPERATING_SYSTEMS)
    find_library(NCURSES_LIB ncurses PATHS ${lib_dir})
    list(APPEND LIB_DEPS ${NCURSES_LIB})
    if (OS_LOWER STREQUAL "linux")
        find_library(TINFO_LIB tinfo PATHS ${lib_dir})
        list(APPEND LIB_DEPS ${TINFO_LIB})
    endif()

    if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
        option(NATIVE_OPTIMIZE "Use -march=native" OFF)
        if (NATIVE_OPTIMIZE)
            list(APPEND LANG_FLAGS -march=native)
        endif()
        list(APPEND LANG_FLAGS ${GCC_COMPATIBLE_FLAGS})
    endif()

    option(ENABLE_LUA "Enable Lua" OFF)
    if (ENABLE_LUA)
        list(APPEND LANG_FLAGS "${_D}SSC_EXTERN_LUA")
        list(APPEND SRC Impl/Lua/Lua.c Impl/Lua/MemMap.c Impl/Lua/Proc.c Impl/Lua/SecureBuffer.c)
        find_library(LUA54 lua5.4 PATHS ${lib_dir})
        if (LUA54)
            list(APPEND LIB_DEPS ${LUA54})
            list(APPEND LANG_FLAGS "${_D}SSC_EXTERN_LUA_5_4")
        else()
            find_library(LUA53 lua5.3 PATHS ${lib_dir})
            list(APPEND LIB_DEPS ${LUA53})
            list(APPEND LANG_FLAGS "${_D}SSC_EXTERN_LUA_5_3")
        endif()
    endif()
endif()

if (WIN32)
    find_library(BCRYPT_LIB bcrypt)
    list(APPEND LIB_DEPS ${BCRYPT_LIB})
endif()

# ---------- Endianness ----------
set(ENDIAN "auto" CACHE STRING "Endian setting")
if (NOT ENDIAN STREQUAL "auto")
    if (ENDIAN STREQUAL "little")
        list(APPEND LANG_FLAGS "${_D}SSC_EXTERN_ENDIAN=SSC_ENDIAN_LITTLE")
    elseif (ENDIAN STREQUAL "big")
        list(APPEND LANG_FLAGS "${_D}SSC_EXTERN_ENDIAN=SSC_ENDIAN_BIG")
    endif()
endif()

list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR})

# ---------- Build target ----------
if (BUILD_STATIC)
    add_library(SSC STATIC ${SRC})
else()
    add_library(SSC SHARED ${SRC})
endif()

target_include_directories(SSC PRIVATE ${INCLUDE_DIRS})
target_compile_options(SSC PRIVATE ${LANG_FLAGS})
target_link_libraries(SSC PRIVATE ${LIB_DEPS})

# ---------- Installation ----------
install(TARGETS SSC DESTINATION ${lib_dir_install})
install(DIRECTORY ../SSC
        DESTINATION ${include_install}
        FILES_MATCHING PATTERN "*.h"
        PATTERN "Impl/" EXCLUDE
        PATTERN ".git" EXCLUDE
        PATTERN ".gitignore" EXCLUDE)

