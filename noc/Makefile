#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#
# Builds CAP Benchmark suite for MPPA-256 architecture.
#

# Directories.
export K1_TOOLCHAIN_DIR = /usr/local/k1tools

# Builds all kernels.
all: cmd_sync cmd_sync_2

# Builds all kernels.
cmd_sync:
	cd sync && $(MAKE) all
	cp sync/output/bin/*.img $(BINDIR)

# Builds all kernels.
cmd_sync_2:
	cd sync_2 && $(MAKE) all
	cp sync_2/output/bin/*.img $(BINDIR)

# # Builds ipc kernel.
# sync_test:
# 	cd sync && $(MAKE) all
# 	cp sync/output/bin/*.img $(BINDIR)