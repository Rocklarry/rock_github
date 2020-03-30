#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

char buf[40960];

int main()
{
	int fd_usb;
	int fd_file;
	int r_cnt,w_cnt;
	int ret;

	fd_usb = open("/dev/artosyn_port1", O_RDWR, S_IRUSR | S_IWUSR);
	fd_file = open("filetorec", O_RDWR, S_IRUSR | S_IWUSR);
	if(fd_usb < 0 || fd_file < 0)
	{
		printf("open file error\n");
		return 0;
	}
	
	while(1)
	{
			r_cnt = read(fd_usb,buf,20480);
			if(r_cnt < 0)	
			{
				close(fd_usb);
				do{
					fd_usb = open("/dev/artosyn_port1", O_RDWR, S_IRUSR | S_IWUSR);
					usleep(100 * 1000);
				}while(fd_usb <= 0);
				continue;
			}
			else if(r_cnt > 0)
			{
				w_cnt = 0;
				do{
					
					ret = write(fd_file,&buf[w_cnt],r_cnt - w_cnt);
					w_cnt += ret;
					
				}while(w_cnt < r_cnt);
				usleep(20000);
			}
			usleep(10000);
 	}

	close(fd_usb);
	close(fd_file);
  return 0;
}

