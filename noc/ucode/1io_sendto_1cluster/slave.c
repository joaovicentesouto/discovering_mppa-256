// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <noc.h>

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int interface_in = 0;
    int interface_out = 0;
    int tag_in = 7;
    int target_tag = 7;
    int target_cluster = 128;

    int id = __k1_get_cluster_id();
    int buffer_size = 28;
    char buffer[buffer_size];

    printf("C#: Alloc and config Portals\n");

    dnoc_rx_alloc(interface_in, tag_in);
    dnoc_rx_config(interface_in, tag_in, buffer, buffer_size, 0);

    int tag_out = cnoc_tx_alloc_auto(interface_out);
    cnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);

    printf("Signal\n");

    cnoc_tx_write(interface_out, tag_out, 0x1);
    
    printf("Recive Msg\n");
    
    dnoc_rx_wait(interface_in, tag_in);
    
    printf("Msg: %s\n", buffer);

    cnoc_tx_free(interface_out, tag_out);
    dnoc_rx_free(interface_in, tag_in);

    printf("Goodbye\n");

	return 0;
}
