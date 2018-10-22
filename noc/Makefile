#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#
# Builds CAP Benchmark suite for MPPA-256 architecture.
#

# Directories.
export LIBSRCDIR = $(CURDIR)/libsrc
export K1_TOOLCHAIN_DIR = /usr/local/k1tools

# Builds all kernels.
all: cmd_sync cmd_portal cmd_mailbox

# Builds sync kernel.
cmd_sync:
	cd sync && $(MAKE) all

# Builds portal kernel.
cmd_portal:
	cd portal && $(MAKE) all

# Builds mailbox kernel.
cmd_mailbox:
	cd mailbox && $(MAKE) all

clean:
	cd sync && $(MAKE) clean
	cd portal && $(MAKE) clean
	cd mailbox && $(MAKE) clean
