/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcrc_red_crc.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA redundancy layer CRC module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redcrc_red_crc.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_redundancy/redcty_red_config_types.h"

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define CRC_TABLE_SIZE (256U)  ///< Size of the CRC lookup table [elements]

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Typedef for the options of the CRC algorithm.
 */
typedef struct {
  uint16_t width;              ///< Length of CRC [bit]
  uint32_t polynomial;         ///< The CRC polynomial without MSB
  uint32_t initial_optimized;  ///< The initial value for the table lookup algorithm
  bool refin;                  ///< true, if reflected input is enabled
  bool refout;                 ///< true, if reflected output is enabled
  uint32_t final_xor;          ///< Value for the final xor operation, hast to be the same length as width
} CrcOptions;

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

PRIVATE bool redcrc_initialized = false;  ///< Initialization state of the module. True, if the module is initialized.

PRIVATE redcty_CheckCodeType redcrc_check_code_type;  ///< Configured check code type.

PRIVATE uint32_t redcrc_table[CRC_TABLE_SIZE];  ///< The CRC lookup table, calculated by calling GenerateCrcTable().

PRIVATE uint32_t redcrc_crc_mask;      ///< CRC mask for internal CRC computation
PRIVATE uint32_t redcrc_crc_high_bit;  ///< CRC high bit for internal CRC computation

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup red_crc
 * @{
 */

/**
 * @brief Reflects the lower number_of_bits of a uint32_t and returns a value containing the reflected bits.
 *
 * This function reflects the specified number of lower bits of the given value and returns a value containing the reflected bits.
 *
 * @param value_in The input value to reflect. The full value range is valid and usable.
 * @param number_of_bits The number of bits which will be reflected. Valid range: ::kMinWidth <= value <= ::kMaxWidth. If the value is outside this range, a
 * ::radef_kInternalError fatal error is thrown.
 * @return The reflected output value. The full value range is valid and usable.
 */
static uint32_t ReflectBits(uint32_t value_in, uint16_t number_of_bits);

/**
 * @brief Generates a CRC lookup table according to the configured ::redcrc_check_code_type.
 *
 * This function generates a CRC lookup table and sets the variables redcrc_crc_mask and redcrc_crc_high_bit according to the options in the configured
 * ::redcrc_check_code_type. The generated CRC lookup table contains ::CRC_TABLE_SIZE elements.
 * The configured check code type must be in the range: ::redcty_kCheckCodeB <= value < ::redcty_kCheckCodeMax. If the value is outside this range, a
 * ::radef_kInternalError fatal error is thrown.
 * The configured CRC width must be in the range: ::kMinWidth <= value <= ::kMaxWidth. If the value is outside this range, a ::radef_kInternalError fatal error
 * is thrown.
 */
static void GenerateCrcTable(void);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

static const uint16_t kMinWidth = 8U;     ///< Minimum supported CRC width [bits]
static const uint16_t kMaxWidth = 32U;    ///< Maximum supported CRC width [bits]
static const uint16_t kBitsPerByte = 8U;  ///< Bits per byte [bits]

/**
 * @brief CRC options for all check code types.
 */
static const CrcOptions kCrcOptions[(uint16_t)redcty_kCheckCodeMax] = {
    {
        ///< Check code type A
        .width = 0U,              ///< Length of CRC [bits]
        .polynomial = 0U,         ///< The CRC polynomial without MSB
        .initial_optimized = 0U,  ///< The initial value for the table lookup algorithm
        .refin = false,           ///< true, if reflected input is enabled
        .refout = false,          ///< true, if reflected output is enabled
        .final_xor = 0U           ///< Value for the final xor operation, hast to be the same length as width
    },
    {
        ///< Check code type B
        .width = 32U,               ///< Length of CRC [bits]
        .polynomial = 0xEE5B42FDU,  ///< The CRC polynomial without MSB
        .initial_optimized = 0U,    ///< The initial value for the table lookup algorithm
        .refin = false,             ///< true, if reflected input is enabled
        .refout = false,            ///< true, if reflected output is enabled
        .final_xor = 0U             ///< Value for the final xor operation, hast to be the same length as width
    },
    {
        ///< Check code type C
        .width = 32U,                      ///< Length of CRC [bits]
        .polynomial = 0x1EDC6F41U,         ///< The CRC polynomial without MSB
        .initial_optimized = 0xFFFFFFFFU,  ///< The initial value for the table lookup algorithm
        .refin = true,                     ///< true, if reflected input is enabled
        .refout = true,                    ///< true, if reflected output is enabled
        .final_xor = 0xFFFFFFFFU           ///< Value for the final xor operation, hast to be the same length as width
    },
    {
        ///< Check code type D
        .width = 16U,             ///< Length of CRC [bits]
        .polynomial = 0x1021,     ///< The CRC polynomial without MSB
        .initial_optimized = 0U,  ///< The initial value for the table lookup algorithm
        .refin = true,            ///< true, if reflected input is enabled
        .refout = true,           ///< true, if reflected output is enabled
        .final_xor = 0U           ///< Value for the final xor operation, hast to be the same length as width
    },
    {
        ///< Check code type E
        .width = 16U,             ///< Length of CRC [bits]
        .polynomial = 0x8005,     ///< The CRC polynomial without MSB
        .initial_optimized = 0U,  ///< The initial value for the table lookup algorithm
        .refin = true,            ///< true, if reflected input is enabled
        .refout = true,           ///< true, if reflected output is enabled
        .final_xor = 0U           ///< Value for the final xor operation, hast to be the same length as width
    }};

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

void redcrc_Init(const redcty_CheckCodeType configured_check_code_type) {
  // Input parameter checks
  raas_AssertTrue(!redcrc_initialized, radef_kAlreadyInitialized);
  raas_AssertU16InRange((uint16_t)configured_check_code_type, (uint16_t)redcty_kCheckCodeMin, (uint16_t)(redcty_kCheckCodeMax)-1U, radef_kInvalidParameter);

  // Initialize local variables
  redcrc_check_code_type = configured_check_code_type;
  redcrc_initialized = true;

  if (redcrc_check_code_type != redcty_kCheckCodeA) {  // Check code type A is no CRC check -> no CRC table is generated
    // Generate CRC table
    GenerateCrcTable();
  }
}

void redcrc_CalculateCrc(const uint16_t data_size, const uint8_t* const data_buffer, uint32_t* const calculated_crc) {
  // Input parameter check
  raas_AssertTrue(redcrc_initialized, radef_kNotInitialized);
  raas_AssertU16InRange(data_size, RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE, RADEF_RED_LAYER_MESSAGE_HEADER_SIZE + RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE,
                        radef_kInvalidParameter);
  raas_AssertNotNull(data_buffer, radef_kInvalidParameter);
  raas_AssertNotNull(calculated_crc, radef_kInvalidParameter);
  // Range checks
  raas_AssertU16InRange((uint16_t)redcrc_check_code_type, (uint16_t)redcty_kCheckCodeB, (uint16_t)(redcty_kCheckCodeMax)-1U, radef_kInternalError);
  raas_AssertU16InRange(kCrcOptions[(uint16_t)redcrc_check_code_type].width, kMinWidth, kMaxWidth, radef_kInternalError);

  const CrcOptions* const kMyCrcOptions = &kCrcOptions[(uint16_t)redcrc_check_code_type];
  uint32_t crc = kMyCrcOptions->initial_optimized;
  uint16_t data_buffer_index = 0U;

  // Process reflected input
  if (kMyCrcOptions->refin) {
    crc = ReflectBits(crc, kMyCrcOptions->width);
  }

  // CRC calculation
  if (!kMyCrcOptions->refin) {
    while (data_buffer_index < data_size) {
      crc = (crc << kBitsPerByte) ^ redcrc_table[((crc >> (kMyCrcOptions->width - kBitsPerByte)) & 0x000000FFU) ^ data_buffer[data_buffer_index]];
      data_buffer_index++;
    }
  } else {
    while (data_buffer_index < data_size) {
      crc = (crc >> kBitsPerByte) ^ redcrc_table[(crc & 0x000000FFU) ^ data_buffer[data_buffer_index]];
      data_buffer_index++;
    }
  }

  // Output processing
  if ((kMyCrcOptions->refout || kMyCrcOptions->refin) && !(kMyCrcOptions->refout && kMyCrcOptions->refin)) {  // refout XOR refin
    // no coverage start (The above condition is never true, with the predefined constant CRC options)
    crc = ReflectBits(crc, kMyCrcOptions->width);
    // no coverage end
  }
  crc ^= kMyCrcOptions->final_xor;
  crc &= redcrc_crc_mask;

  *calculated_crc = crc;
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static uint32_t ReflectBits(uint32_t value_in, uint16_t number_of_bits) {
  // Input parameter check
  raas_AssertU32InRange(number_of_bits, kMinWidth, kMaxWidth, radef_kInternalError);

  uint32_t bit_value = 1U;
  uint32_t value_out = 0U;

  // Reflect the defined bits
  for (uint32_t bit_mask = (uint32_t)1U << (number_of_bits - 1U); bit_mask > 0U; bit_mask >>= 1U) {
    if ((value_in & bit_mask) != 0U) {
      value_out |= bit_value;
    }
    bit_value <<= 1U;
  }
  return value_out;
}

static void GenerateCrcTable(void) {
  // Range checks
  raas_AssertU16InRange((uint16_t)redcrc_check_code_type, (uint16_t)redcty_kCheckCodeB, (uint16_t)(redcty_kCheckCodeMax)-1U, radef_kInternalError);
  raas_AssertU16InRange(kCrcOptions[(uint16_t)redcrc_check_code_type].width, kMinWidth, kMaxWidth, radef_kInternalError);

  uint32_t bit;
  const CrcOptions* const kMyCrcOptions = &kCrcOptions[(uint16_t)redcrc_check_code_type];

  // Initialize CRC mask and CRC high bit
  redcrc_crc_mask = ((((uint32_t)1U << (kMyCrcOptions->width - 1U)) - 1U) << 1U) | 1U;
  redcrc_crc_high_bit = (uint32_t)1U << (kMyCrcOptions->width - 1U);

  // Generate CRC lookup table
  for (uint32_t table_index = 0U; table_index < CRC_TABLE_SIZE; table_index++) {
    uint32_t crc = table_index;
    if (kMyCrcOptions->refin) {
      crc = ReflectBits(crc, kBitsPerByte);
    }
    crc = (crc << (kMyCrcOptions->width - kBitsPerByte));

    for (uint16_t bit_index = 0U; bit_index < kBitsPerByte; bit_index++) {
      bit = crc & redcrc_crc_high_bit;
      crc <<= 1U;
      if (bit != 0U) {
        crc ^= kMyCrcOptions->polynomial;
      }
    }

    if (kMyCrcOptions->refin) {
      crc = ReflectBits(crc, kMyCrcOptions->width);
    }
    crc &= redcrc_crc_mask;
    redcrc_table[table_index] = crc;
  }
}
