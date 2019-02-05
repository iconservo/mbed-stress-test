
#ifndef SV_FLASH_SERIAL_H
#define SV_FLASH_SERIAL_H

#include "mbed.h"

/* GOT THIS DEFINES FROM: https://www.winbond.com/resource-files/w25q16jv%20spi%20revd%2008122016.pdf
 * pp 19-20 */
#define POWER_DOWN 0xB9
#define RELEASE_POWER_DOWN 0xAB
#define READ_FLASH_ID 0x9F

#define READ_ID_BUF_SIZE (4)

#define MANUF_ID 0xEF
#define CHIP_ID 0x4015

class SerialFlash {
   public:
    /*
     * Creates Flash Serial instance for working with external flash
     */

    SerialFlash(PinName mosi, PinName miso, PinName sck, PinName ssel);

    /*
     * Reads flash ID number
     */
    void readID(uint8_t* buffer);

    /*
     * Realeses flash power down
     */
    void releasePowerDown();

    /*
     * Powers down flash
     */
    void powerDown();

    /*
     * Checks if the external flash is present on the SPI bus
     */
    bool isPresent(bool print);

   private:
    SPI spi_inst;
};

#endif
