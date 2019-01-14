BUILD_PATH=$(PWD)/build
CC=gcc
RM=rm -f
CCFLAGS=-c -g -pthread -std=gnu99 -I$(BUILD_PATH)/libcurl_install/include -I$(PWD)/src
LDFLAGS=-L$(BUILD_PATH)/libcurl_install/lib
LDLIBS=-lcurl -lm -lssl -lcrypto -lz

# latest Curl release
LIBCURL_VERSION := 7.63.0

COMMON_OBJ=src/urlconntest.o src/util.o
ALL_SRC=src/urlconntest.c src/sktest.c src/util.c
ALL_HDR=src/urlconntest.h src/sktest.h src/util.h
DEPS=$(BUILD_PATH)/libcurl_install/lib/libcurl.a

all: sktest


# Pull Curl binary and install 
# Disclaimer/credit: libcurl install taken from https://github.com/sabyahsan/Youtube-test/
# 			  		 to speed up time.
$(BUILD_PATH)/curl-$(LIBCURL_VERSION).tar.bz2:
	mkdir -p build
	curl -L http://curl.haxx.se/download/curl-$(LIBCURL_VERSION).tar.bz2 -o $@

$(BUILD_PATH)/curl-$(LIBCURL_VERSION)/configure: $(BUILD_PATH)/curl-$(LIBCURL_VERSION).tar.bz2
	test -d $@ || ( /bin/tar -C build -xf $<; touch $@ )
	rm $(BUILD_PATH)/curl-$(LIBCURL_VERSION)/src/tool_hugehelp.c

$(BUILD_PATH)/libcurl/Makefile: $(BUILD_PATH)/curl-$(LIBCURL_VERSION)/configure
	mkdir -p $(BUILD_PATH)/libcurl/
	cd $(BUILD_PATH)/libcurl/; \
	../curl-$(LIBCURL_VERSION)/configure --disable-shared --enable-static --prefix=/ --libdir=/lib --with-ca-bundle=/etc/ca-bundle.crt --enable-ipv6 --disable-manual --disable-dict --disable-file --disable-file --disable-ftp --disable-gopher --disable-imap --disable-pop3 --disable-rtsp --disable-smtp --disable-telnet --disable-tftp

$(BUILD_PATH)/libcurl_install/lib/libcurl.a: $(BUILD_PATH)/libcurl/Makefile
	mkdir -p $(BUILD_PATH)/libcurl_install/
	cd $(BUILD_PATH)/libcurl/; \
	$(MAKE) install DESTDIR=$(BUILD_PATH)/libcurl_install/	


clean:
	$(RM) -f sktest sktest.o src/sktest.o $(COMMON_OBJ)

.c.o: $(ALL_HDR) $(DEPS)
	$(CC) $(CCFLAGS) $*.c $(LDFLAGS) $(LDLIBS) -o $*.o 

# Compile and run (remove ./sktest if you dont wish to run after make)
sktest: $(DEPS) $(COMMON_OBJ) src/sktest.o
	$(CC) -pthread -o sktest src/sktest.o $(COMMON_OBJ) $(LDFLAGS) $(LDLIBS)
	./sktest -n 10 -H "pragma:" -verboseOn -url http://www.google.com/


