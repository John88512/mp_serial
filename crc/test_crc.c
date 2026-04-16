#include "voltronic_crc.h"
#include <stdio.h>

int main (void) {
    const char *data = "QPI";
    voltronic_crc_t ret_crc;
    ret_crc = calculate_voltronic_crc(data, 3);
    printf ("CRC %u \n", ret_crc);

    return (0);
}
