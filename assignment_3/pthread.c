#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/sysinfo.h>
#include <sys/types.h>


#define NUM_THREADS 128

// SYSLOG prefix
char* prefix = "[COURSE:1][ASSIGNMENT:3]";

/* Used as argument to counterThread() */
typedef struct
{
    int threadIdx;  // ID returned by pthread_create()
} threadParams_t;

// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];
pthread_attr_t rt_sched_attr[NUM_THREADS];
struct sched_param rt_param[NUM_THREADS];
struct sched_param main_param;


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
   int rc;
   int rt_max_prio;
   int i;
   int numberOfProcessors, coreid;
   pid_t mainpid;
   cpu_set_t threadcpu;
   
   
   uname();
   
   
   numberOfProcessors = get_nprocs_conf();
   
   mainpid=getpid();
   rt_max_prio = sched_get_priority_max(SCHED_FIFO);
   
   rc=sched_getparam(mainpid, &main_param);
   main_param.sched_priority=rt_max_prio;
   
   
   
   
   if(rc=sched_setscheduler(mainpid, SCHED_FIFO, &main_param) < 0)
	       perror("******** WARNING: sched_setscheduler. Run with SUDO\n");



   // starts a new thread in the calling process.
   for(i=0; i < NUM_THREADS; i++)
   {
       coreid=i%numberOfProcessors;
       
       CPU_ZERO(&threadcpu);
       CPU_SET(coreid, &threadcpu);
       
       rc=pthread_attr_init(&rt_sched_attr[i]);
       rc=pthread_attr_setinheritsched(&rt_sched_attr[i], PTHREAD_EXPLICIT_SCHED);
       rc=pthread_attr_setschedpolicy(&rt_sched_attr[i], SCHED_FIFO);
       rc=pthread_attr_setaffinity_np(&rt_sched_attr[i], sizeof(cpu_set_t), &threadcpu);
       
       rt_param[i].sched_priority=rt_max_prio-(i%(rt_max_prio-1))-1;
       rc=pthread_attr_setschedparam(&rt_sched_attr[i], &rt_param[i]);
       
       
       threadParams[i].threadIdx=i;
       
       pthread_create(&threads[i],                // pointer to thread descriptor
                      &rt_sched_attr[i],          // use AFFINITY AND SCHEDULER attributes
                      counterThread,              // thread function entry point
                      (void *)&(threadParams[i])  // parameters to pass in
                     );
   }

   // starts a new thread in the calling process.
   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);

   printf("Test complete\n");
}


