

#include "mbed.h"
#include "sv-flash-serial.h"

/*
 * Constructor for FlashSerial class
 */
SerialFlash::SerialFlash(PinName mosi, PinName miso, PinName sck, PinName ssel) : spi_inst(mosi, miso, sck, ssel) {
    printf("Initializing flash... \r\n");

    bool res = isPresent(false);
    if (res) {
        printf("External Flash is present. \r\n");
    } else {
        printf("External Flash is not present. \r\n");
    }
}

void SerialFlash::readID(uint8_t* buffer) {
    char tx_buff[] = {READ_FLASH_ID, 0x00, 0x00, 0x00};
    char rx_buf[READ_ID_BUF_SIZE] = {0x00};

    spi_inst.write(tx_buff, sizeof(tx_buff) / sizeof(tx_buff[0]), rx_buf, READ_ID_BUF_SIZE);

    memcpy(buffer, rx_buf, READ_ID_BUF_SIZE);
}

void SerialFlash::releasePowerDown() {
    char tx_buff[] = {RELEASE_POWER_DOWN, 0x00, 0x00, 0x00, 0x00};
    char rx_buf[] = {0};

    spi_inst.write((const char*)tx_buff, sizeof(tx_buff) / sizeof(tx_buff[0]), rx_buf, 1);
}

void SerialFlash::powerDown() {
    char tx_buff[] = {POWER_DOWN};
    char rx_buf[] = {0};

    spi_inst.write(tx_buff, sizeof(tx_buff) / sizeof(tx_buff[0]), rx_buf, 1);
}

bool SerialFlash::isPresent(bool print) {
    uint8_t buf[READ_ID_BUF_SIZE];

    SerialFlash::readID(buf);

    uint8_t manuf_id = buf[1];
    uint16_t chip_id = ((uint16_t)buf[2] << 8) | buf[3];

    if (manuf_id == MANUF_ID && chip_id == CHIP_ID) {
        if (print) {
            printf("\nFLASH MANUFACTURER ID: %X\n\r", manuf_id);
            printf("FLASH CHIP ID: %x\n\n\r", chip_id);
        }
        return true;
    }

    return false;
}
