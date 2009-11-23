/*
 * Filename      : main.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Sunday, June 4, 2006
 * Last Modified : Wednesday, January 9, 2008
 * Purpose       : Main entry point of epiard (server) codebase
 * Notes         :
 */

#include "includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "common.h"
#include "Engine/simulation.h"
#include "Graphics/font.h"
#include "Graphics/video.h"
#include "UI/ui.h"
#include "Utilities/archive.h"
#include "Utilities/log.h"
#include "Utilities/xml.h"

char dotted_ip[15];
int listener;
int client;
int port = 7386; // the port epiar communicates on
struct sockaddr_in sa;
socklen_t sa_len;

void signal_handler(int signum)
{
	switch(signum) {
	case SIGINT:
		printf("\nReceived interrupt signal. Existing.\n");
		close(client);
		close(listener);
		exit(0);
		break;
	default:
		printf("\nUnknown signal received. Ignoring.\n");
		break;
	}
}

int main(int argc, char *argv[])
{
	listener = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if (listener < 0) {
		printf("Unable to create listener socket: %s\n", strerror(errno));
		return 1;
	}

	sa_len = sizeof(sa);
	memset(&sa, 0, sa_len);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY); /* listen on all interfaces */

	if (bind(listener, (const sockaddr *)&sa, sa_len) < 0) {
		printf("Unable to bind to port %i: %s\n", port, strerror(errno));
		return 1;
	}

	if (listen(listener, 5) < 0) {
		printf("Unable to listen: %s\n", strerror(errno));
		return 1;
	}

	signal(SIGINT, signal_handler);

	for (;;) {
		char sendbuf[1024];
		int sent, length;
		FILE *uptime;

		client = accept(listener, (sockaddr *)&sa, &sa_len);
		if (client < 0) {
			printf("Unable to accept: %s\n", strerror(errno));
			close(listener);
			return 1;
		}

		inet_ntop(AF_INET, &sa.sin_addr, dotted_ip, 15);
		printf("Received connection from %s.\n", dotted_ip);

		uptime = popen("/usr/bin/uptime", "r");
		if (uptime == NULL) {
			strcpy(sendbuf, "Unable to read system's uptime.\n");
		} else {
			sendbuf[0] = '\0';
			fgets(sendbuf, 1023, uptime);
			pclose(uptime);
		}

		length = strlen(sendbuf);
		sent = 0;

		while (sent < length) {
			int amt;

			amt = write(client, sendbuf + sent, length-sent);
			if (amt <= 0) {
				if (errno == EINTR) {
					continue;
				} else {
					printf("Send error: %s\n", strerror(errno));
					break;
				}
			}

			sent += amt;
		}

		close(client);
	}

	return 0;
}

