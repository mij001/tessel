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

That puts `tessel` and `tessel-session` in /usr/local/bin and a session entry in
/usr/local/share/Wayland-sessions.

## Running it

From a tty on a machine with no window manager:

```
tessel-session
```

Logind gives the session the GPU and the input devices, so it needs no root and
no seatd. Press mod+Return for a terminal. Mod is Super, or Alt if you set
`TESSEL_MOD=alt`. Install a terminal first, tessel does not ship one:

```
sudo dnf install kitty
```

`tessel -s <command>` runs a command once the compositor is up, which is handy
for starting a bar or a terminal automatically.

A display manager will also list tessel as a session, via the desktop file.
