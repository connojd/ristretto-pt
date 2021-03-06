//#include <windows.h>
#include <stdio.h>
//#include <tchar.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
//#include <varargs.h>
#include <assert.h>
#include <string.h>
#include "rtrace.h"


#define NOP2 __asm NOP __asm NOP
#define NOP4 NOP2 NOP2
#define NOP8 NOP4 NOP4
#define NOP16 NOP8 NOP8
#define NOP32 NOP16 NOP16
#define NOP64 NOP32 NOP32
#define NOP256 NOP64 NOP64 NOP64 NOP64
#define NOP1K NOP256 NOP256 NOP256 NOP256
#define NOP4K NOP1K NOP1K NOP1K NOP1K

/** msgBuffer size */
#define BUFFERSIZE 256

char msgBuffer[BUFFERSIZE];

typedef void * MoreTopWindow;

unsigned long SIZE = 0x40;

void moreAllocate(char ** inlineStorage, char ** heapStorage)
{
  *heapStorage = (char*) malloc(4096*2);
}

unsigned char checkHalt(MoreTopWindow * destination)
{
  return 0;
}

int spin_test()
{
    int i;
    int ret = 0;

    for (i = 0; i < 10000; i++) {
        ret = i + 1;
    }

    return ret;
}

int straight_test()
{
    int i = 4;
    i = i % 2;
    return i;
}

int call_test()
{
    int j = 0;
    j++;
    j += straight_test();

    return j;
}

void moreTestCalcPi(MoreTopWindow * destination, long double *result,
		    unsigned long long iterations)
{
  unsigned long long i = 0; //used to track iterations

  /* Test prep */
  memset(result, 0, sizeof(long double));
  (*result) = 0.0;

  /* Test loop */
  for (i = 0; i <= iterations && !checkHalt(destination); ++i)
    {
      if (0 == (i % 10000000)) //adjust this value to update UI more frequently
	{
	  printf("Calculating ");
	  printf("Pi: %1.30Lf", (*result)*4);
	  printf(", at ");
	  printf("%llu", i);
	  printf(" iterations %lld\n", i);
	}
      /* Pi approximation: Sum from i->infinity of ((-1)^i)/(1+(i*2)) */
      if (0 == (i%2)) //intentionally sloppy to slow up calculation
	{
	  (*result) += ((float)1)/((float)(1+(i*2))); //casts keep the ints away
	}
      else
	{
	  (*result) -= ((float)1)/((float)(1+(i*2)));
	}
    }

  /* Results */
  if (!checkHalt(destination)) //Skip the results if test aborted
    {
      (*result) *= 4;
      printf("Done");
      printf("\n");
      printf("Pi: %1.30Lf", (*result));
      printf("\n");
    }
}

int main(int argc, char ** argv)
{
  char *storage = NULL;
  char *inlineStorage = NULL;
  char *heapStorage = NULL;
  struct trace *trace = NULL;

  printf("address of spin_test: %p\n", spin_test);
//  printf("address of straight_test: %p\n", straight_test);
//  printf("address of call_test: %p\n", call_test);

//  moreAllocate(&inlineStorage, &heapStorage);
//  storage = heapStorage;
  trace = ristretto_trace_start((char *)spin_test, (char *)spin_test + SIZE - 1);

//  call_test();
//  straight_test();
  spin_test();
//  moreTestCalcPi(NULL, (long double *)storage, 100000000);

  ristretto_trace_stop(trace);
  ristretto_trace_parse(trace);
  ristretto_trace_cleanup(trace);
//  free(heapStorage);
  return 0;
}
