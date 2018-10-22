// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>
#include <utask.h>

#include <spawn.h>
#include <noc.h>

void another_core_function(void);

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
    printf("====== NoC Mailbox: 1 Cluster to 2 IO ======\n");
    
    utask_t t;
	utask_create(&t, NULL, (void*)another_core_function, NULL);

    int interface = 0;
    int tag = 7;
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

    utask_join(t, NULL);

    printf("Goodbye\n");

	return 0;
};

void another_core_function(void)
{
    printf("====== 1 NoC: Mailbox: Diff Interface Test ======\n");
    
    int interface = 1;
    int tag = 16;
    uint64_t init_value = 0;

    printf("1 Alloc and config Mailbox\n");

    cnoc_rx_alloc(interface, tag);
    cnoc_rx_config(interface, tag, MPPA_NOC_CNOC_RX_MAILBOX, init_value);

    // spawn();
    
    printf("1 Recive mailbox\n");

    cnoc_rx_wait(interface, tag);
    uint64_t mailbox = cnoc_rx_read(interface, tag);

    printf("1 Msg: %jx\n", mailbox);

    cnoc_rx_free(interface, tag);

    printf("1 Done\n");

    // join();

    printf("1 Goodbye\n");
}