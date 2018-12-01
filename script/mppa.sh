#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Directories.
export BINDIR=bin
export K1DIR=/usr/local/k1tools/bin

for kernel in sync_1io_to_2c_exec; #handler_1io_to_1c_exec; # sync_1io_to_2c_exec sync_2c_to_1io_exec portal_1io_to_2c_exec portal_2c_to_1io_exec; 
do
	echo " "
	echo "  ========== Running Kernel ==========  "
	$K1DIR/k1-jtag-runner --gdb                               \
		--multibinary=$BINDIR/$kernel.img               \
		--exec-multibin=IODDR0:master
done

if [ "$1" = "noc" ]
then
	for kernel in mailbox_1c_to_1io_exec mailbox_1c_to_2io_exec mailbox_1io_to_2c_exec; 
	do
		echo " "
		echo "  ========== Running Kernel ==========  "
		$K1DIR/k1-jtag-runner                               \
			--multibinary=$BINDIR/$kernel.img               \
			--exec-multibin=IODDR0:master
	done
fi