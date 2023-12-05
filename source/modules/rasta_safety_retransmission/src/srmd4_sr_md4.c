/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srmd4_sr_md4.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA SafRetL MD4 module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srmd4_sr_md4.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define MD4_INPUT_DATA_BLOCK_SIZE (64U)  ///< Size of a input data block used for the MD4 calculation [bytes]
#define BYTES_PER_U32 (4U)               ///< Bytes per uint32_t [bytes]

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Enum for the selection of the basic MD4 functions.
 */
typedef enum {
  kMd4FunctionF = 0U,  ///< Basic MD4 function F
  kMd4FunctionG = 1U,  ///< Basic MD4 function G
  kMd4FunctionH = 2U   ///< Basic MD4 function H
} Md4Function;

/**
 * @brief Structure holding the context data for a MD4 calculation.
 */
typedef struct {
  uint32_t byte_count_low;                    ///< Data size byte count lower 29 bits [bytes], will be multiplied by 8 to get the number of bits.
  uint32_t bit_count_high;                    ///< Data size bit count upper 32 bits [bits].
  uint32_t a;                                 ///< A part of the calculated hash.
  uint32_t b;                                 ///< B part of the calculated hash.
  uint32_t c;                                 ///< C part fo the calculated hash.
  uint32_t d;                                 ///< D part of the calculated hash.
  uint8_t buffer[MD4_INPUT_DATA_BLOCK_SIZE];  ///< Internal buffer for remaining data size < 64 bytes.
  uint32_t block[MD4_INPUT_DATA_BLOCK_SIZE / BYTES_PER_U32];  ///< Internal 32 bit aligned data block.
} Md4Context;

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Context of MD4 calculation.
 */
