
#include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#define NUM_CLUSTERS 16
static mppa_pid_t pids[NUM_CLUSTERS];

void spawn(void)
{
    int i;
	char arg0[4];
	char *args[2];

	/* Spawn slaves. */
	args[1] = NULL;
	for (i = 1; i < 3; i++)
	{	
		sprintf(arg0, "%d", i);
		args[0] = arg0;
		pids[i] = mppa_spawn(i, NULL, "slave", (const char **)args, NULL);
		assert(pids[i] != -1);
	}
}

void join()
{
    int i;
	for (i = 1; i < 3; i++)
		mppa_waitpid(pids[i], NULL, 0);
}

#define MASK ~0x3F
static int rx_tag = 7;
static mppa_aiocb_t aiocb;

void portal_open()
{
    assert(mppa_noc_dnoc_rx_alloc(0, rx_tag) == MPPA_NOC_RET_SUCCESS);
}

void portal_aio_read(char * buffer, int size, int offset)
{
    mppa_noc_dnoc_rx_configuration_t rx_configuration = {
        .buffer_base = buffer,
        .buffer_size = size,
        .current_offset = offset,
        .item_reload = 0,
        .item_counter = 0,
        .event_counter = 0,
        .reload_mode = MPPA_NOC_RX_RELOAD_MODE_INCR_DATA_NOTIF,
        .activation = MPPA_NOC_ACTIVATED,
        .counter_id = 0
    };

    assert(mppa_noc_dnoc_rx_configure(0, rx_tag, rx_configuration) == 0);

    mppa_noc_dnoc_rx_lac_event_counter(0, rx_tag);
}

void portal_aio_wait()
{
    int value = 0;
    while(value == 0)
        value = mppa_noc_dnoc_rx_get_event_counter(0, rx_tag);

    mppa_noc_dnoc_rx_lac_event_counter(0, rx_tag);
    mppa_noc_dnoc_rx_lac_item_counter(0, rx_tag);
}

void portal_close(void)
{
    mppa_close(portal_fd);
}

int main(__attribute__((unused)) int argc, const char **argv)
{
    char buffer[11];
    memset(buffer, 0, 11);

    printf("[IODDR0] MASTER: Start portal\n");

    portal_open();

    portal_aio_read(buffer, 11);

    spawn();

	printf("[IODDR0] MASTER: Wait\n");

    portal_aio_wait();
    
    printf("[IODDR0] MASTER: Msg: %s\n", buffer);

    portal_close();

    printf("[IODDR0] MASTER: End portal\n");

    join();

    printf("[IODDR0] Goodbye\n");

	return 0;
};
