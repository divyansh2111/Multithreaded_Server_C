/*
    References : 
        https://gist.github.com/bsodhi/a24e06e6806b685fb4d633c40638e77b
        https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/un.h>
#include <stddef.h>

// Log Message Function
void log_msg(const char *msg, bool terminate) {
    printf("%s\n", msg);
    if (terminate) exit(-1); /* failure */
}

/** Create a TCP client socket using the socket file provided by the user
 * @param socket_file
 * @param port_no
 * @return client file descriptor
 */
int make_named_socket(const char *socket_file, int port_no) {
    printf("Creating AF_LOCAL socket at path %s\n", socket_file);
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


    if (connect(sock_fd, (struct sockaddr *) &name, sizeof(name)) < 0) 
        log_msg("connect failed", 1);

    return sock_fd;
}

/** Client connects and send data to server
 * @param total_strings_to_be_sent
 * @param string_array
 * @param socket_file
 * @param server_port_no
 */
void send_message_to_socket(int argc, char *argv[], char *socket_file, int port_no) {
    int sockfd = make_named_socket(socket_file, port_no);

    log_msg("CLIENT: Connect to server, about to write some stuff...", false);
    
    // Recieves msg from server regarding whether the request can be enqueued or not.
    char buffer[1024];
    memset(buffer, '\0', sizeof(buffer));
    if (read(sockfd, buffer, sizeof(buffer)) > 0) {
            printf("CLIENT: Received from server:: %s\n", buffer);
        }

    // sending the no_of_arguments to be recieved at the server side 
    uint32_t len = htonl(argc-3);
    write(sockfd, &len, sizeof(len));

    // Sending actual string inputs to the server
    for(int i=3; i<argc; i++){
        memset(buffer, '\0', sizeof(buffer));
        strcpy(buffer, argv[i]);
        int n = send(sockfd, buffer, 1024, 0);
        printf("Sending ==> %s\n", buffer);
    }

    // Recieving Enqueue confirmation from server
    memset(buffer, '\0', sizeof(buffer));
    if (read(sockfd, buffer, sizeof(buffer)) > 0) {
            printf("CLIENT: Received from server:: %s\n", buffer);
        }
    
    // Recieving final result msg from server
    memset(buffer, '\0', sizeof(buffer));
    if (read(sockfd, buffer, sizeof(buffer)) > 0) {
        printf("CLIENT: Received from server:: %s\n", buffer);
    }
        

    log_msg("CLIENT: Processing done, about to exit...", false);
    close(sockfd); 
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s [Local socket file path] [server port] [Name of a dynamically loaded library] \n [Name of a function to call from the DLL] [Arguments to pass to the function(Space Seperated)]\n",
               argv[0]);
        exit(-1);
    }

    send_message_to_socket(argc, argv, argv[1], atoi(argv[2]));
    return 0;
}