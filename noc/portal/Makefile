#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#
# Builds CAP Benchmark suite for MPPA-256 architecture.
#

# Builds all kernels.
all: cmd_1io_sendto_1cluster # cmd_2clusters_sendto_1io

# Builds sync kernel.
cmd_1io_sendto_1cluster:
	cd 1io_sendto_1cluster && $(MAKE) all
	cp 1io_sendto_1cluster/output/bin/*.img $(BINDIR)

# Builds another sync kernel.
cmd_2clusters_sendto_1io:
	cd 2clusters_sendto_1io && $(MAKE) all
	cp 2clusters_sendto_1io/output/bin/*.img $(BINDIR)

clean:
	rm -rf 1io_sendto_2clusters/output
	rm -rf 2clusters_sendto_1io/output