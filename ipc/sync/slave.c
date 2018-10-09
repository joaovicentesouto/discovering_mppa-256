#include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#define MASK_0 0xF
#define MASK_1 0x10
#define MASK_2 0x20

static int sync_fd;

void init_sync(void)
{
    char pathname[128] = "/mppa/sync/128:16";
    sync_fd = mppa_open(pathname, O_WRONLY);
    assert(sync_fd != -1);
}

void sync(uint64_t mask)
{
    mppa_write(sync_fd, &mask, sizeof(mask));
}

void end_sync(void)
{
    mppa_close(sync_fd);
}

int main(__attribute__((unused)) int argc, const char **argv)
{
    // int id = __k1_get_cluster_id();
    char id = argv[0][0];

    printf("[IODDR0] Cluster %c: Start sync\n", id);

    init_sync();

    if (id == '1') //! cluster 1
    {
        printf("[IODDR0] Cluster %c: Wait %d\n", id, MASK_0);
        sync(MASK_0);
    } 
    else //! cluster 2
    {
        printf("[IODDR0] Cluster %c: Wait %d - %d\n", id, MASK_1, MASK_2);
        sync(MASK_1);
        sync(MASK_2);
    }
    
    printf("[IODDR0] Cluster %c: Sync\n", id);

    end_sync();

    printf("[IODDR0] Cluster %c: End Sync\n", id);
	printf("[IODDR0] Cluster %c: Goodbye\n", id);

	return 0;
}
