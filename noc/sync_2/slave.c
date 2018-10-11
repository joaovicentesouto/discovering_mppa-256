// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

//! SYNC

#define MASK ~0x0
static int sync_in;
static int sync_out;

void init_sync(int source_cluster)
{
    //! Notification RX
    //! Alloc buffer
    sync_in = 16;
    assert(mppa_noc_cnoc_rx_alloc(0, sync_in) == MPPA_NOC_RET_SUCCESS);

    mppa_cnoc_mailbox_notif_t notif;
    memset(&notif, 0, sizeof(mppa_cnoc_mailbox_notif_t));
    notif._.enable = 1;
    notif._.evt_en = 1;
    notif._.rm = 16; //! RM on Compute cluster

    //! Configuration
    mppa_noc_cnoc_rx_configuration_t config_rx = { 0 };
    config_rx.mode = MPPA_NOC_CNOC_RX_BARRIER;
    config_rx.init_value = MASK;

    assert(mppa_noc_cnoc_rx_configure(0, sync_in, config_rx, &notif) == 0);

    //! Route TX
    unsigned aux = 0;

    //! Alloc buffer
    assert(mppa_noc_cnoc_tx_alloc_auto(0, &aux, MPPA_NOC_BLOCKING) == MPPA_NOC_RET_SUCCESS);
    sync_out = aux;

    mppa_cnoc_config_t config = { 0 };
    mppa_cnoc_header_t header = { 0 };

    int target_tag = 16;
    int target_cluster = 128;

    mppa_routing_get_cnoc_unicast_route(source_cluster, target_cluster, &config, &header);
    header._.tag = target_tag;

    mppa_noc_cnoc_tx_configure(0, sync_out, config, header);
}

void mppa_wait(void)
{
    mppa_noc_wait_clear_event(0, MPPA_NOC_INTERRUPT_LINE_CNOC_RX, sync_in);

    //! Retrigger
    {
        //! Notification
        mppa_cnoc_mailbox_notif_t notif;
        memset(&notif, 0, sizeof(mppa_cnoc_mailbox_notif_t));
        notif._.enable = 1;
        notif._.evt_en = 1;
        notif._.rm = 1 << __k1_get_cpu_id();

        //! Configuration
        mppa_noc_cnoc_rx_configuration_t config = { 0 };
        config.mode = MPPA_NOC_CNOC_RX_MAILBOX;
        config.init_value = MASK;
        
        assert(mppa_noc_cnoc_rx_configure(0, sync_in, config, &notif) == 0);
    }
}

void mppa_signal(uint64_t value)
{
    mppa_noc_cnoc_tx_push_eot(0, sync_out, value);
}

void end_sync(void)
{
    mppa_noc_cnoc_rx_free(0, sync_in);
    mppa_noc_cnoc_tx_free(0, sync_out);
}

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int id = __k1_get_cluster_id();
    // int id = atoi(argv[0]);

    printf("Start sync\n");

    init_sync(id);
    
    printf("Sync\n");

    uint64_t value = 1 << (id == 1 ? 0 : 1);

    mppa_signal(value);

    printf("Wait %jx\n", value);

    // while(true);

    mppa_wait();

    printf("End Sync\n");

    end_sync();

	printf("Goodbye\n");

	return 0;
}
