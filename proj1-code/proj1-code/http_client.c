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

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE 1024

int 
main(int argc, char ** argv) 
{

    char * server_name = NULL;
    int    server_port = -1;
    char * server_path = NULL;
    char * req_str     = NULL;

    int ret = 0;

    /*parse args */
    if (argc != 4) {
        fprintf(stderr, "usage: http_client <hostname> <port> <path>\n");
        exit(-1);
    }

    server_name = argv[1];
    server_port = atoi(argv[2]);
    server_path = argv[3];
    
    /* Create HTTP request */
    ret = asprintf(&req_str, "GET %s HTTP/1.0\r\n\r\n", server_path);
    if (ret == -1) {
        fprintf(stderr, "Failed to allocate request string\n");
        exit(-1);
    }

//    /*
//     * NULL accesses to avoid compiler warnings about unused variables
//     * You should delete the following lines
//     */
//    (void)server_name;
//    (void)server_port;

    /* make socket */
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "socket error");
        exit(-1);
    }
    struct hostent *hp;
    if((hp = gethostbyname(server_name)) == NULL){
        fprintf(stderr, "server name error");
        exit(-1);
    }

    //give address
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy(&saddr.sin_addr.s_addr, hp->h_addr, hp->h_length);
    saddr.sin_port = htons(server_port);
    printf("Trying %s...\n", inet_ntoa(saddr.sin_addr));

    if(connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0){
        perror("could not connect to server");
        exit(-1);
    }
    printf("connected to %s\n", server_name);

    req_str = malloc(100);
    sprintf(req_str, "GET /%s HTTP/1.0\r\n\r\n",server_path);

    int res;
    int len = strlen(req_str);
    if((res = send(sock, req_str, len, 0)) <= 0){
        perror("send error");
        exit(-1);
    }
    printf("Request made: %s\n",req_str);

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    select(sock+1, &fds, NULL, NULL, NULL);

    char buf[BUFSIZE];
    char header[BUFSIZE];
    char bufcpy[BUFSIZE];
    int head_end = 0;
    while(res > 0 && head_end == 0){
        res = recv(sock, buf, BUFSIZE-1, 0);
        for(int i = 0; i < BUFSIZE; i++){
            if(buf[i] == '\r' && buf[i+1] == '\n' && buf[i+2] == '\r' && buf[i+3] == '\n'){
                head_end = 1;
                strncpy(header,&buf[0],i);
                strcpy(bufcpy,&buf[i]);
            }
        }
        buf[res] = '\0';
    }
    char OK_check[3];
    strncpy(OK_check,&header[9],3);
    if(atoi(OK_check) == 200){
        printf("%s\n",header);
        printf("%s", bufcpy);
        while(res > 0){
            res = recv(sock, buf, BUFSIZE-1, 0);
            buf[res] = '\0';
            printf("%s",buf);
        }
        close(sock);
        return 0;
    }else{
        //printf("test");
        printf("%s\n",header);
        close(sock);
        return -1;
    }
    /* get host IP address  */
    /* Hint: use gethostbyname() */

    /* set address */

    /* connect to the server */

    /* send request message */

    /* wait for response (i.e. wait until socket can be read) */
    /* Hint: use select(), and ignore timeout for now. */

    /* first read loop -- read headers */

    /* examine return code */   
    // Skip protocol version (e.g. "HTTP/1.0")
    // Normal reply has return code 200

    /* print first part of response: header, error code, etc. */

    /* second read loop -- print out the rest of the response: real web content */

    /* close socket */

}
