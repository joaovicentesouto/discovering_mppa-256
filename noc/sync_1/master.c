
#include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

//! SPAWN

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
		pids[i] = mppa_power_base_spawn(i, "cluster_bin", (const char **)args, NULL, MPPA_POWER_SHUFFLING_ENABLED);
		assert(pids[i] != -1);
	}
}

void join(void)
{
    int i, ret;
	for (i = 1; i < 3; i++)
		mppa_power_base_waitpid(i, &ret, 0);
}

//! SYNC

#define MASK ~0x3F
// static int sync_fd;

void init_sync(int rx_id)
{
    //! Alloc rx buffer
    assert(mppa_noc_cnoc_rx_alloc(0, rx_id) == 0);
    
    //! Notification
    mppa_cnoc_mailbox_notif_t notif;
    memset(&notif, 0, sizeof(mppa_cnoc_mailbox_notif_t));
    notif._.enable = 1;
    notif._.evt_en = 1;
    notif._.rm = 1 << __k1_get_cpu_id();

    //! Configuration
    mppa_noc_cnoc_rx_configuration_t config = { 0 };
    config.mode = MPPA_NOC_CNOC_RX_MAILBOX;
    config.init_value = ~0x3F;

    assert(mppa_noc_cnoc_rx_configure(0, rx_id, config, &notif) == 0);
}

void sync(int rx_id)
{
    mppa_noc_wait_clear_event(0, MPPA_NOC_INTERRUPT_LINE_CNOC_RX, rx_id);

    //! Retrigger

    //! Notification
    mppa_cnoc_mailbox_notif_t notif;
    memset(&notif, 0, sizeof(mppa_cnoc_mailbox_notif_t));
    notif._.enable = 1;
    notif._.evt_en = 1;
    notif._.rm = 1 << __k1_get_cpu_id();

    //! Configuration
    mppa_noc_cnoc_rx_configuration_t config = { 0 };
    config.mode = MPPA_NOC_CNOC_RX_MAILBOX;
    config.init_value = ~0x3F;

    assert(mppa_noc_cnoc_rx_configure(0, rx_id, config, &notif) == 0);
}

void end_sync(int rx_id)
{
    mppa_noc_cnoc_rx_free(0, rx_id);
}

//! MAIN

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
    printf("[IODDR0] MASTER: Start sync\n");

    init_sync(16);

    spawn();

	printf("[IODDR0] MASTER: Wait\n");

    sync(16);
    
    printf("[IODDR0] MASTER: Sync\n");

    end_sync(16);

    printf("[IODDR0] MASTER: End Sync\n");

    join();

    printf("[IODDR0] Goodbye\n");

	return 0;
};
