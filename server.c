/*
    References : 
        https://gist.github.com/bsodhi/a24e06e6806b685fb4d633c40638e77b
        https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/
        Stackoverflow
        https://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html
*/

#include <sys/resource.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/un.h>
#include <stddef.h>
#include <semaphore.h>
#include "dynamic_library_loader.h"
#include "queue.h"

// Defining MUTEX to prevent race condition and CONDITION VARIABLE to prevent excessive CPU cycles
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

// Structure whose pointer is sent to Server Thread Function
typedef struct thread_data{
    Queue* q;
    int* client;
}server_thread_data;

// Function to log messages on terminal
void log_msg(const char *msg, bool terminate) {
    printf("%s\n", msg);
    if (terminate) exit(-1); /* failure */
}

/** Accept the data in the form of serialised string from the client 
* @param client_socket_file_descriptor
* @return Request Pointer
*/

Request* get_request(int sock_fd){

    // Notifying the client that data is being accepted
    char msg[100] = "Recieving Your Request";
    write(sock_fd, msg, sizeof(msg)); /* echo as confirmation */

    // Accept the number of strings to be accepted from the client
    int32_t tmp, len;
    read(sock_fd, &tmp, sizeof(tmp));
    len = ntohl(tmp);

    char buffer[1024];
    char **args = malloc(sizeof(char*)*(len-2));
    int n=1;

    // Accept the required number of strings from the client 
    Request* r = (Request*)malloc(sizeof(Request));
    for(int32_t i=0; i<len; i++){
        n = recv(sock_fd, buffer, 1024,0);
        printf("Getting ==> %s\n", buffer);
        if(i==0){
            r->dll_name = malloc(strlen(buffer));
            strcpy(r->dll_name, buffer);
        }
        else if(i==1){
            r->func_name = malloc(strlen(buffer));
            strcpy(r->func_name, buffer);
        }
        else{
            args[i-2] = malloc(strlen(buffer));
            strcpy(args[i-2], buffer);
        }
        memset(buffer, '\0', sizeof(buffer));
    }
    // printf("YES2");
    r->func_args = args;

    return r;
}

/** The server thread function (Enqueues the incoming requests in the queue using server thread pool)
 * @param server_thread_data_pointer( Having client descriptor and queue pointer) 
 * @return NULL
 */

void* server_thread_func(void *arg){
    server_thread_data* s = (server_thread_data*)arg;
    Queue* q = s->q;
    int* p = s->client;
    int client_fd = *p;
    free(arg);
    // free(p);
    
    // mutex lock to prevent race condition 
    pthread_mutex_lock(&mutex);

    // if queue is full send error msg to client and close its socket
    if(q->curr_cap==q->cap){
        char *msg = "Couldn't process, Request Queue full";
        write(client_fd, msg, sizeof(msg));
        close(client_fd);
    }

    // else Enqueue the request and client descriptor for further processing
    else{
        Request* r = get_request(client_fd);
        Data* d = createData(p, r);
        // printf("TRY => %d\n", *p);
        if(enqueue(q, d)<0){
            printf("Error occured while Enqueue");
            char msg[128] = "Error occured while Enqueue, Terminating";
            write(client_fd, msg, sizeof(msg));
            close(client_fd);
        }
        else{
            // conditional variable signals only when there are some requests in the queue
            pthread_cond_signal(&cond_var);
            char msg[100] = "Request Enqueued Successfully";
            write(client_fd, msg, sizeof(msg));
            // printf("server ==> %d\n", client_fd);
        }
    }

    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
    return NULL;
}

/** Function calls the DLL based on input and return the result obtained to the client
 * @param Data_Pointer (having reuest and client descriptor)
 * return NULL
 */
void* handle_dll_tasks(Data* d) {
    log_msg("SERVER: thread_function: starting", false);

    /*Todo Dispatcher*/
    
    char *ans = dll_func(d->req->dll_name, d->req->func_name, d->req->func_args); 
    // printf("client ==> %d\n", *(d->client_soc));
    printf("ANS => %s\n", ans);
    write(*(d->client_soc), ans, strlen(ans));
    close(*(d->client_soc)); 
    free(d);
    free(ans);
    log_msg("SERVER: Done", false);

    return NULL;
}

