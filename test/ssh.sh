#!/bin/sh
# run a command in the test vm (or open a shell with no args).
cd "$(dirname "$0")/.."
exec ssh -q -p 2222 \
	-i .vm/id_ed25519 \
	-o StrictHostKeyChecking=no \
	-o UserKnownHostsFile=/dev/null \
	-o LogLevel=ERROR \
	-o ConnectTimeout=5 \
	dev@127.0.0.1 "$@"
