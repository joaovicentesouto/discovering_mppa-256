// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

//! Spawn section
#define NUM_CLUSTERS 16
static mppa_power_pid_t pids[NUM_CLUSTERS];

void spawn(void);
void join(void);

//! Portal section
#define MASK ~0x3
static int portal_tx, sync_rx;

void portal_open(void);
void portal_wait(void);
void portal_write(int target_cluster, char * buffer, int size, int offset);
void portal_close(void);

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    char buff_1[4];
    char buff_2[7];

        printf(" ====== NoC: Portal 2 ======\n");
        printf(" Open Portals\n");

    portal_open();
    spawn();

	    printf("Wait signal\n");

    portal_wait();
    
        printf("Send\n");

    sprintf(buff_1, "C_1\0");
    portal_write(1, buff_1, 4, 0);

    sprintf(buff_2, "C____2\0");
    portal_write(2, buff_2, 7, 0);

    portal_close();

        printf("Join\n");

    join();

        printf("Goodbye\n");

	return 0;
};

// ====== Portal functions ======

void portal_open(void)
{
    //! portal
    unsigned aux;
    assert(mppa_noc_dnoc_tx_alloc_auto(0, &aux, MPPA_NOC_BLOCKING) == MPPA_NOC_RET_SUCCESS);
    portal_tx = aux;

    //! sync
    sync_rx = 16;
    assert(mppa_noc_cnoc_rx_alloc(0, sync_rx) == MPPA_NOC_RET_SUCCESS);
    
    mppa_cnoc_mailbox_notif_t notif;
    memset(&notif, 0, sizeof(mppa_cnoc_mailbox_notif_t));
    notif._.enable = 1;
    notif._.evt_en = 1;
    notif._.rm = 1 << __k1_get_cpu_id();

    //! Configuration
    mppa_noc_cnoc_rx_configuration_t config = { 0 };
    config.mode = MPPA_NOC_CNOC_RX_BARRIER;
    config.init_value = MASK;

    assert(mppa_noc_cnoc_rx_configure(0, sync_rx, config, &notif) == 0);
}

void portal_wait(void)
{
    mppa_noc_wait_clear_event(0, MPPA_NOC_INTERRUPT_LINE_CNOC_RX, sync_rx);
}

void portal_write(int target_cluster, char * buffer, int size, int offset)
{
    int source_cluster = 128;

    //! CONFIG SESSION

    mppa_dnoc_channel_config_t config = { 0 };
    mppa_dnoc_header_t header = { 0 };
    header._.tag = 7;
    header._.valid = 1;

    MPPA_NOC_DNOC_TX_CONFIG_INITIALIZER_DEFAULT(config, 0);

    mppa_dnoc_push_offset_t off;
    off._.offset = offset;
    off._.protocol = 0x1; //! absolute offset
    off._.valid = 1;

    assert(mppa_routing_get_dnoc_unicast_route(source_cluster, target_cluster, &config, &header) == 0);

    //! SEND SESSION

    mppa_noc_dnoc_tx_flush(0, portal_tx);
    __builtin_k1_fence(); //! Wait

    mppa_noc_dnoc_tx_configure(0, portal_tx, header, config);

    mppa_noc_dnoc_tx_set_push_offset(0, portal_tx, off);
    mppa_noc_dnoc_tx_send_data(0, portal_tx, size, buffer);
    mppa_noc_dnoc_tx_flush_eot(0, portal_tx);
}

void portal_close(void)
{
    mppa_noc_dnoc_tx_free(0, portal_tx);
    mppa_noc_dnoc_rx_free(0, sync_rx);
}

// ====== Spawn functions ======

void spawn(void)
{
    int i;
	char arg0[4];
	char *args[2];

	/* Spawn slaves. */
	args[1] = NULL;
	for (i = 1; i < 3; i++)
	{	
		sprintf(arg0, "%d", i);
		args[0] = arg0;
		pids[i] = mppa_power_base_spawn(i, "slave", (const char **)args, NULL, MPPA_POWER_SHUFFLING_ENABLED);
		assert(pids[i] != -1);
	}
}

void join(void)
{
    int i, ret;
	for (i = 1; i < 3; i++)
		mppa_power_base_waitpid(i, &ret, 0);
}