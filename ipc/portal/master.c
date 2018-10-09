
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

#define MASK ~0x3F
static int portal_fd;
static mppa_aiocb_t aiocb;

void portal_open()
{
    char pathname[128];
    sprintf(pathname, "/mppa/portal/128:7");
    portal_fd = mppa_open(pathname, O_RDONLY);
    assert(portal_fd != -1);
}

void portal_aio_read(char * buffer, int size)
{
    mppa_aiocb_ctor(&aiocb, portal_fd, buffer, size);
    assert(mppa_aio_read(&aiocb) != -1);
}

void portal_aio_wait()
{
    mppa_aio_wait(&aiocb);
}

void portal_close(void)
{
    mppa_close(portal_fd);
}

int main(__attribute__((unused)) int argc, const char **argv)
{
    char buffer[11];
    memset(buffer, 0, 11);

    printf("[IODDR0] MASTER: Start portal\n");

    portal_open();

    portal_aio_read(buffer, 11);

    spawn();

	printf("[IODDR0] MASTER: Wait\n");

    portal_aio_wait();
    
    printf("[IODDR0] MASTER: Msg: %s\n", buffer);

    portal_close();

    printf("[IODDR0] MASTER: End portal\n");

    join();

    printf("[IODDR0] Goodbye\n");

	return 0;
};
