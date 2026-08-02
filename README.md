# SSC — Portable C Library

SSC is a cross-platform C library providing unified wrappers around POSIX and Win32 APIs for file operations, memory-mapped files, memory locking, aligned allocation, endianness conversion, command-line argument parsing, and more. It compiles on Linux, macOS, all major BSDs, and Windows with GCC, Clang, or MSVC.

## Modules

| Module | Description |
| --- | --- |
| `File` | File open, create, close, size queries, directory changes, secret files (Linux memfd) |
| `MemMap` | Memory-map files into RAM; supports read-only, secret, and resizeable maps |
| `MemLock` | Lock pages in physical memory to prevent swapping (Unix-like only) |
| `Memory` | Aligned allocation, page size queries, system memory info, endian-aware load/store |
| `Operation` | Byte rotation, XOR, constant-time comparison, secure zeroing |
| `CommandLineArg` | Structured short (`-x`) and long (`--option`) argument parsing with callbacks |
| `Process` | Query number of processors and executable path |
| `Print` | Format bytes as hex or binary with optional prefixes and newlines |
| `Random` | OS-backed pseudorandom entropy retrieval |
| `SSC_String` | Heap-allocated string type with metadata fields and secure-free support |
| `Swap` | Byte-swapping (16/32/64-bit) using GCC builtins, C++23 `std::byteswap`, or OS primitives |
| `Terminal` | Terminal control via ncurses (`ENABLE_TERMINAL=ON`) |

## Platforms & Compilers

| Platform | Compilers | Minimum version |
| --- | --- | --- |
| Linux (x86_64, ARM, RISC-V) | GCC, Clang | Any modern release |
| macOS / Darwin | GCC, Clang | Any supported Xcode |
| FreeBSD, OpenBSD, NetBSD, DragonFly BSD | GCC, Clang | Any supported release |
| Windows (x86_64) | MSVC | Visual Studio 2019 (v19.0) |

C standard: **C17**. C++11 or higher is also supported via the C linkage wrapper in `Macro.h`.

## Dependencies

**Required:** a C compiler and [CMake](https://cmake.org/) >= 3.16.

Optional, enabled at build time:
- **ncurses + tinfo** — needed only when `ENABLE_TERMINAL=ON` (default). On Linux both packages are required; on macOS and BSDs only ncurses is needed.
- **Lua 5.3 or 5.4** — set `ENABLE_LUA=ON` to include the Lua integration layer (`Impl/Lua/`).

## Build Options

| Option | Default | Description |
| --- | --- | --- |
| `BUILD_STATIC` | OFF | Build a static library instead of shared |
| `ENABLE_TERMINAL` | ON | Include the Terminal (ncurses) module |
| `MEMLOCK` | OFF | Enable memory-locking support (`SSC_EXTERN_MEMLOCK`) |
| `MEMLOCK_THREADSAFE` | OFF | Make MemLock thread-safe when `MEMLOCK=ON` |
| `ENABLE_LUA` | OFF | Include the Lua integration layer |
| `NATIVE_OPTIMIZE` | OFF | Pass `-march=native` to GCC/Clang |
| `ENDIAN` | auto | Force byte order (`little`, `big`, or `auto`) |

## Building

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build                        # requires root or a custom CMAKE_INSTALL_PREFIX
```

### Windows (MSVC)

Open an **x64 Native Tools Command Prompt for VS 2022**, then:

```cmd
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cmake --install build --config Release
```

### Common cmake invocations

```bash
# Static library, no terminal, Lua enabled
cmake -S . -B build \
  -DBUILD_STATIC=ON \
  -DENABLE_TERMINAL=OFF \
  -DENABLE_LUA=ON

# Enable memory locking with thread safety (Linux / BSDs)
cmake -S . -B build -DMEMLOCK=ON -DMEMLOCK_THREADSAFE=ON

# Cross-compile for Windows on Linux via mingw-w64
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=mingw-w64-toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release
```

## Quick Start

```c
#include <SSC/File.h>
#include <stdio.h>

int main(void) {
    if (SSC_FilePath_exists("/etc/hostname")) {
        printf("File exists.\n");
    }
    return 0;
}
```

Compile: `cc -o example example.c -lSSC` (or link the static archive).

## License

BSD 2-clause. Copyright (c) 2020–2025 Stuart Calder. See the [LICENSE](LICENSE) file for full terms.
