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

## Keys

| key           | action               |
|---------------|----------------------|
| mod+Return    | spawn terminal       |
| mod+q         | close window         |
| mod+Shift+q   | quit                 |
| mod+j / k     | focus next / prev    |
| mod+h / l     | shrink / grow master |
| mod+Space     | toggle floating      |
| mod+drag      | move (L), resize (R) |

Terminal comes from `$TESSEL_TERMINAL`, default `kitty`.

## Layout

One master column on the left, everything else stacked down the right.
`mod+h` and `mod+l` move the split. Floating and fullscreen windows are skipped
by the tiler.

## Testing

`test/` is a throwaway Fedora VM, used only to try tessel on a machine that has
no window manager. It is not needed to build or run tessel.

```
sh test/setup.sh      # fedora cloud image + cloud-init seed + overlay disk
sh test/run.sh        # boots it in a qemu window, autologs into tty1
sh test/provision.sh  # build deps and a terminal
sh test/sync.sh       # copy the source in, build, install
```

Then type `tessel-session` at the tty in the QEMU window and it comes up on the
virtio-GPU, same as it would on real hardware.

There is also a headless check that starts tessel with no display and asserts
that n clients got tiled:

```
sh test/smoke.sh 3
```
