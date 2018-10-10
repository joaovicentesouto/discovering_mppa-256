#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#
# Builds CAP Benchmark suite for MPPA-256 architecture.
#

# Directories.
export K1_TOOLCHAIN_DIR = /usr/local/k1tools

# Builds all kernels.
all: cmd_portal # cmd_sync cmd_sync_2

# Builds sync kernel.
cmd_sync:
	cd sync && $(MAKE) all
	cp sync/output/bin/*.img $(BINDIR)

# Builds another sync kernel.
cmd_sync_2:
	cd sync_2 && $(MAKE) all
	cp sync_2/output/bin/*.img $(BINDIR)

# Builds portal kernel.
cmd_portal:
	cd portal && $(MAKE) all
	cp portal/output/bin/*.img $(BINDIR)

# Builds another portal kernel.
cmd_portal:
	cd portal_2 && $(MAKE) all
	cp portal_2/output/bin/*.img $(BINDIR)

clean:
	rm -rf sync/output
	rm -rf sync_2/output
	rm -rf portal/output
	rm -rf portal_2/output