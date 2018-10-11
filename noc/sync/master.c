
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

//! Sync section
#define MASK ~0x3F

void init_sync(int tag_rx);
void sync(int tag_rx);
void end_sync(int tag_rx);

//! Main
int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
        printf(" ====== NoC: Sync 1 ======\n");
        printf(" Open Sync\n");

    init_sync(16);
    spawn();

	    printf("Wait\n");

    sync(16);
    
        printf("Sync\n");

    end_sync(16);

        printf("End Sync\n");

    join();

        printf("Goodbye\n");

	return 0;
};

// ====== Sync functions ======

void init_sync(int tag_rx)
{
    //! Alloc rx buffer
    assert(mppa_noc_cnoc_rx_alloc(0, tag_rx) == 0);
    
    //! Notification
    mppa_cnoc_mailbox_notif_t notif;
    memset(&notif, 0, sizeof(mppa_cnoc_mailbox_notif_t));
    notif._.enable = 1;
    notif._.evt_en = 1;
    notif._.rm = 1 << __k1_get_cpu_id();

    //! Configuration
    mppa_noc_cnoc_rx_configuration_t config = { 0 };
    config.mode = MPPA_NOC_CNOC_RX_BARRIER;
    config.init_value = MASK;

    assert(mppa_noc_cnoc_rx_configure(0, tag_rx, config, &notif) == 0);
}

void sync(int tag_rx)
{
    mppa_noc_wait_clear_event(0, MPPA_NOC_INTERRUPT_LINE_CNOC_RX, tag_rx);

    //! Retrigger

    //! Notification
    mppa_cnoc_mailbox_notif_t notif;
    memset(&notif, 0, sizeof(mppa_cnoc_mailbox_notif_t));
    notif._.enable = 1;
    notif._.evt_en = 1;
    notif._.rm = 1 << __k1_get_cpu_id();

    //! Configuration
    mppa_noc_cnoc_rx_configuration_t config = { 0 };
    config.mode = MPPA_NOC_CNOC_RX_BARRIER;
    config.init_value = ~0x3F;

    assert(mppa_noc_cnoc_rx_configure(0, tag_rx, config, &notif) == 0);
}

void end_sync(int tag_rx)
{
    mppa_noc_cnoc_rx_free(0, tag_rx);
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