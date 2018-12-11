#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#
# Builds CAP Benchmark suite for MPPA-256 architecture.
#

# Directories.
export LIBSRCDIR = $(CURDIR)/libsrc
export K1_TOOLCHAIN_DIR = /usr/local/k1tools

# Builds all kernels.
all: cmd_handler#cmd_sync #cmd_ucode#cmd_sync # cmd_portal cmd_mailbox cmd_ucode cmd_handler

# Builds sync kernel.
cmd_sync:
	cd sync && $(MAKE) all

# Builds portal kernel.
cmd_portal:
	cd portal && $(MAKE) all

# Builds mailbox kernel.
cmd_mailbox:
	cd mailbox && $(MAKE) all

# Builds handler kernel.
cmd_handler:
	cd handler && $(MAKE) all

# Builds ucode kernel.
cmd_ucode:
	cd ucode && $(MAKE) all

clean:
	cd sync && $(MAKE) clean
	cd portal && $(MAKE) clean
	cd mailbox && $(MAKE) clean
	cd handler && $(MAKE) clean
	cd ucode && $(MAKE) clean
