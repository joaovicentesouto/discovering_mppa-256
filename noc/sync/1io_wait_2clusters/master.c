
// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>

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
    
    printf(" ** LOCK 3 ** \n");

    cnoc_rx_alloc(interface, tag+0);
    cnoc_rx_alloc(interface, tag+1);
    cnoc_rx_alloc(interface, tag+2);
    cnoc_rx_alloc(interface, tag+3);
    cnoc_rx_alloc(interface, tag+4);

    cnoc_rx_config(interface, tag+0, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    cnoc_rx_config(interface, tag+1, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    cnoc_rx_config(interface, tag+2, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    cnoc_rx_config(interface, tag+3, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    cnoc_rx_config(interface, tag+4, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    spawn();

    printf("Wait 0\n");
    cnoc_rx_wait(interface, tag+0);
    printf("Wait 1\n");
    cnoc_rx_wait(interface, tag+1);
    printf("Wait 2\n");
    cnoc_rx_wait(interface, tag+2);
    printf("Wait 3\n");
    cnoc_rx_wait(interface, tag+3);
    printf("Wait 4\n");
    cnoc_rx_wait(interface, tag+4);
    printf("Finish\n");

    cnoc_rx_free(interface, tag+0);
    cnoc_rx_free(interface, tag+1);
    cnoc_rx_free(interface, tag+2);
    cnoc_rx_free(interface, tag+3);
    cnoc_rx_free(interface, tag+4);

    printf("Join\n");

    join();

    printf("Goodbye\n");

	return 0;
};
