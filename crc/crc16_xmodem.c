/*********************************************************************
 * FILENAME:      crc16_xmodem.c
 * DESCRIPTION:   Calculates the 16 bit crc using standard XMODEM algorithm
 *                initial value 0x0000, polynomial 0x1021, no input/output
 *                reflection, and no final XOR
 *                Voltronic specials: if a byte value is 0x28, 0x0D
 *                or 0x0A, then add one to the byte
 * AUTHOR:        JnG
 * DATE:          2026-04-16
 * MODIFICATIONS: Initial version
 * DEPENDENCIES:
 *********************************************************************/

/* includes */
#include "crc16_xmodem.h"
#include <stddef.h>
#include <stdint.h>

/* defines */
#define INITIAL 0x0000
#define POLYNOMIAL 0x1021
#define ITERATIONS 8
#define HIGH_BYTE_MASK 0x8000

uint16_t crc16_xmodem(const uint8_t* data, size_t len) {
  uint16_t crc = INITIAL;
  uint8_t highByte = 0;
  uint8_t lowByte = 0;

  while (len--) {
    crc ^= (uint16_t)(*data++) << 8;
    for (unsigned i = 0; i < ITERATIONS; i++) {
      if (crc & HIGH_BYTE_MASK) {
        crc = (crc << 1) ^ POLYNOMIAL;
      } else {
        crc <<= 1;
      }
    }
  }

  highByte = (uint8_t)(crc >> 8);
  if ((highByte == 0x28) || (highByte == 0x0D) || (highByte == 0x0A)) {
    crc = crc + (1 << 8);
  }

  lowByte = (uint8_t)(crc & 0xFF);
  if ((lowByte == 0x28) || (lowByte == 0x0D) || (lowByte == 0x0A)) {
    crc = crc + 1;
  }

  return crc;
}
