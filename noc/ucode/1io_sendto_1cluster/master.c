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
    
    int id = 128;
    int interface = 0;
    int interface_out = 0;
    int tag_in = 7;
    int target_tag = 7;
    int target_cluster = 0;
    
    char buffer[] = "Parte1*Parte2*Parte3*Parte4\0";

    printf("Alloc and config Portals\n");

    cnoc_rx_alloc(interface, tag_in);
    cnoc_rx_config(interface, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    
    //! ====================== UC Alloc ======================
    int tx_tag = dnoc_tx_alloc_auto(interface_out);
    int uc_tag = dnoc_uc_alloc_auto(interface_out);
    //! ==================== UC END Alloc ====================

    spawn();

    printf("Wait signal\n");

    cnoc_rx_wait(interface, tag_in);
    

    int i = 0;
    printf("Config %d\n", i++);
    //! ====================== UC Config ======================
    // dnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);

    mppa_noc_dnoc_uc_configuration_t uc_config = MPPA_NOC_DNOC_UC_CONFIGURATION_INIT;
    uc_config.program_start = 0;

    printf("Config %d\n", i++);
    mppa_noc_dnoc_uc_set_linear_params(&uc_config, 28, 0, 0);
    uc_config.buffer_base = (uintptr_t) buffer;

    printf("Config %d\n", i++);
    mppa_noc_dnoc_uc_configure(interface_out, uc_tag, uc_config);

    mppa_dnoc_channel_config_t config = { 0 };
    mppa_dnoc_header_t header = { 0 };
    header._.tag = target_tag;
    header._.valid = 1;

    MPPA_NOC_DNOC_TX_CONFIG_INITIALIZER_DEFAULT(config, 0);

    printf("Config %d\n", i++);
    assert(mppa_routing_get_dnoc_unicast_route(id, target_cluster, &config, &header) == 0);
    
    printf("Config %d\n", i++);
    assert(mppa_noc_dnoc_tx_configure(interface_out, tx_tag, header, config) == 0); //! == MPPA_NOC_RET_SUCCESS

    mppa_noc_uc_program_run_t program_run = {{ 0 }};
    program_run.activation = 1;
    program_run.semaphore = 1; // Set at least one semaphore.

    //! ==================== UC END Config ====================

    // dnoc_tx_write(interface_out, tag_out, buffer, 4, 0);


    printf("Config %d\n", i++);
    //! ====================== UC Run program ======================
    mppa_noc_dnoc_uc_set_program_run(interface_out, tx_tag, program_run);

    printf("Wait: send to cluster: %s\n", buffer);
    mppa_noc_wait_clear_event(interface_out, MPPA_NOC_INTERRUPT_LINE_DNOC_TX, uc_tag);


    cnoc_rx_free(interface, tag_in);
    dnoc_tx_free(interface_out, tx_tag);
    dnoc_uc_free(interface_out, uc_tag);

    printf("Done\n");

    join();

    printf("Goodbye\n");

	return 0;
};
