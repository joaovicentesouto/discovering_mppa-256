# Copyright (C) 2013-2016 Kalray SA. All rights reserved.
# This code is Kalray proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of Kalray SA.

export K1_TOOLCHAIN_DIR="/usr/local/k1tools"
export CC = $(K1_TOOLCHAIN_DIR)/bin/k1-gcc
export LD = $(K1_TOOLCHAIN_DIR)/bin/k1-gcc

export SYSTEM   = bare
export ARCH     = k1b
export BOARD    = developer
export CLUSTER ?= node

export CFLAGS  += -std=c99 -Wall -O3 -I
export CFLAGS  += -mos=$(SYSTEM) -mcluster=$(CLUSTER) -march=$(ARCH) -mboard=$(BOARD)
export LDFLAGS += -Tlink.ld

export LIBS += -lvbsp -lmppanoc -lmpparouting -mhypervisor

export EXEC_PREFIX = hello-test
export MULTI_PREFIX = hello-test
export POSFIX = $(CLUSTER)

export EXECBIN = $(EXEC_PREFIX)-$(CLUSTER)
export MULTIBIN = $(MULTI_PREFIX)-$(CLUSTER).mpk

.PHONY: test

all: image
	
test:
	$(CC) $(CFLAGS) $(LDFLAGS) test/*.c -o $(EXECBIN) $(LIBS)

image: test
	$(K1_TOOLCHAIN_DIR)/bin/k1-create-multibinary --clusters $(EXECBIN) -T $(MULTIBIN)

run-cc: $(MULTI_PREFIX)-node.mpk
	$(K1_TOOLCHAIN_DIR)/bin/k1-jtag-runner --multibinary=$(MULTI_PREFIX)-node.mpk --exec-file=Cluster0:$(EXEC_PREFIX)-node

run-cc-gdb: $(MULTI_PREFIX)-node.mpk
	$(K1_TOOLCHAIN_DIR)/bin/k1-jtag-runner --gdb --multibinary=$(MULTI_PREFIX)-node.mpk --exec-file=Cluster0:$(EXEC_PREFIX)-node

run-io: $(MULTI_PREFIX)-ioddr.mpk
	$(K1_TOOLCHAIN_DIR)/bin/k1-jtag-runner --multibinary=$(MULTI_PREFIX)-ioddr.mpk --exec-file=IODDR0:$(EXEC_PREFIX)-ioddr

run-io-gdb: $(MULTI_PREFIX)-ioddr.mpk
	$(K1_TOOLCHAIN_DIR)/bin/k1-jtag-runner --gdb --multibinary=$(MULTI_PREFIX)-ioddr.mpk --exec-file=IODDR0:$(EXEC_PREFIX)-ioddr

clean:
	rm -rf $(EXEC_PREFIX)-node $(MULTI_PREFIX)-node.mpk
	rm -rf $(EXEC_PREFIX)-ioddr $(MULTI_PREFIX)-ioddr.mpk
