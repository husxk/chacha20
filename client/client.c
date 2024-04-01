#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>

#include "chacha20.h"
#include "protocol.h"
#include "../common.h"

static struct sockaddr_in
server_addr;

static void
error(const char* msg)
{
  fprintf(stderr, "\n%s\n", msg);
  exit(1);
}

static int
open_udp()
{
  const int sockfd = socket(AF_INET, SOCK_DGRAM , 0);

  if(sockfd < 0 )
    error("error while creating socket");

  // set timeout
  struct timeval read_timeout;
  read_timeout.tv_sec = 0;
  read_timeout.tv_usec = 10;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

  memset(&server_addr, 0, sizeof(struct sockaddr_in));

 server_addr.sin_family      = AF_INET;
 server_addr.sin_addr.s_addr = inet_addr(IP);
 server_addr.sin_port        = htons(PORT);

  const char* msg = "Ok";
  sendto(sockfd, msg, strlen(msg), 0,
        (const struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));

 return sockfd;
}

static int
read_udp(const int socketfd, uint8_t* buffer, const int size)
{
  socklen_t len = sizeof(struct sockaddr_in);
  const int recv = recvfrom(socketfd, buffer, size, 0,
                            (struct sockaddr*)&server_addr, &len);

  if(recv < 0)
  {
    if(errno == EAGAIN || errno == EWOULDBLOCK)
      return 0;
    error("Error while receiving from server");
  }
  return recv;
}

static void
print(const uint8_t* data, const size_t size)
{
  for(size_t i = 0; i < size; i++)
    printf("%02X\n", data[i]);
}

int
main()
{
  const int socketfd = open_udp();

  counter_packet p;
  int res;

  while((res = read_udp(socketfd, (void*)&p, sizeof(counter_packet))) != 0)
  {
    const size_t data_size = res - sizeof(p.counter);

    // debug print encrypted
    //print((uint8_t*)&p, res);

    decrypt(p.data, data_size, p.counter);

    // debug print raw bytes
    print((uint8_t*)&p, res);
  }
  close(socketfd);
}
