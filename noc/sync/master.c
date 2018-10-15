
// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>

//! Sync section
#define MASK ~0x3F

void init(int tag_rx);
void sync(int tag_rx);
void end(int tag_rx);

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
        printf(" ====== NoC: Sync 1 ======\n");
        printf(" Open Sync\n");

    init(16);
    spawn();

	    printf("Wait 0x%x\n", MASK);

    sync(16);
    
        printf("Sync\n");

    end(16);

        printf("End Sync\n");

    join();

        printf("Goodbye\n");

	return 0;
};

//! ================ Functions ================

void init(int tag_rx)
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

void end(int tag_rx)
{
    mppa_noc_cnoc_rx_free(0, tag_rx);
}