PRIVATE Md4Context srmd4_context;

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup sr_md4
 * @{
 */

/**
 * @brief Inner function for MD4 calculation.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 * @implementsReq{RASW-634} Safety Code
 *
 * @param [inout] ctx Pointer to the MD4 context data structure. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [in] data Pointer to the input data. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [in] number_of_blocks Number of the 64 byte input data blocks [blocks]. Full value range is valid and usable.
 */
static void Md4Body(Md4Context *const ctx, const uint8_t *const data, const uint32_t number_of_blocks);

/**
 * @brief Calculation of MD4. Allows incremental calculation.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 * @implementsReq{RASW-634} Safety Code
 *
 * @param [inout] ctx Pointer to the MD4 context data structure. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [in] data Pointer to the input data. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [in] size Size of the input data [bytes]. Full value range is valid and usable.
 */
static void Md4Update(Md4Context *const ctx, const uint8_t *const data, const uint32_t size);

/**
 * @brief Final MD4 calculations, generating the MD4 hash.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 * @implementsReq{RASW-634} Safety Code
 *
 * @param [inout] ctx Pointer to the MD4 context data structure. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [out] result The MD4 hash result. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void Md4Final(Md4Context *ctx, uint8_t *result);

/**
 * @brief Helper function to set the MD4 context data buffer memory to a dedicated value.
 *
 * @pre Following condition must respected: size + md4_context_buffer_start_index has a valid range of: 0U <= value <=MD4_INPUT_DATA_BLOCK_SIZE.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 *
 * @param [in] value Value to set. Full value range is valid and usable.
 * @param [in] size Size of data to copy [bytes]. Valid range: 0 <= value <= ::MD4_INPUT_DATA_BLOCK_SIZE.
 * @param [in] md4_context_buffer_start_index Write start index in the MD4 context buffer [bytes]. Valid range: 0 <= value < ::MD4_INPUT_DATA_BLOCK_SIZE.
 * @param [inout] md4_context The MD4 context. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void SetContextBuffer(const uint8_t value, const uint32_t size, const uint32_t md4_context_buffer_start_index, Md4Context *const md4_context);

/**
 * @brief Helper function to copy memory to a MD4 context data buffer memory.
 *
 * @pre Following condition must respected: size + md4_context_buffer_start_index has a valid range of: 0U <= value <=MD4_INPUT_DATA_BLOCK_SIZE.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 *
 * @param [in] source Pointer to source data buffer.
 * @param [in] size Size of data to copy [bytes]. Valid range: 0 <= value <= ::MD4_INPUT_DATA_BLOCK_SIZE.
 * @param [in] md4_context_buffer_start_index Write start index in the MD4 context buffer [bytes]. Valid range: 0 <= value < ::MD4_INPUT_DATA_BLOCK_SIZE.
 * @param [inout] md4_context The MD4 context. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void CopyToContextBuffer(const uint8_t *const source, const uint32_t size, const uint32_t md4_context_buffer_start_index, Md4Context *const md4_context);

/**
 * @brief Helper function to clear the MD4 context data.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 *
 * @param [inout] md4_context The MD4 context. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void ClearMd4ContextData(Md4Context *const md4_context);

/**
 * @brief Helper to write a Uint32 value in a byte array.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 *
 * @param [in] source Uint32 source value. Full value range is valid and usable.
 * @param [out] destination Pointer to destination byte array. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void WriteU32toByteArray(const uint32_t source, uint8_t *const destination);

/**
 * @brief Basic MD4 calculation function F, which returns F = z ^ (x & (y ^ z)).
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 *
 * @param [in] x Input parameter x. The full value range is valid and usable.
 * @param [in] y Input parameter y. The full value range is valid and usable.
 * @param [in] z Input parameter z. The full value range is valid and usable.
 * @return unit32_t The calculation result.
 */
static uint32_t FunctionF(const uint32_t x, const uint32_t y, const uint32_t z);

/**
 * @brief Basic MD4 calculation function G, which returns G = (x & (y | z)) | (y & z).
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 *
 * @param [in] x Input parameter x. The full value range is valid and usable.
 * @param [in] y Input parameter y. The full value range is valid and usable.
 * @param [in] z Input parameter z. The full value range is valid and usable.
 * @return unit32_t The calculation result.
 */
static uint32_t FunctionG(const uint32_t x, const uint32_t y, const uint32_t z);

/**
 * @brief Basic MD4 calculation function H, which returns H = x ^ y ^ z.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 *
 * @param [in] x Input parameter x. The full value range is valid and usable.
 * @param [in] y Input parameter y. The full value range is valid and usable.
 * @param [in] z Input parameter z. The full value range is valid and usable.
 * @return unit32_t The calculation result.
 */
static uint32_t FunctionH(const uint32_t x, const uint32_t y, const uint32_t z);

/**
 * @brief The MD4 transformation function for all three rounds.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 *
 * @param [in] md4_function Basic MD4 function used for this step. The values ::kMd4FunctionF, ::kMd4FunctionG, ::kMd4FunctionH are valid and usable. For other
 * values a ::radef_kInternalError fatal error is thrown.
 * @param [inout] a Input/output parameter a. The full value range is valid and usable.
 * @param [in] b Input parameter b. The full value range is valid and usable.
 * @param [in] c Input parameter c. The full value range is valid and usable.
 * @param [in] d Input parameter d. The full value range is valid and usable.
 * @param [in] x Input parameter x. The full value range is valid and usable.
 * @param [in] s Input parameter s. The full value range is valid and usable.
 */
static void Step(const Md4Function md4_function, uint32_t *const a, const uint32_t b, const uint32_t c, const uint32_t d, const uint32_t x, const uint32_t s);

/**
 * @brief Reads 4 input data bytes and stores them in a properly aligned Uint32 value in a MD4 context data block and returns this Uint32 value.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 *
 * @param [in] index_32_bit Index in the MD4 context data block [bytes]. The full value range is valid and usable.
 * @param [in] data Pointer to start of current input data block. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [inout] ctx Pointer to the MD4 context data structure. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @return uint32_t Set Uint32 value at given index.
 */
static uint32_t Set(const uint8_t index_32_bit, const uint8_t *const data, Md4Context *const ctx);

/**
 * @brief Get a properly aligned Uint32 value from a MD4 context data block.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 *
 * @param [in] index_32_bit Index in the MD4 context data block [bytes]. The full value range is valid and usable.
 * @param [in] ctx Pointer to the MD4 context data structure.  If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @return unit32_t Requested Uint32 value at given index.
 */
static uint32_t Get(const uint8_t index_32_bit, const Md4Context *const ctx);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

static const uint32_t kDataBlockSizeBits = 6U;        ///< Number of bits for the size of a data block used for the MD4 calculation [bits]
static const uint32_t kDataBlockSizeBitMask = 0x3FU;  ///< Bits used for masking the size of a data block used for the MD4 calculation
static const uint32_t kDataBitCountLowIndex = 56U;    ///< Data block index of lower data bit counter [bytes]
static const uint32_t kDataBitCountHighIndex = 60U;   ///< Data block index of upper data bit counter [bytes]
static const uint32_t kAc1 = 0x5A827999U;             ///< Constant for ::FunctionG calculation
static const uint32_t kAc2 = 0x6ED9EBA1U;             ///< Constant for ::FunctionH calculation

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
void srmd4_CalculateMd4(const srcty_Md4InitValue md4_initial_value, const uint16_t data_size, const uint8_t *const data_buffer,
                        srmd4_Md4 *const calculated_md4) {
  // Input parameter check
  raas_AssertNotNull(data_buffer, radef_kInvalidParameter);
  raas_AssertNotNull(calculated_md4, radef_kInvalidParameter);
  raas_AssertU16InRange(data_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE - RADEF_MAX_SR_LAYER_SAFETY_CODE_SIZE,
                        radef_kInvalidParameter);

  ClearMd4ContextData(&srmd4_context);

  srmd4_context.a = md4_initial_value.init_a;
  srmd4_context.b = md4_initial_value.init_b;
  srmd4_context.c = md4_initial_value.init_c;
  srmd4_context.d = md4_initial_value.init_d;

  Md4Update(&srmd4_context, data_buffer, data_size);

  Md4Final(&srmd4_context, calculated_md4->md4);
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

/*
 * This processes one or more 64-byte data blocks, but does NOT update the bit
 * counters.
 */
static void Md4Body(Md4Context *const ctx, const uint8_t *const data, const uint32_t number_of_blocks) {
  // Input parameter check
  raas_AssertNotNull(ctx, radef_kInternalError);
  raas_AssertNotNull(data, radef_kInternalError);

  const uint8_t *current_data_block;  // pointer to current processed 64 byte data block
  uint32_t a, b, c, d;
  uint32_t saved_a, saved_b, saved_c, saved_d;  // cppcheck-suppress variableScope

  uint32_t block_counter = number_of_blocks;

  current_data_block = data;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  while (block_counter > 0U) {
    saved_a = a;
    saved_b = b;
    saved_c = c;
    saved_d = d;

    /* Round 1 */
    Step(kMd4FunctionF, &a, b, c, d, Set(0U, current_data_block, ctx), 3U);
    Step(kMd4FunctionF, &d, a, b, c, Set(1U, current_data_block, ctx), 7U);
    Step(kMd4FunctionF, &c, d, a, b, Set(2U, current_data_block, ctx), 11U);
    Step(kMd4FunctionF, &b, c, d, a, Set(3U, current_data_block, ctx), 19U);
    Step(kMd4FunctionF, &a, b, c, d, Set(4U, current_data_block, ctx), 3U);
    Step(kMd4FunctionF, &d, a, b, c, Set(5U, current_data_block, ctx), 7U);
    Step(kMd4FunctionF, &c, d, a, b, Set(6U, current_data_block, ctx), 11U);
    Step(kMd4FunctionF, &b, c, d, a, Set(7U, current_data_block, ctx), 19U);
    Step(kMd4FunctionF, &a, b, c, d, Set(8U, current_data_block, ctx), 3U);
    Step(kMd4FunctionF, &d, a, b, c, Set(9U, current_data_block, ctx), 7U);
    Step(kMd4FunctionF, &c, d, a, b, Set(10U, current_data_block, ctx), 11U);
    Step(kMd4FunctionF, &b, c, d, a, Set(11U, current_data_block, ctx), 19U);
    Step(kMd4FunctionF, &a, b, c, d, Set(12U, current_data_block, ctx), 3U);
    Step(kMd4FunctionF, &d, a, b, c, Set(13U, current_data_block, ctx), 7U);
    Step(kMd4FunctionF, &c, d, a, b, Set(14U, current_data_block, ctx), 11U);
    Step(kMd4FunctionF, &b, c, d, a, Set(15U, current_data_block, ctx), 19U);

    /* Round 2 */
    Step(kMd4FunctionG, &a, b, c, d, Get(0U, ctx) + kAc1, 3U);
    Step(kMd4FunctionG, &d, a, b, c, Get(4U, ctx) + kAc1, 5U);
    Step(kMd4FunctionG, &c, d, a, b, Get(8U, ctx) + kAc1, 9U);
    Step(kMd4FunctionG, &b, c, d, a, Get(12U, ctx) + kAc1, 13U);
    Step(kMd4FunctionG, &a, b, c, d, Get(1U, ctx) + kAc1, 3U);
    Step(kMd4FunctionG, &d, a, b, c, Get(5U, ctx) + kAc1, 5U);
    Step(kMd4FunctionG, &c, d, a, b, Get(9U, ctx) + kAc1, 9U);
    Step(kMd4FunctionG, &b, c, d, a, Get(13U, ctx) + kAc1, 13U);
    Step(kMd4FunctionG, &a, b, c, d, Get(2U, ctx) + kAc1, 3U);
    Step(kMd4FunctionG, &d, a, b, c, Get(6U, ctx) + kAc1, 5U);
    Step(kMd4FunctionG, &c, d, a, b, Get(10U, ctx) + kAc1, 9U);
    Step(kMd4FunctionG, &b, c, d, a, Get(14U, ctx) + kAc1, 13U);
    Step(kMd4FunctionG, &a, b, c, d, Get(3U, ctx) + kAc1, 3U);
    Step(kMd4FunctionG, &d, a, b, c, Get(7U, ctx) + kAc1, 5U);
    Step(kMd4FunctionG, &c, d, a, b, Get(11U, ctx) + kAc1, 9U);
    Step(kMd4FunctionG, &b, c, d, a, Get(15U, ctx) + kAc1, 13U);

    /* Round 3 */
    Step(kMd4FunctionH, &a, b, c, d, Get(0U, ctx) + kAc2, 3U);
    Step(kMd4FunctionH, &d, a, b, c, Get(8U, ctx) + kAc2, 9U);
    Step(kMd4FunctionH, &c, d, a, b, Get(4U, ctx) + kAc2, 11U);
    Step(kMd4FunctionH, &b, c, d, a, Get(12U, ctx) + kAc2, 15U);
    Step(kMd4FunctionH, &a, b, c, d, Get(2U, ctx) + kAc2, 3U);
    Step(kMd4FunctionH, &d, a, b, c, Get(10U, ctx) + kAc2, 9U);
    Step(kMd4FunctionH, &c, d, a, b, Get(6U, ctx) + kAc2, 11U);
    Step(kMd4FunctionH, &b, c, d, a, Get(14U, ctx) + kAc2, 15U);
    Step(kMd4FunctionH, &a, b, c, d, Get(1U, ctx) + kAc2, 3U);
    Step(kMd4FunctionH, &d, a, b, c, Get(9U, ctx) + kAc2, 9U);
    Step(kMd4FunctionH, &c, d, a, b, Get(5U, ctx) + kAc2, 11U);
    Step(kMd4FunctionH, &b, c, d, a, Get(13U, ctx) + kAc2, 15U);
    Step(kMd4FunctionH, &a, b, c, d, Get(3U, ctx) + kAc2, 3U);
    Step(kMd4FunctionH, &d, a, b, c, Get(11U, ctx) + kAc2, 9U);
    Step(kMd4FunctionH, &c, d, a, b, Get(7U, ctx) + kAc2, 11U);
    Step(kMd4FunctionH, &b, c, d, a, Get(15U, ctx) + kAc2, 15U);

    a += saved_a;
    b += saved_b;
    c += saved_c;
    d += saved_d;

    current_data_block = &current_data_block[MD4_INPUT_DATA_BLOCK_SIZE];
    --block_counter;
  }

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;
}

static void Md4Update(Md4Context *const ctx, const uint8_t *const data, const uint32_t size) {
  // Input parameter check
  raas_AssertNotNull(ctx, radef_kInternalError);
  raas_AssertNotNull(data, radef_kInternalError);

  const uint8_t *current_data = data;
  uint32_t current_size = size;

  ctx->byte_count_low = current_size;

  ctx->bit_count_high += current_size >> 29U;  // Update upper data bit counter in bits

  if (current_size >= MD4_INPUT_DATA_BLOCK_SIZE) {
    Md4Body(ctx, current_data, current_size >> kDataBlockSizeBits);
    current_data = &current_data[(current_size >> kDataBlockSizeBits) * MD4_INPUT_DATA_BLOCK_SIZE];
    current_size &= kDataBlockSizeBitMask;
  }

  // Copy remaining data bytes to buffer for Md4Final()
  CopyToContextBuffer(current_data, current_size, 0U, ctx);
}

static void Md4Final(Md4Context *ctx, uint8_t *result) {
  // Input parameter check
  raas_AssertNotNull(ctx, radef_kInternalError);
  raas_AssertNotNull(result, radef_kInternalError);

  uint32_t used_buffer_bytes, unused_buffer_bytes;

  used_buffer_bytes = ctx->byte_count_low & kDataBlockSizeBitMask;

  ctx->buffer[used_buffer_bytes] = 0x80U;  // Set the first padding bit to 1
  ++used_buffer_bytes;

  unused_buffer_bytes = MD4_INPUT_DATA_BLOCK_SIZE - used_buffer_bytes;

  if (unused_buffer_bytes < (BYTES_PER_U32 * 2U)) {
    // not enough space to add the bit counters to the buffer of this data block
    SetContextBuffer(0U, unused_buffer_bytes, used_buffer_bytes, ctx);  // Set remaining padding bits of this data block to 0
    Md4Body(ctx, ctx->buffer, 1U);                                      // Perform an additional MD4 calculation of this data block
    used_buffer_bytes = 0U;
    unused_buffer_bytes = MD4_INPUT_DATA_BLOCK_SIZE;
  }

  // Set remaining padding bits to 0
  SetContextBuffer(0U, unused_buffer_bytes - (BYTES_PER_U32 * 2U), used_buffer_bytes, ctx);

  WriteU32toByteArray(ctx->byte_count_low << 3U,
                      &ctx->buffer[kDataBitCountLowIndex]);  // Multiply byte_count_low by 8 and write lower 32 bits of the bit counter to the buffer
  WriteU32toByteArray(ctx->bit_count_high, &ctx->buffer[kDataBitCountHighIndex]);  // Write upper 32 bits of the bit counter to the buffer

  Md4Body(ctx, ctx->buffer, 1U);  // Final MD4 calculation

  // Write calculated MD4 hash to result
  WriteU32toByteArray(ctx->a, &result[0U]);
  WriteU32toByteArray(ctx->b, &result[BYTES_PER_U32]);
  WriteU32toByteArray(ctx->c, &result[BYTES_PER_U32 * 2U]);
  WriteU32toByteArray(ctx->d, &result[BYTES_PER_U32 * 3U]);

  ClearMd4ContextData(ctx);
}

static void SetContextBuffer(const uint8_t value, const uint32_t size, const uint32_t md4_context_buffer_start_index, Md4Context *const md4_context) {
  // Input parameter check
  raas_AssertNotNull(md4_context, radef_kInternalError);
  raas_AssertU32InRange(md4_context_buffer_start_index, 0U, MD4_INPUT_DATA_BLOCK_SIZE - 1U, radef_kInternalError);
  raas_AssertU32InRange(size, 0U, MD4_INPUT_DATA_BLOCK_SIZE, radef_kInternalError);
  raas_AssertU32InRange(size + md4_context_buffer_start_index, 0U, MD4_INPUT_DATA_BLOCK_SIZE, radef_kInternalError);

  for (uint32_t index = 0U; index < size; ++index) {
    //lint -save -e661 (Out of bounds pointer access is checked by input parameter check)
    md4_context->buffer[md4_context_buffer_start_index + index] = value;
    //lint -restore
  }
}

static void CopyToContextBuffer(const uint8_t *const source, const uint32_t size, const uint32_t md4_context_buffer_start_index,
                                Md4Context *const md4_context) {
  // Input parameter check
  raas_AssertNotNull(md4_context, radef_kInternalError);
  raas_AssertNotNull(source, radef_kInternalError);
  raas_AssertU32InRange(md4_context_buffer_start_index, 0U, MD4_INPUT_DATA_BLOCK_SIZE - 1U, radef_kInternalError);
  raas_AssertU32InRange(size, 0U, MD4_INPUT_DATA_BLOCK_SIZE, radef_kInternalError);
  raas_AssertU32InRange(size + md4_context_buffer_start_index, 0U, MD4_INPUT_DATA_BLOCK_SIZE, radef_kInternalError);

  for (uint32_t index = 0U; index < size; ++index) {
    //lint -save -e661 (Out of bounds pointer access is checked by input parameter check)
    md4_context->buffer[md4_context_buffer_start_index + index] = source[index];
    //lint -restore
  }
}

static void ClearMd4ContextData(Md4Context *const md4_context) {
  // Input parameter check
  raas_AssertNotNull(md4_context, radef_kInternalError);

  md4_context->a = 0U;
  md4_context->b = 0U;
  md4_context->c = 0U;
  md4_context->d = 0U;
  md4_context->byte_count_low = 0U;
  md4_context->bit_count_high = 0U;

  SetContextBuffer(0U, MD4_INPUT_DATA_BLOCK_SIZE, 0U, md4_context);

  for (uint32_t index = 0U; index < (MD4_INPUT_DATA_BLOCK_SIZE / BYTES_PER_U32); ++index) {
    md4_context->block[index] = 0U;
  }
}

static void WriteU32toByteArray(const uint32_t source, uint8_t *const destination) {
  // Input parameter check
  raas_AssertNotNull(destination, radef_kInternalError);

  destination[0U] = (uint8_t)source;
  destination[1U] = (uint8_t)(source >> 8U);
  destination[2U] = (uint8_t)(source >> 16U);
  destination[3U] = (uint8_t)(source >> 24U);
}

static uint32_t FunctionF(const uint32_t x, const uint32_t y, const uint32_t z) {
  return (z ^ (x & (y ^ z)));
}

static uint32_t FunctionG(const uint32_t x, const uint32_t y, const uint32_t z) {
  return ((x & (y | z)) | (y & z));
}

static uint32_t FunctionH(const uint32_t x, const uint32_t y, const uint32_t z) {
  return (x ^ y ^ z);
}

static void Step(const Md4Function md4_function, uint32_t *const a, const uint32_t b, const uint32_t c, const uint32_t d, const uint32_t x, const uint32_t s) {
  // Input parameter check
  raas_AssertNotNull(a, radef_kInternalError);

  switch (md4_function) {
    case kMd4FunctionF:
      *a += (FunctionF(b, c, d) + x);
      break;
    case kMd4FunctionG:
      *a += (FunctionG(b, c, d) + x);
      break;
    case kMd4FunctionH:
      *a += (FunctionH(b, c, d) + x);
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);
      break;
      // no coverage end
  }
  *a = ((*a << s) | ((*a & 0xFFFFFFFFU) >> (32U - s)));
}

static uint32_t Set(const uint8_t index_32_bit, const uint8_t *const data, Md4Context *const ctx) {
  // Input parameter check
  raas_AssertNotNull(data, radef_kInternalError);
  raas_AssertNotNull(ctx, radef_kInternalError);

  return ctx->block[index_32_bit] = (uint32_t)data[index_32_bit * BYTES_PER_U32] | ((uint32_t)data[(index_32_bit * BYTES_PER_U32) + 1U] << 8U) |
                                    ((uint32_t)data[(index_32_bit * BYTES_PER_U32) + 2U] << 16U) | ((uint32_t)data[(index_32_bit * BYTES_PER_U32) + 3U] << 24U);
}

static uint32_t Get(const uint8_t index_32_bit, const Md4Context *const ctx) {
  // Input parameter check
  raas_AssertNotNull(ctx, radef_kInternalError);

  return ctx->block[index_32_bit];
}
