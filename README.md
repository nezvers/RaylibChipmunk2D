# raylib starter

Just a simple Makefile based project to build games with raylib for HTML and Windows.

## Prerequisites
- a Windows OS
- CMD or Powershell
- make and gcc ([mingw](http://mingw-w64.org/doku.php) provides both)

## What does it do?
Raylib is added as a submodule targeting a specific commit (currently v3.7.0). Running the build command for the first time will generate the required static libraries for both web and windows platforms.

## Usage
```sh
# clone this repository
git clone git@github.com:tducasse/raylib_starter.git
# get the raylib source
git submodule update --init
```

Then, in a cmd or powershell terminal:
- `make web=1` to build for HTML
- `make` to build for windows
- `make clean_web` to clean the web build folder
- `make clean_desktop` to clean the desktop build folder