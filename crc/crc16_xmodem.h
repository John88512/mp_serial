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
#ifndef CRC16_XMODEM_H
#define CRC16_XMODEM_H
#include <stddef.h>
#include <stdint.h>

/**
 * @fn uint16_t crc16_xmodem(const uint8_t* data, size_t len);
 *
 * @brief Calculates the 16 bit crc using standard XMODEM algorithm
 *        initial value 0x0000, polynomial 0x1021, no input/output
 *        reflection, and no final XOR
 *        Voltronic specials: if a byte value is 0x28, 0x0D
 *        or 0x0A, then add one to the byte
 *
 * @param const uint8_t* data - to calculate crc16/XMODEM
 * @param size_t len - number of bytes in data to calculate crc16/XMODEM
 * @return crc16/XMODEM
 */
uint16_t crc16_xmodem(const uint8_t* data, size_t len);

#endif
