#include <mppaipc.h>
// #include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <spawn.h>

//! Portal section
#define MASK ~0x3F
static int portal_fd;
static mppa_aiocb_t aiocb;

void portal_open(void);
void portal_close(void);
void portal_aio_read(char * buffer, int size);
void portal_aio_wait(void);

//! Main
int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    char buffer[11];
    memset(buffer, 0, 11);

        printf(" ====== IPC: Portal 1 ======\n");
        printf(" Open portals\n");

    portal_open();
    portal_aio_read(buffer, 11);
    spawn();

	    printf("Wait aio read\n");

    portal_aio_wait();
    
        printf("Message: %s\nClose portals", buffer);

    portal_close();

        printf("Join\n");

    join();

        printf("Goodbye\n");

	return 0;
};

//! ================ Functions ================

void portal_open(void)
{
    char pathname[128];
    sprintf(pathname, "/mppa/portal/128:7");
    portal_fd = mppa_open(pathname, O_RDONLY);
    assert(portal_fd != -1);
}

void portal_close(void)
{
    mppa_close(portal_fd);
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