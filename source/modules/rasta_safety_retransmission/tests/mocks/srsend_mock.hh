/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srsend_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the srsend module
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRSEND_MOCK_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRSEND_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "srtyp_sr_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta srsend class
 */
class srsendMock {
private:
  static srsendMock *instance;  ///< srsendMock instance

public:
  /**
  * @brief srsendMock constructor
  */
  srsendMock();
  /**
  * @brief srsendMock destructor
  */
  virtual ~srsendMock();

  /**
  * @brief Mock Method object for the srsend_Init function
  */
  MOCK_METHOD(void, srsend_Init, (const uint32_t configured_connections));

  /**
  * @brief Mock Method object for the srsend_InitBuffer function
  */
  MOCK_METHOD(void, srsend_InitBuffer, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srsend_AddToBuffer function
  */
  MOCK_METHOD(void, srsend_AddToBuffer, (const uint32_t connection_id, const srtyp_SrMessage * message));

  /**
  * @brief Mock Method object for the srsend_ReadMessageToSend function
  */
  MOCK_METHOD(radef_RaStaReturnCode, srsend_ReadMessageToSend, (const uint32_t connection_id, srtyp_SrMessage * message));

  /**
  * @brief Mock Method object for the srsend_PrepareBufferForRetr function
  */
  MOCK_METHOD(void, srsend_PrepareBufferForRetr, (const uint32_t connection_id, const uint32_t sequence_number_for_retransmission, const srtyp_SrMessageHeaderCreate message_header, uint32_t * new_current_sequence_number));

  /**
  * @brief Mock Method object for the srsend_IsSequenceNumberInBuffer function
  */
  MOCK_METHOD(radef_RaStaReturnCode, srsend_IsSequenceNumberInBuffer, (const uint32_t connection_id, const uint32_t sequence_number));

  /**
  * @brief Mock Method object for the srsend_RemoveFromBuffer function
  */
  MOCK_METHOD(void, srsend_RemoveFromBuffer, (const uint32_t connection_id, const uint32_t confirmed_sequence_number));

  /**
  * @brief Mock Method object for the srsend_GetFreeBufferEntries function
  */
  MOCK_METHOD(uint16_t, srsend_GetFreeBufferEntries, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srsend_GetUsedBufferEntries function
  */
  MOCK_METHOD(uint16_t, srsend_GetUsedBufferEntries, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srsend_GetNumberOfMessagesToSend function
  */
  MOCK_METHOD(uint16_t, srsend_GetNumberOfMessagesToSend, (const uint32_t connection_id));

  /**
  * @brief Get the Instance object
  *
  * @return srsendMock*
  */
  static srsendMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRSEND_MOCK_HH_
