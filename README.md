# tessel

A small tiling Wayland compositor built on wlroots 0.18. Master/stack layout,
xdg-shell, server side decorations, keyboard and pointer.

It is a normal window manager: build it, install it, log into a tty on a machine
with no desktop, and start it.

## Build

Needs a C++ compiler, make, pkg-config, Wayland-scanner, and the development
headers for wlroots 0.18, Wayland, Wayland-protocols, pixman, libxkbcommon and
libinput. On Fedora:

```
sudo dnf install gcc-c++ make pkgconf-pkg-config wlroots-devel wayland-devel \
    wayland-protocols-devel pixman-devel libxkbcommon-devel libinput-devel
```

Then:

```
make
sudo make install
```
