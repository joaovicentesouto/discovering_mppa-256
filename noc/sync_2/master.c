
// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>

//! Sync section
#define MASK ~0x3
static int sync_in;
static int sync_out;

void init(void);
void wait_signal(void);
void send_signal(void);
void end(void);

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
        printf(" ====== NoC: Sync 2 ======\n");
        printf(" Open Sync\n");

    init();
    spawn();

	    printf("Wait\n");

    wait_signal();
    
        printf("Sync\n");

    send_signal();
    end();

        printf("Join\n");

    join();

        printf("Goodbye\n");

	return 0;
};

//! ================ Functions ================

void init(void)
{   
    //! Notification RX
    {
        //! Alloc buffer
        sync_in = 16;
        assert(mppa_noc_cnoc_rx_alloc(0, sync_in) == MPPA_NOC_RET_SUCCESS);
        
        mppa_cnoc_mailbox_notif_t notif;
        memset(&notif, 0, sizeof(mppa_cnoc_mailbox_notif_t));
        notif._.enable = 1;
        notif._.evt_en = 1;
        notif._.rm = 1 << __k1_get_cpu_id();

        //! Configuration
        mppa_noc_cnoc_rx_configuration_t config = { 0 };
        config.mode = MPPA_NOC_CNOC_RX_BARRIER;
        config.init_value = MASK;

        assert(mppa_noc_cnoc_rx_configure(0, sync_in, config, &notif) == 0);
    }

    //! Route TX
    {
        unsigned aux = 0;
        
        assert(mppa_noc_cnoc_tx_alloc_auto(0, &aux, MPPA_NOC_BLOCKING) == MPPA_NOC_RET_SUCCESS);
        sync_out = aux;
    
        // mppa_cnoc_config_t config = { 0 };
        // mppa_cnoc_header_t header = { 0 };
        // mppa_cnoc_config_t config_2 = { 0 };
        // mppa_cnoc_header_t header_2 = { 0 };

        // int target_tag = 16;
        // int source_cluster = 128;
        // mppa_node_t target_clusters[2] = {1, 2};

        // mppa_routing_get_cnoc_multicast_route(
        //     source_cluster, target_clusters, 2,
        //     &config, &header,
        //     &config_2, &header_2
        // );

        // header._.tag = target_tag;

        // mppa_noc_cnoc_tx_configure(0, sync_out, config, header);
    }
}

void wait_signal(void)
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

void send_signal(void)
{
    //! Route TX
    int i;
    for (i = 1; i < 3; i++) {
        mppa_cnoc_config_t config = { 0 };
        mppa_cnoc_header_t header = { 0 };

        int target_tag = 16;
        int target_cluster = i;

        mppa_routing_get_cnoc_unicast_route(128, target_cluster, &config, &header);
        header._.tag = target_tag;

        mppa_noc_cnoc_tx_configure(0, sync_out, config, header);

        uint64_t value = ~0;
        mppa_noc_cnoc_tx_push_eot(0, sync_out, value);
    }
}

void end(void)
{
    mppa_noc_cnoc_rx_free(0, sync_in);
    mppa_noc_cnoc_tx_free(0, sync_out);
}
