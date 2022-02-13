/*
 * CS 1652 Project 1 
 * (c) Jack Lange, 2020
 * (c) Amy Babay, 2022
 * (c) <Zhimin Li & Yuanyi Li>
 *
 * Computer Science Department
 * University of Pittsburgh
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define BUFSIZE 1024
#define FILENAMESIZE 100


static int
handle_connection(int sock)
{

    char * ok_response_f  = "HTTP/1.0 200 OK\r\n"        \
        					"Content-type: text/plain\r\n"                  \
        					"Content-length: %d \r\n\r\n";
    // treat as success
    char * notok_response = "HTTP/1.0 404 FILE NOT FOUND\r\n"   \
        					"Content-type: text/html\r\n\r\n"                       \
        					"<html><body bgColor=black text=white>\n"               \
        					"<h2>404 FILE NOT FOUND</h2>\n"
                            "</body></html>\n";
    int res = -1;
    int len = 0;
    int ok = 1;

    char fileName[FILENAMESIZE];
    char buf[BUFSIZE];
    if ((len = read(sock, buf, sizeof(buf) - 1)) <= 0) {
        perror("read error");
        ok = -1;
    }
    buf[BUFSIZE-1] = '\0';
    //breaks string str into a series of tokens using the delimiter delim to extract request and header
    char *url;
    const char s[2] = " "; //delim
    //first token: "GET"
    url = strtok(buf,s);
    //second token: host (filename)
    url = strtok(NULL,s);
    strcpy(fileName, url);
    FILE * f = fopen(fileName, "r+");
    if (f == NULL ){
        ok = -1; //file not found, notok_response
    }
    if (ok == 1) {
        if ((res = write(sock, ok_response_f, strlen(ok_response_f))) <= 0) {
            perror("write error");
        }
        //write read file to sock
        if ((res = write(sock, buf, len)) <= 0) {
            perror("write error");
        }
    } else {
        if ((res = write(sock, notok_response, strlen(notok_response))) <= 0) {
            perror("write error");
        }
    }
    close(sock);
    /* first read loop -- get request and headers*/

    /* parse request to get file name */
    /* Assumption: this is a GET request and filename contains no spaces*/

    /* open and read the file */

    /* send response */

    /* close socket and free pointers */
    return ok;
}


int
main(int argc, char ** argv)
{
    int server_port = -1;
    int ret         =  0;
    int sock        = -1;
    socklen_t clilen;
    /* parse command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: http_server1 port\n");
        exit(-1);
    }

    server_port = atoi(argv[1]);

    if (server_port < 1500) {
        fprintf(stderr, "INVALID PORT NUMBER: %d; can't be < 1500\n", server_port);
        exit(-1);
    }

    /* initialize and make socket */
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "socket error");
        exit(-1);
    }

    /* set server address */
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_port = htons(server_port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_family = AF_INET;
    /* bind listening socket */
    if ((bind(sock, (struct sockaddr *)&sa, sizeof(sa))) < 0) {
        perror( "tcp_server: bind error");
        exit(-1);
    }
    /* start listening */
    if (listen(sock, 32) < 0) {
        perror("tcp_server: listen error");
        exit(-1);
    }
    /* connection handling loop: wait to accept connection */
    clilen = sizeof(sa);
    int sock2;
    while (1) {
        if ((sock2 = accept(sock,(struct sockaddr *)&sa,&clilen)) < 0) {
            perror("fail to accept");
            exit(-1);
        }
        /* handle connections */
        ret = handle_connection(sock2);
        if (ret < 0) {
            perror("fail to handle connection");
            exit(-1);
        }
    }
}
