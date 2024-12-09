CC = gcc
CFLAGS = -Wall -pthread
TARGET = server
OBJS = server.c server_client.c list.c

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f $(TARGET) *.o
