/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * Copyright (C) 2013-2019 Kalray SA.
 *
 * All rights reserved.
 *
 */
/* Must come first. */

#include <mOS_common_types_c.h>
#include <mOS_vcore_u.h>

/**
    * @name Binary Sections
    */
/**@{*/
#define SECTION_TEXT __attribute__((section(".locked_text")))        /**< Text section.              */
#define SECTION_BINDESC __attribute__((section(".locked.binaries"))) /**< Binary descriptor section. */
/**@}*/

/**
    * @name Binary Symbols
    */
/**@{*/
extern int _bin_start_frame;   /**< Binary start frame.     */
extern int _bin_end_frame;     /**< Binary end frame.       */
#ifdef __k1io__
extern int _ddr_frame_start;   /**< DDR start frame.        */
extern int _ddr_frame_end;     /**< DDR end frame.          */
extern int __dtb_debut_offset; /**< Device tree offset.     */
extern int __dtb_size;         /**< Device tree Size.       */
#endif
extern int _user_stack_start;  /**< User stack start.       */
extern int _user_stack_end;    /**< User stack end.         */
extern int _vstart;            /**< Hypervisor entry point. */
extern int _scoreboard_offset; /**< Scoreboard offset.      */
extern int _kend;              /**< Kernel end address.     */
/**@}*/


/**
    * @name Page Shifts and Masks
    */
/**@{*/
#if defined(__ioddr__) || defined(__ioeth__)
    #define K1B_HUGE_PAGE_SHIFT 16                  /**< Huge Page Shift   */
#elif defined(__node__)
    #define K1B_HUGE_PAGE_SHIFT 15                  /**< Huge Page Shift   */
#endif
#define K1B_PAGE_SHIFT      12                      /**< Page Shift        */
#define K1B_PGTAB_SHIFT     22                      /**< Page Table Shift  */
#define K1B_PAGE_MASK       (~(K1B_PAGE_SIZE - 1))  /**< Page Mask         */
#define K1B_PGTAB_MASK      (~(K1B_PGTAB_SIZE - 1)) /**< Page Table Mask   */
/**@}*/

/**
    * @name Size of Pages and Page Tables
    */
/**@{*/
#define K1B_HUGE_PAGE_SIZE (1 << K1B_HUGE_PAGE_SHIFT) /**< Huge Page Size (in bytes)            */
#define K1B_PAGE_SIZE  (1 << K1B_PAGE_SHIFT)          /**< Page Size (in bytes)                 */
#define K1B_PGTAB_SIZE (1 << K1B_PGTAB_SHIFT)         /**< Page Table Size (in bytes)           */
#define K1B_PTE_SIZE   4                              /**< Page Table Entry Size (in bytes)     */
#define K1B_PDE_SIZE   4                              /**< Page Directory Entry Size (in bytes) */
/**@}*/


/**
    * @brief Places object in a binary section.
    *
    * @param x Target binary section.
    */
#define SECTION(x) __attribute__((section(x)))

/**
    * @brief Makes a symbol overwritable.
    */
#define OVERRIDE __attribute__((weak))

/**
    * @name Data TLBE Cache Policies
    */
/**@{*/
#define K1B_DTLBE_CACHE_POLICY_DEVICE    0 /**< Device Access */
#define K1B_DTLBE_CACHE_POLICY_BYPASS    1 /**< Cache Bypass  */
#define K1B_DTLBE_CACHE_POLICY_WRTHROUGH 2 /**< Write Through */
/**@*/

/**
    * @name Instruction TLBE Cache Policies
    */
/**@{*/
#define K1B_ITLBE_CACHE_POLICY_BYPASS 0 /**< Cache Bypass  */
#define K1B_ITLBE_CACHE_POLICY_ENABLE 2 /**< Cache Enable  */
/**@}*/

/**
    * @brief TLB Entry Protection Attributes
    *
    * @todo When running in bare-metal (without Hypervisor) we should
    * revise this.
    */
/**@{*/
#define K1B_TLBE_PROT_NONE  0 /**< None                    */
#define K1B_TLBE_PROT_R     5 /**< Read-Only               */
#define K1B_TLBE_PROT_RW    9 /**< Read and Write          */
#define K1B_TLBE_PROT_RX   11 /**< Read and Execute        */
#define K1B_TLBE_PROT_RWX  13 /**< Read, Write and Execute */
/**@}*/

/**
    * @name TLB Entry Status
    */
/**@{*/
#define K1B_TLBE_STATUS_INVALID   0 /**< Invalid         */
#define K1B_TLBE_STATUS_PRESENT   1 /**< Present         */
#define K1B_TLBE_STATUS_MODIFIED  2 /**< Modified        */
#define K1B_TLBE_STATUS_AMODIFIED 3 /**< Atomic Modified */
/**@}*/

