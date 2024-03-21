/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redrbf_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the redrbf module
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDRBF_MOCK_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDRBF_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "redtyp_red_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta redrbf class
 */
class redrbfMock {
private:
  static redrbfMock *instance;  ///< redrbfMock instance

public:
  /**
  * @brief redrbfMock constructor
  */
  redrbfMock();
  /**
  * @brief redrbfMock destructor
  */
  virtual ~redrbfMock();

  /**
  * @brief Mock Method object for the redrbf_Init function
  */
  MOCK_METHOD(void, redrbf_Init, (const uint32_t configured_red_channels));

  /**
  * @brief Mock Method object for the redrbf_InitBuffer function
  */
  MOCK_METHOD(void, redrbf_InitBuffer, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the redrbf_AddToBuffer function
  */
  MOCK_METHOD(void, redrbf_AddToBuffer, (const uint32_t red_channel_id, const redtyp_RedundancyMessagePayload * message_payload));

  /**
  * @brief Mock Method object for the redrbf_ReadFromBuffer function
  */
  MOCK_METHOD(radef_RaStaReturnCode, redrbf_ReadFromBuffer, (const uint32_t red_channel_id, const uint16_t buffer_size, uint16_t * message_size, uint8_t * message_buffer));

  /**
  * @brief Mock Method object for the redrbf_GetFreeBufferEntries function
  */
  MOCK_METHOD(uint16_t, redrbf_GetFreeBufferEntries, (const uint32_t red_channel_id));

  /**
  * @brief Get the Instance object
  *
  * @return redrbfMock*
  */
  static redrbfMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDRBF_MOCK_HH_
