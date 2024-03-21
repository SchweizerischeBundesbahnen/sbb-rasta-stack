/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srrece_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the srrece module
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRRECE_MOCK_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRRECE_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "srtyp_sr_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta srrece class
 */
class srreceMock {
private:
  static srreceMock *instance;  ///< srreceMock instance

public:
  /**
  * @brief srreceMock constructor
  */
  srreceMock();
  /**
  * @brief srreceMock destructor
  */
  virtual ~srreceMock();

  /**
  * @brief Mock Method object for the srrece_Init function
  */
  MOCK_METHOD(void, srrece_Init, (const uint32_t configured_connections, const uint16_t configured_n_send_max));

  /**
  * @brief Mock Method object for the srrece_InitBuffer function
  */
  MOCK_METHOD(void, srrece_InitBuffer, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srrece_AddToBuffer function
  */
  MOCK_METHOD(void, srrece_AddToBuffer, (const uint32_t connection_id, const srtyp_SrMessagePayload * message_payload));

  /**
  * @brief Mock Method object for the srrece_ReadFromBuffer function
  */
  MOCK_METHOD(radef_RaStaReturnCode, srrece_ReadFromBuffer, (const uint32_t connection_id, srtyp_SrMessagePayload * message_payload));

  /**
  * @brief Mock Method object for the srrece_GetPayloadSizeOfNextMessageToRead function
  */
  MOCK_METHOD(uint32_t, srrece_GetPayloadSizeOfNextMessageToRead, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srrece_GetFreeBufferEntries function
  */
  MOCK_METHOD(uint16_t, srrece_GetFreeBufferEntries, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srrece_GetUsedBufferEntries function
  */
  MOCK_METHOD(uint16_t, srrece_GetUsedBufferEntries, (const uint32_t connection_id));

  /**
  * @brief Get the Instance object
  *
  * @return srreceMock*
  */
  static srreceMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRRECE_MOCK_HH_
