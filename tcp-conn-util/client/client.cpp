#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */
#include <unistd.h>

#include "time_util.h"

#define err_log(errlog) \
  do {                  \
    perror(errlog);     \
    exit(1);            \
  } while (0)

int main(int argc, const char *argv[]) {
  int sockfd;
  int acceptfd;
  struct sockaddr_in serveraddr, clientaddr;

  if (argc != 3) {
    fprintf(stderr, "Usage:%s serverip port.\n", argv[0]);
    return -1;
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    err_log("fail to socket");
  }

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
  serveraddr.sin_port = htons(atoi(argv[2]));

  int64_t start = TimeUtil::CurrentTimeInMicroseconds();

  if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
    err_log("fail to connect");
  }

  int64_t end1 = TimeUtil::CurrentTimeInMicroseconds();

  printf("connect cost: %ld\n", end1 - start);

  close(sockfd);

  int64_t end2 = TimeUtil::CurrentTimeInMicroseconds();

  printf("close cost: %ld\n", end2 - end1);
  printf("ttl cost: %ld\n", end2 - start);

  return 0;
}

// g++ --std=c++11 -I../utils client.cpp