#define K1B_TLBE_INITIALIZER(pn,s,g,asn,fn,ae,pa,cp,es) \
(                              \
    (((es)  & 0x3ULL)     <<  0) | \
    (((cp)  & 0x3ULL)     <<  2) | \
    (((pa)  & 0xfULL)     <<  4) | \
    (((ae)  & 0xfULL)     <<  8) | \
    (((fn)  & 0xfffffULL) << 12) | \
    (((asn) & 0x1ffULL)   << 32) | \
    (((g)   & 0x1ULL)     << 42) | \
    (((s)   & 0x1ULL)     << 43) | \
    (((pn)  & 0xfffffULL) << 44)   \
)

/**
 * @brief Ethernet TLB entry.
 */
#define K1B_TLBE_ETHERNET \
	K1B_TLBE_INITIALIZER(\
		0x04200,                       \
		0x0,                           \
		0x0,                           \
		0x04400,                       \
		0x00,                          \
		0x0,                           \
		K1B_TLBE_PROT_RW,              \
		K1B_DTLBE_CACHE_POLICY_DEVICE, \
		K1B_TLBE_STATUS_AMODIFIED      \
	)

/**
 * @brief DDR TLB entry.
 */
#define K1B_TLBE_DDR \
	K1B_TLBE_INITIALIZER(\
		0x80000,                          \
		0x0,                              \
		0x0,                              \
		0x80000,                          \
		0x00,                             \
		0x0,                              \
		K1B_TLBE_PROT_RW,                 \
		K1B_DTLBE_CACHE_POLICY_WRTHROUGH, \
		K1B_TLBE_STATUS_AMODIFIED         \
	)

/**
 * @brief Null TLB entry.
 */
#define K1B_TLBE_NULL \
	K1B_TLBE_INITIALIZER(\
		0x00000,                       \
		0x0,                           \
		0x0,                           \
		0x00000,                       \
		0x00,                          \
		0x0,                           \
		K1B_TLBE_PROT_NONE,            \
		K1B_ITLBE_CACHE_POLICY_ENABLE, \
		K1B_TLBE_STATUS_INVALID        \
	)

/**
 * @brief User-space hook for RM.
 */
extern int _vhook_rm OVERRIDE;

extern int MOS_RESERVED;
extern int _MOS_SECURITY_LEVEL;
extern int __MPPA_BURN_TX;
extern int __MPPA_BURN_FDIR;

/**
 * @brief Binary descriptor.
 */
volatile mOS_bin_desc_t bin_descriptor SECTION_BINDESC OVERRIDE =
{
	.smem_start_frame   = (int)&_bin_start_frame,                /* Binary start frame. */
	.smem_end_frame     = (int)&_bin_end_frame,                  /* Binary end frame.   */

#ifdef __k1io__
	.ddr_start_frame    = (int)&_ddr_frame_start,                /* DDR start frame. */
	.ddr_end_frame      = (int)&_ddr_frame_end,                  /* DDR end frame.   */
#else
	.ddr_start_frame    = 0,                                     /* DDR start frame. */
	.ddr_end_frame      = 0,                                     /* DDR end frame.   */
#endif
	.entry_point        = (uint32_t) & _vstart,                  /* Hypervisor entry point. */

#ifdef __k1io__
	.dtb_debut			= (uint32_t) &__dtb_debut_offset,        /* Device tree offset. */
	.dtb_size			= (int) &__dtb_size,                     /* Device tree size.   */
#endif

	.scoreboard_offset  = ( int ) &(_scoreboard_offset),         /* Hypervisor scoreboard. */

#ifdef __k1io__
	.pe_pool            = (0x1 << ((BSP_NB_RM_IO_P ))) - 1,      /* Required PE set. */
#else
	.pe_pool            = (0x1 << ((BSP_NB_PE_P & ~(0x3)))) - 1, /* Required PE Set. */
#endif

	.tlb_small_size     = K1B_PAGE_SIZE,                         /* Default page size. */
	.tlb_big_size       = K1B_HUGE_PAGE_SIZE,                    /* Huge page size.    */

	.security_level     = (int) &_MOS_SECURITY_LEVEL,            /* Security level. */

	/*
	 * Initial LTLB mapping.
	 */
	.ltlb               = {
#ifdef __k1io__
		// {._dword = K1B_TLBE_DDR       },
		{ ._dword =  K1B_TLBE_INITIALIZER(0xc0000, 0x0, 0x0, 0x0, 0x80000, 0x0, 0xd, 0x2, 0x3) },
		{._dword = K1B_TLBE_ETHERNET  },
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      }
		// { ._dword =  K1B_TLBE_INITIALIZER(0x4420, 0x0, 0x0, 0x0, 0x4400, 0x0, 0x9, 0x0, 0x3) },
		// { ._dword =  K1B_TLBE_INITIALIZER(0xc0002, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0) },
		// { ._dword =  K1B_TLBE_INITIALIZER(0xc0002, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0) },
		// { ._dword =  K1B_TLBE_INITIALIZER(0xc0002, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0) },
		// { ._dword =  K1B_TLBE_INITIALIZER(0xc0002, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0) },
		// { ._dword =  K1B_TLBE_INITIALIZER(0xc0002, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0) },
		// { ._dword =  K1B_TLBE_INITIALIZER(0xc0002, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0) },
		// { ._dword =  K1B_TLBE_INITIALIZER(0xc0002, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0) },
		// { ._dword =  K1B_TLBE_INITIALIZER(0xc0002, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0) }
#else
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      },
		{._dword = K1B_TLBE_NULL      }
