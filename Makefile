HEADER_FILES := $(wildcard headers/*)
SOURCE_FILES := $(wildcard src/*)

binaries/out: $(HEADER_FILES) $(SOURCE_FILES)
	gcc src/hash.c src/main.c -lcrypto -I headers -o binaries/out

clean:
	rm -f binaries/*