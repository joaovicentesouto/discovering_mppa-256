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

//! Portal section
#define MASK ~0x3
static int portal_fd, sync_fd;

void portal_open(void);
void portal_close(void);
void portal_wait(void);
void portal_write(int rank, char * buffer, int size, int offset);

//! Main
int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    char buff_1[4];
    char buff_2[7];
    
        printf(" ====== IPC: Portal 2 ======\n");
        printf(" Open portals\n");

    portal_open();
    spawn();

	    printf("Wait\n");

    portal_wait();
    
        printf("Send\n");

    sprintf(buff_1, "C_1\0");
    sprintf(buff_2, "C_____2");

    portal_write(0, buff_1, 4, 0);
    portal_write(1, buff_2, 7, 0);


        printf("Join\n");

    portal_close();
    join();

        printf("Goodbye\n");

	return 0;
};

// ====== Portal functions ======

void portal_open(void)
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

void portal_close(void)
{
    mppa_close(portal_fd);
    mppa_close(sync_fd);
}

void portal_wait(void)
{
    uint64_t value;
    mppa_read(sync_fd, &value, sizeof(value));
}

void portal_write(int rank, char * buffer, int size, int offset)
{
    mppa_ioctl(portal_fd, MPPA_TX_SET_RX_RANK, rank);
    mppa_pwrite(portal_fd, buffer, size, offset);
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