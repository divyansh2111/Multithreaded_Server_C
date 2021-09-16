#include "server_utils.h"
#include "queue.h"
#include "dynamic_library_loader.h"

int main(){

    // // Checking create named socket function
    if(0!=make_named_socket(8080, 1))
        printf("Testcase-1 Passed\n");
    else    
        printf("Testcase-1 Failed\n");

    // // Check createQueue Function
    Queue* q = createQueue(5);
    if(q && q->cap==5)
       printf("Testcase-2 Passed\n");
    else    
        printf("Testcase-2 Failed\n"); 

    int *cli = malloc(sizeof(int));
    *cli = 2;
    char *dll = malloc(64);
    strcpy(dll, "/lib/x86_64-linux-gnu/libm.so.6");
    char *func = malloc(10);
    strcpy(func, "sin");
    char **args = malloc(2);
    args[0] = malloc(5); args[1] = malloc(5);
    memset(args[0], 0, sizeof(args[0]));
    memset(args[1], 0, sizeof(args[1]));
    strcpy(args[0], "5.0");
    strcpy(args[1], "2.0");

    Request* req = createRequest(dll, func, args);

    // Check createRequest function
    if(!strcmp(req->dll_name, dll) && !(strcmp(req->func_name,func)) && !(strcmp(args[0], req->func_args[0])))
        printf("Testcase-3 Passed\n");
    else    
        printf("Testcase-3 Failed\n"); 

    Data* d = createData(cli, req);

    // Check createData function
    if(d->client_soc==cli && d->req==req)
        printf("Testcase-4 Passed\n");
    else    
        printf("Testcase-4 Failed\n");

    int enq = enqueue(q,d); 

    // Check whether enqueue is working 
    if(enq==1)
        printf("Testcase-5 Passed\n");
    else    
        printf("Testcase-5 Failed\n");

    for(int i=0; i<5; i++)
        enq = enqueue(q,d);
    
    // Check for max capacity limit of enqueue function
    if(enq==-1)
        printf("Testcase-6 Passed\n");
    else    
        printf("Testcase-6 Failed\n");

    // check working of dequeue function
    Data* data = dequeue(q);

    if(data)   
        printf("Testcase-7 Passed\n");
    else    
        printf("Testcase-7 Failed\n");

    for(int i=0; i<5; i++)
        data = dequeue(q);
    
    // Check for dequeue from empty queue
    if(data==NULL)   
        printf("Testcase-8 Passed\n");
    else    
        printf("Testcase-8 Failed\n");

    
    char* ans = dll_func(dll, func, args, 1);

    // checking for sin
    if(strcmp(ans,"-0.95892")==0)
        printf("Testcase-9 Passed\n");
    else    
        printf("Testcase-9 Failed\n");

    // checking for cos
    memset(func, 0, sizeof(func));
    strcpy(func, "cos");
    ans = dll_func(dll, func, args, 1);
    if(strcmp(ans,"0.28366")==0)
        printf("Testcase-10 Passed\n");
    else    
        printf("Testcase-10 Failed\n");

    // checking for tan
    memset(func, 0, sizeof(func));
    strcpy(func, "tan");
    ans = dll_func(dll, func, args, 1);
    if(strcmp(ans,"-3.3805")==0)
        printf("Testcase-11 Passed\n");
    else    
        printf("Testcase-11 Failed\n");

    // checking for pow
    memset(func, 0, sizeof(func));
    strcpy(func, "pow");
    ans = dll_func(dll, func, args,1);
    if(strcmp(ans,"25")==0)
        printf("Testcase-12 Passed\n");
    else    
        printf("Testcase-12 Failed\n");

    // checking for sinh
    memset(func, 0, sizeof(func));
    strcpy(func, "sinh");
    ans = dll_func(dll, func, args, 1);
    if(strcmp(ans,"74.203")==0)
        printf("Testcase-13 Passed\n");
    else    
        printf("Testcase-13 Failed\n");

    // checking for cosh
    memset(func, 0, sizeof(func));
    strcpy(func, "cosh");
    ans = dll_func(dll, func, args, 1);
    if(strcmp(ans,"74.21")==0)
        printf("Testcase-14 Passed\n");
    else    
        printf("Testcase-14 Failed\n");

    // checking for tanh
    memset(func, 0, sizeof(func));
    strcpy(func, "tanh");
    ans = dll_func(dll, func, args, 1);
    if(strcmp(ans,"0.99991")==0)
        printf("Testcase-15 Passed\n");
    else    
        printf("Testcase-15 Failed\n");

    // checking for log
    memset(func, 0, sizeof(func));
    strcpy(func, "log");
    ans = dll_func(dll, func, args, 1);
    if(strcmp(ans,"1.6094")==0)
        printf("Testcase-16 Passed\n");
    else    
        printf("Testcase-16 Failed\n");

    // checking for exp
    memset(func, 0, sizeof(func));
    strcpy(func, "exp");
    ans = dll_func(dll, func, args, 1);
    if(strcmp(ans,"148.41")==0)
        printf("Testcase-17 Passed\n");
    else    
        printf("Testcase-17 Failed\n");

    // checking for sqrt
    memset(func, 0, sizeof(func));
    strcpy(func, "sqrt");
    ans = dll_func(dll, func, args, 1);
    if(strcmp(ans,"2.2361")==0)
        printf("Testcase-18 Passed\n");
    else    
        printf("Testcase-18 Failed\n");
    
    // checking for unsupported function
    memset(func, 0, sizeof(func));
    strcpy(func, "abs");
    ans = dll_func(dll, func, args, 1);
    if(strcmp(ans,"Unsupported dll name or function name")==0)
        printf("Testcase-19 Passed\n");
    else    
        printf("Testcase-19 Failed\n");

    // checking for unsupported dll
    memset(dll, 0, sizeof(dll));
    strcpy(dll, "/lib/x86_64-linux-gnu/libm.so.5");
    ans = dll_func(dll, func, args, 1);
    if(strcmp(ans,"DLL not supported")==0)
        printf("Testcase-20 Passed\n");
    else    
        printf("Testcase-20 Failed\n");

    
    free(q); free(cli); free(req); free(d); 
    
    return 0;
}