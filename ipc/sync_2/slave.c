#include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#define MASK ~0x0
static int sync_in;
static int sync_out;

void init(uint64_t mask);
void end(void);
void wait_signal(void);
void send_signal(uint64_t mask);

int main(__attribute__((unused)) int argc, const char **argv)
{
    int id = atoi(argv[0]);

        printf("C%d, Start sync\n", id);

    init(MASK);

        printf("C%d, Signal\n", id);

    if (id == 1)
        send_signal(1 << 0);
    else
        send_signal(1 << 1);
    
        printf("C%d, Wait\n", id);

    wait_signal();
    end();

	    printf("C%d, Goodbye\n", id);

	return 0;
}

//! ================ Functions ================

void init(uint64_t mask)
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

void send_signal(uint64_t mask)
{
    mppa_write(sync_out, &mask, sizeof(mask));
}
