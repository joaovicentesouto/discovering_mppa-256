#include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#define MASK ~0x0
static int sync_in;
static int sync_out;

void init_sync(uint64_t mask)
{
    char path_in[128];
    char path_out[128];
    sprintf(path_out, "/mppa/sync/128:16");
    sprintf(path_in, "/mppa/sync/[0..15]:16");

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

void mppa_signal(uint64_t mask)
{
    mppa_write(sync_out, &mask, sizeof(mask));
}

void end_sync(void)
{
    mppa_close(sync_in);
    mppa_close(sync_out);
}

int main(__attribute__((unused)) int argc, const char **argv)
{
    // int id = __k1_get_cluster_id();
    int id = atoi(argv[0]);

    printf("[IODDR0] Cluster %d: Start sync\n", id);

    init_sync(MASK);

    if (id == 1) //! cluster 1
    {
        printf("[IODDR0] Cluster %d: Wait %d\n", id);
        mppa_signal(1 << 0);
    } 
    else //! cluster 2
    {
        printf("[IODDR0] Cluster %d: Wait\n", id);
        mppa_signal(1 << 1);
    }
    
    printf("[IODDR0] Cluster %d: Sync\n", id);

    mppa_wait();

    end_sync();

    printf("[IODDR0] Cluster %d: End Sync\n", id);
	printf("[IODDR0] Cluster %d: Goodbye\n", id);

	return 0;
}
