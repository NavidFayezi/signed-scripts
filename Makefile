HEADER_FILES := $(wildcard headers/*)
SOURCE_FILES := $(wildcard src/*)

binaries/out: $(HEADER_FILES) $(SOURCE_FILES)
	gcc $(SOURCE_FILES) -lcrypto -I headers -o binaries/out

binaries/test_server: headers/sockets.h src/sockets.c  tests/test_server.c
	gcc src/sockets.c tests/test_server.c -I headers -o binaries/test_server

binaries/test_client: headers/sockets.h src/sockets.c tests/test_client.c
	gcc src/sockets.c tests/test_client.c -I headers -o binaries/test_client

binaries/test_signature_correctness: headers/signature.h src/signature.c tests/test_signature_correctness.c
	gcc src/signature.c src/utils.c tests/test_signature_correctness.c -lcrypto  -I headers -o binaries/test_signature_correctness

test: binaries/test_server binaries/test_client binaries/test_signature_correctness
	./binaries/test_server &
	sleep 1
	./binaries/test_client
	./binaries/test_signature_correctness

keys:
	openssl genpkey -algorithm RSA -out keys/private_key.pem -pkeyopt rsa_keygen_bits:4096
	openssl rsa -pubout -in keys/private_key.pem -out keys/public_key.pem

clean:
	rm -f binaries/*
	rm -f keys/*


.PHONY: keys clean