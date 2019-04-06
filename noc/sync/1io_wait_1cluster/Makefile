#
# Copyright(C) 2014 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Toolchain
export CC = $(TOOLCHAIN)/bin/k1-gcc
export LD = $(TOOLCHAIN)/bin/k1-ld
export AR = $(TOOLCHAIN)/bin/k1-ar

# Toochain Location
export TOOLCHAIN=/usr/local/k1tools

# Toolchain Configuration
export CFLAGS   = -std=c99 -fno-builtin # -ansi
export CFLAGS  += -Wall -Wextra -Werror -Wa,--warn # -pedantic-errors
export CFLAGS  += -Winit-self -Wswitch-default -Wfloat-equal
export CFLAGS  += -Wundef -Wshadow -Wuninitialized -Wlogical-op
export CFLAGS  += -Wvla # -Wredundant-decls
export CFLAGS  += -I $(INCDIR)

# Compiler Options
export CFLAGS += -D_KALRAY_MPPA256 -D_NOC_ -D_MAILBOX_

export CFLAGS_CC = $(CFLAGS) -D_MASTER_ -march=k1b -mos=bare -mcluster=node 

export CFLAGS_IO = $(CFLAGS) -march=k1b -mos=bare -mcluster=ioddr 

# Linker Options
export LDFLAGS += -Wl,--defsym=_LIBNOC_DISABLE_FIFO_FULL_CHECK=0 #-O=essai

export LDFLAGS_CC = $(LDFLAGS) -march=k1b -mos=bare -mcluster=node 

export LDFLAGS_IO = $(LDFLAGS) -march=k1b -mos=bare -mcluster=ioddr

LIBS = -lvbsp -lutask -lmppapower -lmppanoc -lmpparouting -mhypervisor


# Builds All Object Files
all:
	$(CC) $(CFLAGS_IO) master.c $(wildcard $(LIBSRCDIR)/*.c) $(LDFLAGS_IO) -o master $(LIBS)
	$(CC) $(CFLAGS_CC) slave.c $(LIBSRCDIR)/noc.c $(LDFLAGS_CC) -o slave $(LIBS) 
	$(TOOLCHAIN)/bin/k1-create-multibinary -f --boot master --clusters slave -T 1io_wait_1cluster_exec.img

# Cleans All Object Files
clean:
	rm -rf master slave
