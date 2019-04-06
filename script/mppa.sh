#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Directories.
export BINDIR=bin
export K1DIR=/usr/local/k1tools/bin

for kernel in 1io_wait_1cluster_exec;
do
	echo " "
	echo "  ========== Running Kernel ==========  "
	$K1DIR/k1-jtag-runner                               \
		--multibinary=$BINDIR/$kernel.img               \
		--exec-multibin=IODDR0:master
done
