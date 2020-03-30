server: server.c comm.c
	gcc -o $@ $^
client: client.c comm.c
	gcc -o $@ $^

clean:
	$(RM) server client .*.sw?
