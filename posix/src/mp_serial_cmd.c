/*
 * MP Serial Command Sender
 * =========================
 * Raw ttyUSB*: CMD_MAX_LEN chars via scanf, sends CR cmds, reads RESP_MAX_LEN responses (1s timeout).
 * Loops until '0'. No hw flow control.
 *
 * Usage: mp_serial_cmd
 * Permissions: dialout group.
 *
 * Author: JnG
 * Date: January 2026
 * Changes:
 * - Full defines: CMD_MAX_LEN=20, RESP_MAX_LEN=256, etc. (no truncation)
 * - Prototypes + funcs after main, scanf input
 * - No hw flow/CRTSCTS, raw 8N1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>

/* Constants - descriptive, no magic numbers */
#define CMD_MAX_LEN          20     /* Max input command chars */
#define RESP_MAX_LEN         256    /* Max response buffer size */
#define DEV_PATH_MAX         20     /* /dev/ttyUSBx path length */
#define SERIAL_BAUD          B2400  /* Default baud rate */
#define READ_TIMEOUT_MS      1000   /* Read timeout milliseconds */
#define READ_TIMEOUT_TENTHS  10     /* VTIME units (0.1s) = 1000/100 */
#define TTYUSB_PREFIX_LEN    6      /* strlen("ttyUSB") */

/* Function prototypes */
int find_ttyusb(void);
int configure_serial(int fd);
int send_command(int fd, const char *cmd);
int receive_response(int fd, char *response, size_t max_len);
int print_buffer_hex_chars(const char *inputString);

int main(void)
{
    char cmd[CMD_MAX_LEN + 1]; /* max length + 1 for \0 */
    char response[RESP_MAX_LEN];
    int fd;
    int bytes_read;
    int scanned;
    int bytes_printed;

    // fd = find_ttyusb();
    // if (fd < 0) {
    //     perror("find_ttyusb");
    //     return 1;
    // }

    /* set USB port */
    fd = 1; 
    /* open ttyUBS(fd) */
    if (configure_serial(fd) < 0) {
        close(fd);
        return 1;
    }

    printf("MP Serial CR loop - commands have max %d characters. '0' quits.\n", CMD_MAX_LEN);

    while (1) {
        printf("\nEnter command >  ");
        fflush(stdout);
        printf("\n");

        /* commands should not be longer than 20 characters*/
        scanned = scanf(" %20s", cmd);
        if (scanned != 1) {
            perror("scanf");
            continue;
        }

        /* check for exiting command */
        if (cmd[0] == '0') {
            printf("Quitting.\n");
            break;
        }

        bytes_printed = print_buffer_hex_chars(cmd);
        printf("number of bytes pringes %i\n", bytes_printed);

        /* replace newline, if present, with CR */
        cmd[strcspn(cmd, "\n")] = *"\r";

        if (send_command(fd, cmd) < 0) {
            perror("send_command");
            continue;
        }
     
        printf("Sent: '%s\\r'\n", cmd);

        bytes_read = receive_response(fd, response, RESP_MAX_LEN);
        if (bytes_read > 0) {
            printf("Rcvd (%d bytes): '%s'\n", bytes_read, response);
        } else if (bytes_read == 0) {
            printf("Timeout (%d ms)\n", READ_TIMEOUT_MS);
        } else {
            perror("receive_response");
        }
    }

    close(fd);
    return 0;
}

/**
 * find_ttyusb - Finds and opens the first available /dev/ttyUSB* device.
 *
 * Inputs: None
 * Outputs: None
 * Returns: File descriptor (>=0) or -1 on error.
 */
