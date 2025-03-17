
#include <netinet/in.h>
#include <sys/socket.h>
#include "include/sans.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>


int verification(char *response)
{
    return (strncmp(response, "250", 3) == 0);
}

int smtp_agent(const char* host,int port)
{   
    char email[256];
    char path[256];
    scanf("%s", email);
    scanf("%s", path);
  
    // Buffer to store incoming data
    char buffer[1024];

    // Accept connection from the client
    int sID=sans_connect(host,port,IPPROTO_TCP);

    // Receive data packet from the client
    int number_of_bytes_received=sans_recv_pkt(sID,buffer,sizeof(buffer)-1);

    buffer[number_of_bytes_received]='\0';

    printf("%s",buffer);

    // Beginning the SMPTP agent protocol
    //declaring sBuffer char array
    char sBuffer[1024];

    // crafting the HELO msg
    snprintf(sBuffer, sizeof(sBuffer), "HELO %s \r\n", host);
    int bytes_transmitted=sans_send_pkt(sID,sBuffer,strlen(sBuffer));
    // printf(bytes_transmitted);
    number_of_bytes_received=sans_recv_pkt(sID,buffer,sizeof(buffer)-1);
    // printf(number_of_bytes_received);

    buffer[number_of_bytes_received]='\0';
    printf("%s",buffer);

    // print senders email
    snprintf(sBuffer, sizeof(sBuffer), "MAIL FROM: %s\r\n", email);
    bytes_transmitted=sans_send_pkt(sID,sBuffer,strlen(sBuffer));
    number_of_bytes_received=sans_recv_pkt(sID,buffer,sizeof(buffer));

    buffer[number_of_bytes_received]='\0';
    if(number_of_bytes_received<0 || !verification(buffer)){
        printf("MAIL FROM: failed");
        return 0;
    }
    printf("%s",buffer);


    // print  receivers email
    snprintf(sBuffer, sizeof(sBuffer), "RCPT TO: %s\r\n", email);
    bytes_transmitted=sans_send_pkt(sID,sBuffer,strlen(sBuffer));
    number_of_bytes_received=sans_recv_pkt(sID,buffer,sizeof(buffer));

    buffer[number_of_bytes_received]='\0';
    if(bytes_transmitted<0 || number_of_bytes_received<0 || !verification(buffer)){
        printf("reciever : command failed");
        sans_disconnect(sID);
        return 0;
    }
    printf("%s",buffer);


    //DATA
    snprintf(sBuffer, sizeof(sBuffer), "DATA\r\n");
    bytes_transmitted=sans_send_pkt(sID,sBuffer,strlen(sBuffer));
    number_of_bytes_received=sans_recv_pkt(sID,buffer,sizeof(buffer));

    buffer[number_of_bytes_received]='\0';
    if(strncmp(buffer, "354", 3) != 0){
        printf("DATA command is failed") ;
        sans_disconnect(sID);
        return 0;
    }
    printf("%s",buffer);
    //defining the file descriptor and opening it in read mode
    FILE *file_fd=fopen(path, "r");

    while(fgets(sBuffer, sizeof(sBuffer), file_fd)){
        if(sans_send_pkt(sID, sBuffer, strlen(sBuffer)) < 0){
            fprintf(stderr, "Failed sending email data.\n");
            fclose(file_fd);
            sans_disconnect(sID);
            return 0;
        }
    }

    fclose(file_fd);
    snprintf(sBuffer, sizeof(sBuffer), "\r\n.\r\n");
    if(sans_send_pkt(sID,sBuffer,strlen(sBuffer)) < 0)
    {
        fprintf(stderr, "Failed sending the termination string.\n");
        sans_disconnect(sID);
        return 0;
    }

    //getting the  response from the server
    number_of_bytes_received=sans_recv_pkt(sID,buffer,sizeof(buffer));
    if(number_of_bytes_received<0){
        printf("Failed in receiving the token message");
        sans_disconnect(sID);
        return 0;
    }
    buffer[number_of_bytes_received]='\0';
    printf("%s",buffer);

    //exiting   the mail sending process
    snprintf(sBuffer,sizeof(sBuffer),"QUIT\r\n");
    bytes_transmitted=sans_send_pkt(sID,sBuffer,strlen(sBuffer));
    number_of_bytes_received=sans_recv_pkt(sID,buffer,sizeof(buffer));
    buffer[number_of_bytes_received]='\0';
    printf("%s",buffer);

    sans_disconnect(sID);
    return -1;
}