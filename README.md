# SSC
# POSIX/Win32 Wrapper Library
## Purpose
Provide a simple, generalized interface between operating systems for file manipulation,
memory-mapping, memory-locking, and more.
## Dependencies
-	[meson](https://mesonbuild.com) Build System
## Linux, MacOS, BSDs
-	On Linux we need the ncurses development libraries and tinfo installed.
-	On MacOS and the BSDs we only need the ncurses development libraries installed.
## Win64
-	Minimum __Windows Vista/Server 2008__
-	Minimum Visual Studio 2019
### Building SSC with meson
#### Linux, MacOS, BSD OSs
1. git clone [SSC](https://github.com/stuartcalder/SSC) and cd into it.
2. Execute the following:
```
	$ mkdir builddir
	$ meson --backend=ninja builddir
	$ cd builddir
	$ ninja
	# ninja install
```
#### Windows OSs
1. git clone [SSC](https://github.com/stuartcalder/SSC) and cd into it.
2. Open a __"x64 Native Tools Command Prompt for VS 2022"__ cmd prompt, then cd into the cloned SSC project directory.
3. Execute the following:
```
	mkdir builddir
	meson --backend=ninja builddir
	cd builddir
	ninja
	ninja install
```
