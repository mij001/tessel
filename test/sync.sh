#!/bin/sh
# push the source into the test vm, build and install it there.
set -e
cd "$(dirname "$0")/.."
tar cf - Makefile src test 2>/dev/null \
	| sh test/ssh.sh 'rm -rf ~/tessel && mkdir -p ~/tessel && tar xf - -C ~/tessel'
sh test/ssh.sh 'cd ~/tessel && make 2>&1 | tail -5'
