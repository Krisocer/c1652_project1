/*
 * CS 1652 Project 1 
 * (c) Jack Lange, 2020
 * (c) Amy Babay, 2022
 * (c) <Student names here>
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
    (void) notok_response;
    int res = -1;
    int len = 0;
    int ok = 0;

    char buf[BUFSIZE];
    if ((len = recv(sock, buf, sizeof(buf) - 1,0)) <= 0) {
        perror("read error");
        ok = -1;
    }
    buf[len] = '\0';
    //breaks string str into a series of tokens using the delimiter delim to extract requested filename
    char *fn;
    const char s[2] = " "; //delim
    const char s2[2] = "/";
    //first token: "GET"
    fn = strtok(buf,s2);
    if(strcmp(fn,"GET ") != 0){
        ok = -1;
    }
    //second token: host (filename)
    fn = strtok(NULL,s);
    FILE * fp = fopen(fn, "rb");
    if (fp == NULL ) ok = -1;
    if (ok == 0) {
        //puts("test");
        if ((res = send(sock, ok_response_f, strlen(ok_response_f),0)) <= 0) {
            perror("write error");
        }
        //write read file to sock
        char fc[1024];
        while(fgets(fc, 1024, fp)){
            if ((res = send(sock, fc, strlen(fc),0) <= 0)){
                perror("write error");
            }
        }
        fclose(fp);
    }else{
        res = send(sock, notok_response, strlen(notok_response),0) <= 0;
        if(res < 0){ printf("send error3"); }
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
main(int argc, char ** argv) {
    int server_port = -1;
    int ret         =  0;
    int accept_sock = -1;
    //int income_sock;
    struct sockaddr_in saddr;
    //int client;
    //char buf[BUFSIZE];

    /* parse command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: http_server1 port\n");
        exit(-1);
    }

    server_port = atoi(argv[1]);

    if (server_port < 1500) {
        fprintf(stderr, "Requested port(%d) must be above 1500\n", server_port);
        exit(-1);
    }


    if ((accept_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("socket creation error");
        exit(-1);
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(server_port);

    if (bind(accept_sock, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        printf("bind error");
        exit(-1);
    }
    printf("server is now bound to port %d\n", server_port);

    if (listen(accept_sock, 4) < 0) {
        printf("listen error");
        exit(-1);
    }
    puts("listening for incoming connections");

    /* initialize and make socket */

    /* set server address */

    /* bind listening socket */

    /* start listening */

    /* connection handling loop: wait to accept connection */
    fd_set fds, temp_fds;
    int max_fd = accept_sock;
    FD_ZERO(&fds);
    FD_ZERO(&temp_fds);
    FD_SET(accept_sock, &fds);

    while(1){
        //puts("test");
        temp_fds = fds;
        int s = select(max_fd+1, &temp_fds, 0,0,0);
        if(s <= 0){
            printf("select error");;
        }else{
            for(int i = 0; i < max_fd+1; i++){
                if(FD_ISSET(i,&temp_fds)){
                    if(i == accept_sock) {// accept the accept socket
                        //clen = sizeof(caddr);
                        int client = accept(accept_sock, NULL, NULL);
                        if(client < 0) {printf("accept error");}
                        if (client > max_fd) max_fd = client;
                        FD_SET(client, &fds);
                    }else{// handle the connection socket
                        ret = handle_connection(i);
                        if(ret < 0){ puts("file not found or unknown command");}
                        close(i);
                        FD_CLR(i, &fds);
                    }
                }
            }
        }
        //printf("test");
        /* create read list */

        /* do a select() */

        /* process sockets that are ready:
         *     for the accept socket, add accepted connection to connections
         *     for a connection socket, handle the connection
         */
    }
}
