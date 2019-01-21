gcc ircserver.c utilities/*/*.c protobufs/payload.pb-c.c -DLOG_USE_COLOR -lprotobuf-c -o server.out 
./server.out
