#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <arpa/inet.h>

void read_cb(struct bufferevent *bev, void *ctx) {
	struct evbuffer *input = bufferevent_get_input(bev);
	size_t len = evbuffer_get_length(input);
	char *data = malloc(len + 1);
	
	if (data == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	
	evbuffer_copyout(input, data, len);
	data[len] = '\0';
	
	printf("Received from server: %s\n", data);
	
	free(data);
}

void event_cb(struct bufferevent *bev, short events, void *ctx) {
	if (events & BEV_EVENT_CONNECTED) {
		printf("Connected to the server. You can start typing.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Connection error or server closed the connection.\n");
		bufferevent_free(bev);
	}
}

void stdin_read_cb(evutil_socket_t fd, short events, void *arg) {
	char msg[256];
	struct bufferevent *bev = (struct bufferevent *)arg;
	
	if (fgets(msg, sizeof(msg), stdin) == NULL) {
		// Handle error or EOF
		event_base_loopbreak(bufferevent_get_base(bev));
		return;
	}
	
	// Remove newline character
	size_t len = strlen(msg);
	if (len > 0 && msg[len - 1] == '\n') {
		msg[len - 1] = '\0';
	}
	
	// Send the message to the server
	bufferevent_write(bev, msg, strlen(msg));
}

int main() {
	struct event_base *base = event_base_new();
	if (base == NULL) {
		perror("event_base_new");
		exit(EXIT_FAILURE);
	}
	
	struct bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	if (bev == NULL) {
		perror("bufferevent_socket_new");
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	
	// Connect to the server
	if (bufferevent_socket_connect(bev, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bufferevent_socket_connect");
		exit(EXIT_FAILURE);
	}
	
	// Set callbacks
	bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	
	// Set up stdin as an event source
	struct event *ev_stdin = event_new(base, fileno(stdin), EV_READ | EV_PERSIST, stdin_read_cb, bev);
	if (ev_stdin == NULL) {
		perror("event_new for stdin");
		exit(EXIT_FAILURE);
	}
	event_add(ev_stdin, NULL);
	
	// Enter the event loop
	event_base_dispatch(base);
	
	// Clean up
	event_free(ev_stdin);
	bufferevent_free(bev);
	event_base_free(base);
	
	return 0;
}

