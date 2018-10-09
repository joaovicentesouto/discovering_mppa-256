
#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Directories.
export BINDIR  = $(CURDIR)/bin

# Builds all kernels for MPPA-256.
all-ipc: 
	mkdir -p bin
	cd ipc && $(MAKE) all BINDIR=$(BINDIR)

# Builds all kernels for MPPA-256.
all-noc: 
	mkdir -p bin
	cd noc && $(MAKE) all BINDIR=$(BINDIR)

# Cleans compilation files.
clean:
	cd ipc && $(MAKE) clean BINDIR=$(BINDIR)
	cd noc && $(MAKE) clean BINDIR=$(BINDIR)