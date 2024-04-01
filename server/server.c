#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "chacha20.h"
#include "protocol.h"
#include "../common.h"

static void
error(const char* msg)
{
  fprintf(stderr, "\n%s\n", msg);
  exit(1);
}

static struct sockaddr_in
server_addr;

static struct sockaddr_in
client_addr;

static int
open_udp()
{
  const int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0)
    error("error while crearing socket");

  memset(&server_addr, 0, sizeof(struct sockaddr_in));

  server_addr.sin_family      = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(IP);
  server_addr.sin_port        = htons(PORT);

  if(bind(sockfd, (const struct sockaddr*)&server_addr , sizeof(struct sockaddr_in)) < 0)
    error("error while binding");

  memset(&client_addr, 0, sizeof(struct sockaddr_in));

  socklen_t len = sizeof(struct sockaddr_in);
  char buffer[CON_LEN];

  recvfrom(sockfd, buffer, CON_LEN, 0,
           (struct sockaddr*)&client_addr, &len);
  return sockfd;
}

static int
read_file(const int fd, void* buffer, size_t size)
{
  int res = read(fd, buffer, size);

  if(res < 0)
    error("Reading from file failed");
  return res;
}

static int 
send_data(const int socketfd, const uint8_t* buffer, const size_t size)
{
  const int res = sendto(socketfd, buffer, size, MSG_CONFIRM,
                         (const struct sockaddr*) &client_addr, sizeof(struct sockaddr));
  if(res < 0)
    error("Error while sending");
  return res;
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
  uint32_t counter = encrypt_init();

  const int fd = open("data.txt", O_RDONLY);
  if(fd < 0 )
  {
    close(fd);
    error("Cannot open file");
  }

  const int socketfd = open_udp();
    counter_packet p;

  int res;
  while((res = read_file(fd, (void*)&p.data, DATA_SIZE)) != 0)
  {
    p.counter = counter;
    const size_t size = res + sizeof(p.counter);

    // debug print raw bytes
    print((uint8_t*)&p, size);

    counter = encrypt((void*)&p.data, res);
    send_data(socketfd, (void*)&p, size);

    // debug print enccrypted
    // print((uint8_t*)&p, size);
  }

  close(fd);
  close(socketfd);
}
