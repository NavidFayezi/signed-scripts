HEADER_FILES := $(wildcard headers/*)
SOURCE_FILES := $(wildcard src/*)

binaries/out: $(HEADER_FILES) $(SOURCE_FILES)
	gcc $(SOURCE_FILES) -lcrypto -I headers -o binaries/out

clean:
	rm -f binaries/*