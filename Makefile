
CC = g++ -Wno-write-strings
SERVER_FILE = multi_server.cpp
HTTP_SERVER_FILE = http_server.cpp

all: server client

server: $(SERVER_FILE) $(HTTP_SERVER_FILE)
	$(CC) $(SERVER_FILE) $(HTTP_SERVER_FILE) -g  -lpthread -o server

client: multi_client.cpp
	$(CC) multi_client.cpp -lpthread -o client

clean:
	rm -f server client
