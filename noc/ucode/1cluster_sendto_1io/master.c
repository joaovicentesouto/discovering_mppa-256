// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>
#include <noc.h>

#define MASK ~0x1

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    printf("====== NoC Micro-code: 1 IO to 1 Cluster ======\n");
    
    int interface_in = 0;
    int tag_in = 7;
    
    int buffer_size = 89;
    char buffer[buffer_size];

    printf("IO#: Alloc and config Portals\n");

    dnoc_rx_alloc(interface_in, tag_in);
    dnoc_rx_config(interface_in, tag_in, buffer, buffer_size, 0);

    spawn();
    
    printf("Recive Msg\n");
    
    dnoc_rx_wait(interface_in, tag_in, buffer_size);
    
    printf("Msg: %s\n", buffer);

    join();

    dnoc_rx_free(interface_in, tag_in);

    printf("Goodbye\n");

	return 0;
};
