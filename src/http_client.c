#include <netinet/in.h>
#include "include/sans.h"
#include <stdio.h>
#include <string.h>

// void get_response(int sock) 
// {
//  return 0;
// }
int http_client(const char* host, int port)
{
  char sMethod[10];
  char sPath[1000];

  //scanning the method from user
  //printf("Enter an method: ");
  scanf("%s", sMethod);
  if(strcmp(sMethod,"GET"))
  {
    printf("Invalid method ");
    return 0;
  }
  //printf("Enter an path: ");
  scanf("%s",sPath);
  // if(sPath[0]=='/')
  // {
  //   printf("Invalid Path format ");
  //   return 0;
  // }
  int sID=sans_connect(host,port,IPPROTO_TCP);
  // if(!sID)
  // {
  //   printf("connection not established");
  // }
  // else
  // {
  //   printf("connection Established");
  // }
  char buffer[1024];
  int len = snprintf(buffer, sizeof(buffer),
                      "%s /%s HTTP/1.1\r\n"
                      "Host: %s:%d\r\n"
                      "User-Agent: sans/1.0\r\n"
                      "Cache-Control: no-cache\r\n"
                      "Connection: close\r\n"
                      "Accept: */*\r\n"
                      "\r\n",
                      sMethod,sPath,host,port);
  buffer[len]='\0';
  sans_send_pkt(sID,buffer,sizeof(buffer)-1);
  int sBuffer=0;
  while((sBuffer=sans_recv_pkt(sID,  buffer, sizeof(buffer)-1))>0)
  {
    buffer[sBuffer]='\0';
    // buffer= buffer.replace(" ","");
    printf("%s",buffer);
  }
  sans_disconnect(sID);
	return 0;
}
