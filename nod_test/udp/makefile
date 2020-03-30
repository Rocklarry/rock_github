#makefile
CFLAGS = -Wall -g
CC = gcc
TARGET1 = server
TARGET2 = client
 
all:$(TARGET1) $(TARGET2)
$(TARGET1):$(TARGET1).o
		$(CC) $(CFLAGS) -pthread -o $@ $^
$(TARGET2):$(TARGET2).o
		$(CC) $(CFLAGS) -o $@ $^
%.o:%.c %.h
		$(CC) -c $(CFLAGS) -o $@ $<
clean:
		rm -f $(TARGET1) $(TARGET2) *.o
