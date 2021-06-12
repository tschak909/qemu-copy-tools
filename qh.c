/**
 * qh - NAT friendly copy for qemu hosts
 *
 * usage:
 *
 * send:
 * qh <filename>
 *
 * receive:
 * qh
 *
 * Compile with:
 *   cc -o qh qh.c
*/

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define PORT 12345
#define BUFSIZE 1024

char filename[256];
char buf[1024];

int fd, servfd, clifd, calen, len;

struct sockaddr_in sa, ca;

int main(int argc, char* argv[])
{
  servfd = socket(AF_INET, SOCK_STREAM, 0);
  if (servfd < 0)
    {
      perror("Could not create socket");
      exit(1);
    }

  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  sa.sin_port = htons(PORT);

  if ((bind(servfd, (struct sockaddr *)&sa, sizeof(sa))) != 0)
    {
      perror("Could not bind socket");
      exit(1);
    }

  if ((listen(servfd, 5)) != 0)
    {
      perror("Could not listen");
      exit(1);
    }
  else
    printf("%s: Waiting for connection.\n",argv[0]);
  calen = sizeof(ca);
  
  clifd = accept(servfd, (struct sockaddr *)&ca, &calen);

  if (argc < 2) /* no filename - assume recieve from guest */
    {
      len = read(clifd, &filename, sizeof(filename));
      if (len != sizeof(filename))
	{
	  printf("Read %d bytes for filename, aborting.\n",len);
	  close(clifd);
	  close(servfd);
	  exit(1);
	}
      
      fd = creat(filename,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      
      while (len = read(clifd, buf, sizeof(buf)))
	write(fd, buf, len);
      
      if (errno != 0)
	perror("Aborted");
      
    }
  else /* Send to guest */
    {
      strcpy(filename, argv[1]);
      write(clifd, filename, sizeof(filename));

      fd = open(argv[1],O_RDONLY);
      if (fd < 0)
	{
	  perror("Could not open file");
	  close(clifd);
	  close(servfd);
	  exit(1);
	}

      while (len = read(fd, buf, 1024)) 
	write(clifd, buf, len);
    }
  
  close(fd);
  close(clifd);
  close(servfd);

  return(0);
}
