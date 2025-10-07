#!/bin/sh
# type a line at the vm's tty1, as if you were sat at the qemu window.
set -e
cd "$(dirname "$0")/.."
LINE=$1
[ -n "$LINE" ] || { echo "usage: sh test/tty.sh '<command>'"; exit 1; }

keys=""
while [ -n "$LINE" ]; do
	ch=$(printf %.1s "$LINE"); LINE=${LINE#?}
	case "$ch" in
	[a-z0-9]) k=$ch ;;
	[A-Z])    k="shift-$(printf %s "$ch" | tr 'A-Z' 'a-z')" ;;
	-)        k=minus ;;
	_)        k=shift-minus ;;
	.)        k=dot ;;
	/)        k=slash ;;
	" ")      k=spc ;;
	*)        echo "unsupported character: $ch" >&2; exit 1 ;;
	esac
	keys="$keys $k"
done
