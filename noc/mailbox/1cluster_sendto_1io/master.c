// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>
#include <noc.h>

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
    printf("====== NoC Mailbox: 1 Cluster to 1 IO ======\n");
    
    int interface = 0;
    int tag = 16;
    uint64_t init_value = 0;

    printf("Alloc and config Mailbox\n");

    cnoc_rx_alloc(interface, tag);
    cnoc_rx_config(interface, tag, MPPA_NOC_CNOC_RX_MAILBOX, init_value);

    spawn();
    
    printf("Recive mailbox\n");

    cnoc_rx_wait(interface, tag);
    uint64_t mailbox = cnoc_rx_read(interface, tag);

    printf("Msg: %jx\n", mailbox);

    cnoc_rx_free(interface, tag);

    printf("Done\n");

    join();

    printf("Goodbye\n");

	return 0;
};
