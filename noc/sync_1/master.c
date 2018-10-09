
#include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

//! SPAWN

#define NUM_CLUSTERS 16
static mppa_pid_t pids[NUM_CLUSTERS];

void spawn(void)
{
    int i;
	char arg0[4];
	char *args[2];

	/* Spawn slaves. */
	args[1] = NULL;
	for (i = 1; i < 3; i++)
	{	
		sprintf(arg0, "%d", i);
		args[0] = arg0;
		pids[i] = mppa_power_base_spawn(i, "cluster_bin", (const char **)args, NULL, MPPA_POWER_SHUFFLING_ENABLED);
		assert(pids[i] != -1);
	}
}

void join(void)
{
    int i, ret;
	for (i = 1; i < 3; i++)
		mppa_power_base_waitpid(i, &ret, 0);
}

//! SYNC

#define MASK ~0x3F
static int sync_fd;

void init_sync(uint64_t mask)
{
    char pathname[128];
    sprintf(pathname, "/mppa/sync/128:16");
    sync_fd = mppa_open(pathname, O_RDONLY);
    assert(sync_fd != -1);

    mppa_ioctl(sync_fd, MPPA_RX_SET_MATCH, mask);
}

void sync()
{
    uint64_t value;
    mppa_read(sync_fd, &value, sizeof(value));
}

void end_sync(void)
{
    mppa_close(sync_fd);
}

//! MAIN

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
    printf("[IODDR0] MASTER: Start sync\n");

    init_sync(MASK);

    spawn();

	printf("[IODDR0] MASTER: Wait\n");

    sync();
    
    printf("[IODDR0] MASTER: Sync\n");

    end_sync();

    printf("[IODDR0] MASTER: End Sync\n");

    join();

    printf("[IODDR0] Goodbye\n");

	return 0;
};
