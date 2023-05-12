#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define PAGE_NUM 16

#ifndef LINE_SIZE
#define LINE_SIZE 64
#endif

unsigned char shared[PAGE_SIZE * PAGE_NUM];

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
  for (i = 0; i < PAGE_SIZE * PAGE_NUM; i++)
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
  for (i = 0; i < PAGE_NUM; i++) {
    j = rand() % 4096 + 4096 * i;
    shared[j] = rand();
  }
}

void dec_dtlb_read() {
  /*
  * Decrease DTLB read by reading the same page
  */
  int i, j;
  for (i = 0; i < PAGE_NUM; i++) {
    j = rand() % 4096;
    shared[j] = rand();
  }
}

void inc_store() {
  /*
  * Increase memory store
  */
  register int i;
  for (int i = 0; i < PAGE_SIZE; i++) {
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
  for (i = 0; i < PAGE_NUM; i++) {
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
  for (i = 0; i < PAGE_NUM; i++) {
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

void inc_branch_miss() {
  register int i;
  int j = 0;
  for (i = 0; i < 1000; i++)
    if (i % 4)
      j++;
}

void dec_branch_miss() {
  register int i;
  unsigned long long j = 0;
  for (i = 0; i < 1000; i++)
    j += i;
}

void inc_l1d_miss() {
  /*
  * Increase L1D miss by accessing different cache lines
  */
  register int i;
  for (i = 0; i < PAGE_NUM * PAGE_SIZE / LINE_SIZE; i++)
    _mm_clflush(&shared[i * LINE_SIZE]);
  for (i = 0; i < 1000; i++) {
    int j = rand() % (65536 / 64);
    shared[j * 64] = rand();
  }
}

void dec_l1d_miss(int n) {
  /*
  * Decrease L1D miss by access the same cache line
  */
    int i, j;
    unsigned char k = 0;
    for (i = 0; i < n; i++)
        for (j = 0; j < LINE_SIZE; j++) 
            k += shared[j];
    shared[rand() % LINE_SIZE] = k;
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