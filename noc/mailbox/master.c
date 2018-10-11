// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

//! Spawn section
#define NUM_CLUSTERS 16
static mppa_power_pid_t pids[NUM_CLUSTERS];

void spawn(void);
void join(void);

//! Mailbox section
static int mailbox_rx = 16;

void init();
void recive_mailbox(uint64_t * mailbox);
void end();

//! Main
int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
        printf(" ====== NoC: Mailbox ======\n");
        printf(" Init Mailbox\n");

    init();
    spawn();
    
        printf("Recive mailbox\n");

    uint64_t mailbox;
    recive_mailbox(&mailbox);

        printf("Msg: %jx\n", mailbox);

    end();

        printf("Join\n");

    join();

        printf("Goodbye\n");

	return 0;
};

// ====== Mailbox functions ======

void init()
{
    //! ============= MAILBOX =============
    assert(mppa_noc_cnoc_rx_alloc(0, mailbox_rx) == 0);
    
    //! Notification
    mppa_cnoc_mailbox_notif_t notif;
    memset(&notif, 0, sizeof(mppa_cnoc_mailbox_notif_t));
    notif._.enable = 1;
    notif._.evt_en = 1;
    notif._.rm = 1 << __k1_get_cpu_id();

    //! Configuration
    mppa_noc_cnoc_rx_configuration_t config = { 0 };
    config.mode = MPPA_NOC_CNOC_RX_MAILBOX;
    config.init_value = 0;

    assert(mppa_noc_cnoc_rx_configure(0, mailbox_rx, config, &notif) == 0);
}

void end()
{
    mppa_noc_cnoc_rx_free(0, mailbox_rx);
}

void recive_mailbox(uint64_t * mailbox)
{
    mppa_noc_wait_clear_event(0, MPPA_NOC_INTERRUPT_LINE_CNOC_RX, mailbox_rx);
    *mailbox = mppa_noc_cnoc_rx_get_value(0, mailbox_rx);

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
    config.init_value = 0;

    assert(mppa_noc_cnoc_rx_configure(0, mailbox_rx, config, &notif) == 0);
}

// ====== Spawn functions ======

void spawn(void)
{
    int i;
	char arg0[4];
	char *args[2];

	/* Spawn slaves. */
	args[1] = NULL;
	for (i = 0; i < 1; i++)
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
	for (i = 0; i < 1; i++)
		mppa_power_base_waitpid(i, &ret, 0);
}