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

#define SAMPLES 6 // TODO: CONFIGURE THIS

// Hint: You can find these by looking up entries in /sys/devices/system/cpu/cpu0/cache
#define L1_CACHE_SIZE 32768// TODO: Find cache size
#define LINE_SIZE 64// TODO: Find line size
#define ASSOCIATIVITY 8// TODO: Find associativity

#define L1_NUM_SETS (L1_CACHE_SIZE/(LINE_SIZE*ASSOCIATIVITY))

#define NUM_OFFSET_BITS 6// TODO: Find number of offset bits
#define NUM_INDEX_BITS 6// TODO: Find number of index bits
#define NUM_OFF_IND_BITS (NUM_OFFSET_BITS + NUM_INDEX_BITS)

uint64_t eviction_counts[L1_NUM_SETS] = {0};
__attribute__ ((aligned (64))) unsigned char shared_array[L1_CACHE_SIZE];

/* TODO:
 *
 * This function implements the Trojan that sends a message
 * to the spy over the cache covert channel.  Note that the
 * message forgoes case sensitivity to maximize the covert
 * channel bandwidth.
 *
 * Your job is to flush the right line to mount an
 * appropriate FLUSH+RELOAD attack.  Remember
 * that in both these attacks, we only need to time the spy
 * and not the trojan.
 *
 * Note that you may need to serialize execution wherever
 * appropriate.
 */
void trojan(char byte)
{
    int set;

    if (byte >= 'a' && byte <= 'z') {
        byte -= 32;
    }
    if (byte == 10 || byte == 13) { // encode a new line
        set = 63;
    } else if (byte >= 32 && byte < 96) {
        set = (byte - 32);
    } else {
        printf("fr trojan: unrecognized character %c\n", byte);
        exit(1);
    }

    /* TODO:
     * Your attack code goes in here.
     *
     */  
    CLFLUSH(&shared_array[LINE_SIZE*set]);
    // int i, j;
    // unsigned char junk;
    // for (i = 0; i < L1_CACHE_SIZE; i+=LINE_SIZE)
    //     for (j = 0; j < 1; j++)
    //         CLFLUSH(&shared_array[i+j]);
    // for (i = 0; i < ASSOCIATIVITY; i++)
    //     junk = shared_array[i * LINE_SIZE * L1_NUM_SETS + LINE_SIZE * set];
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
 * Your job is to the reload each line and gather measurements
 * to identify the line flushed by the Trojan.  Remember
 * that in both attacks, we only need to time the spy 
 * and not the Trojan.
 *
 * Note that you may need to serialize execution wherever
 * appropriate.
 */
char spy()
{
    int i, max_set = 0, max_time = 0;

    uint64_t start = 0, end = 0;

    unsigned char junk;
    int j;
    // Reload the cache line by line and take measurements
    for (i = 0; i < L1_NUM_SETS; i++) {
        /* TODO:
         * Your attack code goes in here.
         *
         */  
        CPUID();
        RDTSC(start);
        junk = shared_array[i*LINE_SIZE];
        CPUID();
        RDTSC(end);
        if (max_time < end - start) {
            max_time = end - start;
            max_set = i;
        }
        /*
        * In every iteration of trying
        * Decrease L1D miss
        */
        dec_l1d_miss(1);
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
