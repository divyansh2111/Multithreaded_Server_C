# Multithreaded Server in C with Dynamically Loaded Functions


## Problem Statement


The problem involves the development of an end to end system capable of dynamically invoking a function of user’s choice, passing the parameters provided by the user to the function and returning the final result to  the user. In this problem we need to develop two socket endpoints, that is, server and client. The server accepts the client’s request in a multithreaded fashion to handle multiple requests simultaneously and enqueue the requests in a queue. Then comes the dispatcher which handles the requests through another thread pool and invokes the DL loader which further loads the DL and the required function followed by performing the operations and sending the result back to the client. 


## Logic of the Program and File Structure


The various modules and their implementation is explained as follows:
* server_utils.h - This module comprises the server side logic including multithreading for accepting the requests from clients and implementation of the dispatcher. The stack memory and file descriptors of the entire program are limited based on the user input  value. First we create the AF-INET (tcp) socket with an IP address of 127.0.0.1 (localhost) and bind it to a user defined port number. Next we create an array of pthreads (threadpool) to handle multiple incoming requests simultaneously. The logic here is to create and assign new threads to each client request till max thread limit is reached. After that the program waits (keeps incoming requests in hold) till any of the threads completes its task and becomes free. Now the new requests are assigned to the free threads and the process continues. Inside their thread functions, the server first accepts the number of string arguments that are to be received and then it reads from the server that exact number of times to get all the info provided by the client.  Once it receives the necessary parameters of the request it enqueues it to the request queue. This is done within mutex lock to prevent simultaneous enqueue by multiple threads (race condition). 


For the Dispatcher part, another thread pool is created and inside each thread function there is an infinite while loop which tries to check whether there is any pending request on the queue. If there is any request present, it dequeues the request inside mutex lock and passes it to the Dynamic Library Loader module for further processing. In this implementation a conditional variable is also used to minimize the CPU cycles consumed by the thread while looking for requests in the queue. Conditional wait is called while the queue is empty and conditional signal is only called when there is an enqueue operation, thus signaling the threads that pending requests are there and then the dequeue is performed. 
* queue.h - In this module, the queue is implemented using a doubly linked list approach. Each node of the linked list consists of is client socket descriptor and the request parameters received from it. A maximum capacity is also set based on the user input. It has two functions:
   * enqueue which returns 1 for successful operation and 0 otherwise.
   * dequeue which returns the node data pointer for successful operation and NULL otherwise 


* dynamic_library_loader.h - This module helps in loading the Dynamic library and the corresponding function based on user input. The only supported DLL is “/lib/x86_64-linux-gnu/libm.so.6” and the supported functions are “sin”, “cos”, “tan”, “sinh”, “cosh”, “tanh”, “pow”, “sqrt”, “log” and “exp”. In case extra arguments are provided by the user, the required arguments are taken from the top neglecting the extra ones towards the end.


* server.c - It takes the necessary arguments from the command line passed by the user and calls the required functions from the server_utils.h to get the server up and running.


* client.c - It takes in the server port as input (IP address set to localhost) and creates an AF-INET socket for the client. Then it passes the necessary data in stream-wise encoded fashion to the server where it is decoded and used for further  calculations. It also receives status of the process from the server at various stages of request execution.


* unit_testing.c - It involves the sample testing of individual functions except those involving direct read or write from client to server or vice-versa. It helps in checking the correctness of each function.


* multi_client.bash & sequential_client.bash - First is a bash script to run the client command 50 times almost simultaneously in order to closely simulate multiple simultaneous incoming client requests and check the working of multithreading in the server. Second is a simple sequential request script.


## Steps to Compile and Run the Program


* First we can run the unit testing to check for working of most of the individual functions :
```
        gcc unit_testing.c -o unit_testing -pthread -ldl
        ./unit_testing
```

* We can start the server using the following commands :

```
        gcc  server.c -o server -pthread -ldl
./server {port no.} {threadpool size} {queue capacity} {total memory limit(MB)} {FD limit}
Eg.          ./server 8000 8 100 10 10
```

* In case of binding fail try another port_no. or use cmd “fuser 8000/tcp” to free the port.
* FD limit must be provided in terms of kilounits, like an input 10 will set limit to 10*1024.


* Now in a separate terminal we can run the following commands for the client
```
        gcc client.c -o client
        ./client {server port no} {dll name} {func name} {space separated params}
Eg.         ./client 8000 /lib/x86_64-linux-gnu/libm.so.6 pow 5 2
```

* We may also run the bash file for multiple client request after compiling client.c (gcc client.c -o client)
[cmd params like port no (default-8000) can be changed directly in the bash file]

```
        ./multi_client.bash
Or        time ./multi_client.bash    to get the time consumed.
```

We may run the same command for sequential_client.bash, and notice that it takes a longer time as compared to multi_client.bash.
* In case of permission denied, try “chmod u+x multi_client.bash” followed by the above commands.
* The print statements in the terminal for multi_client.bash are a bit jumbled due to simultaneous requests.
* Note that sequential_client.bash also sends the requests quickly but since the execution is quicker than the request interval, it simulates a kind of single thread setup.

