
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
#include <noc.h>

#define MASK ~0x3F

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
    printf("====== NoC Sync: 1 IO wait 2 Clusters ======\n");
    
    int interface = 0;
    int tag = 16;
    
    printf("Alloc and config Sync\n");

    cnoc_rx_alloc(interface, tag);
    cnoc_rx_config(interface, tag, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    spawn();

    printf("Wait\n");

    cnoc_rx_wait(interface, tag);
    
    printf("Done\n");

    cnoc_rx_free(interface, tag);

    printf("Join\n");

    join();

    printf("Goodbye\n");

	return 0;
};
