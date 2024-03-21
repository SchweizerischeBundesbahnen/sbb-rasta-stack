/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file test_helper.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version ec42764479c9e717baa11444b5d7c50467003f68
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 *
 * @brief Source file for unit and integration test helper functions
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "test_helper.hh"

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

uint8_t getDataFromBuffer_u8(const void * buffer, uint16_t byteIdx){
    const uint8_t * data;             // data byte array
    uint8_t retVal = 0;         // return value

    data = (const uint8_t *)buffer;
    retVal = data[byteIdx];

    return retVal;
}

uint16_t getDataFromBuffer_u16(const void * buffer, uint16_t byteIdx){
    const uint8_t * data;             // data byte array
    uint16_t retVal = 0;        // return value

    data = (const uint8_t *)buffer;
    retVal = (data[byteIdx + 1] << 8) | data[byteIdx];

    return retVal;
}

uint32_t getDataFromBuffer_u32(const void * buffer, uint16_t byteIdx){
    const uint8_t * data;             // data byte array
    uint32_t retVal = 0;        // return value

    data = (const uint8_t *)buffer;
    retVal = (data[byteIdx + 3] << 24) | (data[byteIdx + 2] << 16) | (data[byteIdx + 1] << 8) | data[byteIdx];

    return retVal;
}

void setDataInBuffer_u8(void * buffer, uint8_t value, uint16_t byteIdx){
    uint8_t * data;             // data byte array

    data = (uint8_t *)buffer;
    data[byteIdx] = value;
}


void setDataInBuffer_u16(void * buffer, uint16_t value, uint16_t byteIdx){
    uint8_t * data;             // data byte array

    data = (uint8_t *)buffer;
    data[byteIdx + 0] = (uint8_t)(value & 0xFF);
    data[byteIdx + 1] = (uint8_t)((value >> 8) & 0xFF);
}


void setDataInBuffer_u32(void * buffer, uint32_t value, uint16_t byteIdx){
    uint8_t * data;             // data byte array

    data = (uint8_t *)buffer;
    data[byteIdx + 0] = (uint8_t)(value & 0xFF);
    data[byteIdx + 1] = (uint8_t)((value >> 8) & 0xFF);
    data[byteIdx + 2] = (uint8_t)((value >> 16) & 0xFF);
    data[byteIdx + 3] = (uint8_t)((value >> 24) & 0xFF);

}
