#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Directories.
export BINDIR=bin
export K1DIR=/usr/local/k1tools/bin

# Default Parameters.
export NPROCS=2

if [ -z $1 ]
then
	echo "You must specify ipc or noc mode."
	exit 1
fi

if [ "$1" = "ipc" ];
then
	echo "  ========== IPC MODE ==========  "
	for kernel in portal_exec;
	do
		echo "  ========== Running SYNC Kernel ==========  "
		$K1DIR/k1-jtag-runner                               \
			--multibinary=$BINDIR/$kernel.img               \
			--exec-multibin=IODDR0:master
	done
elif [ "$1" = "noc" ];
then
	echo "  ========== NOC MODE ==========  "
	for kernel in portal_exec;
	do
		echo "  ========== Running SYNC Kernel ==========  "
		$K1DIR/k1-jtag-runner                               \
			--multibinary=$BINDIR/$kernel.img               \
			--exec-multibin=IODDR0:io_bin
	done
fi


