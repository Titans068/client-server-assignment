//
// Created by admin on 11/25/21.
//

//include header files
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//set port as macro
#define PORT 5010

//main function
int main(int argc, char **argv) {
    //set char arrays to BUFSIZ (default buffer size) macro, equivalent to size of 8192
    char buf[BUFSIZ], buf2[BUFSIZ], message[BUFSIZ], serverReply[BUFSIZ];
    int SOCKET;
    //create struct variable describing an Internet socket address.
    struct sockaddr_in server;
    //this means the socket will be of IPv4 address and TCP. Returns a file descriptor for the new socket, or -1 for errors
    //If PROTOCOL is zero, one is chosen automatically. Returns a file descriptor for the new socket, or -1 for errors.
    SOCKET = socket(AF_INET, SOCK_STREAM, 0);
    //if socket creation is erroneous then print a message describing the meaning of the value of errno.
    if (SOCKET < 0) {
        perror("Could not create socket");
        return -1;
    }
    printf("Socket created\n");
    //set ip address
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    //ipv4 address
    server.sin_family = AF_INET;
    //converts the unsigned short integer hostshort from host byte order to network byte order
    server.sin_port = htons(PORT);
    //Open a connection on socket FD to peer at ADDR (which LEN bytes long). Return 0 on success, -1 for errors.
    if (connect(SOCKET, (struct sockaddr *) &server, sizeof(struct sockaddr_in)) < 0) {
        perror("Could not connect");
        return -1;
    }
    //Set sizeof(serverReply) bytes of addressof serverReply to 0.
    memset(&serverReply, 0, sizeof(serverReply));
    printf("Connected to server.\nEnter first number: ");
    //user input
    scanf("%s", buf);
    //flush input stream
    fflush(stdin);
    printf("Enter second number: ");
    scanf("%s", buf2);
    //concatenation
    strcat(buf, " ");
    strcat(buf, buf2);
    //copy src to dest
    strcpy(message, buf);
    //Send strlen(message) bytes of message to SOCKET.  Returns the number sent or -1.
    if (send(SOCKET, message, strlen(message), 0) < 0) {
        perror("Failed to send message");
        return -1;
    }
    //Read sizeof(serverReply) bytes into serverReply from SOCKET. Returns the number read or -1 for errors
    if (recv(SOCKET, serverReply, sizeof(serverReply), 0) < 0) {
        perror("Could not receive message");
        return -1;
    }
    printf("Server: %s", serverReply);
    //close the File Descriptor
    close(SOCKET);
}