/** Dispatcher thread function (performs dequeue and call handle_tasks on the axquired data)
 * @param Queue_pointer
 * return NULL
 */
void* dispatcher_thread_func(void* q){

    while(1){
        Data* d;
        // Mutex Lock to prevent race condition
        pthread_mutex_lock(&mutex);
        if((d=dequeue((Queue*)q)) == NULL){
            // Conditional wait till the queue is empty to prevent burning excess cpu cycles even when there is no work
            pthread_cond_wait(&cond_var, &mutex);
            d = dequeue((Queue*)q);
        }
        pthread_mutex_unlock(&mutex);

        if(d)
            handle_dll_tasks(d);
    }
}

/** Create a TCP server socket using the socket file provided by the user
 * @param socket_file
 * @param port_no
 * @return server file descriptor
 */

int make_named_socket(const char *socket_file, int port_no) {
    printf("Creating AF_INET socket at path %s\n", socket_file);
    if (access(socket_file, F_OK) != -1) {
        log_msg("An old socket file exists, removing it.", false);
        if (unlink(socket_file) != 0) {
            log_msg("Failed to remove the existing socket file.", true);
        }
    }

    struct sockaddr_in name;
    // Create the socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        log_msg("Failed to create socket.", true);
    }

    // Bind a name to the socket
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = inet_addr("127.0.0.1");
    name.sin_port = htons(port_no);


    if (bind(sock_fd, (struct sockaddr *) &name, sizeof(name)) < 0) 
        log_msg("bind failed", 1);
    

    return sock_fd;
}

/** Server starts listening to requests and create two thread pools to handle tasks for server and despatcher seperately
 * @param socket_file
 * @param max_connections
 * @param port_no
 * @param max_threads
 * @param Queue_pointer
 */

void start_server_socket(char *socket_file, int max_connects, int port_no, int max_threads, Queue* q) {
    int sock_fd = make_named_socket(socket_file, port_no);

    /* listen for clients, up to MaxConnects */
    if (listen(sock_fd, max_connects) < 0) {
        log_msg("Listen call on the socket failed. Terminating.", true); /* terminate */
    }
    log_msg("Listening for client connections...\n", false);
    /* Listens indefinitely */
    pthread_t dispatcher_threads[max_threads];
    pthread_t server_threads[max_threads];

    // Create dispatcher thread pool waiting for dequeuing requests from the queue 
    for(int i=0; i<max_threads; i++)
        pthread_create(&dispatcher_threads[i], NULL, dispatcher_thread_func, q);
    
    int i=0;

    while (1) {
        //client address
        struct sockaddr_in caddr;

        // address length
        int len = sizeof(caddr);  

        printf("Waiting for incoming connections...\n");

        // accepts blocks
        int client_fd = accept(sock_fd, (struct sockaddr *) &caddr, &len);  

        if (client_fd < 0) {
            log_msg("accept() failed. Continuing to next.", 0);
            continue;
        }

        int *p = malloc(sizeof(int));
        *p = client_fd;

        server_thread_data *s = (server_thread_data*)malloc(sizeof(server_thread_data));
        s->q = q; // Queue pointer
        s->client = p; // client descriptor

        // server thread to handle incoming connections
        pthread_create(&server_threads[i++], NULL, server_thread_func, s);

        // once all threads are engaged, we wait till the threads finishes it task, before reassigning tasks
        if(i==max_threads){
            i=0;
            while(i<max_threads)
                pthread_join(server_threads[i++],NULL);
            
            i=0;
        }  
    } 
}

int main(int argc, char *argv[]) {
    if (argc < 6) {
        printf("Usage: %s  [Local socket file path] [Server Backlog] [Port No.] [No. of threads] [Size of Queue]\n", argv[0]);
        exit(-1);
    }

    // Defining our Request Queue
    int QUEUE_CAP = atoi(argv[5]);
    Queue* q = createQueue(QUEUE_CAP);

    start_server_socket(argv[1], atoi(argv[2]),atoi(argv[3]), atoi(argv[4]), q);

    return 0;
}