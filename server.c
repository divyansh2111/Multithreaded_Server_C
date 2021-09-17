#include <stdio.h>
#include <sys/resource.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "queue.h"
#include "dynamic_library_loader.h"
#include "resource_check.h"
#include "server_utils.h"

int main(int argc, char *argv[]) {

    if (argc < 6) {
        printf("Usage: %s [Port No.] [No. of threads] [Size of Queue] [Combined Memory Limit(MB)] [Combined File Limit]\n", argv[0]);
        exit(-1);
    }

    // Set Combined memory limit
    struct rlimit old_lim, lim, new_lim;
      
    // Setting new value
    lim.rlim_cur = 1024*1024;
    lim.rlim_max = atoi(argv[4])*1024*1024;
  
    // Setting limits
    if(setrlimit(RLIMIT_STACK, &lim) == -1)
        fprintf(stderr, "%s\n", strerror(errno));

    // Printing new limits
    if( getrlimit(RLIMIT_STACK, &new_lim) == 0)
        printf("New combined memory limit %ld bytes\n" , new_lim.rlim_max);
    else
        fprintf(stderr, "%s\n", strerror(errno));

    // Set Combined File Descriptor limit
  
    // Setting new value
    lim.rlim_cur = 1024;
    lim.rlim_max = atoi(argv[5])*1024;
  
      
    // Setting limits
    if(setrlimit(RLIMIT_NOFILE, &lim) == -1)
        fprintf(stderr, "%s\n", strerror(errno));
      
    // Printing new limits
    if( getrlimit(RLIMIT_NOFILE, &new_lim) == 0)
        printf("New combined file descriptor limit %ld * 1024\n" , new_lim.rlim_max / 1024);
    else
        fprintf(stderr, "%s\n", strerror(errno));

    // Defining our Request Queue
    int QUEUE_CAP = atoi(argv[3]);
    Queue* q = createQueue(QUEUE_CAP);

    start_server_socket(atoi(argv[1]),atoi(argv[2]), q);

    return 0;
}