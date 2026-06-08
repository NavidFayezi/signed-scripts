HEADER_FILES := $(wildcard headers/*)
SOURCE_FILES := $(wildcard src/*)

binaries/out: $(HEADER_FILES) $(SOURCE_FILES)
	gcc $(SOURCE_FILES) -lcrypto -I headers -o binaries/out

binaries/test_server: headers/sockets.h src/sockets.c  tests/test_server.c
	gcc src/sockets.c tests/test_server.c -I headers -o binaries/test_server

binaries/test_client: headers/sockets.h src/sockets.c tests/test_client.c
	gcc src/sockets.c tests/test_client.c -I headers -o binaries/test_client

test: binaries/test_server binaries/test_client
	./binaries/test_server &
	sleep 1
	./binaries/test_client

clean:
	rm -f binaries/*