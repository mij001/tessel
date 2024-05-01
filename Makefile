PKGS := wlroots-0.18 wayland-server xkbcommon pixman-1

CXX ?= g++
CXXFLAGS ?= -O2 -g -std=c++17 -Wall -Wextra -Wno-unused-parameter
CXXFLAGS += -DWLR_USE_UNSTABLE -Ibuild/include $(shell pkg-config --cflags $(PKGS))
LDLIBS := $(shell pkg-config --libs $(PKGS))

# wlroots headers use c99 [static n] array params, which c++ rejects.
# keep a patched copy and put it ahead of the system one.
WLR_INC := $(shell pkg-config --variable=includedir wlroots-0.18)/wlroots-0.18

SRC := $(wildcard src/*.cpp)
OBJ := $(patsubst src/%.cpp,build/%.o,$(SRC))

all: build/tessel

build/tessel: $(OBJ)
	$(CXX) -o $@ $^ $(LDLIBS)

build/include/.stamp: | build
	mkdir -p build/include
	cp -r $(WLR_INC)/wlr build/include/
	find build/include/wlr -name '*.h' | xargs sed -i 's/\[static [0-9][0-9]*\]/[]/g'
	touch $@

build/%.o: src/%.cpp build/include/.stamp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build:
	mkdir -p build

clean:
	rm -rf build

.PHONY: all clean
