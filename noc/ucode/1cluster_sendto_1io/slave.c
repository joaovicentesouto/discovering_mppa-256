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
    char buffer[] = "My message CC!\0";

    printf("C# Alloc and config Portals\n");
    
    //! ====================== UC Alloc ======================
    int tx_tag = tx;
    int uc_tag = uc;
    //! ==================== UC END Alloc ====================

    //! ====================== UC Config ======================
	mppa_dnoc_channel_config_t config_tx = { 0 };
	mppa_dnoc_header_t header = { 0 };
	header._.valid = 1;
	header._.tag = target_tag;
  	MPPA_NOC_DNOC_TX_CONFIG_INITIALIZER_DEFAULT(config_tx, 0);
	config_tx ._.payload_max = 32; // for better performances

    if (mppa_routing_get_dnoc_unicast_route(id, target_cluster, &config_tx, &header) != 0) {
        printf("mppa_routing_get_dnoc_unicast_route failed\n");
        return;
	}
			      
	if (mppa_noc_dnoc_tx_configure(interface_out, tx_tag, header, config_tx) != 0)
    {
        printf("mppa_noc_dnoc_tx_configure failed\n");
        return;
    }

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
    {
        printf("C#: DEU RUIM\n");
        return;
    }

	mppa_noc_dnoc_uc_set_program_run(interface_out, uc_tag, program_run);

	printf("C#: Send to myself %s\n", buffer);
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
    int tag_in = 42;
    int interface = 0;

    char buffer_in[128];

    sprintf(buffer_in, "Not Works");

    dnoc_rx_alloc(interface, tag_in);
    dnoc_rx_config(interface, tag_in, buffer_in, 128, 0);
    
    // //! ====================== UC Alloc ======================
    int tx = dnoc_tx_alloc_auto(interface);
    int uc = dnoc_uc_alloc_auto(interface);
    // //! ==================== UC END Alloc ====================

    func(interface, id, tag_in, id, tx, uc);
    wait_(interface, tx, uc);

    printf("Wait to myself\n");

    dnoc_rx_wait(interface, tag_in, 128);

    printf("Receive from myself: %s\n", buffer_in);

    dnoc_tx_free(interface, tx);
    dnoc_uc_free(interface, uc);
    dnoc_rx_free(interface, tag_in);

    printf("Goodbye\n");

	return 0;
}
