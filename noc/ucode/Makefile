#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#
# Builds CAP Benchmark suite for MPPA-256 architecture.
#

# Builds all kernels.
all: cmd_1cluster_sendto_1io cmd_1io_sendto_1cluster # cmd_1cluster_sendto_1io

# Builds sync kernel.
cmd_1io_sendto_1cluster:
	cd 1io_sendto_1cluster && $(MAKE) all
	cp 1io_sendto_1cluster/output/bin/*.img $(BINDIR)

# Builds another sync kernel.
cmd_1cluster_sendto_1io:
	cd 1cluster_sendto_1io && $(MAKE) all
	cp 1cluster_sendto_1io/output/bin/*.img $(BINDIR)

clean:
	rm -rf 1io_sendto_1cluster/output
	rm -rf 1cluster_sendto_1io/output