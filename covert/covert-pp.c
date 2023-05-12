#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "pollute.h"
#ifdef _MSC_VER
#include <intrin.h> /* for rdtscp and clflush */
#pragma optimize("gt",on)
#else
#include <x86intrin.h> /* for rdtscp and clflush */
#endif

// Access hardware timestamp counter
#define RDTSC(cycles) __asm__ volatile ("rdtsc" : "=a" (cycles));

// Serialize execution
#define CPUID() asm volatile ("CPUID" : : : "%rax", "%rbx", "%rcx", "%rdx");

// Flush a cache line
#define CLFLUSH(address) _mm_clflush(address);

#define SAMPLES 8 // TODO: CONFIGURE THIS

// Hint: You can find these by looking up entries in /sys/devices/system/cpu/cpu0/cache
#define L1_CACHE_SIZE 32768// TODO: Find cache size
#define LINE_SIZE 64// TODO: Find line size
#define ASSOCIATIVITY 8// TODO: Find associativity

#define L1_NUM_SETS (L1_CACHE_SIZE/(LINE_SIZE*ASSOCIATIVITY))

#define NUM_OFFSET_BITS 6// TODO: Find number of offset bits
#define NUM_INDEX_BITS 6// TODO: Find number of index bits
#define NUM_OFF_IND_BITS (NUM_OFFSET_BITS + NUM_INDEX_BITS)

uint64_t eviction_counts[L1_NUM_SETS] = {0};
__attribute__ ((aligned (64))) uint64_t trojan_array[32*4096];
__attribute__ ((aligned (64))) uint64_t spy_array[4096];


/* This function provides an eviction set address, given the
 * base address of a trojan/spy array, the required cache
 * set ID, and way ID.
 *
 * TODO: Describe the algorithm to a TA
 *
 */
uint64_t* get_eviction_set_address(uint64_t *base, int set, int way)
{
    uint64_t tag_bits = (((uint64_t)base) >> NUM_OFF_IND_BITS);
    int idx_bits = (((uint64_t)base) >> NUM_OFFSET_BITS) & 0x3f;

    if (idx_bits <= set) {
        return (uint64_t *)((((tag_bits << NUM_INDEX_BITS) + set) << NUM_OFFSET_BITS) +
                            (L1_NUM_SETS * LINE_SIZE * way));
    } else {
        return (uint64_t *)((((tag_bits << NUM_INDEX_BITS) +
                               (L1_NUM_SETS + set)) << NUM_OFFSET_BITS) +
                            (L1_NUM_SETS * LINE_SIZE * way));
    }
}

/* This function sets up a trojan/spy eviction set using the
 * function above.  The eviction set is essentially a linked
 * list that spans all ways of the conflicting cache set.
 *
 * i.e., way-0 -> way-1 -> ..... way-assoc -> NULL
 *
 */
void setup(uint64_t *base, int assoc)
{
    uint64_t i, j;
    uint64_t *eviction_set_addr;
    int idx;
    // printf("%p\n", base);
    // Prime the cache set by set (i.e., prime all lines in a set)
    for (i = 0; i < L1_NUM_SETS; i++) {
        /* TODO:
         * Set up the eviction set here.
         *
         */  
        for (j = 0; j < assoc; j++) {
            eviction_set_addr = get_eviction_set_address(base, i, j);
            if (j != 0) {
                base[idx] = eviction_set_addr - base;
            }
            idx = eviction_set_addr - base;
            // printf("%d %p %p %d %p %p\n", idx, eviction_set_addr, base, eviction_set_addr - base, &base[idx], &base[idx+1]);
            // base = eviction_set_addr;
            // printf("%p\n", eviction_set_addr);
        }
        // printf("\n");
        /*
        * In every iteration of trying
        * Decrease L1D miss
        */
        dec_l1d_miss(10);
    }
}

/* TODO:
 *
 * This function implements the Trojan that sends a message
 * to the spy over the cache covert channel.  Note that the
 * message forgoes case sensitivity to maximize the covert
 * channel bandwidth.
 *
 * Your job is to use the right eviction set to mount an
 * appropriate PRIME+PROBE attack.  Remember that in both
 * attacks, we only need to time the spy and not the Trojan.
 *
 * Note that you may need to serialize execution wherever
 * appropriate.
 */
void trojan(char byte)
{
    int set;
    uint64_t *eviction_set_addr;

    if (byte >= 'a' && byte <= 'z') {
        byte -= 32;
    }
    if (byte == 10 || byte == 13) { // encode a new line
        set = 63;
    } else if (byte >= 32 && byte < 96) {
        set = (byte - 32);
    } else {
        printf("pp trojan: unrecognized character %c\n", byte);
        exit(1);
    }
    
    /* TODO:
     * Your attack code goes in here.
     *
     */  
    int offset;
    eviction_set_addr = get_eviction_set_address(trojan_array, set, 0);
    offset = eviction_set_addr - trojan_array;
    while (trojan_array[offset]) {
        offset = trojan_array[offset];
    }
}

/* TODO:
 *
 * This function implements the spy that receives a message
 * from the Trojan over the cache covert channel.  Evictions
 * are timed using appropriate hardware timestamp counters
 * and recorded in the eviction_counts array.  In particular,
 * only record evictions to the set that incurred the maximum
 * penalty in terms of its access time.
 *
 * Your job is to use the right eviction set to mount an
 * appropriate PRIME+PROBE attack.  Remember that in both
 * attacks, we only need to time the spy and not the Trojan.
 *
 * Note that you may need to serialize execution wherever
 * appropriate.
 */
char spy()
{
    int i, max_set = 0, max_time = 0;
    uint64_t *eviction_set_addr;

    uint64_t start = 0, end = 0;

    int offset;

    // Probe the cache line by line and take measurements
    for (i = 0; i < L1_NUM_SETS; i++) {
        /* TODO:
         * Your attack code goes in here.
         *
         */  
        CPUID();
        RDTSC(start);
        eviction_set_addr = get_eviction_set_address(spy_array, i, 0);
        offset = eviction_set_addr - spy_array;
        while (spy_array[offset]) {
            offset = spy_array[offset];
        }
        CPUID();
        RDTSC(end);
        if (end - start > max_time) {
            max_time = end - start;
            max_set = i;
        }
    }
    eviction_counts[max_set]++;
}

int main()
{
    FILE *in, *out;
    in = fopen("transmitted-secret.txt", "r");
    out = fopen("received-secret.txt", "w");

    int j, k;
    int max_count = 0, max_set = 0;

    // TODO: CONFIGURE THIS -- currently, 32*assoc to force eviction out of L2
    setup(trojan_array, ASSOCIATIVITY*32);
// return 0;
    setup(spy_array, ASSOCIATIVITY);
    
    for (;;) {
        char msg = fgetc(in);
        if (msg == EOF) {
            break;
        }
        for (k = 0; k < SAMPLES; k++) {
          trojan(msg);
          spy();
        }
        for (j = 0; j < L1_NUM_SETS; j++) {
            if (eviction_counts[j] > max_count) {
                max_count = eviction_counts[j];
                max_set = j;
            }
            eviction_counts[j] = 0;
        }
        if (max_set >= 33 && max_set <= 59) {
            max_set += 32;
        } else if (max_set == 63) {
            max_set = -22;
        }
        fprintf(out, "%c", 32 + max_set);
        max_count = max_set = 0;
    }
    fclose(in);
    fclose(out);
}
