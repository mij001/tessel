#!/bin/sh
set -e
cd "$(dirname "$0")/.."
V=.vm
[ -f $V/disk.qcow2 ] || sh test/setup.sh

if [ -f $V/pid ] && kill -0 "$(cat $V/pid)" 2>/dev/null; then
	echo "vm already running (pid $(cat $V/pid))"
	exit 0
fi

ACCEL=tcg; CPU=max
[ -w /dev/kvm ] && { ACCEL=kvm; CPU=host; }

setsid qemu-system-x86_64 \
	-M q35 -accel $ACCEL -cpu $CPU -m ${VM_RAM:-2560} -smp ${VM_CPUS:-4} \
	-drive file=$V/disk.qcow2,if=virtio,format=qcow2 \
	-drive file=$V/seed.iso,if=virtio,format=raw,readonly=on \
	-netdev user,id=n0,hostfwd=tcp::2222-:22 \
	-device virtio-net-pci,netdev=n0 \
	-serial file:$V/console.log -monitor none -display none \
	-pidfile $V/pid </dev/null >/dev/null 2>&1 &
disown 2>/dev/null || true

echo "waiting for ssh..."
for i in $(seq 1 150); do
	if sh test/ssh.sh true 2>/dev/null; then echo "vm up"; exit 0; fi
	sleep 2
done
echo "timeout; see $V/console.log"; exit 1
