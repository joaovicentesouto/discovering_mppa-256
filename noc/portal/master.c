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

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    printf(" ====== NoC: Portal 1 ======\n");
    
    int interface = 0;
    int tag = 7;
    int size = 11;
    int offset = 0;
    char buffer[size];
    memset(buffer, 0, size);
    
    printf("Alloc and config Portal\n");

    dnoc_rx_alloc(interface, tag);
    dnoc_rx_config(interface, tag, buffer, size, offset);

    spawn();

    printf("Wait\n");

    dnoc_rx_wait(interface, tag);
    
    printf("Msg: %s\n", buffer);

    dnoc_rx_free(interface, tag);

    printf("Done\n");

    join();

    printf("Goodbye\n");

	return 0;
};
