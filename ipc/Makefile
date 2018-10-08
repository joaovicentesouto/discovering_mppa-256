#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#
# Builds CAP Benchmark suite for MPPA-256 architecture.
#

# Directories.
export K1_TOOLCHAIN_DIR = /usr/local/k1tools

# Builds all kernels.
all:
	cd sync && $(MAKE) all
	cp sync/output/bin/*.img $(BINDIR)

# # Builds ipc kernel.
# sync_test:
# 	cd sync && $(MAKE) all
# 	cp sync/output/bin/*.img $(BINDIR)