#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Directories.
export BINDIR=bin
export K1DIR=/usr/local/k1tools/bin

# Default Parameters.
export NPROCS=2

for kernel in sync_1_exec sync_2_exec;
do
	echo "  ========== Running SYNC Kernel ==========  "
	$K1DIR/k1-jtag-runner                               \
		--multibinary=$BINDIR/$kernel.img               \
		--exec-multibin=IODDR0:master
done
