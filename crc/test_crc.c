#include <stdio.h>
#include "crc16_xmodem.h"

int main(void) {
  const uint8_t* data = (const uint8_t*)"QPI";
  uint16_t ret_crc;
  ret_crc = crc16_xmodem(data, 3);
  printf("CRC 0x%X \n", ret_crc);

  return (0);
}
