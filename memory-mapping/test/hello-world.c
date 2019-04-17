#include "stdio.h"
#include "stdlib.h"
#include <inttypes.h>

#include <mOS_vcore_u.h>
#include <mppa_routing.h>
#include <mppa_noc.h>
#include <vbsp.h>

/**
 * @brief TLBs size.
 */
#ifdef __k1io__
	#define K1B_LTLB_LENGTH 16
#else
	#define K1B_LTLB_LENGTH 8
#endif
#define K1B_JTLB_LENGTH 128
#define K1B_JTLB_OFFSET 0
#define K1B_LTLB_OFFSET 256

/**
 * @brief TLB entry size (in bytes).
 */
#define K1B_TLBE_SIZE 8

/**
 * @brief TLB entry.
 */
struct tlbe
{
	unsigned status       :  2; /**< Entry Status (ES)          */
	unsigned cache_policy :  2; /**< Cache Policy (CP)          */
	unsigned protection   :  4; /**< Protection Attributes (PA) */
	unsigned addr_ext     :  4; /**< Address  Extension (AE)    */
	unsigned frame        : 20; /**< Frame Number (FN)          */
	unsigned addrspace    :  9; /**< Address Space Number (ANS) */
	unsigned              :  1; /**< Reserved                   */
	unsigned global       :  1; /**< Global Page Indicator (G)  */
	unsigned size         :  1; /**< Page Size (S)              */
	unsigned page         : 20; /**< Page Number (PN)           */
} __attribute__((packed));

/**
 * @brief Get vaddr.
 */
static inline uint32_t k1b_tlbe_vaddr_get(const struct tlbe *tlbe)
{
	unsigned pns = ((tlbe->page << 1) | tlbe->size);
	unsigned pn = pns >> (__builtin_k1_ctz(pns) + 1);
	return (pn << (__builtin_k1_ctz(pns) + 12));
}

/**
 * @brief Get paddr.
 */
static inline uint32_t k1b_tlbe_paddr_get(const struct tlbe *tlbe)
{
	return (tlbe->frame << 12);
}

/**
 * @brief Read a tlbe.
 */
static inline int k1b_tlbe_read(struct tlbe *tlbe, int idx)
{
	__k1_tlb_entry_t _tlbe;
	_tlbe = mOS_readtlb(idx);
	memcpy(tlbe, &_tlbe, K1B_TLBE_SIZE);
	return (0);
}

/**
 * @brief Print tlbs.
 */
void k1b_tlb_init(void)
{
	struct tlbe entry;

	/* Read JTLB into memory. */
	printf("JTLB:\n");
	for (int i = 0; i < K1B_JTLB_LENGTH; i++)
	{
		k1b_tlbe_read(&entry, K1B_JTLB_OFFSET + i);

		printf("TLB %d : vaddr: 0x%08"PRIx32" => paddr: 0x%08"PRIx32"\n",
			i,
			k1b_tlbe_vaddr_get(&entry),
			k1b_tlbe_paddr_get(&entry)
		);
	}

	/* Read LTLB into memory. */
	printf("\nLTLB:\n");
	for (int i = 0; i < K1B_LTLB_LENGTH; i++)
	{
		k1b_tlbe_read(&entry, K1B_LTLB_OFFSET + i);

		printf("TLB %d : vaddr: 0x%08"PRIx32" => paddr: 0x%08"PRIx32"\n",
			i,
			k1b_tlbe_vaddr_get(&entry),
			k1b_tlbe_paddr_get(&entry)
		);
	}
}

#define BUFFER_SIZE 8
static void test_dnoc_rx(void)
{
	int interface = 0;
	int tag = 8;
	char buffer[BUFFER_SIZE];

	if (mppa_noc_dnoc_rx_alloc(interface, tag) != 0)
	{
		printf("Erro alloc!\n");
		return;
	}

	uint32_t base_dnoc_rx   = 0x2000000;
	uint32_t tag_8_location = 0x2000200; // Base + 512 (2^8)

	printf("After!\n");
	printf("0x%08"PRIx32" = 0x%08"PRIx32"\n", base_dnoc_rx, *((uint32_t *) base_dnoc_rx));
	printf("0x%08"PRIx32" = 0x%08"PRIx32"\n", tag_8_location, *((uint32_t *) tag_8_location));

	mppa_noc_dnoc_rx_configuration_t config = {
		.buffer_base    = (uintptr_t) &buffer[0],
		.buffer_size    = BUFFER_SIZE,
		.current_offset = 0,
		.item_reload    = 0,
		.item_counter   = 0,
		.event_counter  = 0,
		.reload_mode    = 0,
		.activation     = MPPA_NOC_ACTIVATED,
		.counter_id     = 0
	};

	__builtin_k1_wpurge();
	__builtin_k1_fence();
	__builtin_k1_dinval();

	if (mppa_noc_dnoc_rx_configure(interface, tag, config) != 0)
	{
		printf("Erro configure!\n");
		return;
	}

	mppa_noc_dnoc_rx_free(interface, tag);
}

int main(void)
{
	int pid = __k1_get_cpu_id();

	printf("Hello World: %d\n", pid);

	k1b_tlb_init();

	printf("Config and alloc dnoc rx\n");

	test_dnoc_rx();

	printf("Working\n");

	return 0;
}
