/*********************************************************************
 * FILENAME:      mp_serial.c
 * DESCRIPTION:   Serial port interface to the Masterpower Omega 3M
 *                Finds serial adapter, opens serial port, sends and
 *                decodes commands
 *                Original code comes from send_receive.c from
 *                libserialport library
 *
 * AUTHOR:        JnG
 * DATE:          2026-01-30
 * MODIFICATIONS: Initial version
 * DEPENDENCIES:  libserialports library
 *********************************************************************/
/* includes */
#include <libserialport.h>
#include <stdio.h> /* printf() srtlen() */
#include <stdlib.h>
#include <string.h> /* strlen() */
#include "findport.h"

/* local function prototypes*/
int check(enum sp_return result);
int printString(const char* string);

int main(int argc, char** argv) {
  char** port_names = argv + 1;

  /* The ports we will use. */
  struct sp_port* serial_port;

  /* Open and configure each port. */
  printf("Looking for port %s.\n", port_names[i]);
  check(sp_get_port_by_name(port_names[i], &ports[i]));
  printf("Opening port.\n");
  check(sp_open(ports[i], SP_MODE_READ_WRITE));
  printf("Setting port to 2400 8N1, no flow control.\n");
  check(sp_set_baudrate(ports[i], 2400));
  check(sp_set_bits(ports[i], 8));
  check(sp_set_parity(ports[i], SP_PARITY_NONE));
  check(sp_set_stopbits(ports[i], 1));
  check(sp_set_flowcontrol(ports[i], SP_FLOWCONTROL_NONE));
}
/* Now send some data on each port and receive it back. */
for (int tx = 0; tx < num_ports; tx++) {
  /* Get the ports to send and receive on. */
  int rx = num_ports == 1 ? 0 : ((tx == 0) ? 1 : 0);
  struct sp_port* tx_port = ports[tx];
  struct sp_port* rx_port = ports[rx];
  /* The data we will send. */
  char* data = "QPI\xBE\xAC\x0D";
  int size = strlen(data);
  /* We'll allow a 1 second timeout for send and receive. */
  unsigned int timeout = 1000;
  /* On success, sp_blocking_write() and sp_blocking_read()
   * return the number of bytes sent/received before the
   * timeout expired. We'll store that result here. */
  int result;
  /* Send data. */
  printf("Sending '%s' (%d bytes) on port %s.\n", data, size,
         sp_get_port_name(tx_port));
  result = check(sp_blocking_write(tx_port, data, size, timeout));
  /* Check whether we sent all of the data. */
  if (result == size)
    printf("Sent %d bytes successfully.\n", size);
  else
    printf("Timed out, %d/%d bytes sent.\n", result, size);
  /* Allocate a buffer to receive data. */
  char* buf = malloc(size + 1);
  /* Try to receive the data on the other port. */
  printf("Receiving %d bytes on port %s.\n", size, sp_get_port_name(rx_port));
  result = check(sp_blocking_read(rx_port, buf, size, timeout));
  /* Check whether we received the number of bytes we wanted. */
  if (result == size)
    printf("Received %d bytes successfully.\n", size);
  else
    printf("Timed out, %d/%d bytes received.\n", result, size);
  /* Check if we received the same data we sent. */
  buf[result] = '\0';
  printf("Received '%s'.\n", buf);
  /* Free receive buffer. */
  free(buf);
}
/* Close ports and free resources. */
for (int i = 0; i < num_ports; i++) {
  check(sp_close(ports[i]));
  sp_free_port(ports[i]);
}
return 0;
}

/* Helper functions */
/*** check - for error handling ***/
int check(enum sp_return result) {
  /* For this example we'll just exit on any error by calling abort(). */
  char* error_message;
  switch (result) {
    case SP_ERR_ARG:
      printf("Error: Invalid argument.\n");
      abort();
    case SP_ERR_FAIL:
      error_message = sp_last_error_message();
      printf("Error: Failed: %s\n", error_message);
      sp_free_error_message(error_message);
      abort();
    case SP_ERR_SUPP:
      printf("Error: Not supported.\n");
      abort();
    case SP_ERR_MEM:
      printf("Error: Couldn't allocate memory.\n");
      abort();
    case SP_OK:
    default:
      return result;
  }
}

/*** printString - print buffer as ascii and hex for debug ***/
int printString(const char* string) {
  int i;

  printf("\n");
  for (i = 0; string[i] != '\0'; i++) {
    printf("%x ", (int)string[i]);  // Cast to int for clarity
  }
  printf("\n");
  for (i = 0; string[i] != '\0'; i++) {
    if ((int)(string[i] > 31) & (int)(string[i] < 127)) {
      printf("%c ", (char)string[i]);  // Cast to char for clarity
    } else {
      printf("*");
    }
  }
  printf("\n");

  if (i > 0) {
    return (i);
  } else {
    return (-1);
  }
} /* printString */