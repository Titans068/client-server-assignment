//
// Created by admin on 11/25/21.
//

//include header files
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdbool.h>

//set port as macro
#define PORT 5010

//main function
int main(int argc, char *argv[]) {
    //set char arrays to BUFSIZ (default buffer size) macro, equivalent to size of 8192
    char msg[BUFSIZ], reply[BUFSIZ];
    //create struct variable describing an Internet socket address.
    struct sockaddr_in server;
    int SOCKET, ACCEPT, READ, sockSize, num1, num2, option = 1, maxClients = 30,
            h, clientSocket[maxClients], maxsd, sd, SELECT;
    //File Descriptor set (typedef struct)
    fd_set readfds;
    //Assign descriptors to 0
    for (h = 0; h < maxClients; h++)
        clientSocket[h] = 0;

    //this means the socket will be of IPv4 address and TCP. Returns a file descriptor for the new socket, or -1 for errors
    //If PROTOCOL is zero, one is chosen automatically. Returns a file descriptor for the new socket, or -1 for errors.
    SOCKET = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKET == -1) {
        perror("Could not create socket");
        return -1;
    }
    //Set socket FD's option OPTNAME at protocol level LEVEL to *OPTVAL (which is OPTLEN bytes long).
    //Returns 0 on success, -1 for errors.
    if (setsockopt(SOCKET, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
        perror("Could not set OPTNAME");
        return -1;
    }
    //ipv4 address
    server.sin_family = AF_INET;
    //INADDR_ANY - address to accept any incoming messages
    server.sin_addr.s_addr = INADDR_ANY;
    //converts the unsigned short integer hostshort from host byte order to network byte order
    server.sin_port = htons(PORT);
    printf("Created socket.\n");
    // When a socket is created with socket(), it exists in a name space (address family)
    // but has no address assigned to it. bind() assigns  the address  specified  by addr
    // to the socket referred to by the file descriptor SOCKET. addrlen specifies the size,
    // in bytes, of the  address structure  pointed to by addr. On success, zero is returned.
    // On error, -1 is returned, and errno is set appropriately.
    if (bind(SOCKET, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Could not bind");
        return -1;
    }
    //Prepare to accept connections on socket FD.
    //1 connection request will be queued before further requests are refused.
    //Returns 0 on success, -1 for errors.
    if (listen(SOCKET, 1) < 0) {
        perror("Listen failed");
        return -1;
    }
    printf("Server is listening.\n");
    sockSize = sizeof(struct sockaddr_in);
    //infinite loop
    while (true) {
        //This macro removes all file descriptors from set.
        FD_ZERO(&readfds);
        //Sets the bit for the file descriptor SOCKET in the file descriptor set &readfds.
        FD_SET(SOCKET, &readfds);
        maxsd = SOCKET;
        //loop through clients
        for (h = 0; h < maxClients; h++) {
            sd = clientSocket[h];
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > maxsd) maxsd = sd;
        }
        //Check the first NFDS descriptors each in READFDS (if not NULL) for read
        //readiness, in WRITEFDS (if not NULL) for write readiness, and in EXCEPTFDS
        //(if not NULL) for exceptional conditions.  If TIMEOUT is not NULL, time out
        //after waiting the interval specified therein.  Returns the number of ready
        //descriptors, or -1 for errors.
        SELECT = select(maxsd + 1, &readfds, NULL, NULL, NULL);
        //errno - number of last error
        if ((SELECT < 0) && (errno != EINTR))
            perror("select error");

        // select()  modifies  the  contents  of  the sets according to the
        // rules described below.  After calling select(),  the  FD_ISSET()
        // macro  can be used to test if a file descriptor is still present
        // in a set.  FD_ISSET() returns nonzero if the file descriptor  fd
        // is present in set, and zero if it is not.
        if (FD_ISSET(SOCKET, &readfds)) {
            //Await a connection on socket FD. When a connection arrives, open a new socket to communicate with it,
            //set *ADDR (which is *ADDR_LEN bytes long) to the address of the connecting
            //peer and *ADDR_LEN to the address's actual length, and return the new socket's descriptor, or -1 for errors.
            ACCEPT = accept(SOCKET, (struct sockaddr *) &server, (socklen_t *) &sockSize);
            if (ACCEPT < 0) {
                perror("Could not accept client");
                return -1;
            }
            //loop for available clients to accept connection
            for (h = 0; h < maxClients; h++) {
                if (clientSocket[h] == 0) {
                    clientSocket[h] = ACCEPT;
                    break;
                }
            }
            printf("Client has joined the server.\n");
        }
        for (h = 0; h < maxClients; h++) {
            sd = clientSocket[h];
            if (FD_ISSET(sd, &readfds)) {
                //Read sizeof(msg) into msg from sd.  Return the number read, -1 for errors or 0 for EOF.
                READ = read(sd, msg, sizeof(msg));
                if (READ == -1) {
                    perror("Could not receive message");
                    return -1;
                }
                if (READ == 0) {
                    printf("Client disconnected\n");
                    //flush output stream
                    fflush(stdout);
                    //set File Descriptor to 0
                    clientSocket[h] = 0;
                }
                //Divide char* into tokens separated by delimiters
                if (sscanf(msg, "%d %d", &num1, &num2) == 2) {
                    printf("Client: %d, %d\n", num1, num2);
                    //write formatted output to reply
                    sprintf(reply, "%d\n", num1 + num2);
                    //Write strlen(reply) bytes of reply to sd. Return the number written, or -1.
                    if (write(sd, reply, strlen(reply)) < 0) {
                        perror("Could not send message");
                        return -1;
                    }
                    //Set sizeof(reply) bytes of addressof reply to 0.
                    memset(&reply, 0, sizeof(reply));
                } else {
                    //reply informing client that they sent a string that cannot be converted to an integer
                    printf("Conversion error.\n");
                    strcpy(reply, "Conversion error.\n");
                    if (write(sd, reply, strlen(reply)) < 0) {
                        perror("Could not send message");
                        return -1;
                    }
                }
            }
        }
    }
}
