# shim
OS Abstraction C Library
## Purpose
Provide a simple, generalized interface between operating systems for file manipulation,
memory-mapping, memory-locking, and other such features.
## Dependencies
-	[meson](https://mesonbuild.com) Build System
## Linux, OSX, BSDs
-	On Linux we need the ncurses development libraries and tinfo installed.
-	On OSX and the BSDs we need the ncurses development libararies.
## Win64
-	Minimum __Windows Vist/Server 2008__
-	Minimum Visual Studio 2019
### Building Shim with meson
#### OSX, BSD OSs
1. git clone [shim](https://github.com/stuartcalder/shim) and cd into it.
2. Execute the following:
```
	$ mkdir builddir
	$ meson --backend=ninja builddir
	$ cd builddir
	$ ninja
	# ninja install
```
#### Windows OSs
1. git clone [shim](https://github.com/stuartcalder/shim) and cd into it.
2. Open a __"x64 Native Tools Command Prompt for VS 2019"__ cmd prompt, then cd into the cloned shim project directory.
3. Execute the following:
```
	mkdir builddir
	meson --backend=ninja builddir
	cd builddir
	ninja
	ninja install
```
