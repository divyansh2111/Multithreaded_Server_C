/*
    References : 
        https://gist.github.com/bsodhi/a24e06e6806b685fb4d633c40638e77b
        https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/
        Stackoverflow
        https://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html
*/

#ifndef server_utils

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/un.h>
#include <stddef.h>
#include <errno.h>
#include "queue.h"
#include "dynamic_library_loader.h"
#include "resource_check.h"

#define server_backlog 100

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
            printf("SERVER: Error occured while Enqueue");
            char msg[128] = "Error occured while Enqueue, Terminating";
            write(client_fd, msg, sizeof(msg));
            close(client_fd);
        }
        else{
            // conditional variable signals only when there are some requests in the queue
            pthread_cond_signal(&cond_var);
            char msg[100] = "Request Enqueued Successfully";
            write(client_fd, msg, sizeof(msg));
            log_msg("SERVER: Request Enqueued", false);
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
    
    char *ans = dll_func(d->req->dll_name, d->req->func_name, d->req->func_args, 0); 
    int *r = malloc(sizeof(int));
    // printf("client ==> %d\n", *(d->client_soc));
    printf("SERVER: Result obtained from DLL => %s\n", ans);
    char res[256] = "Result => ";
    strcat(res, ans);
    
    if(write(*(d->client_soc), res, strlen(res))<=0)
        *r=0;
    else    
        *r=1;

    close(*(d->client_soc)); 
    free(d);
    free(ans);
    // sleep(2);
    log_msg("SERVER: Done", false);
    
    return r;
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

        if(d){
            log_msg("SERVER: Request Dispatched. Calling DLL Now.", false);
            int *r = (int*)handle_dll_tasks(d);
            free(r);
        }
    }

    return NULL;
}

/** Create a TCP server socket using the socket file provided by the user
 * @param socket_file
 * @param port_no
 * @param flag (Tells whether or not to show the print statements)
 * @return server file descriptor
 */

int make_named_socket(int port_no, int flag) {
    if(!flag)
        printf("Creating AF_INET socket with ip 127.0.0.1 and port no. %d\n", port_no);

    struct sockaddr_in name;
    // Create the socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        if(!flag)
            log_msg("Failed to create socket.", true);
        else
            return 0;
    }

    // Bind a name to the socket
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = inet_addr("127.0.0.1");
    name.sin_port = htons(port_no);


    if (bind(sock_fd, (struct sockaddr *) &name, sizeof(name)) < 0) 
        if(!flag)
            log_msg("bind failed", 1);
        else
            return 0;    

    return sock_fd;
}

/** Server starts listening to requests and create two thread pools to handle tasks for server and despatcher seperately
 * @param socket_file
 * @param max_connections
 * @param port_no
 * @param max_threads
 * @param Queue_pointer
 */

void start_server_socket(int port_no, int max_threads, Queue* q) {
    int sock_fd = make_named_socket( port_no, 0);

    /* listen for clients, up to MaxConnects */
    if (listen(sock_fd, server_backlog) < 0) {
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

        printf("\nWaiting for incoming connections...\n");

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

#define server_utils
#endif