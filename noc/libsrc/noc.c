#include <stdint.h>
#include <mppa_noc.h>
#include <mppa_routing.h>
#include <stdio.h>

//! Control NoC

void cnoc_rx_alloc(int interface, int tag)
{
    assert(mppa_noc_cnoc_rx_alloc(interface, tag) == 0);
}

void cnoc_rx_free(int interface, int tag)
{
    mppa_noc_cnoc_rx_free(interface, tag);
}

void cnoc_tx_alloc(int interface, int tag)
{
    assert(mppa_noc_cnoc_tx_alloc(interface, tag) == 0);
}

void cnoc_tx_free(int interface, int tag)
{
    mppa_noc_cnoc_tx_free(interface, tag);
}

unsigned cnoc_tx_alloc_auto(int interface)
{
    unsigned tag;
    mppa_noc_cnoc_tx_alloc_auto(interface, &tag, MPPA_NOC_BLOCKING);
    
    return tag;
}

void cnoc_rx_config(int interface, int tag, mppa_noc_cnoc_rx_mode_t mode, uint64_t value)
{
    //! Configuration
    mppa_noc_cnoc_rx_configuration_t config = { 0 };
    config.mode = mode;
    config.init_value = value;
    
    // //! Notification
    // mppa_cnoc_mailbox_notif_t notif;
    // memset(&notif, 0, sizeof(mppa_cnoc_mailbox_notif_t));
    // notif._.enable = 1;
    // notif._.evt_en = 1;

    // #ifdef _MASTER_
    //     notif._.rm = 1 << interface;
    // #else
    //     notif._.rm = 1;
    // #endif

    // printf("V: %jx  --- rm: %x\n", value, notif._.rm);

    assert(mppa_noc_cnoc_rx_configure(interface, tag, config, NULL) == 0);
}

void cnoc_tx_config(int interface, int source_tag, int source_cluster, int target_tag, int target_cluster)
{
    mppa_cnoc_config_t config = { 0 };
    mppa_cnoc_header_t header = { 0 };
    
    mppa_routing_get_cnoc_unicast_route(source_cluster, target_cluster, &config, &header);
    header._.tag = target_tag;

    assert(mppa_noc_cnoc_tx_configure(interface, source_tag, config, header) == 0);
}

void cnoc_rx_wait(int interface, int tag)
{
    mppa_noc_wait_clear_event(interface, MPPA_NOC_INTERRUPT_LINE_CNOC_RX, tag);   
}

uint64_t cnoc_rx_read(int interface, int tag)
{
    return mppa_noc_cnoc_rx_get_value(interface, tag);
}

void cnoc_tx_write(int interface, int tag, uint64_t value)
{
    mppa_noc_cnoc_tx_push_eot(interface, tag, value);
}

//! Data NoC

void dnoc_rx_alloc(int interface, int tag)
{
    assert(mppa_noc_dnoc_rx_alloc(interface, tag) == 0);
}

void dnoc_rx_free(int interface, int tag)
{
    mppa_noc_dnoc_rx_free(interface, tag);
}

void dnoc_tx_alloc(int interface, int tag)
{
    assert(mppa_noc_dnoc_tx_alloc(interface, tag) == 0);
}

void dnoc_tx_free(int interface, int tag)
{
    mppa_noc_dnoc_tx_free(interface, tag);
}

unsigned dnoc_tx_alloc_auto(int interface)
{
    unsigned tag;
    mppa_noc_dnoc_tx_alloc_auto(interface, &tag, MPPA_NOC_BLOCKING);
    
    return tag;
}

void dnoc_rx_config(int interface, int tag, char * buffer, int size, int offset)
{
    mppa_noc_dnoc_rx_configuration_t config = {
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

    assert(mppa_noc_dnoc_rx_configure(interface, tag, config) == 0);

    mppa_noc_dnoc_rx_lac_event_counter(interface, tag);
}

void dnoc_rx_wait(int interface, int tag)
{
    mppa_noc_wait_clear_event(interface, MPPA_NOC_INTERRUPT_LINE_DNOC_RX, tag);

    mppa_noc_dnoc_rx_lac_event_counter(interface, tag);
    mppa_noc_dnoc_rx_lac_item_counter(interface, tag);
}

void dnoc_tx_config(int interface, int tag, int source_cluster, int target_tag, int target_cluster)
{
    mppa_dnoc_channel_config_t config = { 0 };
    mppa_dnoc_header_t header = { 0 };
    header._.tag = target_tag;
    header._.valid = 1;

    MPPA_NOC_DNOC_TX_CONFIG_INITIALIZER_DEFAULT(config, 0);

    assert(mppa_routing_get_dnoc_unicast_route(source_cluster, target_cluster, &config, &header) == 0);
    assert(mppa_noc_dnoc_tx_configure(interface, tag, header, config) == 0); //! == MPPA_NOC_RET_SUCCESS
}

void dnoc_tx_write(int interface, int tag, char * buffer, int size, int offset)
{
    mppa_noc_dnoc_tx_flush(interface, tag);

    mppa_dnoc_push_offset_t off;
    off._.offset = offset;
    off._.protocol = 0x1; //! absolute offset
    off._.valid = 1;

    __builtin_k1_fence(); //! Wait flush
 
    mppa_noc_dnoc_tx_set_push_offset(interface, tag, off);
    mppa_noc_dnoc_tx_send_data(interface, tag, size, buffer);
    mppa_noc_dnoc_tx_flush_eot(interface, tag);
}
