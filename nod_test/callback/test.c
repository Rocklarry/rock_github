#include <stdio.h>

char const*const LCD_FILE
        = "/sys/class/leds/lcd-backlight/brightness";

static int write_int(char *path, int value)
{
    int fd;
    static int already_warned = 0;

    fd = open(path, O_RDWR);
    if (fd >= 0) {
        char buffer[20];
        int bytes = snprintf(buffer, sizeof(buffer), "%d\n", value);
        ssize_t amt = write(fd, buffer, (size_t)bytes);
        close(fd);
        return amt == -1 ? -errno : 0;
    } else {
        if (already_warned == 0) {
            ALOGE("write_int failed to open %s, errno = %d\n", path, errno);
            already_warned = 1;
        }
        return -errno;
    }
}

int main(int argc, char *argv[])
{
	printf("Hello, world\n");
	 if (!access(LCD_FILE, F_OK)) {
            err = write_int(LCD_FILE, brightness);
	
	return 0;
}
