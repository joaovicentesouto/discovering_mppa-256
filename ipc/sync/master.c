#include <mppaipc.h>
// #include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>


#include <spawn.h>

//! Sync section
#define MASK ~0x3F
static int sync_fd;

void init(uint64_t mask);
void end(void);
void wait_signal(void);

int main(__attribute__((unused)) int argc, const char **argv)
{
        printf(" ====== IPC: Sync 1 ======\n");
        printf(" Open portals\n");

    init(MASK);
    spawn();

	    printf("Wait\n");

    wait();
    
        printf("Sync\n");

    end();

        printf("Join\n");

    join();

        printf("Goodbye\n");

	return 0;
};

//! ================ Functions ================

void init(uint64_t mask)
{
    char pathname[128];
    sprintf(pathname, "/mppa/sync/128:16");
    sync_fd = mppa_open(pathname, O_RDONLY);
    assert(sync_fd != -1);

    mppa_ioctl(sync_fd, MPPA_RX_SET_MATCH, mask);
}

void end(void)
{
    mppa_close(sync_fd);
}

void wait_signal(void)
{
    uint64_t value;
    mppa_read(sync_fd, &value, sizeof(value));
}
