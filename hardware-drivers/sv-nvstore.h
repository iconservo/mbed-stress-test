//------------------------------------------------------------------------------
#ifndef _SV_NVSTORE_H_
#define _SV_NVSTORE_H_
//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include "nvstore.h"

#define DEBUG_NVSTORE_PRINT_ENABLE 1
#if DEBUG_NVSTORE_PRINT_ENABLE
#define NVSTORE_DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define NVSTORE_DEBUG_PRINTF(...)
#endif

/* This value is written to NVStore by the INIT_NVSTORE_KEY key
 * and means that the memory has already been initialized and used
 */
#define NVSTORE_USED_FLAG 170

class SVNVStore {
   public:
    SVNVStore(NVStore& nvstore) : _nvstore(nvstore) { nv_store_initialize(); }

    typedef enum {
        INIT_NVSTORE_KEY = 0,
        WINC_MAC_NVSTORE_KEY = 1,
    } nvstore_keys;

    int nv_store_set_mac(uint8_t* mac, uint16_t sz) {
        NVSTORE_DEBUG_PRINTF("Writing MAC to NVStore...");
        int rc = _nvstore.set(SVNVStore::WINC_MAC_NVSTORE_KEY, sz, mac);
        if (rc) {
            NVSTORE_DEBUG_PRINTF("Error, %i\r\n", rc);
            return rc;
        }
        NVSTORE_DEBUG_PRINTF("OK\r\n");
        return rc;
    }

    int nv_store_set_once_mac(uint8_t* mac, uint16_t sz) {
        NVSTORE_DEBUG_PRINTF("Writing Non-changing MAC to NVStore...");
        int rc = _nvstore.set_once(SVNVStore::WINC_MAC_NVSTORE_KEY, sz, mac);
        if (rc) {
            NVSTORE_DEBUG_PRINTF("Error, %i\r\n", rc);
            return rc;
        }
        NVSTORE_DEBUG_PRINTF("OK\r\n");
        return rc;
    }

    int nv_store_get_mac(uint8_t* mac, uint16_t mac_len) {
        uint16_t actual_len_bytes;
        int rc = _nvstore.get(SVNVStore::WINC_MAC_NVSTORE_KEY, mac_len, mac, actual_len_bytes);
        return rc;
    }

   private:
    // initializing and checking is NVStore already used
    int nv_store_initialize(void) {
        NVSTORE_DEBUG_PRINTF("Init NVStore... ");
        int rc = _nvstore.init();
        if (rc) {
            NVSTORE_DEBUG_PRINTF("Error %i\r\n", rc);
            return rc;
        } else {
            NVSTORE_DEBUG_PRINTF("OK\r\n");
        }
        // Show NVStore size, maximum number of keys and area addresses and sizes
        NVSTORE_DEBUG_PRINTF("NVStore size is %d.\n", _nvstore.size());
        NVSTORE_DEBUG_PRINTF(
            "NVStore max number of keys is %d (out of %d possible ones in this flash configuration).\n",
            _nvstore.get_max_keys(), _nvstore.get_max_possible_keys());
        NVSTORE_DEBUG_PRINTF("NVStore areas:\n");
        for (uint8_t area = 0; area < NVSTORE_NUM_AREAS; area++) {
            uint32_t area_address;
            size_t area_size;
            _nvstore.get_area_params(area, area_address, area_size);
            NVSTORE_DEBUG_PRINTF("Area %d: address 0x%08lx, size %d (0x%x).\n", area, area_address, area_size,
                                 area_size);
        }
        uint32_t value;
        uint16_t actual_len_bytes;
        _nvstore.get(INIT_NVSTORE_KEY, sizeof(value), &value, actual_len_bytes);
        if (value != NVSTORE_USED_FLAG) {
            // Clear NVStore data. Should only be done once at factory configuration
            NVSTORE_DEBUG_PRINTF("Reset NVStore... ");
            rc = _nvstore.reset();
            if (rc) {
                NVSTORE_DEBUG_PRINTF("Error %i \r\n", rc);
                return rc;
            }
            NVSTORE_DEBUG_PRINTF("OK\r\n");

            // Now set some values to the same INIT_NVSTORE_KEY key
            value = NVSTORE_USED_FLAG;
            rc = _nvstore.set(INIT_NVSTORE_KEY, sizeof(value), &value);
            NVSTORE_DEBUG_PRINTF("Set value INIT_NVSTORE_KEY to value NVSTORE_USED_FLAG(0x%2X).\r\n", value);

            // Write WINC MAC to NVStore
            uint8_t mac[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36};
            nv_store_set_mac(mac, sizeof(mac));
        } else {
            NVSTORE_DEBUG_PRINTF("NVstore already used. INIT_NVSTORE_KEY value is (0x%2X).\r\n", value);
        }
        return rc;
    }

    NVStore& _nvstore;
};
//------------------------------------------------------------------------------
#endif  // _SV_NVSTORE_H_
        //------------------------------------------------------------------------------