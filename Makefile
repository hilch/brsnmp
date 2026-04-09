PROJECT := brsnmp

# Auto-detect MSYS2 32-bit MinGW compiler (required for PviCom.lib compatibility)
# Falls back to system g++ if MSYS2 is not installed
MSYS2_GCC := $(if $(wildcard C:/msys64/mingw32/bin/g++.exe),C:/msys64/mingw32/bin/g++.exe,)
CXX := $(if $(MSYS2_GCC),$(MSYS2_GCC),g++)

# On Windows with MSYS2, GNU make may default to sh.exe which breaks cmd syntax in rules
SHELL := $(if $(MSYS2_GCC),cmd,/bin/sh)

CXXFLAGS_COMMON := -Wall -std=c++14 -fexceptions -Iinclude
LDFLAGS_COMMON := -static-libstdc++
LDLIBS := libraries/PviCom.lib

SRC := src/main.cpp src/MyPviConnection.cpp

DEBUG_OBJDIR := obj/Debug
RELEASE_OBJDIR := obj/Release
DEBUG_BINDIR := bin/Debug
RELEASE_BINDIR := bin/Release

DEBUG_CXXFLAGS := $(CXXFLAGS_COMMON) -g -D_DEBUG
RELEASE_CXXFLAGS := $(CXXFLAGS_COMMON) -Os
RELEASE_LDFLAGS := $(LDFLAGS_COMMON) -s -static-libgcc -static

DEBUG_OBJS := $(patsubst src/%.cpp,$(DEBUG_OBJDIR)/%.o,$(SRC))
RELEASE_OBJS := $(patsubst src/%.cpp,$(RELEASE_OBJDIR)/%.o,$(SRC))

DEBUG_TARGET := $(DEBUG_BINDIR)/$(PROJECT).exe
RELEASE_TARGET := $(RELEASE_BINDIR)/$(PROJECT).exe

.PHONY: all debug release clean

all: release

debug: $(DEBUG_TARGET)

release: $(RELEASE_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	@if not exist "$(DEBUG_BINDIR)" mkdir "$(DEBUG_BINDIR)"
	$(CXX) $(DEBUG_OBJS) -o $@ $(LDFLAGS_COMMON) $(LDLIBS)

$(RELEASE_TARGET): $(RELEASE_OBJS)
	@if not exist "$(RELEASE_BINDIR)" mkdir "$(RELEASE_BINDIR)"
	$(CXX) $(RELEASE_OBJS) -o $@ $(RELEASE_LDFLAGS) $(LDLIBS)

$(DEBUG_OBJDIR)/%.o: src/%.cpp
	@if not exist "$(DEBUG_OBJDIR)" mkdir "$(DEBUG_OBJDIR)"
	$(CXX) $(DEBUG_CXXFLAGS) -c $< -o $@

$(RELEASE_OBJDIR)/%.o: src/%.cpp
	@if not exist "$(RELEASE_OBJDIR)" mkdir "$(RELEASE_OBJDIR)"
	$(CXX) $(RELEASE_CXXFLAGS) -c $< -o $@

clean:
	@if exist obj rmdir /S /Q obj
	@if exist bin rmdir /S /Q bin