// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#define MASK_0 0xF
#define MASK_1 0x10
#define MASK_2 0x20

static int portal_rx, sync_tx;

void portal_open(int source_cluster)
{
    //! Portal
    portal_rx = 7;
    assert(mppa_noc_dnoc_rx_alloc(0, portal_rx) == MPPA_NOC_RET_SUCCESS);

    //! Sync
    unsigned aux = 0;

    //! Alloc buffer
    assert(mppa_noc_cnoc_tx_alloc_auto(0, &aux, MPPA_NOC_BLOCKING) == MPPA_NOC_RET_SUCCESS);
    sync_tx = aux;

    mppa_cnoc_config_t config = { 0 };
    mppa_cnoc_header_t header = { 0 };

    int target_tag = 16;
    int target_cluster = 128;

    mppa_routing_get_cnoc_unicast_route(source_cluster, target_cluster, &config, &header);
    header._.tag = target_tag;

    mppa_noc_cnoc_tx_configure(0, sync_tx, config, header);
}

void portal_signal(uint64_t mask)
{
    mppa_noc_cnoc_tx_push_eot(0, sync_tx, mask);
}

void portal_read(char * buffer, int size, int offset)
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

void portal_wait()
{
    int event = mppa_noc_wait_clear_event(0, MPPA_NOC_INTERRUPT_LINE_DNOC_RX, portal_rx);
    printf("[IODDR0] ..: events counter: %d\n", event);

    mppa_noc_dnoc_rx_lac_event_counter(0, portal_rx);
    mppa_noc_dnoc_rx_lac_item_counter(0, portal_rx);
}

void portal_close(void)
{
    mppa_noc_dnoc_rx_free(0, portal_rx);
    mppa_noc_dnoc_tx_free(0, sync_tx);
}

int main(__attribute__((unused)) int argc, const char **argv)
{
    // int id = __k1_get_cluster_id();
    int id = atoi(argv[0]);
    int offset = id == 1 ? 0 : 1;
    int buffer_size = id == 1 ? 4 : 7;

    printf("[IODDR0] Cluster %d: Start portal\n", id);

    portal_open(id);

    printf("[IODDR0] Cluster %d: send\n", id);

    portal_signal(1 << offset);
    
    printf("[IODDR0] Cluster %d: Sync\n", id);

    char buffer[buffer_size];
    portal_read(buffer, buffer_size, 0);
    portal_wait();
    
    printf("[IODDR0] Cluster %d: B: %s\n", id, buffer);

    printf("[IODDR0] Cluster %d: End Sync\n", id);

    portal_close();

	printf("[IODDR0] Cluster %d: Goodbye\n", id);

	return 0;
}
