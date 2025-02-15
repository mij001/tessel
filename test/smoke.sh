#!/bin/sh
# host side: build + install in the vm, then assert n clients get tiled.
set -e
cd "$(dirname "$0")/.."
N=${1:-3}
CLIENT=${2:-foot}

HEADLESS=1 sh test/run.sh >/dev/null
sh test/sync.sh >/dev/null

OUT=$(sh test/ssh.sh "SETTLE=${SETTLE:-4} sh ~/tessel/test/guest-smoke.sh $N $CLIENT")
echo "$OUT"

GOT=$(echo "$OUT" | grep -c 'map .* views)' || true)
if [ "$GOT" -eq "$N" ]; then
	echo "PASS: $N/$N $CLIENT windows tiled"
else
	echo "FAIL: only $GOT/$N mapped"
	exit 1
fi
