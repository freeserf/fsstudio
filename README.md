FreeSerf Studio
===============

A resource browser and exporter for [Freeserf](https://github.com/freeserf/freeserf),
a remake of the original Settlers game. It reads the data files of the original
game (DOS or Amiga version) and Freeserf custom data, and shows sprites,
animations and sounds side by side for every source. Resources can be saved or
copied one by one, or exported in bulk to the Freeserf custom data format.

Build status
------------
[![Windows Build Status](https://github.com/freeserf/fsstudio/actions/workflows/windows.yml/badge.svg)](https://github.com/freeserf/fsstudio/actions/workflows/windows.yml)
[![Linux Build Status](https://github.com/freeserf/fsstudio/actions/workflows/linux.yml/badge.svg)](https://github.com/freeserf/fsstudio/actions/workflows/linux.yml)
[![macOS Build Status](https://github.com/freeserf/fsstudio/actions/workflows/macos.yml/badge.svg)](https://github.com/freeserf/fsstudio/actions/workflows/macos.yml)

Last build binaries
-------------------
Packages for every build are attached as artifacts to the workflow runs
(a GitHub login is required to download them):

* [Windows x64](https://github.com/freeserf/fsstudio/actions/workflows/windows.yml?query=branch%3Amaster) – NSIS installer
* [macOS](https://github.com/freeserf/fsstudio/actions/workflows/macos.yml?query=branch%3Amaster) – DMG
* [Linux](https://github.com/freeserf/fsstudio/actions/workflows/linux.yml?query=branch%3Amaster) – tarball

Game data
---------
By default FSStudio looks for the game data in `~/.local/share/freeserf`:

* DOS data file `spae.pa` (or `SPAD.PA`, `SPAF.PA`, `SPAU.PA` depending on the
  language of the game);
* Amiga files `gfxheader`, `gfxfast`, `gfxchip`, `gfxpics`, `sounds`, `music`;
* Freeserf custom data in the `custom` subdirectory.

Other locations can be chosen in *File → Sources*. Every source has its own
panel; the panels can be hidden in the *View* menu and rearranged by dragging
their tabs.

Settings and the window layout are kept in `fsstudio.ini` in the user
preferences folder (`~/Library/Application Support/freeserf/fsstudio` on
macOS, `~/.local/share/freeserf/fsstudio` on Linux,
`%APPDATA%\freeserf\fsstudio` on Windows).

Command line options:

* `--select <resource>:<index>` – start with a resource selected, e.g.
  `--select art_box:3`;
* `--screenshot <file.png>` – save the window to a PNG file and exit
  (settings are not changed).

Building
--------
FSStudio is built with CMake 3.21+ and a C++17 compiler. The user interface
uses [SDL3](https://libsdl.org) 3.4 or newer and
[Dear ImGui](https://github.com/ocornut/imgui). Freeserf and Dear ImGui sources
are used as git submodules:

```
git clone --recursive https://github.com/freeserf/fsstudio.git
cd fsstudio
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The system SDL3 is used when it is found (e.g. `brew install sdl3` on macOS),
otherwise CMake downloads SDL3 and links it statically. To always build SDL3
from source, add `-DFSSTUDIO_VENDORED_SDL=ON`; on Linux this requires the
[SDL3 build dependencies](https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md).

To build a redistributable package (NSIS installer on Windows, DMG on macOS,
tarball on Linux):

```
cd build
cpack
```

A statically linked SDL3 needs nothing else; a shared system SDL3 is copied
into the package. Building the Windows installer requires
[NSIS](https://nsis.sourceforge.io).

Known limitations
-----------------
* Music (MIDI in the DOS version, MOD in the Amiga version) can't be played,
  only saved to a file. Sound effects play normally.
* The Freeserf custom data format keeps one set of `delta`/`offset` values per
  sprite, so for sprites whose mask and image have different offsets (e.g.
  serf torsos in the DOS data) the export uses the mask values.
