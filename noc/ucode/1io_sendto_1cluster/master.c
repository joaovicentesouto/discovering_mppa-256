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

void func(int interface_out, int id, int target_tag, int target_cluster, int tx_tag, int uc_tag)
{
	mppa_dnoc_channel_config_t config_tx = { 0 };
	mppa_dnoc_header_t header = { 0 };
	header._.valid = 1;
	header._.tag = target_tag;
  	MPPA_NOC_DNOC_TX_CONFIG_INITIALIZER_DEFAULT(config_tx, 0);
	config_tx ._.payload_max = 32; // for better performances

    if (mppa_routing_get_dnoc_unicast_route(id, target_cluster, &config_tx, &header) != 0)
        return -1;

	if (mppa_noc_dnoc_tx_configure(interface_out, tx_tag, header, config_tx) != 0)
        return -1;

	mppa_noc_dnoc_uc_configuration_t uc_configuration;
	memset(&uc_configuration, 0, sizeof(uc_configuration));

	uc_configuration.program_start = (uintptr_t) mppa_noc_linear_firmware_eot_event;
	uc_configuration.buffer_base = (uintptr_t) buffer;
	uc_configuration.buffer_size = sizeof(buffer);

	mppa_noc_dnoc_uc_set_linear_params(&uc_configuration, sizeof(buffer), 0, 0);	
	
    if (mppa_noc_dnoc_uc_configure(interface_out, uc_tag, uc_configuration) != 0)
        return -1;

	mppa_noc_uc_program_run_t program_run;
	memset(&program_run, 0, sizeof(mppa_noc_uc_program_run_t));
	program_run.semaphore = 1;
	program_run.activation = 1;

	mppa_noc_dnoc_uc_set_program_run(interface_out, uc_tag, program_run);

    return (0);
}

void wait_(int interface_out, int uc_tag)
{
    printf("Done %u\n", mppa_noc_wait_clear_event(interface_out, MPPA_NOC_INTERRUPT_LINE_DNOC_TX, uc_tag));
}

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    printf("====== NoC Micro-code: 1 IO to 1 Cluster ======\n");
    
    int id = 128;
    int interface = 0;
    int interface_out = 0;
    int tag_in = 7;
    int target_tag = 7;
    int target_cluster = 0;
    
    char buffer[] = "Esta mensagem deve ser grande, ou seja, maior que 32 do payload... espero que funcione!\0";

    printf("Alloc and config Portals, buffer size: %d\n", sizeof(buffer));

    cnoc_rx_alloc(interface, tag_in);
    cnoc_rx_config(interface, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    
    //! ====================== UC Alloc ======================
    int tx = dnoc_tx_alloc_auto(interface_out);
    int uc = dnoc_uc_alloc_auto(interface_out);
    uc = dnoc_uc_alloc_auto(interface_out);
    dnoc_uc_free(interface_out, 0);
    //! ==================== UC END Alloc ====================

    spawn();

    printf("Wait signal\n");

    cnoc_rx_wait(interface, tag_in);

    //! ====================== UC Config ======================

    // Configure the TX (without mOS)

	// mppa_dnoc_channel_config_t config_tx = { 0 };
	// mppa_dnoc_header_t header = { 0 };
	// header._.valid = 1;
	// header._.tag = target_tag;
  	// MPPA_NOC_DNOC_TX_CONFIG_INITIALIZER_DEFAULT(config_tx, 0);
	// config_tx ._.payload_max = 32; // for better performances

    // if (mppa_routing_get_dnoc_unicast_route(id, target_cluster, &config_tx, &header) != 0) {
	//   printf("mppa_routing_get_dnoc_unicast_route failed\n");
	//   return -1;
	// }
			      
	// mppa_noc_dnoc_tx_configure(interface_out, tx_tag, header, config_tx);


    // // Configure the UC (without mOS)

	// mppa_noc_dnoc_uc_configuration_t uc_configuration;
	// memset(&uc_configuration, 0, sizeof(uc_configuration));

	// uc_configuration.program_start = (uintptr_t) mppa_noc_linear_firmware_eot_event;
	// uc_configuration.buffer_base = (uintptr_t) buffer;
	// uc_configuration.buffer_size = sizeof(buffer);

	// mppa_noc_dnoc_uc_set_linear_params(&uc_configuration, sizeof(buffer), 0, 0);
	
	// mppa_noc_dnoc_uc_configure(interface_out, uc_tag, uc_configuration);


    // // Run the uc program


	// mppa_noc_uc_program_run_t program_run;
	// memset(&program_run, 0, sizeof(mppa_noc_uc_program_run_t));
	// program_run.semaphore = 1;
	// program_run.activation = 1;
	// mppa_noc_dnoc_uc_set_program_run(interface_out, uc_tag, program_run);

    // dnoc_tx_uc_config_and_write(interface_out, tx_tag, uc_tag, id, target_tag, target_cluster, buffer);

	printf("IO: Sent data via microprogram\n");

    //! ==================== UC END Config ====================

    printf("Wait: send to cluster %d: %s\n", sizeof(buffer), buffer);

    func(interface_out, id, target_tag, target_cluster, tx, uc);
    wait_(interface_out, uc);

    // printf("Done %u\n", mppa_noc_wait_clear_event(interface_out, MPPA_NOC_INTERRUPT_LINE_DNOC_TX, uc_tag));

    join();

    dnoc_tx_free(interface_out, tx);
    dnoc_uc_free(interface_out, uc);

    printf("Goodbye\n");

	return 0;
};
