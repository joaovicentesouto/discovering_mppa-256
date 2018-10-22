#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#
# Builds CAP Benchmark suite for MPPA-256 architecture.
#

# Builds all kernels.
all: cmd_1io_sendto_2clusters cmd_1clusters_sendto_1io cmd_1clusters_sendto_2io

# Builds sync kernel.
cmd_1io_sendto_2clusters:
	cd 1io_sendto_2clusters && $(MAKE) all
	cp 1io_sendto_2clusters/output/bin/*.img $(BINDIR)

# Builds another sync kernel.
cmd_1clusters_sendto_1io:
	cd 1cluster_sendto_1io && $(MAKE) all
	cp 1cluster_sendto_1io/output/bin/*.img $(BINDIR)

# Builds another sync kernel.
cmd_1clusters_sendto_2io:
	cd 1cluster_sendto_2io && $(MAKE) all
	cp 1cluster_sendto_2io/output/bin/*.img $(BINDIR)

clean:
	rm -rf 1io_sendto_2clusters/output
	rm -rf 1cluster_sendto_1io/output
	rm -rf 1cluster_sendto_2io/output