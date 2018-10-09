
#include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

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

#define MASK ~0x3
static int sync_in;
static int sync_out;

void init_sync(uint64_t mask)
{
    char path_in[128];
    char path_out[128];
    sprintf(path_in, "/mppa/sync/128:16");
    sprintf(path_out, "/mppa/sync/[0..15]:16");

    sync_in = mppa_open(path_in, O_RDONLY);
    sync_out = mppa_open(path_out, O_WRONLY);
    
    assert(sync_in != -1);
    assert(sync_out != -1);

    mppa_ioctl(sync_in, MPPA_RX_SET_MATCH, mask);
}

void mppa_wait(void)
{
    uint64_t value;
    mppa_read(sync_in, &value, sizeof(value));
}

void mppa_signal(void)
{
    int ranks[2] = {1, 2};
    mppa_ioctl(sync_out, MPPA_TX_SET_RX_RANKS, 2, ranks);

    uint64_t value = ~0;
    mppa_write(sync_out, &value, sizeof(value));
}

void end_sync(void)
{
    mppa_close(sync_in);
    mppa_close(sync_out);
}

int main(__attribute__((unused)) int argc, const char **argv)
{
    printf("[IODDR0] MASTER: Start sync\n");

    init_sync(MASK);

    spawn();

	printf("[IODDR0] MASTER: Wait\n");

    mppa_wait();
    
    printf("[IODDR0] MASTER: Sync\n");

    mppa_signal();

    end_sync();

    printf("[IODDR0] MASTER: End Sync\n");

    join();

    printf("[IODDR0] Goodbye\n");

	return 0;
};
