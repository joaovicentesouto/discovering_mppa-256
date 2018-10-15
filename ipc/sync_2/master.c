#include <mppaipc.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include <spawn.h>

//! Sync section
#define MASK ~0x3
static int sync_in;
static int sync_out;

void init(uint64_t mask);
void end(void);
void wait_signal(void);
void send_signal(void);

int main(__attribute__((unused)) int argc, const char **argv)
{
        printf(" ====== IPC: Sync 2 ======\n");
        printf(" Open portals\n");

    init(MASK);
    spawn();

	    printf("Wait\n");

    wait_signal();
    
        printf("Sync\n");

    send_signal();
    end();

        printf("Join\n");

    join();

        printf("Goodbye\n");

	return 0;
};

//! ================ Functions ================

void init(uint64_t mask)
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

void end(void)
{
    mppa_close(sync_in);
    mppa_close(sync_out);
}

void wait_signal(void)
{
    uint64_t value;
    mppa_read(sync_in, &value, sizeof(value));
}

void send_signal(void)
{
    int ranks[2] = {1, 2};
    mppa_ioctl(sync_out, MPPA_TX_SET_RX_RANKS, 2, ranks);

    uint64_t value = ~0;
    mppa_write(sync_out, &value, sizeof(value));
}
