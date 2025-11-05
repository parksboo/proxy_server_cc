CC = g++
CFLAGS = -std=c++23 -Wall -Wextra -O2 -I$(shell brew --prefix openssl@3)/include
LIBS = -L$(shell brew --prefix openssl@3)/lib -lssl -lcrypto
INCLUDES = -Iinclude

TARGET = proxy
SRCS = proxy.cc server.cc worker_thread.cc strutil.cc http/request.cc http/response.cc

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LIBS)

debug:
	$(MAKE) CFLAGS="$(CFLAGS) -g -DDEBUG -O0 -pthread -fsanitize=address -fno-omit-frame-pointer"

clean:
	rm -f $(TARGET)


