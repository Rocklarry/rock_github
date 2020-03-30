.PHONY: all clean

all:rest_server

S_FLAGS = $(E_CFLAG)
S_FLAGS += -I external/mongoose
S_FLAGS += -I external/jsmn
S_FLAGS += -I network
S_FLAGS += -I server
S_FLAGS += -I ssdp
S_LDFLAG = $(E_LDFLAG)
S_LDFLAG += -L.
S_LDFLAG += -lavs_auth -lnetwork -lssdp

clean:
	find $(CURDIR) -name \*.o |xargs rm -f
	find $(CURDIR) -name \*.d |xargs rm -f
	find $(CURDIR) -name \*.so |xargs rm -f
	find $(CURDIR) -name \*.out |xargs rm -f

libavs_auth.so:
	$(CC) $(E_CFLAG) $(E_LDFLAG) -fPIC -shared -Wl,-soname,libavs_auth.so -o libavs_auth.so -C avs_auth/avs_auth.c
libnetwork.so:
	$(CC) $(E_CFLAG) $(E_LDFLAG) -I external/nmtool -fPIC -shared -Wl,-soname,libnetwork.so -o libnetwork.so -C network/network.c
libssdp.so:
	$(CC) $(E_CFLAG) $(E_LDFLAG) -fPIC -shared -Wl,-soname,libssdp.so -o libssdp.so -C ssdp/ssdp.c
rest_server:libavs_auth.so libnetwork.so libssdp.so
	$(CC) $(S_FLAGS) $(S_LDFLAG) -o rest_server -C server/server.c external/mongoose/mongoose.c external/jsmn/jsmn.c
