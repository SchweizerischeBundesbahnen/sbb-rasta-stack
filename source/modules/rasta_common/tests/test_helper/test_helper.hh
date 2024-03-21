/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file test_helper.hh
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version ec42764479c9e717baa11444b5d7c50467003f68
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 *
 * @brief Header file for unit and integration test helper functions
 */

#ifndef SOURCE_MODULES_RASTA_COMMON_TESTS_TEST_HELPER_TEST_HELPER_HH_
#define SOURCE_MODULES_RASTA_COMMON_TESTS_TEST_HELPER_TEST_HELPER_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Custom matcher to verify if a value is in a certain range
 */
MATCHER_P2(InRange, a, b, "to be in range [" + std::to_string(a) + " ... " + std::to_string(b) + "]") { return a <= arg && arg <= b; }

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------

/**
 * @brief Get uint8 value from buffer
 *
 * This function reads a uint8 value from a data buffer at index byteIdx and
 * returns it to the caller.
 *
 * @param buffer    pointer to the data buffer
 * @param byteIdx   read position
 * @return uint8 value read from the data buffer
 */
uint8_t getDataFromBuffer_u8(const void * buffer, uint16_t byteIdx);

/**
 * @brief Get uint16 value from buffer
 *
 * This function reads a uint16 value from a data buffer at index byteIdx and
 * returns it to the caller. Thereby, the little endian byte order will be applied.
 *
 * @param buffer        pointer to the data buffer
 * @param byteIdx       read position
 * @return uint16 value read from the data buffer
 */
uint16_t getDataFromBuffer_u16(const void * buffer, uint16_t byteIdx);

/**
 * @brief Get uint32 value from buffer
 *
 * This function reads a uint32 value from a data buffer at index byteIdx and
 * returns it to the caller. Thereby, the little endian byte order will be applied.
 *
 * @param buffer        pointer to the data buffer
 * @param byteIdx       read position
 * @return uint32 value read from the data buffer
 */
uint32_t getDataFromBuffer_u32(const void * buffer, uint16_t byteIdx);

/**
 * @brief Write a uint8 value to a given buffer
 *
 * This function write a uint8 value to a data buffer at index byteIdx.
 *
 * @param buffer    pointer to the data buffer
 * @param value     value to be written
 * @param byteIdx   read position
 */
void setDataInBuffer_u8(void * buffer, uint8_t value, uint16_t byteIdx);

/**
 * @brief Write a uint16 value to a given buffer
 *
 * This function writes a uint16 value to a data buffer at index byteIdx
 * with little endian byte order.
 *
 * @param buffer        pointer to the data buffer
 * @param value         value to be written
 * @param byteIdx       read position
 */
void setDataInBuffer_u16(void * buffer, uint16_t value, uint16_t byteIdx);

/**
 * @brief Write a uint32 value to a given buffer
 *
 * This function writes a uint32 value to a data buffer at index byteIdx
 * with little endian byte order.
 *
 * @param buffer        pointer to the data buffer
 * @param value         value to be written
 * @param byteIdx       read position
 */
void setDataInBuffer_u32(void * buffer, uint32_t value, uint16_t byteIdx);








#endif // SOURCE_MODULES_RASTA_COMMON_TESTS_TEST_HELPER_TEST_HELPER_HH_
