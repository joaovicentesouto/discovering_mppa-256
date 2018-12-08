// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <noc.h>

void func(int interface_out, int id, int target_tag, int target_cluster, int tx, int uc)
{
    char buffer[] = "Esta mensagem deve ser grande, ou seja, maior que 32 do payload... espero que funcione!\0";

    printf("C# Alloc and config Portals, buffer size: %d\n", sizeof(buffer));
    
    //! ====================== UC Alloc ======================
    // int tx_tag = dnoc_tx_alloc_auto(interface_out);
    // int uc_tag = dnoc_uc_alloc_auto(interface_out);
    // uc_tag = dnoc_uc_alloc_auto(interface_out);
    // dnoc_uc_free(interface_out, 0);
    int tx_tag = tx;
    int uc_tag = uc;
    //! ==================== UC END Alloc ====================

    printf("C# Config %d\n", uc_tag);

    //! ====================== UC Config ======================

    // Configure the TX (without mOS)

	mppa_dnoc_channel_config_t config_tx = { 0 };
	mppa_dnoc_header_t header = { 0 };
	header._.valid = 1;
	header._.tag = target_tag;
  	MPPA_NOC_DNOC_TX_CONFIG_INITIALIZER_DEFAULT(config_tx, 0);
	config_tx ._.payload_max = 32; // for better performances

    if (mppa_routing_get_dnoc_unicast_route(id, target_cluster, &config_tx, &header) != 0) {
	  printf("mppa_routing_get_dnoc_unicast_route failed\n");
	}
			      
	mppa_noc_dnoc_tx_configure(interface_out, tx_tag, header, config_tx);

    // Configure the UC (without mOS)

	mppa_noc_dnoc_uc_configuration_t uc_configuration;
	memset(&uc_configuration, 0, sizeof(uc_configuration));

	uc_configuration.program_start = (uintptr_t) mppa_noc_linear_firmware_eot_event;
	uc_configuration.buffer_base = (uintptr_t) buffer;
	uc_configuration.buffer_size = sizeof(buffer);

	mppa_noc_dnoc_uc_set_linear_params(&uc_configuration, sizeof(buffer), 0, 0);
	
	mppa_noc_dnoc_uc_configure(interface_out, uc_tag, uc_configuration, header, config_tx);


    // Run the uc program


	mppa_noc_uc_program_run_t program_run;
	memset(&program_run, 0, sizeof(mppa_noc_uc_program_run_t));
	program_run.semaphore = 1;
	program_run.activation = 1;

    if (mppa_noc_dnoc_uc_link(interface_out, uc_tag, tx_tag, uc_configuration) != 0)
        printf("C#: DEU RUIM\n");


	printf("C#: set microprogram\n");

	mppa_noc_dnoc_uc_set_program_run(interface_out, uc_tag, program_run);

    // dnoc_tx_uc_config_and_write(interface_out, tx_tag, uc_tag, id, target_tag, target_cluster, buffer);

	printf("C#: Sent data via microprogram\n");

    //! ==================== UC END Config ====================

    printf("C# Wait: send to cluster %d: %s\n", sizeof(buffer), buffer);

    // printf("Done %u\n", mppa_noc_wait_clear_event(interface_out, MPPA_NOC_INTERRUPT_LINE_DNOC_TX, uc_tag));

    // if (mppa_noc_dnoc_uc_unlink(interface_out, uc_tag, tx_tag) != 0)
    //     printf("C#: DEU RUIM\n");

    // dnoc_tx_free(interface_out, tx_tag);
    // dnoc_uc_free(interface_out, uc_tag);
}

void wait_(int interface_out, int tx_tag, int uc_tag)
{
    printf("Done %u\n", mppa_noc_wait_clear_event(interface_out, MPPA_NOC_INTERRUPT_LINE_DNOC_TX, uc_tag));

    if (mppa_noc_dnoc_uc_unlink(interface_out, uc_tag, tx_tag) != 0)
        printf("C#: DEU RUIM\n");
}

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int id = 0;
    int interface_out = 0;
    int target_tag = 7;
    int target_cluster = 128;

    // char buffer[] = "Esta mensagem deve ser grande, ou seja, maior que 32 do payload... espero que funcione!\0";

    // printf("C# Alloc and config Portals, buffer size: %d\n", sizeof(buffer));
    
    // //! ====================== UC Alloc ======================
    int tx = dnoc_tx_alloc_auto(interface_out);
    int uc = dnoc_uc_alloc_auto(interface_out);
    uc = dnoc_uc_alloc_auto(interface_out);
    dnoc_uc_free(interface_out, 0);
    // //! ==================== UC END Alloc ====================

    // printf("C# Config %d\n", uc_tag);

    // //! ====================== UC Config ======================

    // // Configure the TX (without mOS)

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
	
	// mppa_noc_dnoc_uc_configure(interface_out, uc_tag, uc_configuration, header, config_tx);


    // // Run the uc program


	// mppa_noc_uc_program_run_t program_run;
	// memset(&program_run, 0, sizeof(mppa_noc_uc_program_run_t));
	// program_run.semaphore = 1;
	// program_run.activation = 1;

    // if (mppa_noc_dnoc_uc_link(interface_out, uc_tag, tx_tag, uc_configuration) != 0)
    //     printf("C#: DEU RUIM\n");


	// printf("C#: set microprogram\n");

	// mppa_noc_dnoc_uc_set_program_run(interface_out, uc_tag, program_run);

    // // dnoc_tx_uc_config_and_write(interface_out, tx_tag, uc_tag, id, target_tag, target_cluster, buffer);

	// printf("C#: Sent data via microprogram\n");

    // //! ==================== UC END Config ====================

    // printf("C# Wait: send to cluster %d: %s\n", sizeof(buffer), buffer);

    // printf("Done %u\n", mppa_noc_wait_clear_event(interface_out, MPPA_NOC_INTERRUPT_LINE_DNOC_TX, uc_tag));

    // if (mppa_noc_dnoc_uc_unlink(interface_out, uc_tag, tx_tag) != 0)
    //     printf("C#: DEU RUIM\n");

    // dnoc_tx_free(interface_out, tx_tag);
    // dnoc_uc_free(interface_out, uc_tag);

    // int tx = 0, uc = 0;
    func(interface_out, id, target_tag, target_cluster, tx, uc);
    wait_(interface_out, tx, uc);

    dnoc_tx_free(interface_out, tx);
    dnoc_uc_free(interface_out, uc);

    printf("Goodbye\n");

	return 0;
}
