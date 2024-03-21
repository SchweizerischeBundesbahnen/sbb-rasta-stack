/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_sradno.cc
 *
 * @author Patrick Haldi, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6b952d4d5bcc7a5d8110a75f29f90f9169261d33
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,BUG 4932: update tests to verify the return value,09.12.2022, M. Kuhn}
 *
 * @brief Unit test file for the adapter notifications module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_sradno.hh"

#include "rasta_safety_retransmission/sradno_sr_adapter_notifications.h"
#include "rasta_safety_retransmission/srnot_sr_notifications.h"
#include "srcor_sr_core.h"
#include "srrece_sr_received_buffer.h"
#include "srsend_sr_send_buffer.h"
#include "srstm_sr_state_machine.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define BUF_SIZE 20U  ///< buffer size

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Custom matcher to check equality of transport channel diagnostic data
 */
MATCHER_P(EqRedDiagData, other, "Equality matcher for type radef_TransportChannelDiagnosticData") {
    return std::tie(arg.n_diagnosis, arg.n_missed, arg.t_drift, arg.t_drift2)
        == std::tie(other.n_diagnosis, other.n_missed, other.t_drift, other.t_drift2);
}

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test Class Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized message received notification test
 *
 * Parameter order:
 * - red_channel_id (uint32_t)
 * - event (srtyp_ConnectionEvents)
 * - free_rece_buf_entries (uint32_t)
 * - free_send_buf_entries (uint32_t)
 * - sequence_number_in_seq (bool)
 * - confirmed_timestamp_in_seq (bool)
 * - messages_to_receive (uint32_t)
 * - expect_process_sm (bool)
 * - exp_return_value (radef_RaStaReturnCode)
 * .
 */
class MessageReceiveNotificationTestWithParam : public sradnoTest,
    public testing::WithParamInterface<std::tuple<uint32_t, srtyp_ConnectionEvents, uint32_t, uint32_t, bool, bool, uint32_t, bool, radef_RaStaReturnCode>> {
public:
  const uint32_t red_channel_id = std::get<0>(GetParam()); ///< redundancy channel ID
  const srtyp_ConnectionEvents event = std::get<1>(GetParam()); ///< event
  const uint32_t free_rece_buf_entries = std::get<2>(GetParam()); ///< free rece buf entries
  const uint32_t free_send_buf_entries = std::get<3>(GetParam()); ///< free send buf entries
  const bool sequence_number_in_seq = std::get<4>(GetParam()); ///< sequence number in sequence
  const bool confirmed_timestamp_in_seq = std::get<5>(GetParam()); ///< confirmed timestamp in sequence
  const uint32_t messages_to_receive = std::get<6>(GetParam()); ///< number of messages to receive
  const bool expect_process_sm = std::get<7>(GetParam()); ///< indicates if the state machine is called
  const radef_RaStaReturnCode exp_return_value = std::get<8>(GetParam()); ///< expected return value
};

/**
 * @brief Test class for value-parametrized diagnostic notification test
 *
 * Parameter order:
 * - red_channel_id (uint32_t)
 * - tr_channel_id (uint32_t)
 * - n_diagnosis (uint32_t)
 * - n_missed (uint32_t)
 * - t_drift (uint32_t)
 * - t_drift2 (uint32_t)
 * - exp_return_value (radef_RaStaReturnCode)
 * .
 */
class DiagnosticNotificationTestWithParam : public sradnoTest,
    public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, radef_RaStaReturnCode>> {
public:
  const uint32_t red_channel_id = std::get<0>(GetParam()); ///< red channel ID
  const uint32_t tr_channel_id = std::get<1>(GetParam()); ///< tr channel ID
  const uint32_t n_diagnosis = std::get<2>(GetParam()); ///< N diagnosis
  const uint32_t n_missed = std::get<3>(GetParam()); ///< N missed
  const uint32_t t_drift = std::get<4>(GetParam()); ///< T drift
  const uint32_t t_drift2 = std::get<5>(GetParam()); ///< T drift2
  const radef_RaStaReturnCode exp_return_value = std::get<6>(GetParam()); ///< expected return value
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_adapter_notifications
 * @{
 */

/**
 * @test        @ID{sradnoTest001} Verify message received notification function
 *
 * @details     This test verifies that a call of the sr adapter message received
 *              notification function triggers the right calls to the ReceiveMessage
 *              function and the state machine.
 *
 * Test steps:
 * - Set expectations
 *  - srcor_SetReceivedMessagePendingFlag is called once (before loop)
 *  - srcor_GetReceivedMessagePendingFlag is called for every message received,
 *    or once if no message was received
 *  - srrece_GetFreeBufferEntries can be called any number of times, and will return the configured value
 *  - srsend_GetFreeBufferEntries can be called any number of times, and will return the configured value
 *  - If the buffers are ready: srcor_ReceiveMessage is called
 *  - If a message was read: srstm_ProcessConnectionStateMachine is called
 * - Call sradno_MessageReceivedNotification and verify the return value
 * .
 *
 * @testParameter
 * - red. channel Id: Redundancy channel identification
 * - event: Received Event to process
 * - free rece buf entries: Free received buffer entries on redundancy channel (messages) (max value: configured n_send_max)
 * - free send buf entries: Free send buffer entries on redundancy channel (messages) (max value: configured n_send_max)
 * - sequence number in seq.: Indicates if sequence number of received message in sequence
 * - confirmed timestamp in seq.: Indicates if confirmed timestamp of received message in sequence
 * - msgs to receive: Number of messages to receive
 * - expect process Sm: Indicates if the state machine is expect to be called
 * - return value: The expected function return value
 * | Test Run | Input parameter | Test config                                                                                                                                       |||||| Expected values                                              |||
 * |----------|-----------------|------------------------------|-----------------------|-----------------------|-------------------------|-----------------------------|-----------------|-------------------|-------------------------|------------------|
 * |          | red. channel Id | event                        | free rece buf entries | free send buf entries | sequence number in seq. | confirmed timestamp in seq. | msgs to receive | expect process Sm | return value            | Test Result      |
 * | 0        | 0               | srtyp_kConnEventDataReceived | 20                    | 20                    | true                    | true                        | 1               | true              | radef_kNoError          | Normal operation |
 * | 1        | 1               | srtyp_kConnEventDataReceived | 20                    | 20                    | true                    | true                        | 1               | true              | radef_kNoError          | Normal operation |
 * | 2        | 2               | srtyp_kConnEventDataReceived | 20                    | 20                    | true                    | true                        | 1               | true              | radef_kInvalidParameter | Normal operation |
 * | 3        | 0               | srtyp_kConnEventNone         | 20                    | 20                    | false                   | false                       | 1               | false             | radef_kNoError          | Normal operation |
 * | 4        | 0               | srtyp_kConnEventOpen         | 20                    | 20                    | false                   | false                       | 1               | true              | radef_kNoError          | Normal operation |
 * | 5        | 0               | srtyp_kConnEventMax          | 20                    | 20                    | false                   | false                       | 1               | true              | radef_kNoError          | Normal operation |
 * | 6        | 0               | srtyp_kConnEventDataReceived | 0                     | 20                    | true                    | true                        | 0               | false             | radef_kNoError          | Normal operation |
 * | 7        | 0               | srtyp_kConnEventDataReceived | 1                     | 20                    | true                    | true                        | 1               | true              | radef_kNoError          | Normal operation |
 * | 8        | 0               | srtyp_kConnEventDataReceived | 20                    | 0                     | true                    | true                        | 0               | false             | radef_kNoError          | Normal operation |
 * | 9        | 0               | srtyp_kConnEventDataReceived | 20                    | 1                     | true                    | true                        | 0               | false             | radef_kNoError          | Normal operation |
 * | 10       | 0               | srtyp_kConnEventDataReceived | 20                    | 2                     | true                    | true                        | 0               | false             | radef_kNoError          | Normal operation |
 * | 11       | 0               | srtyp_kConnEventDataReceived | 20                    | 3                     | true                    | true                        | 1               | true              | radef_kNoError          | Normal operation |
 * | 12       | 0               | srtyp_kConnEventDataReceived | 20                    | 20                    | false                   | true                        | 1               | true              | radef_kNoError          | Normal operation |
 * | 13       | 0               | srtyp_kConnEventDataReceived | 20                    | 20                    | true                    | false                       | 1               | true              | radef_kNoError          | Normal operation |
 * | 14       | 0               | srtyp_kConnEventDataReceived | 20                    | 20                    | true                    | true                        | 2               | true              | radef_kNoError          | Normal operation |
 * | 15       | 0               | srtyp_kConnEventDataReceived | 20                    | 20                    | true                    | true                        | 20              | true              | radef_kNoError          | Normal operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-654} Component sr_adapter_notifications Overview
 * @verifyReq{RASW-655} Message Received Notification
 * @verifyReq{RASW-335} Message Received Notification Structure
 * @verifyReq{RASW-338} Redundancy Channel Id
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 * @verifyReq{RASW-900} Error Code
 */
TEST_P(MessageReceiveNotificationTestWithParam, sradnoTest001VerifyMessageReceivedNotificationFunction) {
  // set expectations /////////////////////////////
  const uint32_t connection_id = red_channel_id;
  uint32_t possible_read_count = 0;

  if (messages_to_receive >= free_rece_buf_entries) {
    possible_read_count = free_rece_buf_entries;
  } else {
    possible_read_count = messages_to_receive;
  }

  if (radef_kNoError == exp_return_value) {
    EXPECT_CALL(srcor_mock, srcor_SetReceivedMessagePendingFlag(connection_id))
        .Times(1)
    ;
    // sequence for while loop
    {
      InSequence s1;
      if (possible_read_count >= 1) {
        EXPECT_CALL(srcor_mock, srcor_IsReceivedMsgPendingAndBuffersNotFull(connection_id)).Times(possible_read_count).WillRepeatedly(Return(true));
      }
      EXPECT_CALL(srcor_mock, srcor_IsReceivedMsgPendingAndBuffersNotFull(connection_id)).WillOnce(Return(false));
    }
    if (messages_to_receive > 0) {
      EXPECT_CALL(srcor_mock, srcor_ReceiveMessage(connection_id, _, _, _))
          .Times(messages_to_receive).WillRepeatedly(DoAll(
              SetArgPointee<1>(event),
              SetArgPointee<2>(sequence_number_in_seq),
              SetArgPointee<3>(confirmed_timestamp_in_seq)
          ))
      ;
    }
    else {
      EXPECT_CALL(srcor_mock, srcor_ReceiveMessage)
          .Times(0)
      ;
    }
    if (expect_process_sm) {
      EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine(connection_id,
          event,
          sequence_number_in_seq,
          confirmed_timestamp_in_seq
      )).Times(messages_to_receive);
    }
    else {
      EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine)
          .Times(0)
      ;
    }
  }

  // perform test /////////////////////////////////
  EXPECT_EQ(exp_return_value, sradno_MessageReceivedNotification(red_channel_id));
}

INSTANTIATE_TEST_SUITE_P(
  verifyMessageReceiveNotificationFunction,
  MessageReceiveNotificationTestWithParam,
  ::testing::Values(
//                    red_channel_id
//                    |             event
//                    |             |                                   free_rece_buf_entries
//                    |             |                                   |          free_send_buf_entries
//                    |             |                                   |          |          sequence_number_in_seq
//                    |             |                                   |          |          |      confirmed_timestamp_in_seq
//                    |             |                                   |          |          |      |      messages_to_be_read
//                    |             |                                   |          |          |      |      |   expect_process_sm
//                    |             |                                   |          |          |      |      |   |      exp_return_value
//                    |             |                                   |          |          |      |      |   |      |
      // vary red_channel_id
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  BUF_SIZE,  true,  true,  1,  true,  radef_kNoError),
      std::make_tuple(1U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  BUF_SIZE,  true,  true,  1,  true,  radef_kNoError),
      std::make_tuple(2U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  BUF_SIZE,  true,  true,  1,  true,  radef_kInvalidParameter),
      // vary event
      std::make_tuple(0U,           srtyp_kConnEventNone,               BUF_SIZE,  BUF_SIZE,  false, false, 1,  false, radef_kNoError),
      std::make_tuple(0U,           srtyp_kConnEventOpen,               BUF_SIZE,  BUF_SIZE,  false, false, 1,  true,  radef_kNoError),
      std::make_tuple(0U,           srtyp_kConnEventMax,                BUF_SIZE,  BUF_SIZE,  false, false, 1,  true,  radef_kNoError),
      // vary free_rece_buf_entries
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       0U,        BUF_SIZE,  true,  true,  0,  false, radef_kNoError),
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       1U,        BUF_SIZE,  true,  true,  1,  true,  radef_kNoError),
      // vary free_send_buf_entries
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  0U,        true,  true,  0,  false, radef_kNoError),
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  1U,        true,  true,  0,  false, radef_kNoError),
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  2U,        true,  true,  0,  false, radef_kNoError),
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  3U,        true,  true,  1,  true,  radef_kNoError),
      // vary sequence_number_in_seq
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  BUF_SIZE,  false, true,  1,  true,  radef_kNoError),
      // vary confirmed_timestamp_in_seq
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  BUF_SIZE,  true,  false, 1,  true,  radef_kNoError),
      // vary expect_receive_messages
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  BUF_SIZE,  true,  true,  2,  true,  radef_kNoError),
      std::make_tuple(0U,           srtyp_kConnEventDataReceived,       BUF_SIZE,  BUF_SIZE,  true,  true,  20, true,  radef_kNoError)
  )
);

/**
 * @test        @ID{sradnoTest002} Verify diagnostic notification function
 *
 * @details     This test verifies that a call of the sr adapter diagnostic notification
 *              function correctly forwards all variables.
 *
 * Test steps:
 * - Set expectations:
 *  - srnot_RedDiagnosticNotification is called once (with the defined params)
 * - Call sradno_DiagnosticNotification and verify the return value
 * .
 *
 * @testParameter
 * - red. channel Id: Redundancy channel identification
 * - tr channel Id: Transport channel identification
 * - N diagnosis: Diagnosis window size
 * - N missed: Number of messages which are not received on this transport channel within Tseq from the first reception on an other transport channel
 * - T drift: Sum of the delays of received messages in relation to the fastest transport channel
 * - T drift2: Sum of the squares of the delays of received messages in relation to the fastest transport channel
 * - return value: The expected function return value
 * .
 * | Test Run | Input parameter                                                                 |||||| Expected values                           ||
 * |----------|-----------------|---------------|-------------|------------|------------|------------|-------------------------|------------------|
 * |          | red. channel Id | tr channel Id | N diagnosis | N missed   | T drift    | T drift2   | return value            | Test Result      |
 * | 0        | 0               | 2             | 3           | 4          | 5          | 6          | radef_kNoError          | Normal operation |
 * | 1        | 1               | 2             | 3           | 4          | 5          | 6          | radef_kNoError          | Normal operation |
 * | 2        | 2               | 2             | 3           | 4          | 5          | 6          | radef_kInvalidParameter | Normal operation |
 * | 3        | 1               | 0             | 3           | 4          | 5          | 6          | radef_kNoError          | Normal operation |
 * | 4        | 1               | 0xffffffff    | 3           | 4          | 5          | 6          | radef_kNoError          | Normal operation |
 * | 5        | 1               | 2             | 0           | 4          | 5          | 6          | radef_kNoError          | Normal operation |
 * | 6        | 1               | 2             | 0xffffffff  | 4          | 5          | 6          | radef_kNoError          | Normal operation |
 * | 7        | 1               | 2             | 3           | 0          | 5          | 6          | radef_kNoError          | Normal operation |
 * | 8        | 1               | 2             | 3           | 0xffffffff | 5          | 6          | radef_kNoError          | Normal operation |
 * | 9        | 1               | 2             | 3           | 4          | 0          | 6          | radef_kNoError          | Normal operation |
 * | 10       | 1               | 2             | 3           | 4          | 0xffffffff | 6          | radef_kNoError          | Normal operation |
 * | 11       | 1               | 2             | 3           | 4          | 5          | 0          | radef_kNoError          | Normal operation |
 * | 12       | 1               | 2             | 3           | 4          | 5          | 0xffffffff | radef_kNoError          | Normal operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-654} Component sr_adapter_notifications Overview
 * @verifyReq{RASW-656} Diagnostic Notification
 * @verifyReq{RASW-332} Diagnostic Notification Structure
 * @verifyReq{RASW-331} Redundancy Channel Id
 * @verifyReq{RASW-334} Transport Channel Id
 * @verifyReq{RASW-333} Transport Channel Diagnostic Data
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-557} Red Diagnostic Notification
 * @verifyReq{RASW-325} RedL Diagnostic Notification Structure
 * @verifyReq{RASW-323} Connection Identification
 * @verifyReq{RASW-315} Redundancy Channel Diagnostic Data
 * @verifyReq{RASW-475} Struct Redundancy Channel Diagnostic Data Structure
 * @verifyReq{RASW-471} Transport Channel Id
 * @verifyReq{RASW-469} N diagnosis
 * @verifyReq{RASW-473} N missed
 * @verifyReq{RASW-472} T drift
 * @verifyReq{RASW-467} T drift2
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 * @verifyReq{RASW-901} Error Code
 */
