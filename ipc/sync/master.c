#include <mppaipc.h>
// #include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>


#include <mppa.h>

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
    sprintf(pathname, "/mppa/sync/128:16");
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
