#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Directories.
export BINDIR=bin
export K1DIR=/usr/local/k1tools/bin

for kernel in sync_exec sync_2_exec portal_exec portal_2_exec;
do
	echo " "
	echo "  ========== Running Kernel ==========  "
	$K1DIR/k1-jtag-runner                               \
		--multibinary=$BINDIR/$kernel.img               \
		--exec-multibin=IODDR0:master
done

if [ "$1" = "noc" ]
then
	echo " "
	echo "  ========== Running Kernel ==========  "
	$K1DIR/k1-jtag-runner                               \
		--multibinary=$BINDIR/mailbox_exec.img          \
		--exec-multibin=IODDR0:master
fi