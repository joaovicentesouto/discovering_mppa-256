
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
static int portal_fd, sync_fd;

void portal_open()
{
    char pathname[128];
    sprintf(pathname, "/mppa/portal/[1,2]:7");
    portal_fd = mppa_open(pathname, O_WRONLY);
    assert(portal_fd != -1);

    sprintf(pathname, "/mppa/sync/128:16");
    sync_fd = mppa_open(pathname, O_RDONLY);
    assert(sync_fd != -1);

    mppa_ioctl(sync_fd, MPPA_RX_SET_MATCH, (uint64_t) MASK);
}

void portal_wait()
{
    uint64_t value;
    mppa_read(sync_fd, &value, sizeof(value));
}

void portal_write(int rank, char * buffer, int size, int offset)
{
    mppa_ioctl(portal_fd, MPPA_TX_SET_RX_RANK, rank);
    mppa_pwrite(portal_fd, buffer, size, offset);
}

void portal_close(void)
{
    mppa_close(portal_fd);
    mppa_close(sync_fd);
}

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    printf("[IODDR0] MASTER: Start portal\n");

    portal_open();

    spawn();

	printf("[IODDR0] MASTER: Wait\n");

    portal_wait();
    
    printf("[IODDR0] MASTER: send\n");

    char buff_1[4];
    sprintf(buff_1, "C_1\0");
    portal_write(0, buff_1, 4, 0);

    char buff_2[7];
    sprintf(buff_2, "C_____2");
    portal_write(1, buff_2, 7, 0);


    printf("[IODDR0] MASTER: End portal\n");

    portal_close();
    join();

    printf("[IODDR0] Goodbye\n");

	return 0;
};
