/**
 * POSIX C program to send raw serial data on /dev/ttyUSB1.
 * Configured for 2400 baud, 8 data bits, no parity, 1 stop bit (8N1).
 * Uses termios for raw mode configuration, writes binary data, and handles errors.
 * Compile: gcc -o serial_send serial_send.c
 * Usage: sudo ./serial_send (requires dialout group permissions)
 */

#include <stdio.h>      /* For printf, perror */
#include <stdlib.h>     /* For exit codes */
#include <fcntl.h>      /* For open flags */
#include <unistd.h>     /* For read, write, close */
#include <termios.h>    /* For termios structure and functions */
#include <errno.h>      /* For errno */
#include <string.h>     /* For strerror (via perror) */

/**
 * Main function.
 * Opens /dev/ttyUSB1, configures raw serial parameters, sends sample data.
 * Returns 0 on success, 1 on error.
 */
int main()
{
    /* Serial port device path */
    const char *port = "/dev/ttyUSB1";

    /* Open port in read/write, non-controlling tty, synchronous mode */
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        perror("Failed to open serial port");
        return 1;
    }

    /* Termios structure for port configuration */
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0)
    {
        perror("Failed to get termios attributes");
        close(fd);
        return 1;
    }

    /* 
     * Configure local modes for raw input/output:
     * Disable canonical mode (line buffering), echo, echo erase, signals.
     */
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* 
     * Configure input modes:
     * Disable software flow control (XON/XOFF), ignore break, strip parity,
     * map CR to NL, etc. for raw binary data.
     */
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    /* 
     * Configure control modes for 8N1:
     * 8 data bits (CS8), no parity (PARENB/PARODD off), 1 stop bit (CSTOPB off),
     * enable receiver (CREAD), ignore modem status (CLOCAL).
     */
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= (CREAD | CLOCAL);

    /* Disable hardware (RTS/CTS) flow control */
    tty.c_cflag &= ~CRTSCTS;

    /* Disable all output processing */
    tty.c_oflag = 0;

    /* 
     * Configure special characters for non-blocking reads:
     * VMIN=0: return immediately if no data
     * VTIME=1: 0.1 second read timeout
     */
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 1;

    /* Set input and output baud rates to 2400 */
    cfsetispeed(&tty, B2400);
    cfsetospeed(&tty, B2400);

    /* Apply configuration immediately */
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        perror("Failed to set termios attributes");
        close(fd);
        return 1;
    }

    /* Flush input/output buffers before writing */
    tcflush(fd, TCIOFLUSH);

    /* Sample raw binary data to transmit (customize as needed) */
    unsigned char raw_data[] = {0x01, 0x02, 0xFF, 0xAA, 0x55};
    size_t data_len = sizeof(raw_data);

    /* Write raw data to serial port */
    ssize_t bytes_written = write(fd, raw_data, data_len);
    if (bytes_written < 0)
    {
        perror("Failed to write data");
    }
    else
    {
        printf("Sent %zd bytes at 2400 8N1: ", bytes_written);
        for (size_t i = 0; i < (size_t)bytes_written; i++)
        {
            printf("0x%02X ", raw_data[i]);
        }
        printf("\n");
    }

    /* Close serial port file descriptor */
    close(fd);
    return 0;
}
