#include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#define MASK_0 0xF
#define MASK_1 0x10
#define MASK_2 0x20

static int portal_fd, sync_fd;
static mppa_aiocb_t aiocb;

void portal_open(void);
void portal_close(void);
void portal_signal(uint64_t mask);
void portal_aio_read(char * buffer, int size);
void portal_aio_wait(void);

int main(__attribute__((unused)) int argc, const char **argv)
{
    int id = atoi(argv[0]);
    int offset = id == 1 ? 0 : 1;
    int buffer_size = id == 1 ? 4 : 7;

        printf("C%d, Start portal\n", id);

    portal_open();

        printf("C%d, Send\n", id);

    portal_signal(1 << offset);
    
        printf("C%d, Sync\n", id);

    char buffer[buffer_size];
    portal_aio_read(buffer, buffer_size);
    portal_aio_wait();
    
        printf("C%d, B: %s\n", id, buffer);

    portal_close();

	    printf("C%d, Goodbye\n", id);

	return 0;
}

//! ================ Functions ================

void portal_open(void)
{
    portal_fd = mppa_open("/mppa/portal/[1,2]:7", O_RDONLY);
    assert(portal_fd != -1);

    sync_fd = mppa_open("/mppa/sync/128:16", O_WRONLY);
    assert(sync_fd != -1);
}

void portal_close(void)
{
    mppa_close(portal_fd);
    mppa_close(sync_fd);
}

void portal_signal(uint64_t mask)
{
    mppa_write(sync_fd, &mask, sizeof(mask));
}

void portal_aio_read(char * buffer, int size)
{
    mppa_aiocb_ctor(&aiocb, portal_fd, buffer, size);
	assert(mppa_aio_read(&aiocb) != -1);
}

void portal_aio_wait(void)
{
    mppa_aio_wait(&aiocb);
}
