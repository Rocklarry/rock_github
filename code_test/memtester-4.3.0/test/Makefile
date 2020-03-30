
CFLAGS += -I$(ROOTFS)/usr/include
LIBS += -L$(ROOTFS)/usr/lib
TARGET := memtester
OBJS := $(patsubst %.c, %.o, $(wildcard *.c))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	-@rm -f $(TARGET) $(OBJS)