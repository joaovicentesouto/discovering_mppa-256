#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Executable.
EXEC = portal_exec

# Sets the target platform.
platforms := hardware

# Compilation flags flags
cflags := -O3
lflags := -lm
k1-lflags := -lmppaipc

# Builds master binary.
io-bin := master
master-cflags := -mos=rtems -I $(INCDIR)
master-srcs := master.c $(LIBDIR)/mppa.c

# Builds slave binary
cluster-bin := slave
slave-lflags := -mos=rtems
slave-srcs := slave.c

# build image (master + slave)
mppa-bin := multibin
multibin-objs := master slave
multibin-name := $(EXEC).img

# must be at the end of the makefile!
include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.mppaipc