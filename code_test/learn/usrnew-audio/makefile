#CC:=arm-none-linux-gnueabi-gcc
CC:=gcc
cdz:main.o card.o meter.o mycom.o convertion.o sql.o timer.o  mytcpip.o cdzDataType.o cdzChargeRecord.o cdzProtocol.o audio_play.o AlarmRecord.o Gprs.o KuoZhan.o display.o
	$(CC) main.o card.o meter.o mycom.o convertion.o sql.o timer.o  mytcpip.o cdzDataType.o cdzChargeRecord.o cdzProtocol.o audio_play.o AlarmRecord.o Gprs.o KuoZhan.o display.o -lsqlite3 -L/home/sqlite/sqlite-arm/_install/lib -lpthread -o cdz 
#	cp cdz /work/nfs -rf 
main.o:main.c public.h Card.h Meter.h DataType.h mycom.h convertion.h timer.h sql.h cdzProtocol.h audio_play.h display.h
	$(CC) -c main.c -o main.o
card.o:Card.c Card.h DataType.h mycom.h public.h
	$(CC) -c Card.c -o card.o
meter.o:Meter.c Meter.h 
	$(CC) -c Meter.c -o meter.o
mycom.o:mycom.c mycom.h
	$(CC) -c mycom.c -o mycom.o
convertion.o:convertion.c DataType.h 
	$(CC) -c convertion.c -o convertion.o
sql.o:sql.c sql.h public.h AlarmRecord.h
	$(CC) -c sql.c -I/home/sqlite/sqlite-arm/_install/include -o sql.o 
timer.o:timer.c timer.h public.h
	$(CC) -c timer.c -o timer.o
mytcpip.o:mytcpip.c mytcpip.h
	$(CC) -c mytcpip.c -o mytcpip.o
cdzDataType.o: cdzDataType.c cdzDataType.h
	$(CC) -c cdzDataType.c -o cdzDataType.o
cdzChargeRecord.o:cdzChargeRecord.c cdzChargeRecord.h 
	$(CC) -c cdzChargeRecord.c -o cdzChargeRecord.o
cdzProtocol.o:cdzProtocol.c cdzProtocol.h cdzDataType.h cdzChargeRecord.h DataType.h mytcpip.h Gprs.h -lpthread 
	$(CC) -c cdzProtocol.c -o cdzProtocol.o -lpthread

audio_play.o:audio_play.c audio_play.h
	$(CC) -c audio_play.c -o audio_play.o 
AlarmRecord.o:AlarmRecord.c AlarmRecord.h convertion.h sql.h
	$(CC) -c AlarmRecord.c -o AlarmRecord.o

Gprs.o:Gprs.c DataType.h cdzProtocol.h Gprs.h convertion.h
	$(CC) -c Gprs.c -o Gprs.o
KuoZhan.o:KuoZhanBoard.c KuoZhanBoard.h Meter.h
	$(CC) -c KuoZhanBoard.c -o KuoZhan.o
display.o:display.c display.h public.h
	$(CC) -c display.c -o display.o
clean:
	rm -rf *.o cdz


