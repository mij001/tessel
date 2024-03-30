#!/bin/sh
# install what tessel needs to build, plus a terminal to test with.
set -e
cd "$(dirname "$0")/.."
sh test/ssh.sh 'sudo dnf install -y -q \
	gcc-c++ make pkgconf-pkg-config \
	wlroots-devel wayland-devel wayland-protocols-devel \
	libxkbcommon-devel pixman-devel libinput-devel \
	foot kitty mesa-dri-drivers' 2>&1 | tail -4
sh test/ssh.sh 'pkg-config --modversion wlroots-0.18; foot --version'
