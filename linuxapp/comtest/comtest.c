/*************************************************************************
	> File Name: comtest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Sun 12 Jun 2016 02:33:26 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/serial.h>



static int real_config_fd (int fd)
{
    struct termios stbuf;
    int speed;
    int bits;
    int parity;
    int stopbits;


    memset (&stbuf, 0, sizeof (struct termios));
    if (tcgetattr (fd, &stbuf) != 0) {
        perror();
    }

    stbuf.c_iflag &= ~(IGNCR | ICRNL | IUCLC | INPCK | IXON | IXANY | IGNPAR );
    stbuf.c_oflag &= ~(OPOST | OLCUC | OCRNL | ONLCR | ONLRET);
    stbuf.c_lflag &= ~(ICANON | XCASE | ECHO | ECHOE | ECHONL);
    stbuf.c_lflag &= ~(ECHO | ECHOE);
    stbuf.c_cc[VMIN] = 1;
    stbuf.c_cc[VTIME] = 0;
    stbuf.c_cc[VEOF] = 1;

    /* Use software handshaking */
    stbuf.c_iflag |= (IXON | IXOFF | IXANY);

    /* Set up port speed and serial attributes; also ignore modem control
     * lines since most drivers don't implement RTS/CTS anyway.
     */
    stbuf.c_cflag &= ~(CBAUD | CSIZE | CSTOPB | PARENB | CRTSCTS);
//    stbuf.c_cflag |= (speed | bits | CREAD | 0 | parity | stopbits | CLOCAL);
    stbuf.c_cflag |= (CS8 | CS8 | CREAD | 0 | 0 | 0 | CLOCAL);

    if (tcsetattr (fd, TCSANOW, &stbuf) < 0) {
        return 0;
    }

    return 1;
}


int mm_serial_port_open ()
{
    char *devfile = "/dev/ttyAP0";
    const char *device;
    struct serial_struct sinfo;
    struct termios old_t;

    printf ("(%s) opening serial port...", device);


    /* Only open a new file descriptor if we weren't given one already */
        int fd = open (devfile, O_RDWR | O_EXCL | O_NONBLOCK | O_NOCTTY);

    if (fd < 0) {
        perror(devfile);
        return 0;
    }

    if (ioctl (fd, TIOCEXCL) < 0) {
        perror ("Could not lock serial device %s: %s");
        goto error;
    }

    /* Flush any waiting IO */
    tcflush (fd, TCIOFLUSH);

    if (tcgetattr (fd, &old_t) < 0) {
        perror ("Could not open serial device %s: %s");
        goto error;
    }

    /* Don't wait for pending data when closing the port; this can cause some
     * stupid devices that don't respond to URBs on a particular port to hang
     * for 30 seconds when probin fails.
     */
    if (ioctl (fd, TIOCGSERIAL, &sinfo) == 0) {
    	sinfo.closing_wait = ASYNC_CLOSING_WAIT_NONE;
        ioctl (fd, TIOCSSERIAL, &sinfo);
    }

error:
    close (fd);
    fd = -1;
    return 0;
}

int main()
{
	mm_serial_port_open();
	return 0;
}
