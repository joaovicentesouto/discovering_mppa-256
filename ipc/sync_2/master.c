#include <mppaipc.h>
// #include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include <mppa.h>

//! Sync section
#define MASK ~0x3
static int sync_in;
static int sync_out;

void init_sync(uint64_t mask);
void end_sync(void);
void mppa_wait(void);
void mppa_signal(void);

//! Main
int main(__attribute__((unused)) int argc, const char **argv)
{
        printf(" ====== IPC: Sync 2 ======\n");
        printf(" Open portals\n");

    init_sync(MASK);
    spawn();

	    printf("Wait\n");

    mppa_wait();
    
        printf("Sync\n");

    mppa_signal();
    end_sync();

        printf("Join\n");

    join();

        printf("Goodbye\n");

	return 0;
};


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

void end_sync(void)
{
    mppa_close(sync_in);
    mppa_close(sync_out);
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