#endif
	},

	/*
	 * @initial JTLB mapping.
	 */
	.jtlb               = {
#ifdef __k1dp__
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword  = K1B_TLBE_NULL   },
		{ ._dword =  K1B_TLBE_INITIALIZER(0x08, 0x1, 0x1, 0x00, 0x08, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x8000   PA: 0x8000   */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x44, 0x0, 0x1, 0x00, 0x40, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x44000  PA: 0x40000  */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x09, 0x1, 0x1, 0x00, 0x09, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x9000   PA: 0x9000   */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x4c, 0x0, 0x1, 0x00, 0x48, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x4c000  PA: 0x48000  */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x0a, 0x1, 0x1, 0x00, 0x0a, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xa000   PA: 0xa000   */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x54, 0x0, 0x1, 0x00, 0x50, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x54000  PA: 0x50000  */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x0b, 0x1, 0x1, 0x00, 0x0b, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xb000   PA: 0xb000   */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x5c, 0x0, 0x1, 0x00, 0x50, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x5c000  PA: 0x58000  */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x0c, 0x1, 0x1, 0x00, 0x0c, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xc000   PA: 0xc000   */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x64, 0x0, 0x1, 0x00, 0x58, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x60000  PA: 0x60000  */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x0d, 0x1, 0x1, 0x00, 0x0d, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xd000   PA: 0xd000   */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x6c, 0x0, 0x1, 0x00, 0x60, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x68000  PA: 0x68000  */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x0e, 0x1, 0x1, 0x00, 0x0e, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xe000   PA: 0xe000   */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x74, 0x0, 0x1, 0x00, 0x68, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x70000  PA: 0x70000  */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x0f, 0x1, 0x1, 0x00, 0x0f, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xf000   PA: 0xf000   */,
		{ ._dword =  K1B_TLBE_INITIALIZER(0x7c, 0x0, 0x1, 0x00, 0x78, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x7c000  PA: 0x78000  */,
		{ ._dword =  0x10c00000100dbULL  } /* VA: 0x10000  PA: 0x10000  */,
		{ ._dword =  0x84400000800dbULL  } /* VA: 0x80000  PA: 0x80000  */,
		{ ._dword =  0x11c00000110dbULL  } /* VA: 0x11000  PA: 0x11000  */,
		{ ._dword =  0x8c400000880dbULL  } /* VA: 0x88000  PA: 0x88000  */,
		{ ._dword =  0x12c00000120dbULL  } /* VA: 0x12000  PA: 0x12000  */,
		{ ._dword =  0x94400000900dbULL  } /* VA: 0x90000  PA: 0x90000  */,
		{ ._dword =  0x13c00000130dbULL  } /* VA: 0x13000  PA: 0x13000  */,
		{ ._dword =  0x9c400000980dbULL  } /* VA: 0x98000  PA: 0x98000  */,
		{ ._dword =  0x14c00000140dbULL  } /* VA: 0x14000  PA: 0x14000  */,
		{ ._dword =  0xa4400000a00dbULL  } /* VA: 0xa0000  PA: 0xa0000  */,
		{ ._dword =  0x15c00000150dbULL  } /* VA: 0x15000  PA: 0x15000  */,
		{ ._dword =  0xac400000a80dbULL  } /* VA: 0xa8000  PA: 0xa8000  */,
		{ ._dword =  0x16c00000160dbULL  } /* VA: 0x16000  PA: 0x16000  */,
		{ ._dword =  0xb4400000b00dbULL  } /* VA: 0xb0000  PA: 0xb0000  */,
		{ ._dword =  0x17c00000170dbULL  } /* VA: 0x17000  PA: 0x17000  */,
		{ ._dword =  0xbc400000b80dbULL  } /* VA: 0xb8000  PA: 0xb8000  */,
		{ ._dword =  0x18c00000180dbULL  } /* VA: 0x18000  PA: 0x18000  */,
		{ ._dword =  0xc4400000c00dbULL  } /* VA: 0xc0000  PA: 0xc0000  */,
		{ ._dword =  0x19c00000190dbULL  } /* VA: 0x19000  PA: 0x19000  */,
		{ ._dword =  0xcc400000c80dbULL  } /* VA: 0xc8000  PA: 0xc8000  */,
		{ ._dword =  0x1ac000001a0dbULL  } /* VA: 0x1a000  PA: 0x1a000  */,
		{ ._dword =  0xd4400000d00dbULL  } /* VA: 0xd0000  PA: 0xd0000  */,
		{ ._dword =  0x1bc000001b0dbULL  } /* VA: 0x1b000  PA: 0x1b000  */,
		{ ._dword =  0xdc400000d80dbULL  } /* VA: 0xd8000  PA: 0xd8000  */,
		{ ._dword =  0x1cc000001c0dbULL  } /* VA: 0x1c000  PA: 0x1c000  */,
		{ ._dword =  0xe4400000e00dbULL  } /* VA: 0xe0000  PA: 0xe0000  */,
		{ ._dword =  0x1dc000001d0dbULL  } /* VA: 0x1d000  PA: 0x1d000  */,
		{ ._dword =  0xec400000e80dbULL  } /* VA: 0xe8000  PA: 0xe8000  */,
		{ ._dword =  0x1ec000001e0dbULL  } /* VA: 0x1e000  PA: 0x1e000  */,
		{ ._dword =  0xf4400000f00dbULL  } /* VA: 0xf0000  PA: 0xf0000  */,
		{ ._dword =  0x1fc000001f0dbULL  } /* VA: 0x1f000  PA: 0x1f000  */,
		{ ._dword =  0xfc400000f80dbULL  } /* VA: 0xf8000  PA: 0xf8000  */,
		{ ._dword =  0x20c00000200dbULL  } /* VA: 0x20000  PA: 0x20000  */,
		{ ._dword =  0x104400001000dbULL } /* VA: 0x100000 PA: 0x100000 */,
		{ ._dword =  0x21c00000210dbULL  } /* VA: 0x21000  PA: 0x21000  */,
		{ ._dword =  0x10c400001080dbULL } /* VA: 0x108000 PA: 0x108000 */,
		{ ._dword =  0x22c00000220dbULL  } /* VA: 0x22000  PA: 0x22000  */,
		{ ._dword =  0x114400001100dbULL } /* VA: 0x110000 PA: 0x110000 */,
		{ ._dword =  0x23c00000230dbULL  } /* VA: 0x23000  PA: 0x23000  */,
		{ ._dword =  0x11c400001180dbULL } /* VA: 0x118000 PA: 0x118000 */,
		{ ._dword =  0x24c00000240dbULL  } /* VA: 0x24000  PA: 0x24000  */,
		{ ._dword =  0x124400001200dbULL } /* VA: 0x120000 PA: 0x120000 */,
		{ ._dword =  0x25c00000250dbULL  } /* VA: 0x25000  PA: 0x25000  */,
		{ ._dword =  0x12c400001280dbULL } /* VA: 0x128000 PA: 0x128000 */,
		{ ._dword =  0x26c00000260dbULL  } /* VA: 0x26000  PA: 0x26000  */,
		{ ._dword =  0x134400001300dbULL } /* VA: 0x130000 PA: 0x130000 */,
		{ ._dword =  0x27c00000270dbULL  } /* VA: 0x27000  PA: 0x27000  */,
		{ ._dword =  0x13c400001380dbULL } /* VA: 0x138000 PA: 0x138000 */,
		{ ._dword =  0x28c00000280dbULL  } /* VA: 0x28000  PA: 0x28000  */,
		{ ._dword =  0x144400001400dbULL } /* VA: 0x140000 PA: 0x140000 */,
		{ ._dword =  0x29c00000290dbULL  } /* VA: 0x29000  PA: 0x29000  */,
		{ ._dword =  0x14c400001480dbULL } /* VA: 0x148000 PA: 0x148000 */,
		{ ._dword =  0x2ac000002a0dbULL  } /* VA: 0x2a000  PA: 0x2a000  */,
		{ ._dword =  0x154400001500dbULL } /* VA: 0x150000 PA: 0x150000 */,
		{ ._dword =  0x2bc000002b0dbULL  } /* VA: 0x2b000  PA: 0x2b000  */,
		{ ._dword =  0x15c400001580dbULL } /* VA: 0x158000 PA: 0x158000 */,
		{ ._dword =  0x2cc000002c0dbULL  } /* VA: 0x2c000  PA: 0x2c000  */,
		{ ._dword =  0x164400001600dbULL } /* VA: 0x160000 PA: 0x160000 */,
		{ ._dword =  0x2dc000002d0dbULL  } /* VA: 0x2d000  PA: 0x2d000  */,
		{ ._dword =  0x16c400001680dbULL } /* VA: 0x168000 PA: 0x168000 */,
		{ ._dword =  0x2ec000002e0dbULL  } /* VA: 0x2e000  PA: 0x2e000  */,
		{ ._dword =  0x174400001700dbULL } /* VA: 0x170000 PA: 0x170000 */,
		{ ._dword =  0x2fc000002f0dbULL  } /* VA: 0x2f000  PA: 0x2f000  */,
		{ ._dword =  0x17c400001780dbULL } /* VA: 0x178000 PA: 0x178000 */,
		{ ._dword =  0x30c00000300dbULL  } /* VA: 0x30000  PA: 0x30000  */,
		{ ._dword =  0x184400001800dbULL } /* VA: 0x180000 PA: 0x180000 */,
		{ ._dword =  0x31c00000310dbULL  } /* VA: 0x31000  PA: 0x31000  */,
		{ ._dword =  0x18c400001880dbULL } /* VA: 0x188000 PA: 0x188000 */,
		{ ._dword =  0x32c00000320dbULL  } /* VA: 0x32000  PA: 0x32000  */,
		{ ._dword =  0x194400001900dbULL } /* VA: 0x190000 PA: 0x190000 */,
		{ ._dword =  0x33c00000330dbULL  } /* VA: 0x33000  PA: 0x33000  */,
		{ ._dword =  0x19c400001980dbULL } /* VA: 0x198000 PA: 0x198000 */,
		{ ._dword =  0x34c00000340dbULL  } /* VA: 0x34000  PA: 0x34000  */,
		{ ._dword =  0x1a4400001a00dbULL } /* VA: 0x1a0000 PA: 0x1a0000 */,
		{ ._dword =  0x35c00000350dbULL  } /* VA: 0x35000  PA: 0x35000  */,
		{ ._dword =  0x1ac400001a80dbULL } /* VA: 0x1a8000 PA: 0x1a8000 */,
		{ ._dword =  0x36c00000360dbULL  } /* VA: 0x36000  PA: 0x36000  */,
		{ ._dword =  0x1b4400001b00dbULL } /* VA: 0x1b0000 PA: 0x1b0000 */,
		{ ._dword =  0x37c00000370dbULL  } /* VA: 0x37000  PA: 0x37000  */,
		{ ._dword =  0x1bc400001b80dbULL } /* VA: 0x1b8000 PA: 0x1b8000 */,
		{ ._dword =  0x38c00000380dbULL  } /* VA: 0x38000  PA: 0x38000  */,
		{ ._dword =  0x1c4400001c00dbULL } /* VA: 0x1c0000 PA: 0x1c0000 */,
		{ ._dword =  0x39c00000390dbULL  } /* VA: 0x39000  PA: 0x39000  */,
		{ ._dword =  0x1cc400001c80dbULL } /* VA: 0x1c8000 PA: 0x1c8000 */,
		{ ._dword =  0x3ac000003a0dbULL  } /* VA: 0x3a000  PA: 0x3a000  */,
		{ ._dword =  0x1d4400001d00dbULL } /* VA: 0x1d0000 PA: 0x1d0000 */,
		{ ._dword =  0x3bc000003b0dbULL  } /* VA: 0x3b000  PA: 0x3b000  */,
		{ ._dword =  0x1dc400001d80dbULL } /* VA: 0x1d8000 PA: 0x1d8000 */,
		{ ._dword =  0x3cc000003c0dbULL  } /* VA: 0x3c000  PA: 0x3c000  */,
		{ ._dword =  0x1e4400001e00dbULL } /* VA: 0x1e0000 PA: 0x1e0000 */,
		{ ._dword =  0x3dc000003d0dbULL  } /* VA: 0x3d000  PA: 0x3d000  */,
		{ ._dword =  0x1ec400001e80dbULL } /* VA: 0x1e8000 PA: 0x1e8000 */,
		{ ._dword =  0x3ec000003e0dbULL  } /* VA: 0x3e000  PA: 0x3e000  */,
		{ ._dword =  0x1f4400001f00dbULL } /* VA: 0x1f0000 PA: 0x1f0000 */,
		{ ._dword =  0x3fc000003f0dbULL  } /* VA: 0x3f000  PA: 0x3f000  */,
		{ ._dword =  0x1fc400001f80dbULL } /* VA: 0x1f8000 PA: 0x1f8000 */,

#else
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_INITIALIZER(0x48, 0x0, 0x1, 0x00, 0x40, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x48000  PA: 0x40000 */,
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_INITIALIZER(0x58, 0x0, 0x1, 0x00, 0x50, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x58000  PA: 0x50000 */,
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_INITIALIZER(0x68, 0x0, 0x1, 0x00, 0x60, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x68000  PA: 0x60000 */,
		{ ._dword = K1B_TLBE_NULL  },
		{ ._dword = K1B_TLBE_INITIALIZER(0x78, 0x0, 0x1, 0x00, 0x70, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x78000  PA: 0x70000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0x08, 0x1, 0x1, 0x00, 0x08, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x08000  PA: 0x08000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0x88, 0x0, 0x1, 0x00, 0x80, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x88000  PA: 0x80000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0x09, 0x1, 0x1, 0x00, 0x09, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x09000  PA: 0x09000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0x98, 0x0, 0x1, 0x00, 0x90, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x98000  PA: 0x90000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0x0a, 0x1, 0x1, 0x00, 0x0a, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x0a000  PA: 0x0a000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0xa8, 0x0, 0x1, 0x00, 0xa0, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xa8000  PA: 0xa0000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0x0b, 0x1, 0x1, 0x00, 0x0b, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x0b000  PA: 0x0b000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0xb8, 0x0, 0x1, 0x00, 0xb0, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xb8000  PA: 0xb0000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0x0c, 0x1, 0x1, 0x00, 0x0c, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x0c000  PA: 0x0c000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0xc8, 0x0, 0x1, 0x00, 0xc0, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xc8000  PA: 0xc0000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0x0d, 0x1, 0x1, 0x00, 0x0d, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x0d000  PA: 0x0d000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0xd8, 0x0, 0x1, 0x00, 0xd0, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xd8000  PA: 0xd0000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0x0e, 0x1, 0x1, 0x00, 0x0e, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x0e000  PA: 0x0e000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0xe8, 0x0, 0x1, 0x00, 0xe0, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xe8000  PA: 0xe0000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0x0f, 0x1, 0x1, 0x00, 0x0f, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0x0f000  PA: 0x0f000  */,
		{ ._dword = K1B_TLBE_INITIALIZER(0xf8, 0x0, 0x1, 0x00, 0xf0, 0x0, K1B_TLBE_PROT_RWX, K1B_ITLBE_CACHE_POLICY_ENABLE, K1B_TLBE_STATUS_AMODIFIED) } /* VA: 0xf8000  PA: 0xf0000  */,
		{ ._dword = 0x10c00000100dbULL  } /* VA: 0x10000  PA: 0x10000  */,
		{ ._dword = 0x108400001000dbULL } /* VA: 0x100000 PA: 0x100000 */,
		{ ._dword = 0x11c00000110dbULL  } /* VA: 0x11000  PA: 0x11000  */,
		{ ._dword = 0x118400001100dbULL } /* VA: 0x110000 PA: 0x110000 */,
		{ ._dword = 0x12c00000120dbULL  } /* VA: 0x12000  PA: 0x12000  */,
		{ ._dword = 0x128400001200dbULL } /* VA: 0x120000 PA: 0x120000 */,
		{ ._dword = 0x13c00000130dbULL  } /* VA: 0x13000  PA: 0x13000  */,
		{ ._dword = 0x138400001300dbULL } /* VA: 0x130000 PA: 0x130000 */,
		{ ._dword = 0x14c00000140dbULL  } /* VA: 0x14000  PA: 0x14000  */,
		{ ._dword = 0x148400001400dbULL } /* VA: 0x140000 PA: 0x140000 */,
		{ ._dword = 0x15c00000150dbULL  } /* VA: 0x15000  PA: 0x15000  */,
		{ ._dword = 0x158400001500dbULL } /* VA: 0x150000 PA: 0x150000 */,
		{ ._dword = 0x16c00000160dbULL  } /* VA: 0x16000  PA: 0x16000  */,
		{ ._dword = 0x168400001600dbULL } /* VA: 0x160000 PA: 0x160000 */,
		{ ._dword = 0x17c00000170dbULL  } /* VA: 0x17000  PA: 0x17000  */,
		{ ._dword = 0x178400001700dbULL } /* VA: 0x170000 PA: 0x170000 */,
		{ ._dword = 0x18c00000180dbULL  } /* VA: 0x18000  PA: 0x18000  */,
		{ ._dword = 0x188400001800dbULL } /* VA: 0x180000 PA: 0x180000 */,
		{ ._dword = 0x19c00000190dbULL  } /* VA: 0x19000  PA: 0x19000  */,
		{ ._dword = 0x198400001900dbULL } /* VA: 0x190000 PA: 0x190000 */,
		{ ._dword = 0x1ac000001a0dbULL  } /* VA: 0x1a000  PA: 0x1a000  */,
		{ ._dword = 0x1a8400001a00dbULL } /* VA: 0x1a0000 PA: 0x1a0000 */,
		{ ._dword = 0x1bc000001b0dbULL  } /* VA: 0x1b000  PA: 0x1b000  */,
		{ ._dword = 0x1b8400001b00dbULL } /* VA: 0x1b0000 PA: 0x1b0000 */,
		{ ._dword = 0x1cc000001c0dbULL  } /* VA: 0x1c000  PA: 0x1c000  */,
		{ ._dword = 0x1c8400001c00dbULL } /* VA: 0x1c0000 PA: 0x1c0000 */,
		{ ._dword = 0x1dc000001d0dbULL  } /* VA: 0x1d000  PA: 0x1d000  */,
		{ ._dword = 0x1d8400001d00dbULL } /* VA: 0x1d0000 PA: 0x1d0000 */,
		{ ._dword = 0x1ec000001e0dbULL  } /* VA: 0x1e000  PA: 0x1e000  */,
		{ ._dword = 0x1e8400001e00dbULL } /* VA: 0x1e0000 PA: 0x1e0000 */,
		{ ._dword = 0x1fc000001f0dbULL  } /* VA: 0x1f000  PA: 0x1f000  */,
		{ ._dword = 0x1f8400001f00dbULL } /* VA: 0x1f0000 PA: 0x1f0000 */,
		{ ._dword = 0x20c00000200dbULL  } /* VA: 0x20000  PA: 0x20000  */,
		{ ._dword = 0x208400002000dbULL } /* VA: 0x200000 PA: 0x200000 */,
		{ ._dword = 0x21c00000210dbULL  } /* VA: 0x21000  PA: 0x21000  */,
		{ ._dword = 0x218400002100dbULL } /* VA: 0x210000 PA: 0x210000 */,
		{ ._dword = 0x22c00000220dbULL  } /* VA: 0x22000  PA: 0x22000  */,
		{ ._dword = 0x228400002200dbULL } /* VA: 0x220000 PA: 0x220000 */,
		{ ._dword = 0x23c00000230dbULL  } /* VA: 0x23000  PA: 0x23000  */,
		{ ._dword = 0x238400002300dbULL } /* VA: 0x230000 PA: 0x230000 */,
		{ ._dword = 0x24c00000240dbULL  } /* VA: 0x24000  PA: 0x24000  */,
		{ ._dword = 0x248400002400dbULL } /* VA: 0x240000 PA: 0x240000 */,
		{ ._dword = 0x25c00000250dbULL  } /* VA: 0x25000  PA: 0x25000  */,
		{ ._dword = 0x258400002500dbULL } /* VA: 0x250000 PA: 0x250000 */,
		{ ._dword = 0x26c00000260dbULL  } /* VA: 0x26000  PA: 0x26000  */,
		{ ._dword = 0x268400002600dbULL } /* VA: 0x260000 PA: 0x260000 */,
		{ ._dword = 0x27c00000270dbULL  } /* VA: 0x27000  PA: 0x27000  */,
		{ ._dword = 0x278400002700dbULL } /* VA: 0x270000 PA: 0x270000 */,
		{ ._dword = 0x28c00000280dbULL  } /* VA: 0x28000  PA: 0x28000  */,
		{ ._dword = 0x288400002800dbULL } /* VA: 0x280000 PA: 0x280000 */,
		{ ._dword = 0x29c00000290dbULL  } /* VA: 0x29000  PA: 0x29000  */,
		{ ._dword = 0x298400002900dbULL } /* VA: 0x290000 PA: 0x290000 */,
		{ ._dword = 0x2ac000002a0dbULL  } /* VA: 0x2a000  PA: 0x2a000  */,
		{ ._dword = 0x2a8400002a00dbULL } /* VA: 0x2a0000 PA: 0x2a0000 */,
		{ ._dword = 0x2bc000002b0dbULL  } /* VA: 0x2b000  PA: 0x2b000  */,
		{ ._dword = 0x2b8400002b00dbULL } /* VA: 0x2b0000 PA: 0x2b0000 */,
		{ ._dword = 0x2cc000002c0dbULL  } /* VA: 0x2c000  PA: 0x2c000  */,
		{ ._dword = 0x2c8400002c00dbULL } /* VA: 0x2c0000 PA: 0x2c0000 */,
		{ ._dword = 0x2dc000002d0dbULL  } /* VA: 0x2d000  PA: 0x2d000  */,
		{ ._dword = 0x2d8400002d00dbULL } /* VA: 0x2d0000 PA: 0x2d0000 */,
		{ ._dword = 0x2ec000002e0dbULL  } /* VA: 0x2e000  PA: 0x2e000  */,
		{ ._dword = 0x2e8400002e00dbULL } /* VA: 0x2e0000 PA: 0x2e0000 */,
		{ ._dword = 0x2fc000002f0dbULL  } /* VA: 0x2f000  PA: 0x2f000  */,
		{ ._dword = 0x2f8400002f00dbULL } /* VA: 0x2f0000 PA: 0x2f0000 */,
		{ ._dword = 0x30c00000300dbULL  } /* VA: 0x30000  PA: 0x30000  */,
		{ ._dword = 0x308400003000dbULL } /* VA: 0x300000 PA: 0x300000 */,
		{ ._dword = 0x31c00000310dbULL  } /* VA: 0x31000  PA: 0x31000  */,
		{ ._dword = 0x318400003100dbULL } /* VA: 0x310000 PA: 0x310000 */,
		{ ._dword = 0x32c00000320dbULL  } /* VA: 0x32000  PA: 0x32000  */,
		{ ._dword = 0x328400003200dbULL } /* VA: 0x320000 PA: 0x320000 */,
		{ ._dword = 0x33c00000330dbULL  } /* VA: 0x33000  PA: 0x33000  */,
		{ ._dword = 0x338400003300dbULL } /* VA: 0x330000 PA: 0x330000 */,
		{ ._dword = 0x34c00000340dbULL  } /* VA: 0x34000  PA: 0x34000  */,
		{ ._dword = 0x348400003400dbULL } /* VA: 0x340000 PA: 0x340000 */,
		{ ._dword = 0x35c00000350dbULL  } /* VA: 0x35000  PA: 0x35000  */,
		{ ._dword = 0x358400003500dbULL } /* VA: 0x350000 PA: 0x350000 */,
		{ ._dword = 0x36c00000360dbULL  } /* VA: 0x36000  PA: 0x36000  */,
		{ ._dword = 0x368400003600dbULL } /* VA: 0x360000 PA: 0x360000 */,
		{ ._dword = 0x37c00000370dbULL  } /* VA: 0x37000  PA: 0x37000  */,
		{ ._dword = 0x378400003700dbULL } /* VA: 0x370000 PA: 0x370000 */,
		{ ._dword = 0x38c00000380dbULL  } /* VA: 0x38000  PA: 0x38000  */,
		{ ._dword = 0x388400003800dbULL } /* VA: 0x380000 PA: 0x380000 */,
		{ ._dword = 0x39c00000390dbULL  } /* VA: 0x39000  PA: 0x39000  */,
		{ ._dword = 0x398400003900dbULL } /* VA: 0x390000 PA: 0x390000 */,
		{ ._dword = 0x3ac000003a0dbULL  } /* VA: 0x3a000  PA: 0x3a000  */,
		{ ._dword = 0x3a8400003a00dbULL } /* VA: 0x3a0000 PA: 0x3a0000 */,
		{ ._dword = 0x3bc000003b0dbULL  } /* VA: 0x3b000  PA: 0x3b000  */,
		{ ._dword = 0x3b8400003b00dbULL } /* VA: 0x3b0000 PA: 0x3b0000 */,
		{ ._dword = 0x3cc000003c0dbULL  } /* VA: 0x3c000  PA: 0x3c000  */,
		{ ._dword = 0x3c8400003c00dbULL } /* VA: 0x3c0000 PA: 0x3c0000 */,
		{ ._dword = 0x3dc000003d0dbULL  } /* VA: 0x3d000  PA: 0x3d000  */,
		{ ._dword = 0x3d8400003d00dbULL } /* VA: 0x3d0000 PA: 0x3d0000 */,
		{ ._dword = 0x3ec000003e0dbULL  } /* VA: 0x3e000  PA: 0x3e000  */,
		{ ._dword = 0x3e8400003e00dbULL } /* VA: 0x3e0000 PA: 0x3e0000 */,
		{ ._dword = 0x3fc000003f0dbULL  } /* VA: 0x3f000  PA: 0x3f000  */,
		{ ._dword = 0x3f8400003f00dbULL } /* VA: 0x3f0000 PA: 0x3f0000 */,
#endif
	},

	/*
	 * Required D-NOC RX channels.
	 */
	.rx_pool    = {
			.interface [ 0 ... MOS_NB_DMA_MAX -1 ] = {
				.array64_bit = { ~(0x0ULL), ~(0x0ULL), ~(0x0ULL), ~(0x0ULL) }
			}
	},

	/*
	 * Required micro-cores.
	 */
	.uc_pool    = { .interface [ 0 ... MOS_NB_DMA_MAX -1 ] = ~(0x0)},

	/*
	 * Required D-NoC TX channels.
	 */
	.tx_pool    = {
		.interface [ 0 ... (MOS_NB_DMA_MAX - 1)] = ((0x1 << MOS_NB_TX_CHANNELS) - 1)
	},

	/*
	 * Required C-NoC RX Channels.
	 */
	.mb_pool    = {
		.interface [ 0 ... (MOS_NB_DMA_MAX - 1)] = {
			.array64_bit = { ~(0x0ULL), ~(0x0ULL) }
		}
	},

	/*
	 * Required C-NoC TX channels.
	 */
	.mb_tx_pool = { .interface [0 ... (MOS_NB_DMA_MAX - 1)] = 0xf },

	/*
	 * Required allowed first-dir DMA interfaces.
	 */
	.fdir_pool  = { .interface [0 ... (MOS_NB_DMA_MAX - 1)] = 0x1f },

#ifdef __k1dp__
	.burn_tx    = (int)&__MPPA_BURN_TX,
	.burn_fdir  = (int)&__MPPA_BURN_FDIR,
#else
	.burn_tx    = -1,
	.burn_fdir  = -1,
#endif

	.hook_rm	= (void (*)(void*)) &_vhook_rm /* User-space RM hook. */
};

/**
 * @brief Hypervisor scoreboard.
 */
mOS_scoreboard_t scoreboard __attribute__((section (".scoreboard")));
