#!/bin/sh
# one-time: fetch the fedora cloud image, build a cloud-init seed, make an
# overlay disk. the vm is only here to test tessel on a machine with no wm.
set -e
cd "$(dirname "$0")/.."
V=.vm
IMG=$V/fedora.qcow2
URL=https://dl.fedoraproject.org/pub/archive/fedora/linux/releases/40/Cloud/x86_64/images/Fedora-Cloud-Base-Generic.x86_64-40-1.14.qcow2
mkdir -p $V

[ -f $IMG ] || curl -fL -o $IMG "$URL"
[ -f $V/id_ed25519 ] || ssh-keygen -t ed25519 -N "" -f $V/id_ed25519 -C tessel-test

sed "s|SSH_PUBKEY_PLACEHOLDER|$(cat $V/id_ed25519.pub)|" test/cloud-init.yaml > $V/user-data
printf 'instance-id: tessel-01\nlocal-hostname: tessel-test\n' > $V/meta-data
xorriso -as mkisofs -quiet -output $V/seed.iso -volid CIDATA -joliet -rock \
	$V/user-data $V/meta-data

rm -f $V/disk.qcow2
qemu-img create -q -f qcow2 -F qcow2 -b "$(basename $IMG)" $V/disk.qcow2
qemu-img resize -q $V/disk.qcow2 16G
echo "test vm ready"
