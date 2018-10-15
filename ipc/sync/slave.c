#include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#define MASK_0 0xF
#define MASK_1 0x10
#define MASK_2 0x20

static int sync_fd;

void init(void);
void send_signal(uint64_t mask);
void end(void);

int main(__attribute__((unused)) int argc, const char **argv)
{
    int id = atoi(argv[0]);

        printf("C%d, Start sync\n", id);

    init();

    if (id == 1) //! cluster 1
    {
            printf("C%d, Signal 0x%x\n", id, MASK_0);
        send_signal(MASK_0);
    } 
    else //! cluster 2
    {
            printf("C%d, Signal 0x%x - 0x%x\n", id, MASK_1, MASK_2);
        send_signal(MASK_1);
        send_signal(MASK_2);
    }
    
        printf("C%d, Sync\n", id);

    end();

	    printf("C%d, Goodbye\n", id);

	return 0;
}

//! ================ Functions ================

void init(void)
{
    char pathname[128] = "/mppa/sync/128:16";
    sync_fd = mppa_open(pathname, O_WRONLY);
    assert(sync_fd != -1);
}

void send_signal(uint64_t mask)
{
    mppa_write(sync_fd, &mask, sizeof(mask));
}

void end(void)
{
    mppa_close(sync_fd);
}
