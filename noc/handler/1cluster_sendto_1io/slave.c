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
    int interface_out = 0;
    int target_tag = 7;
    int target_cluster = 128;

    int id = __k1_get_cluster_id();
    
    char buffer[] = "Dummy message\0";
    char buffer2[] = "DUMMY MESSAGE\0";

    printf("Prepare to send\n");

    int tag_out = dnoc_tx_alloc_auto(interface_out);
    
    dnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);
    dnoc_tx_write(interface_out, tag_out, buffer, 7, 0);

    // dnoc_tx_free(interface_out, tag_out);

    sleep(5);

    dnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);
    dnoc_tx_write(interface_out, tag_out, buffer2+7, 7, 7);

    dnoc_tx_free(interface_out, tag_out);
    
    printf("Goodbye\n");

	return 0;
}
