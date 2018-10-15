#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Executable.
EXEC = portal_2_exec

# Use bare libraries
system-name := bare

# Compilation flags.
cflags := -fopenmp -O3 -std=gnu99 -g -mhypervisor -Wall -Wextra -Winit-self -Wswitch-default -Wfloat-equal -Wundef -Wshadow -Wuninitialized #Werror
MPPAFlags := -g -lvbsp -lmppaipc -lutask -lgomp -lmppa_remote -lmppa_async -lmppa_request_engine -lmppapower -lmppanoc -lmpparouting -mhypervisor -Wl,--defsym=_LIBNOC_DISABLE_FIFO_FULL_CHECK=0

# Builds master binary.
io-bin := master
master-cflags := -D_MASTER_ -D_NOC_ -I $(INCDIR)
master-lflags := $(MPPAFlags) -lpcie_queue 
master-srcs := master.c $(wildcard $(LIBSRCDIR)/*.c)

# Builds slave binary.
cluster-bin := slave
cluster-system := $(cluster_system)
cluster-cflags := -D_NOC_ -I $(INCDIR)
cluster-lflags := $(MPPAFlags)
slave-srcs := slave.c $(LIBSRCDIR)/noc.c

# Builds image (master + slave)
mppa-bin := multibin_bin
multibin_bin-objs := master slave
multibin_bin-name := $(EXEC).img

# must be at the end of the makefile!
include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray