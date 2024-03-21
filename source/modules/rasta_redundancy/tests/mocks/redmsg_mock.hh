/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redmsg_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the redmsg module
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDMSG_MOCK_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDMSG_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_redundancy/redcty_red_config_types.h"
#include "redtyp_red_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta redmsg class
 */
class redmsgMock {
private:
  static redmsgMock *instance;  ///< redmsgMock instance

public:
  /**
  * @brief redmsgMock constructor
  */
  redmsgMock();
  /**
  * @brief redmsgMock destructor
  */
  virtual ~redmsgMock();

  /**
  * @brief Mock Method object for the redmsg_Init function
  */
  MOCK_METHOD(void, redmsg_Init, (const redcty_CheckCodeType configured_check_code_type));

  /**
  * @brief Mock Method object for the redmsg_CreateMessage function
  */
  MOCK_METHOD(void, redmsg_CreateMessage, (const uint32_t sequence_number, const redtyp_RedundancyMessagePayload * message_payload, redtyp_RedundancyMessage * redundancy_message));

  /**
  * @brief Mock Method object for the redmsg_CheckMessageCrc function
  */
  MOCK_METHOD(radef_RaStaReturnCode, redmsg_CheckMessageCrc, (const redtyp_RedundancyMessage * redundancy_message));

  /**
  * @brief Mock Method object for the redmsg_GetMessageSequenceNumber function
  */
  MOCK_METHOD(uint32_t, redmsg_GetMessageSequenceNumber, (const redtyp_RedundancyMessage * redundancy_message));

  /**
  * @brief Mock Method object for the redmsg_GetMessagePayload function
  */
  MOCK_METHOD(void, redmsg_GetMessagePayload, (const redtyp_RedundancyMessage * redundancy_message, redtyp_RedundancyMessagePayload * message_payload));

  /**
  * @brief Get the Instance object
  *
  * @return redmsgMock*
  */
  static redmsgMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDMSG_MOCK_HH_