int find_ttyusb(void)
{
    DIR *dir;
    struct dirent *entry;
    char path[20];
    int fd;

    dir = opendir("/dev");
    if (!dir) {
        perror("opendir /dev");
        return -1;
    }

    while ((entry = readdir(dir))) {
        if (strncmp(entry->d_name, "ttyUSB", 6) == 0) {
            // snprintf(path, sizeof(path), "/dev/%s", entry->d_name);
            fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);
            if (fd >= 0) {
                printf("Using serial port: %s\n", path);
                closedir(dir);
                return fd;
            }
        }
    }

    closedir(dir);
    errno = ENOENT;
    fputs("No ttyUSB* device found\n", stderr);
    return -1;
}

/**
 * configure_serial - Configures port for 115200 baud, 8N1, raw mode with read timeout.
 *
 * Inputs: fd - Serial port file descriptor.
 * Outputs: None
 * Returns: 0 on success, -1 on error.
 */
int configure_serial(int fd)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        perror("tcgetattr");
        return -1;
    }

    /* set baud rate to 2400 */
    cfsetospeed(&tty, SERIAL_BAUD);
    cfsetispeed(&tty, SERIAL_BAUD);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | CSTOPB );

    tty.c_iflag &= ~IGNBRK & ~BRKINT & ~PARMRK & ~ISTRIP &
                   ~INLCR & ~IGNCR & ~ICRNL;
    tty.c_oflag &= ~OPOST;
    tty.c_lflag &= ~ECHO & ~ECHONL & ~ICANON & ~ISIG & ~IEXTEN;

    /* Blocking read with 1s timeout: VMIN=1 (wait for 1 char), VTIME=10 (1s in 0.1s units) */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 10;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }

    return 0;
}

/**
 * send_command - Appends '\n' to cmd and sends to serial port.
 *
 * Inputs: fd - Serial port fd.
 *         cmd - Command string (<=15 chars).
 * Outputs: None
 * Returns: Bytes written or -1 on error.
 */
int send_command(int fd, const char *cmd)
{
    char buf[CMD_MAX_LEN];
    int len;

    strcat(buf, cmd);
    len = strlen(buf);
 
    ssize_t wlen = write(fd, buf, len);
    if (wlen < 0) {
        return -1;
    }
    tcdrain(fd);
    return (int)wlen;
}

/**
 * receive_response - Reads response with 1s timeout until timeout or linefeed.
 *
 * Inputs: fd - Serial port fd.
 *         response - Buffer for response (caller ensures size).
 *         max_len - Max buffer length (including '\0').
 * Outputs: response - Null-terminated string up to linefeed (excludes '\n').
 * Returns: Bytes read (>0), 0 (timeout), -1 (error).
 */
int receive_response(int fd, char *response, size_t max_len)
{
    char *p = response;
    size_t remaining = max_len - 1;
    ssize_t n;
    int total = 0;

    while (remaining > 0) {
        n = read(fd, p, 1);
        if (n > 0) {
            total += n;
            if (*p == '\n') {
                *p = '\0';
                return total;
            }
            p++;
            remaining--;
        } else if (n == 0) {
            /* EOF unlikely */
            break;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Timeout */
                *response = '\0';
                return 0;
            }
            return -1;
        }
    }

    *p = '\0';
    return total;
}

/**
 * Prints the contents of a null-terminated string buffer in two formats:
 * first as integer byte values (ASCII codes), then as printable characters.
 * Assumes the buffer read was successful (no error/timeout check here).
 * 
 * @param inputString Null-terminated C string to print.
 * @return Number of bytes printed (excluding null terminator).
 */
int print_buffer_hex_chars(const char *inputString) {
    int i, count = 0;
    
    /* Print as integer bytes (ASCII values) in hex */
    for (i = 0; inputString[i] != '\0'; i++) {
        printf("%x ", (int)inputString[i]);  /* Cast to int for clarity */
        count++;
    }
    printf("\n");
    
    /* Print as characters */
    for (i = 0; inputString[i] != '\0'; i++) {
        printf("%c ", (char)inputString[i]);  /* Cast to char for clarity */
    }
    printf("\n");
    
    return count;
} /* print_buffer_hex_chars */