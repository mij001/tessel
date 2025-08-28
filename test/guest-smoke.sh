#!/bin/sh
# runs inside the vm: headless tessel, map n clients, report what got tiled.
N=${1:-3}
CLIENT=${2:-foot}
export XDG_RUNTIME_DIR=${XDG_RUNTIME_DIR:-/tmp/rt}
mkdir -p "$XDG_RUNTIME_DIR"; chmod 700 "$XDG_RUNTIME_DIR"
export WLR_BACKENDS=headless WLR_RENDERER=pixman WLR_HEADLESS_OUTPUTS=1

pkill -x tessel 2>/dev/null
sleep 1
rm -f /tmp/t.log
setsid tessel >/tmp/t.log 2>&1 &
for i in $(seq 40); do grep -q 'tessel up' /tmp/t.log && break; sleep 0.25; done

WD=$(sed -n 's/.*tessel up on \([a-z0-9_-]*\).*/\1/p' /tmp/t.log)
[ -n "$WD" ] || { echo "compositor did not start"; cat /tmp/t.log; exit 1; }
export WAYLAND_DISPLAY=$WD

i=1
while [ $i -le "$N" ]; do
	$CLIENT -e sleep 10 >/dev/null 2>&1 &
	i=$((i + 1))
	sleep 1.5
done
sleep ${SETTLE:-4}

grep -E 'output |map ' /tmp/t.log
pkill -x tessel 2>/dev/null
exit 0
