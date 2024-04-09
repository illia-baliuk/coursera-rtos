#include <syslog.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>

#define NUM_THREADS 2

// SYSLOG prefix
char* prefix = "[COURSE:1][ASSIGNMENT:1]";

// POSIX thread declarations and scheduling attributes
pthread_t threads[NUM_THREADS];


/* myThread function: submits a message to the Syslog facility. */
void *myThread(void )
{
    syslog(LOG_CRIT, "%s Hello World from Thread!",prefix); // submits a message to the Syslog facility
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

   syslog(LOG_CRIT, "%s Hello World from Main!\n",prefix);  // submits a message to the Syslog facility

   // starts a new thread in the calling process.
   for(i=0; i < NUM_THREADS; i++)
   {
       pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      myThread,      // thread function entry point
                      (void *)0      // parameters to pass in
                     );
   }

   //waits for the threads specified by thread to terminate
   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);

   printf("Test complete\n");
}
