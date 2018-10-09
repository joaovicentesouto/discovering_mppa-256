#include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#define MASK_0 0xF
#define MASK_1 0x10
#define MASK_2 0x20

static int portal_fd;

void portal_open(void)
{
    char pathname[128] = "/mppa/sync/128:7";
    portal_fd = mppa_open(pathname, O_WRONLY);
    assert(portal_fd != -1);
}

void portal_write(char *buffer, int size, int offset)
{
    mppa_pwrite(portal_fd, buffer, size, offset);
}

void portal_close(void)
{
    mppa_close(portal_fd);
}

int main(__attribute__((unused)) int argc, const char **argv)
{
    // int id = __k1_get_cluster_id();
    char id = atoi(argv[0]);

    printf("[IODDR0] Cluster %d: Start portal\n", id);

    portal_open();

    printf("[IODDR0] Cluster %d: send\n", id);

    if (id == 1)
    {
        char buffer[4];
        sprintf(buffer, "C1&");
        portal_write(buffer, 4, 0);
    }
    else
    {
        char buffer[7];
        sprintf(buffer, " Clus2");
        portal_write(buffer, 7, 4);
    }
    
    printf("[IODDR0] Cluster %d: Sync\n", id);

    portal_close();

    printf("[IODDR0] Cluster %d: End Sync\n", id);
	printf("[IODDR0] Cluster %d: Goodbye\n", id);

	return 0;
}
