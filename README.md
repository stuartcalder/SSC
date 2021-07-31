# Base
# General C Library.
C Wrapper Library, and miscellaneous general-purpose utilities.
## Purpose
Provide a simple, generalized interface between operating systems for file manipulation,
memory-mapping, memory-locking, etc.
## Dependencies
-	[meson](https://mesonbuild.com) Build System
## Linux, MacOS, BSDs
-	On Linux we need the ncurses development libraries and tinfo installed.
-	On MacOS and the BSDs we need the ncurses development libraries.
## Win64
-	Minimum __Windows Vist/Server 2008__
-	Minimum Visual Studio 2019
### Building Base with meson
#### Linux, MacOS, BSD OSs
1. git clone [Base](https://github.com/stuartcalder/Base) and cd into it.
2. Execute the following:
```
	$ mkdir builddir
	$ meson --backend=ninja builddir
	$ cd builddir
	$ ninja
	# ninja install
```
#### Windows OSs
1. git clone [Base](https://github.com/stuartcalder/Base) and cd into it.
2. Open a __"x64 Native Tools Command Prompt for VS 2019"__ cmd prompt, then cd into the cloned Base project directory.
3. Execute the following:
```
	mkdir builddir
	meson --backend=ninja builddir
	cd builddir
	ninja
	ninja install
```
