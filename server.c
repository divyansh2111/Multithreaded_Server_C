#include "queue.h"
#include "dynamic_library_loader.h"
#include "resource_check.h"
#include "server_utils.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s [Port No.] [No. of threads] [Size of Queue]\n", argv[0]);
        exit(-1);
    }

    // Defining our Request Queue
    int QUEUE_CAP = atoi(argv[3]);
    Queue* q = createQueue(QUEUE_CAP);

    start_server_socket(atoi(argv[1]),atoi(argv[2]), q);

    return 0;
}