#**************************************************************************************************
#    Raylib makefile to build for windows and web targets.
#    Modified by Thibaud Ducasse (@tducasse)
#    - remove Android, RPI, LINUX, OSX
#    - make everything self contained:
#       - include/
#         - raylib.h
#       - lib/
#         - desktop/
#						- libraylib.a (compiled from source with PLATFORM_DESKTOP)
#					- web/
#						- libraylib.a (compiled from source with PLATFORM_WEB)
#						- shell.html (copied from source)
#**************************************************************************************************
#
#   raylib makefile for Desktop platforms, Raspberry Pi, Android and HTML5
#
#   Copyright (c) 2013-2019 Ramon Santamaria (@raysan5)
#
#   This software is provided "as-is", without any express or implied warranty. In no event
#   will the authors be held liable for any damages arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose, including commercial
#   applications, and to alter it and redistribute it freely, subject to the following restrictions:
#
#     1. The origin of this software must not be misrepresented; you must not claim that you
#     wrote the original software. If you use this software in a product, an acknowledgment
#     in the product documentation would be appreciated but is not required.
#
#     2. Altered source versions must be plainly marked as such, and must not be misrepresented
#     as being the original software.
#
#     3. This notice may not be removed or altered from any source distribution.
#
#**************************************************************************************************

# call `make web=1` to build for HTML
# call `make` to build for windows
# call `make clean_web` to clean the web build folder
# call `make clean_desktop` to clean the desktop build folder

PROJECT_NAME ?= game
INCLUDE_PATH ?= include
LIBRARY_PATH ?= lib
RAYLIB_NAME ?= libraylib.a

# Define compiler path 
COMPILER_PATH ?= C:/mingw/mingw32/bin

# Output folder
FOLDER = build/desktop
# Raylib static library
RAYLIB_PATH = $(LIBRARY_PATH)/desktop
# One of PLATFORM_DESKTOP, PLATFORM_WEB
PLATFORM ?= PLATFORM_DESKTOP
ifeq ($(web), 1)
	PLATFORM = PLATFORM_WEB
	FOLDER = build/web
	RAYLIB_PATH = $(LIBRARY_PATH)/web
endif

RAYLIB_STATIC = $(RAYLIB_PATH)/$(RAYLIB_NAME)

RAYLIB_RELEASE_PATH = $(CURDIR)/$(RAYLIB_PATH)

# Build mode for project: DEBUG or RELEASE
BUILD_MODE ?= RELEASE

# Define include paths for required headers
INCLUDE_PATHS = -I $(INCLUDE_PATH)

LFLAGS = -L $(RAYLIB_PATH) -lraylib -lopengl32 -lgdi32 -lwinmm
# Required for physac examples
#LFLAGS += -static -lpthread

# Define default C compiler: gcc
CC = gcc

# Define compiler flags:
#  -O0                  defines optimization level (no optimization, better for debugging)
#  -O1                  defines optimization level
#  -g                   include debug information on compilation
#  -s                   strip unnecessary data from build -> do not use in debug builds
#  -Wall                turns on most, but not all, compiler warnings
#  -std=c99             defines C language mode (standard C from 1999 revision)
#  -std=gnu99           defines C language mode (GNU C from 1999 revision)
#  -Wno-missing-braces  ignore invalid warning (GCC bug 53119)
#  -D_DEFAULT_SOURCE    use with -std=c99 on Linux and WEB, required for timespec
#  -Wextra
#  -Wmissing-prototypes
#  -Wstrict-prototypes
#  -Wl,--subsystem,windows avoid launching the console with the game
CFLAGS += -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces 

ifeq ($(BUILD_MODE), DEBUG)
	CFLAGS += -g -O0
else
	CFLAGS += -s -O1
endif

ifeq ($(PLATFORM), PLATFORM_WEB)
    # Emscripten required variables
	EMSDK_PATH ?= C:/emsdk
	EMSCRIPTEN_PATH ?= $(EMSDK_PATH)/upstream/emscripten
	CLANG_PATH = $(EMSDK_PATH)/upstream/bin
	PYTHON_PATH = $(EMSDK_PATH)/python/3.9.2-1_64bit
	NODE_PATH = $(EMSDK_PATH)/node/14.15.5_64bit/bin
	export PATH = $(EMSDK_PATH);$(EMSCRIPTEN_PATH);$(CLANG_PATH);$(NODE_PATH);$(PYTHON_PATH);$(COMPILER_PATH):$$(PATH)
	  
    # HTML5 emscripten compiler
    # WARNING: To compile to HTML5, code must be redesigned 
    # to use emscripten.h and emscripten_set_main_loop()
	CC = emcc

    # -os                        # size optimization
    # -o2                        # optimization level 2, if used, also set --memory-init-file 0
    # -s use_glfw=3              # use glfw3 library (context/input management)
    # -s allow_memory_growth=1   # to allow memory resizing -> warning: audio buffers could faIL!
    # -s total_memory=16777216   # to specify heap memory size (default = 16mb)
    # -s use_pthreads=1          # multithreading support
    # -s wasm=0                  # disable web assembly, emitted by default
    # -s emterpretify=1          # enable emscripten code interpreter (very slow)
    # -s emterpretify_async=1    # support synchronous loops by emterpreter
    # -s force_filesystem=1      # force filesystem to load/save files data
    # -s assertions=1            # enable runtime checks for common memory allocation errors (-O1 and above turn it off)
    # --profiling                # include information for code profiling
    # --memory-init-file 0       # to avoid an external memory initialization code file (.mem)
    # --preload-file resources   # specify a resources folder for data compilation
	CFLAGS += -Os -s USE_GLFW=3 -s TOTAL_MEMORY=16777216 --preload-file resources
	ifeq ($(BUILD_MODE), DEBUG)
		CFLAGS += -s ASSERTIONS=1 --profiling
	endif
    # Define a custom shell .html and output extension
	CFLAGS += --shell-file lib/web/shell.html 
	EXT = .html
		LFLAGS = -L $(RAYLIB_PATH) -lraylib 
		FOLDER = build/web
endif

# Define all source files required
OBJS ?= $(wildcard **/*.c)

# Project target defined by PROJECT_NAME
$(PROJECT_NAME): $(RAYLIB_STATIC) make_dirs $(OBJS)
	$(CC) -o $(FOLDER)/$(PROJECT_NAME)$(EXT) $(OBJS) $(CFLAGS) $(INCLUDE_PATHS) $(LFLAGS) -D$(PLATFORM)

$(RAYLIB_STATIC):
	$(MAKE) -C "raylib/src" RAYLIB_RELEASE_PATH=$(RAYLIB_RELEASE_PATH) PLATFORM=$(PLATFORM) -B

# Compile source files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) -D$(PLATFORM)

make_dirs:
	if exist "$(FOLDER)" rmdir /s /q "$(FOLDER)"
	if not exist "$(FOLDER)" mkdir "$(FOLDER)"

# Clean everything
clean: clean_web clean_desktop
	if exist build rmdir /s /q build
	@echo Cleaning done

clean_web:
	if exist "build/web" rmdir /s /q "build/web"
	@echo Cleaned web

clean_desktop:
	if exist "build/desktop" rmdir /s /q "build/desktop"
	@echo Cleaned desktop

clean_libs:
	$(MAKE) -C "raylib/src" clean RAYLIB_RELEASE_PATH=$(RAYLIB_RELEASE_PATH) PLATFORM=$(PLATFORM)

.PHONY: make_dirs clean clean_web clean_desktop $(PROJECT_NAME)