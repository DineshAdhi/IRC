gcc ircserver.c utilities/common/*.c utilities/crypto/*.c utilities/logger/*.c utilities/server/*.c protobufs/payload.pb-c.c -DLOG_USE_COLOR -lprotobuf-c -o server.out 
gcc ircclient.c utilities/common/*.c utilities/crypto/*.c utilities/logger/*.c utilities/client/*.c protobufs/payload.pb-c.c -DLOG_USE_COLOR -lprotobuf-c -o client.out 
