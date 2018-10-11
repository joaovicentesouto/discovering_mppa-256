#include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

//! Spawn section
#define NUM_CLUSTERS 16
static mppa_pid_t pids[NUM_CLUSTERS];

void spawn(void);
void join();

//! Sync section
#define MASK ~0x3F
static int sync_fd;

void init_sync(uint64_t mask);
void end_sync(void);
void wait(void);

//! Main
int main(__attribute__((unused)) int argc, const char **argv)
{
        printf(" ====== IPC: Sync 1 ======\n");
        printf(" Open portals\n");

    init_sync(MASK);
    spawn();

	    printf("Wait\n");

    wait();
    
        printf("Sync\n");

    end_sync();

        printf("Join\n");

    join();

        printf("Goodbye\n");

	return 0;
};

// ====== Sync functions ======
void init_sync(uint64_t mask)
{
    char pathname[128];
    sprintf(pathname, "/mppa/sync/%d:16", __k1_get_cluster_id());
    sync_fd = mppa_open(pathname, O_RDONLY);
    assert(sync_fd != -1);

    mppa_ioctl(sync_fd, MPPA_RX_SET_MATCH, mask);
}

void end_sync(void)
{
    mppa_close(sync_fd);
}

void wait(void)
{
    uint64_t value;
    mppa_read(sync_fd, &value, sizeof(value));
}

// ====== Spawn functions ======
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
		pids[i] = mppa_spawn(i, NULL, "slave", (const char **)args, NULL);
		assert(pids[i] != -1);
	}
}

void join()
{
    int i;
	for (i = 1; i < 3; i++)
		mppa_waitpid(pids[i], NULL, 0);
}