TEST_P(DiagnosticNotificationTestWithParam, sradnoTest002VerifyDiagnosticNotificationFunction) {
  // set expectations /////////////////////////////
  const uint32_t connection_id = red_channel_id;

  const sraty_RedundancyChannelDiagnosticData expected_red_diag_data = {
      .transport_channel_id = tr_channel_id,
      .n_diagnosis = n_diagnosis,
      .n_missed = n_missed,
      .t_drift = t_drift,
      .t_drift2 = t_drift2
  };
  if (radef_kNoError == exp_return_value) {
    EXPECT_CALL(srnot_mock, srnot_RedDiagnosticNotification(connection_id, EqRedDiagData(expected_red_diag_data)))
      .Times(1);
  }

  // perform test /////////////////////////////////
  const radef_TransportChannelDiagnosticData tr_channel_diagnostic_data = {
      .n_diagnosis = n_diagnosis,
      .n_missed = n_missed,
      .t_drift = t_drift,
      .t_drift2 = t_drift2,
  };

  EXPECT_EQ(exp_return_value, sradno_DiagnosticNotification(red_channel_id, tr_channel_id, tr_channel_diagnostic_data));
}

INSTANTIATE_TEST_SUITE_P(
  verifyDiagnosticNotificationFunction,
  DiagnosticNotificationTestWithParam,
  ::testing::Values(
//                    red_channel_id
//                    |           tr_channel_id
//                    |           |           n_diagnosis
//                    |           |           |           n_missed
//                    |           |           |           |           t_drift
//                    |           |           |           |           |           t_drift2
//                    |           |           |           |           |           |           exp_return_value
//                    |           |           |           |           |           |           |
      // vary red_channel_id
      std::make_tuple(0U,         2U,         3U,         4U,         5U,         6U,         radef_kNoError),
      std::make_tuple(1U,         2U,         3U,         4U,         5U,         6U,         radef_kNoError),
      std::make_tuple(2U,         2U,         3U,         4U,         5U,         6U,         radef_kInvalidParameter),
      // vary tr_channel_id
      std::make_tuple(1U,         0U,         3U,         4U,         5U,         6U,         radef_kNoError),
      std::make_tuple(1U,         UINT32_MAX, 3U,         4U,         5U,         6U,         radef_kNoError),
      // vary n_diagnosis
      std::make_tuple(1U,         2U,         0U,         4U,         5U,         6U,         radef_kNoError),
      std::make_tuple(1U,         2U,         UINT32_MAX, 4U,         5U,         6U,         radef_kNoError),
      // vary n_missed
      std::make_tuple(1U,         2U,         3U,         0U,         5U,         6U,         radef_kNoError),
      std::make_tuple(1U,         2U,         3U,         UINT32_MAX, 5U,         6U,         radef_kNoError),
      // vary t_drift
      std::make_tuple(1U,         2U,         3U,         4U,         0U,         6U,         radef_kNoError),
      std::make_tuple(1U,         2U,         3U,         4U,         UINT32_MAX, 6U,         radef_kNoError),
      // vary t_drift2
      std::make_tuple(1U,         2U,         3U,         4U,         5U,         0U,         radef_kNoError),
      std::make_tuple(1U,         2U,         3U,         4U,         5U,         UINT32_MAX, radef_kNoError)
  )
);

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
