#include <syslog.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>

#define NUM_THREADS 128

// SYSLOG prefix
char* prefix = "[COURSE:1][ASSIGNMENT:2]";

/* Used as argument to counterThread() */
typedef struct
{
    int threadIdx;  // ID returned by pthread_create()
} threadParams_t;

// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

/* myThread function: submits a message to the Syslog facility. */
void *counterThread(void *threadp)
{
    int sum=0, i;
    threadParams_t *threadParams = (threadParams_t *)threadp; //  ????

    // calculate
    for(i=1; i < (threadParams->threadIdx)+1; i++)
        sum=sum+i;

    // submits a message to the Syslog facility
    syslog(LOG_CRIT, "%s Thread idx=%d, sum [0..%d]=%d\n",
		    prefix, threadParams->threadIdx,
		    threadParams->threadIdx, sum);
}

/* myThread function: reseive information about the operating system name and system hardware
   and submits it to the Syslog facility. */
void uname(void)
{
   FILE* pstream;  // a pointer to the pipestream
   char buffer[200];  // buffer for pip stream output
   pstream = popen("uname -a","r");  // pipe stream to or from a process
   fread(buffer,sizeof(char),200,pstream);  // read pipe stream output
   syslog(LOG_CRIT, "%s %s", prefix, buffer);  // submits a message to the Syslog facility
}

/* entry point to the program */
int main (int argc, char *argv[])
{
   int i;
   uname();

   // starts a new thread in the calling process.
   for(i=0; i < NUM_THREADS; i++)

   {
       threadParams[i].threadIdx=i;
       pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      counterThread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );
   }

   // starts a new thread in the calling process.
   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);

   printf("Test complete\n");
}


