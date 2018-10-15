
#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Directories.
export INCDIR = $(CURDIR)/include
export BINDIR  = $(CURDIR)/bin

# Builds all kernels for MPPA-256.
all-ipc: 
	mkdir -p $(BINDIR)
	cd ipc && $(MAKE) all BINDIR=$(BINDIR) INCDIR=$(INCDIR)

# Builds all kernels for MPPA-256.
all-noc: 
	mkdir -p $(BINDIR)
	cd noc && $(MAKE) all BINDIR=$(BINDIR) INCDIR=$(INCDIR)

# Cleans compilation files.
clean:
	rm -rf $(BINDIR)
	cd ipc && $(MAKE) clean
	cd noc && $(MAKE) clean