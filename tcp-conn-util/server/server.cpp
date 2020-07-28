#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

#define  err_log(errlog) do{ perror(errlog); exit(1);}while(0)

int main(int argc, const char *argv[])
{
  int sockfd;
  int acceptfd;
  struct sockaddr_in  serveraddr, clientaddr;

  if(argc != 3)
  {
    fprintf(stderr,"Usage:%s server ip port.\n", argv[0]);
    return -1;
  }

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    err_log("fail to socket");
  }

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
  serveraddr.sin_port = htons(atoi(argv[2]));

  if(bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
  {
    err_log("fail to bind");
  }

  if(listen(sockfd, 15) < 0)
  {
    err_log("fail to listen");
  }

  socklen_t  addrlen = sizeof(struct sockaddr_in);
  acceptfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);

  if(acceptfd < 0)
  {
    err_log("fail to acceptfd");
  }

  printf("%s --> %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

  close(acceptfd);
  close(sockfd);

  return 0;
}
