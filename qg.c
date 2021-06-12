/**
 * qg - NAT friendly copy for qemu guest
 *
 * usage:
 *
 * send:
 * qg <filename>
 *
 * receive:
 * qg
 *
 * Compile with:
 *   cc -o qg qg.c
 */

#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 12345
#define BUFSIZE 1024

int sockfd;
int fd;
char* buf;
int len;

struct sockaddr_in sa;
char* test="This is a test\n";
char filename[256];

main(argc, argv)
int argc;
char *argv[];
{
	buf = (char *)malloc(BUFSIZE);

	if (buf == NULL)
	{
		printf("Could not allocate %d bytes\n",BUFSIZE);
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		perror("Could not create socket");
		exit(1);
	}

	/* sa is in bss, so everything already zeroed */

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr("10.0.2.2");
	sa.sin_port = htons(PORT);
	
	if (connect(sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
	{
		perror("Could not connect to host");
		exit(1);
	}

	if (argc < 2) /* no filename, assume receive. */
	{
		len = read(sockfd, filename, sizeof(filename));
		if (len != sizeof(filename))
		{
			printf("Short read of filename by %d bytes.",len);
			close(sockfd);
			exit(1);
		}
		
		printf("%s\n",filename);

		fd = creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (fd < 0)
		{
			perror(filename);
			close(sockfd);
			exit(1);
		}

		while (len = read(sockfd, buf, sizeof(buf)))
			write(fd, buf, len);
		printf("%d\n",len);
	}
	else
	{
		fd = open(argv[1],O_RDONLY);
	
		if (fd < 0)
		{
			perror(argv[1]);
			close(sockfd);
			exit(1);
		}

		strcpy(filename, argv[1]);
		write(sockfd, filename, sizeof(filename));
	
		while (len = read(fd, buf, sizeof(buf)))
			write(sockfd, buf, len);
	}


	if (errno != 0)
		perror("Error");

	close(fd);
	close(sockfd);
	free(buf);		
}
