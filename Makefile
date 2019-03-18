CC = gcc
FLAGS = -DSERVER_DEBUG=1 -DCLIENT_DEBUG=1

default : main
	mv client server bin/
	export PATH=$(PATH):$(PWD)/bin/
	make clean

main : commonutil.o clienthandler.o clientutil.o serverhandler.o serverutil.o log.o aes256.o payload.o base64.o sha256.o
	CC $(FLAGS) ircserver.c commonutil.o serverhandler.o serverutil.o log.o aes256.o payload.o base64.o sha256.o -lprotobuf-c -o server
	CC $(FLAGS) ircclient.c commonutil.o clienthandler.o clientutil.o log.o aes256.o payload.o base64.o sha256.o -lprotobuf-c -o client

commonutil.o : utilities/common/commonutil.c include/commonutil.h log.o
	CC -c $(FLAGS) utilities/common/commonutil.c

serverutil.o : utilities/server/serverutil.c include/serverutil.h log.o
	CC -c $(FLAGS) utilities/server/serverutil.c 

clientutil.o : utilities/client/clientutil.c include/clientutil.h log.o
	CC -c $(FLAGS) utilities/client/clientutil.c 

serverhandler.o: utilities/server/serverhandler.c include/serverhandler.h
	CC -c $(FLAGS) utilities/server/serverhandler.c 

clienthandler.o: utilities/client/clienthandler.c include/clienthandler.h
	CC -c $(FLAGS) utilities/client/clienthandler.c

log.o : utilities/logger/log.c include/log.h
	CC -c $(FLAGS) utilities/logger/log.c -DLOG_USE_COLOR

aes256.o: utilities/crypto/aes256.c include/aes256.h
	CC -c $(FLAGS) utilities/crypto/aes256.c 

base64.o: utilities/crypto/base64.c include/base64.h
	CC -c $(FLAGS) utilities/crypto/base64.c

sha256.o: utilities/crypto/sha256.c include/sha256.h
	CC -c $(FLAGS) utilities/crypto/sha256.c

payload.o: protobufs/payload.pb-c.c protobufs/payload.pb-c.h
	CC -c $(FLAGS) protobufs/payload.pb-c.c -o payload.o

clean: 
	rm -rf logs
	mkdir logs
	rm *.o
