// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

//! Spawn section
#define NUM_CLUSTERS 16
static mppa_power_pid_t pids[NUM_CLUSTERS];

void spawn(void);
void join(void);

//! Portal section
static int portal_rx = 7;

void portal_open(void);
void portal_close(void);
void portal_aio_read(char * buffer, int size, int offset);
void portal_aio_wait(void);

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    char buffer[11];
    memset(buffer, 0, 11);

        printf(" ====== NoC: Portal 1 ======\n");
        printf(" Open Portals\n");

    portal_open();
    portal_aio_read(buffer, 11, 0);
    spawn();

	    printf("Wait\n");

    portal_aio_wait();
    
        printf("Msg: %s\n", buffer);

    portal_close();

        printf("Join\n");

    join();

        printf("Goodbye\n");

	return 0;
};

// ====== Portal functions ======

void portal_open(void)
{
    assert(mppa_noc_dnoc_rx_alloc(0, portal_rx) == MPPA_NOC_RET_SUCCESS);
}

void portal_close(void)
{
    mppa_noc_dnoc_rx_free(0, portal_rx);
}

void portal_aio_read(char * buffer, int size, int offset)
{
    mppa_noc_dnoc_rx_configuration_t rx_configuration = {
        .buffer_base = (uintptr_t) buffer,
        .buffer_size = size,
        .current_offset = offset,
        .item_reload = 0,
        .item_counter = size,
        .event_counter = 0,
//      .reload_mode = MPPA_NOC_RX_RELOAD_MODE_INCR_DATA_NOTIF,     //! Increment item and event counter
        .reload_mode = MPPA_NOC_RX_RELOAD_MODE_DECR_DATA_NO_RELOAD, //! Decrement item, when 0 is reached, generate an event
        .activation = MPPA_NOC_ACTIVATED,
        .counter_id = 0
    };

    assert(mppa_noc_dnoc_rx_configure(0, portal_rx, rx_configuration) == 0);

    mppa_noc_dnoc_rx_lac_event_counter(0, portal_rx); //! Clean events register etc...
}

void portal_aio_wait(void)
{
    // int event = 0, item = 0;
    // while(event == 0 || item < 11)
    // {
    //     Use with while, busy wait
    //     event = mppa_noc_dnoc_rx_get_event_counter(0, portal_rx); 
    //     item = mppa_noc_dnoc_rx_get_item_counter(0, portal_rx);
    // }

    int event = mppa_noc_wait_clear_event(0, MPPA_NOC_INTERRUPT_LINE_DNOC_RX, portal_rx);
    printf("events counter: %d\n", event);

    mppa_noc_dnoc_rx_lac_event_counter(0, portal_rx);
    mppa_noc_dnoc_rx_lac_item_counter(0, portal_rx);
}

// ====== Spawn functions ======

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
		pids[i] = mppa_power_base_spawn(i, "slave", (const char **)args, NULL, MPPA_POWER_SHUFFLING_ENABLED);
		assert(pids[i] != -1);
	}
}

void join(void)
{
    int i, ret;
	for (i = 1; i < 3; i++)
		mppa_power_base_waitpid(i, &ret, 0);
}