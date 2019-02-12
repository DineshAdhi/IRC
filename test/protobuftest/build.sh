gcc pack.c amessage.pb-c.c ../../utilities/crypto/aes256.c ../../utilities/crypto/base64.c -lprotobuf-c -o pack.o 
gcc unpack.c amessage.pb-c.c ../../utilities/crypto/aes256.c ../../utilities/crypto/base64.c -lprotobuf-c -o unpack.o
gcc packunpackaes.c amessage.pb-c.c ../../utilities/crypto/aes256.c ../../utilities/crypto/base64.c -lprotobuf-c -o packunpackaes.o

# ./pack.o 4 5 | ./unpack.o 

./packunpackaes.o 45623 2