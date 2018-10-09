#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Executable.
EXEC = sync_exec

# Use bare libraries
system-name := bare

# Compilation flags.
cflags := -fopenmp -O3 -std=gnu99 -g -mhypervisor -Wall -Wextra -Winit-self -Wswitch-default -Wfloat-equal -Wundef -Wshadow -Wuninitialized #Werror
MPPAFlags := -g -lvbsp -lmppaipc -lutask -lgomp -lmppa_remote -lmppa_async -lmppa_request_engine -lmppapower -lmppanoc -lmpparouting -mhypervisor -Wl,--defsym=_LIBNOC_DISABLE_FIFO_FULL_CHECK=0

# Builds master binary.
io-bin := io_bin
io_bin-cflags := -D_MASTER_
io_bin-lflags := $(MPPAFlags) -lpcie_queue 
io_bin-srcs := master.c

# Builds slave binary.
cluster-bin := cluster_bin
cluster-cflags := -I .
cluster-system := $(cluster_system)
cluster-lflags := $(MPPAFlags)
cluster_bin-srcs := slave.c

# Builds image (master + slave)
mppa-bin := multibin_bin
multibin_bin-objs := io_bin cluster_bin
multibin_bin-name := $(EXEC).img

# must be at the end of the makefile!
include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray