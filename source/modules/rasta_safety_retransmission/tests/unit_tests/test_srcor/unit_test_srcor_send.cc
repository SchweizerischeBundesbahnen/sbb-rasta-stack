/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srcor_send.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 0e2d52fb20e3afe4ee3b5800471e64319a7d4cfe
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,BUG 4933: Duplicated diagnostic notification,06.12.2022,M. Kuhn}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4959: Added connection id check before setting last confirmed seq nbr,12.12.2022,N. Fricker}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4960: Added connection id check before setting seq nbr and timestamp rx,12.12.2022,N. Fricker}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4961: Moved setting timestamp rx value to prevent out of boundary access,12.12.2022,N. Fricker}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4962: Moved setting timestamp and sequence number to prevent out of boundary access,12.12.2022,N. Fricker}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4963: Moved connection setup to prevent out of boundary access,12.12.2022,N. Fricker}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4964: Moved setting timestamp and sequence number to prevent out of boundary access,12.12.2022,N. Fricker}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4965: Moved setting confirmed timestamp and sequence number to prevent out of boundary access,12.12.2022,N. Fricker}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4966: Added connection id check before setting message in buffer,12.12.2022,N. Fricker}
 *
 * @brief Unit test file for the send functions from the core module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srcor.hh"

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

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
 * @brief Test class for value-parametrized SendPendingMessages tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - message type (srtyp_SrMessageType)
 * - used buffer entries (uint16_t)
 * - amount of messages to send (uint16_t)
 * - opposite receive buffer size (uint16_t)
 * - return code of srsend_ReadMessageToSend function (radef_RaStaReturnCode)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class SendPendingMessagesTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, srtyp_SrMessageType, uint16_t, uint16_t, uint16_t, radef_RaStaReturnCode, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  srtyp_SrMessageType message_type =                      std::get<2>(GetParam());    ///< message type
  uint16_t used_buffer_entries  =                         std::get<3>(GetParam());    ///< used buffer entries
  uint16_t messages_to_send =                             std::get<4>(GetParam());    ///< messages to send
  uint16_t opposite_receive_buffer_size =                 std::get<5>(GetParam());    ///< opposite receive buffer size
  radef_RaStaReturnCode read_message_result =             std::get<6>(GetParam());    ///< return code of srsend_ReadMessageToSend function
  radef_RaStaReturnCode expected_error =                  std::get<7>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized SendDataMessage tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - indicates if a message is added to the temporary buffer (bool)
 * - sequence number of message (uint32_t)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class SendDataMessageTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool, uint32_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  bool write_message_to_temporary_buffer =                std::get<2>(GetParam());    ///< indicates if a message is added to temporary buffer
  uint32_t sequence_number =                              std::get<3>(GetParam());    ///< sequence number of message
  radef_RaStaReturnCode expected_error =                  std::get<4>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized SendConnReqMessage tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class SendConnReqMessageTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  radef_RaStaReturnCode expected_error =                  std::get<2>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized SendConnRespMessage tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - sequence number of message (uint32_t)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class SendConnRespMessageTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  uint32_t sequence_number =                              std::get<2>(GetParam());    ///< sequence number of message
  radef_RaStaReturnCode expected_error =                  std::get<3>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized SendDiscReqMessage tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - sequence number of message (uint32_t)
 * - disconnect reason (sraty_DiscReason)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class SendDiscReqMessageTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, sraty_DiscReason, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  uint32_t sequence_number =                              std::get<2>(GetParam());    ///< sequence number of message
  sraty_DiscReason disconnect_reason =                    std::get<3>(GetParam());    ///< disconnect reason
  radef_RaStaReturnCode expected_error =                  std::get<4>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized SendHbMessage tests
 *
 * This class simply inherits from SendConnRespMessageTestWithParam due to equal parameter
 */
class SendHbMessageTestWithParam : public SendConnRespMessageTestWithParam
{

};

/**
 * @brief Test class for value-parametrized SendRetrReqMessage tests
 *
 * This class simply inherits from SendConnRespMessageTestWithParam due to equal parameter
 */
class SendRetrReqMessageTestWithParam : public SendConnRespMessageTestWithParam
{

};

/**
 * @brief Test class for value-parametrized SendConnectionStateNotification tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - connection_state (sraty_ConnectionStates)
 * - disconnect reason (sraty_DiscReason)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class SendConnectionStateNotificationTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, sraty_ConnectionStates, sraty_DiscReason, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  sraty_ConnectionStates connection_state =               std::get<2>(GetParam());    ///< connection state
  sraty_DiscReason disconnect_reason =                    std::get<3>(GetParam());    ///< disconnect reason
  radef_RaStaReturnCode expected_error =                  std::get<4>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized WriteMessagePayloadToTemporaryBuffer tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - message payload size (uint16_t)
 * - indicates if message in buffer (bool)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class WriteMessagePayloadToTemporaryBufferTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint16_t, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  uint16_t message_payload_size =                         std::get<2>(GetParam());    ///< message payload size
  bool message_in_buffer =                                std::get<3>(GetParam());    ///< indicates if message in buffer
  radef_RaStaReturnCode expected_error =                  std::get<4>(GetParam());    ///< expected error
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_core
 * @{
 */

/**
 * @test        @ID{srcorTest007} Verify the SendPendingMessages function
 *
 * @details     This test verifies the SendPendingMessages function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Initialize the module
 * - Set confirmed seq nbr to verify if in header
 * - Set expectations
 *   - For allow message to send check (used buffer entries and messages to send parameter)
 *   - If message can be send -> For Reading message, get and update message header and send message call with message content check
 * - Call function with given connection ID
 * - Verify timestamp and confirmed sequence number in header
 * - Verify time stamp tx and last send confirmed sequence number
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Message type: Message type of messages to send
 * - Used buffer entries: Number of used buffer entries (Messages)
 * - Messages to send: Number of messages to send
 * - Opposite buffer size: Size of opposite buffer (Messages)
 * - Read msg return code: Return value of srsend_ReadMessageToSend function call
 * - Expected error: Expected error code when calling srcor_SendPendingMessages function
 * .
 * | Test Run | Input parameter | Test config                                                                                                               |||||| Expected values                             ||
 * |----------|-----------------|------------------------|--------------|---------------------|------------------|----------------------|------------------------|-------------------------|--------------------|
 * |          | Connection Id   | Configured connections | Message type | Used buffer entries | Messages to send | Opposite buffer size | Read msg return code   | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | ConnReq      | 1                   | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 1        | 0U              | 1                      | ConnResp     | 1                   | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 1                      | RetrReq      | 1                   | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 3        | 0U              | 1                      | RetrResp     | 1                   | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 4        | 0U              | 1                      | DiscReq      | 1                   | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 5        | 0U              | 1                      | Hb           | 1                   | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 6        | 0U              | 1                      | Data         | 1                   | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 7        | 0U              | 1                      | RetrData     | 1                   | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 8        | 0U              | 1                      | Data         | 1                   | 0                | 20                   | radef_kNoMessageToSend | radef_kNoError          | Normal operation   |
 * | 9        | 0U              | 1                      | Data         | 20                  | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 10       | 0U              | 1                      | Data         | 20                  | 20               | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 11       | 0U              | 1                      | Data         | 1                   | 0                | 20                   | radef_kNoMessageToSend | radef_kNoError          | Normal operation   |
 * | 12       | 0U              | 1                      | Data         | 20                  | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 13       | 0U              | 1                      | Data         | 20                  | 20               | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 14       | 0U              | 1                      | Data         | 10                  | 10               | 10                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 15       | 0U              | 1                      | ConnReq      | 1                   | 1                | 20                   | radef_kNoMessageToSend | radef_kInternalError    | Fatal error thrown |
 * | 16       | 0U              | 2                      | ConnReq      | 1                   | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 17       | 1U              | 2                      | ConnReq      | 1                   | 1                | 20                   | radef_kNoError         | radef_kNoError          | Normal operation   |
 * | 18       | 1U              | 1                      | ConnReq      | 1                   | 1                | 20                   | radef_kNoError         | radef_kInvalidParameter | Fatal error thrown |
 * | 19       | 2U              | 2                      | ConnReq      | 1                   | 1                | 20                   | radef_kNoError         | radef_kInvalidParameter | Fatal error thrown |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-587} Send Pending Messages Function
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-651} Send Message Function
 * @verifyReq{RASW-364} Send Message Function Structure
 * @verifyReq{RASW-385} Message Data
 * @verifyReq{RASW-387} Message Size
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-422} Timer Value
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(SendPendingMessagesTestWithParam, srcorTest007VerifySendPendingMessagesFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config =   default_config;
  test_config.number_of_connections =                     configured_connections;
  srtyp_SrMessageHeader message_header =                  {0, message_type, 0, 0, 0, 0, 0, 0};
  srtyp_SrMessage message =                               {UT_SRCOR_MESSAGE_LEN_MAX, {}};
  uint16_t messages_to_send_count =                       messages_to_send;
  uint32_t timer_values[messages_to_send] =               {};
  uint32_t confirmed_seq_number =                         10;

  // parameter values to check
  srtyp_SrMessageHeaderUpdate message_header_updates[messages_to_send] = {};
  const uint8_t * message_data = {};

  // add content to message
  for (uint16_t i = 0U; i < message.message_size; i++)
  {
    message.message[i] = test_message_data_u8[i];
  }

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  if(expected_error != radef_kInvalidParameter)
  {
    // set last confirmed seq nbr
    srcor_rasta_connections[connection_id].confirmed_sequence_number_tx = confirmed_seq_number;
  }

  // set expectations
  if(expected_error == radef_kNoError || read_message_result != radef_kNoError)
  {
    InSequence s;
    for (uint16_t i = 0; i <= messages_to_send; i++)
    {
      // expect calls for allow message to send check. srsend_GetNumberOfMessagesToSend gets called two times
      EXPECT_CALL(srsend_mock, srsend_GetUsedBufferEntries(connection_id)).WillOnce(Return(used_buffer_entries));
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(messages_to_send_count));

      // calculate not confirmed messages (used buffer entries - messages to send)
      uint16_t not_confirmed_messages = used_buffer_entries;
      not_confirmed_messages -= messages_to_send_count;

      // check if allowed to send message
      if(messages_to_send_count > 0 && opposite_receive_buffer_size > not_confirmed_messages)
      {
        if(read_message_result == radef_kNoError)
        {
          // expect calls for sending message
          EXPECT_CALL(srsend_mock, srsend_ReadMessageToSend(connection_id, _)).WillOnce(DoAll(SetArgPointee<1>(message), Return(read_message_result)));
          EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).WillOnce(Return(timer_values[i]));
          EXPECT_CALL(srmsg_mock, srmsg_GetMessageHeader(_, _)).WillOnce(SetArgPointee<1>(message_header));
          EXPECT_CALL(srmsg_mock, srmsg_UpdateMessageHeader(_, _)).WillOnce(SaveArg<0>(&message_header_updates[i]));
          EXPECT_CALL(sradin_mock, sradin_SendMessage(_, UT_SRCOR_MESSAGE_LEN_MAX, _)).WillOnce(SaveArg<2>(&message_data));
        }
        else
        {
          EXPECT_CALL(srsend_mock, srsend_ReadMessageToSend(connection_id, _)).WillOnce(Return(read_message_result));
          // internal error gets thrown when no message could be read although previously checked if messages are in buffer
          break;
        }

        // decrement messages to send count for srsend_GetNumberOfMessagesToSend call return value
        messages_to_send_count--;

        // increment timer value for next message
        timer_values[i + 1] = timer_values[i] + 5;
      }
      else
      {
        // message not allowed to send anymore
        break;
      }
    }
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_SendPendingMessages(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_SendPendingMessages(connection_id));

    if(messages_to_send > 0)
    {
      // verify passed message headers parameter
      for(uint16_t i = 0; i < messages_to_send; i ++)
      {
        // verify header update timestamp and confirmed seq number (must be 0 if ConnReq message)
        EXPECT_EQ(message_header_updates[i].time_stamp, timer_values[i]);

        if(message_type == srtyp_kSrMessageConnReq)
        {
          EXPECT_EQ(message_header_updates[i].confirmed_sequence_number, 0);
        }
        else
        {
          EXPECT_EQ(message_header_updates[i].confirmed_sequence_number, confirmed_seq_number);
        }
      }

      // verify message content
      for(uint16_t i = 0; i < message.message_size; i++)
      {
        EXPECT_EQ(message_data[i], test_message_data_u8[i]);
      }

      // verify time stamp tx
      EXPECT_EQ(srcor_rasta_connections[connection_id].time_stamp_tx, timer_values[messages_to_send - 1]);

      // verify last send confirmed sequence number
      EXPECT_EQ(srcor_rasta_connections[connection_id].last_send_confirmed_sequence_number_tx, message_header_updates[messages_to_send - 1].confirmed_sequence_number);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySendPendingMessagesFunction,
  SendPendingMessagesTestWithParam,
  ::testing::Values(
    // vary message type
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageConnReq, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageConnResp, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageRetrReq, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageRetrResp, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageDiscReq, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageHb, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageData, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageRetrData, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    // vary messages to send and used buffer size
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageData, 1, 0, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoMessageToSend, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageData, 20, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageData, 20, 20, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    // vary opposite buffer size
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageData, 1, 0, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageData, 20, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageData, 20, 20, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageData, 10, 10, 10, radef_kNoError, radef_kNoError),
    // vary read message result
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageConnReq, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoMessageToSend , radef_kInternalError),
    // vary configured connections and connection ids
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageConnReq, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, srtyp_kSrMessageConnReq, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, srtyp_kSrMessageConnReq, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), srtyp_kSrMessageConnReq, 1, 1, UT_SRCOR_SEND_BUFFER_SIZE_MAX, radef_kNoError, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest008} Verify the SendDataMessage function
 *
 * @details     This test verifies the SendDataMessage function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Initialize the module
 * - Write message payload to temporary buffer if parameter true
 * - Set expectations
 *   - For create and add message to buffer
 *   - For message allowed to send check
 *   - For send pending messages call
 * - Call the function
 * - Verify header (sender id, receiver id, seq number, confirmed timestamp) which gets used to create message
 * - Verify that sequence number TX is incremented for the next message to send
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Add msg to temporary buffer: Indicates if a message gets added to the temporary buffer
 * - Msg seq number: Sequence number of message
 * - Expected error: Expected error code when calling function
 * .
 * | Test Run | Input parameter | Test config                                                           ||| Expected values                             ||
 * |----------|-----------------|------------------------|-----------------------------|------------------|-------------------------|--------------------|
 * |          | Connection Id   | Configured connections | Add msg to temporary buffer | Msg seq number   | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | false                       | 0U               | radef_kNoMessageToSend  | Fatal error thrown |
 * | 1        | 0U              | 1                      | true                        | 0U               | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 1                      | true                        | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 3        | 0U              | 2                      | true                        | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 4        | 1U              | 2                      | true                        | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 5        | 1U              | 1                      | false                       | 0U               | radef_kInvalidParameter | Fatal error thrown |
 * | 6        | 2U              | 2                      | false                       | 0U               | radef_kInvalidParameter | Fatal error thrown |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-584} Send Data Message Function
 * @verifyReq{RASW-593} Write Message Payload to Temporary Buffer Function
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-651} Send Message Function
 * @verifyReq{RASW-364} Send Message Function Structure
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(SendDataMessageTestWithParam, srcorTest008VerifySendDataMessageFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config =     default_config;
  test_config.number_of_connections =                       configured_connections;
  uint16_t payload_size =                                   UT_SRCOR_PAYLOAD_LEN_MAX;
  uint8_t message_payload[payload_size] =                   {};
  uint32_t time_stamp_rx =                                  1000;

  // parameter values to check
  srtyp_SrMessageHeaderCreate message_header =              {};

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  if(expected_error != radef_kInvalidParameter)
  {
    // set current seq number on connection
    srcor_rasta_connections[connection_id].sequence_number_tx = sequence_number;

    // set timestamp rx to verify in header
    srcor_rasta_connections[connection_id].time_stamp_rx = time_stamp_rx;

    if(write_message_to_temporary_buffer)
    {
      srcor_WriteMessagePayloadToTemporaryBuffer(connection_id, payload_size, message_payload);
    }
  }

  // set expectations
  if(expected_error == radef_kNoError)
  {
    EXPECT_CALL(srmsg_mock, srmsg_CreateDataMessage(_, _, _)).WillOnce(SaveArg<0>(&message_header));
    EXPECT_CALL(srsend_mock, srsend_AddToBuffer(connection_id, _)).Times(1);

    // set allow send message check expectations to allow one message to send
    EXPECT_CALL(srsend_mock, srsend_GetUsedBufferEntries(connection_id)).Times(2).WillRepeatedly(Return(1));

    {
      InSequence s;
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(1));
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(0));
    }

    // set expect calls for sending message
    EXPECT_CALL(srsend_mock, srsend_ReadMessageToSend(connection_id, _)).WillOnce(Return(radef_kNoError));
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_GetMessageHeader(_, _)).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_UpdateMessageHeader(_, _)).Times(1);
    EXPECT_CALL(sradin_mock, sradin_SendMessage(_, _, _)).Times(1);
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_SendDataMessage(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_SendDataMessage(connection_id));

    // verify header which was used to create message
    EXPECT_EQ(default_config.connection_configurations[connection_id].sender_id, message_header.sender_id);
    EXPECT_EQ(default_config.connection_configurations[connection_id].receiver_id, message_header.receiver_id);
    EXPECT_EQ(sequence_number, message_header.sequence_number);
    EXPECT_EQ(srcor_rasta_connections[connection_id].time_stamp_rx, message_header.confirmed_time_stamp);

    // verify seq number gets incremented for next message
    EXPECT_EQ(sequence_number + 1, srcor_rasta_connections[connection_id].sequence_number_tx);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySendDataMessageFunction,
  SendDataMessageTestWithParam,
  ::testing::Values(
    // vary adding message to temp buffer
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, false, 0, radef_kNoMessageToSend),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, 0, radef_kNoError),
    // vary seq id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, UINT32_MAX, radef_kNoError),
    // vary configured connections and connection ids
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, true, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, true, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, false, 0, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), false, 0, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest010} Verify the SendConnReqMessage function
 *
 * @details     This test verifies the SendConnReqMessage function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Initialize the module
 * - Set any value to the time_stamp_rx except 0
 * - Set expectations
 *   - For create and add message to buffer
 *   - For message allowed to send check
 *   - For send pending messages call
 * - Call the function
 * - Verify header (sender id, receiver id, seq number, confirmed timestamp) which gets used to create message
 * - Verify time_stamp_rx is reset
 * - Verify next sequence number to send
 * - Verify that seq number tx gets incremented for next message
 * - Verify protocol version
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Expected error: Expected error code when calling srcor_SendConnReqMessage function
 * .
 * | Test Run | Input parameter | Test config            | Expected values                             ||
 * |----------|-----------------|------------------------|-------------------------|--------------------|
 * |          | Connection Id   | Configured connections | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | radef_kNoError          | Normal operation   |
 * | 1        | 0U              | 2                      | radef_kNoError          | Normal operation   |
 * | 2        | 1U              | 2                      | radef_kNoError          | Normal operation   |
 * | 3        | 1U              | 1                      | radef_kInvalidParameter | Fatal error thrown |
 * | 4        | 2U              | 2                      | radef_kInvalidParameter | Fatal error thrown |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-582} Send ConnReq Message Function
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-651} Send Message Function
 * @verifyReq{RASW-364} Send Message Function Structure
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(SendConnReqMessageTestWithParam, srcorTest010VerifySendConnReqMessageFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;

  // parameter values to check
  srtyp_SrMessageHeaderCreate message_header = {};
  srtyp_ProtocolVersion protocol_version = {};

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  // set expectations
  if(expected_error == radef_kNoError)
  {
    // set to any value > 0 to check if reset
    srcor_rasta_connections[connection_id].time_stamp_rx = 100U;

    EXPECT_CALL(srmsg_mock, srmsg_CreateConnReqMessage(_, _, test_config.n_send_max, _)).WillOnce(DoAll(SaveArg<0>(&message_header),SaveArg<1>(&protocol_version)));
    EXPECT_CALL(srsend_mock, srsend_AddToBuffer(connection_id, _)).Times(1);

    // set allow send message check expectations to allow one message to send
    EXPECT_CALL(srsend_mock, srsend_GetUsedBufferEntries(connection_id)).Times(2).WillRepeatedly(Return(1));

    {
      InSequence s;
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(1));
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(0));
    }

    // set expect calls for sending message
    EXPECT_CALL(srsend_mock, srsend_ReadMessageToSend(connection_id, _)).WillOnce(Return(radef_kNoError));
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_GetMessageHeader(_, _)).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_UpdateMessageHeader(_, _)).Times(1);
    EXPECT_CALL(sradin_mock, sradin_SendMessage(_, _, _)).Times(1);
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_SendConnReqMessage(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_SendConnReqMessage(connection_id));

    // verify header which was used to create message
    EXPECT_EQ(default_config.connection_configurations[connection_id].sender_id, message_header.sender_id);
    EXPECT_EQ(default_config.connection_configurations[connection_id].receiver_id, message_header.receiver_id);
    EXPECT_EQ(0U, message_header.sequence_number);
    EXPECT_EQ(0U, srcor_rasta_connections[connection_id].time_stamp_rx);
    EXPECT_EQ(srcor_rasta_connections[connection_id].time_stamp_rx, message_header.confirmed_time_stamp);

    // verify seq number tx gets incremented for next message
    EXPECT_EQ(1, srcor_rasta_connections[connection_id].sequence_number_tx);

    // verify protocol version
    for(uint16_t i = 0; i < 4; i ++)
    {
      EXPECT_EQ(protocol_version.version[i], expected_protocol_version.version[i]);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySendConnReqMessageFunction,
  SendConnReqMessageTestWithParam,
  ::testing::Values(
    // vary configured connections and connection ids
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest009} Verify the SendConnRespMessage function
 *
 * @details     This test verifies the SendConnRespMessage function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Initialize the module
 * - Set any value to the time_stamp_rx
 * - Set expectations
 *   - For create and add message to buffer
 *   - For message allowed to send check
 *   - For send pending messages call
 * - Call the function
 * - Verify header (sender id, receiver id, seq number, confirmed timestamp) which gets used to create message
 * - Verify that sequence number TX is incremented for the next message to send
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Seq nbr: Sequence number of message
 * - Expected error: Expected error code when calling srcor_SendConnRespMessage function
 * .
 * | Test Run | Input parameter | Test config                         || Expected values                             ||
 * |----------|-----------------|------------------------|-------------|-------------------------|--------------------|
 * |          | Connection ID   | Configured connections | Seq nbr     | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | 0U          | radef_kNoError          | Normal operation   |
 * | 1        | 0U              | 1                      | 0xffffffffU | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 2                      | 0xffffffffU | radef_kNoError          | Normal operation   |
 * | 3        | 1U              | 2                      | 0xffffffffU | radef_kNoError          | Normal operation   |
 * | 4        | 1U              | 1                      | 0U          | radef_kInvalidParameter | Fatal error raised |
 * | 5        | 2U              | 2                      | 0U          | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-583} Send ConnResp Message Function
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-651} Send Message Function
 * @verifyReq{RASW-364} Send Message Function Structure
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(SendConnRespMessageTestWithParam, srcorTest009VerifySendConnRespMessageFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint32_t confirmed_timestamp = 100;

  // parameter values to check
  srtyp_SrMessageHeaderCreate message_header = {};

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  // set expectations
  if(expected_error == radef_kNoError)
  {
    // set timestamp and seq nbr to verify in header
    srcor_rasta_connections[connection_id].time_stamp_rx = confirmed_timestamp;
    srcor_rasta_connections[connection_id].sequence_number_tx = sequence_number;

    EXPECT_CALL(srmsg_mock, srmsg_CreateConnRespMessage(_, _, test_config.n_send_max, _)).WillOnce(SaveArg<0>(&message_header));
    EXPECT_CALL(srsend_mock, srsend_AddToBuffer(connection_id, _)).Times(1);

    // set allow send message check expectations to allow one message to send
    EXPECT_CALL(srsend_mock, srsend_GetUsedBufferEntries(connection_id)).Times(2).WillRepeatedly(Return(1));

    {
      InSequence s;
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(1));
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(0));
    }

    // set expect calls for sending message
    EXPECT_CALL(srsend_mock, srsend_ReadMessageToSend(connection_id, _)).WillOnce(Return(radef_kNoError));
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_GetMessageHeader(_, _)).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_UpdateMessageHeader(_, _)).Times(1);
    EXPECT_CALL(sradin_mock, sradin_SendMessage(_, _, _)).Times(1);
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_SendConnRespMessage(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_SendConnRespMessage(connection_id));

    // verify header which was used to create message
    EXPECT_EQ(default_config.connection_configurations[connection_id].sender_id, message_header.sender_id);
    EXPECT_EQ(default_config.connection_configurations[connection_id].receiver_id, message_header.receiver_id);
    EXPECT_EQ(sequence_number, message_header.sequence_number);
    EXPECT_EQ(srcor_rasta_connections[connection_id].time_stamp_rx, message_header.confirmed_time_stamp);

    // verify seq number gets incremented for next message
    EXPECT_EQ(sequence_number + 1, srcor_rasta_connections[connection_id].sequence_number_tx);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySendConnRespMessageFunction,
  SendConnRespMessageTestWithParam,
  ::testing::Values(
    // vary configured connections and connection ids
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, 0, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), 0, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest011} Verify the SendDiscReqMessage function
 *
 * @details     This test verifies the SendDiscReqMessage function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Initialize the module
 * - Set values to time_stamp_rx, sequence_number_tx and confirmed_sequence_number_tx (to verify them in header)
 * - Call srcor_SetDiscDetailedReason to verify detail reason in srmsg_CreateDiscReqMessage call
 * - Set expectations
 *   - For creating message and updating header
 *   - For sending message, init buffer (for reset), closing channel and sending diagnostics data
 * - Call the function
 * - Verify header (sender id, receiver id, seq number, confirmed timestamp) which gets used to create message
 * - Verify header update (confirmed seq nbr and timestamp)
 * - Verify that sequence number TX is incremented for the next message to send
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Disconnect reason: Disconnect reason
 * - Configured connections: Number of configured connections
 * - Msg seq number: Sequence number of message
 * - Expected error: Expected error code when calling srcor_SendDiscReqMessage function
 * .
 * | Test Run | Input parameter                      || Test config                              || Expected values                             ||
 * |----------|---------------|-----------------------|------------------------|------------------|-------------------------|--------------------|
 * |          | Connection Id | Disconnect reason     | Configured connections | Msg seq number   | Expected error          | Test Result        |
 * | 0        | 0U            | UserRequest           | 1                      | 0U               | radef_kNoError          | Normal operation   |
 * | 1        | 0U            | UserRequest           | 1                      | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 2        | 0U            | NotInUse              | 1                      | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 3        | 0U            | UnexpectedMessage     | 1                      | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 4        | 0U            | SequenceNumberError   | 1                      | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 5        | 0U            | Timeout               | 1                      | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 6        | 0U            | ServiceNotAllowed     | 1                      | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 7        | 0U            | ProtocolVersionError  | 1                      | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 8        | 0U            | RetransmissionFailed  | 1                      | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 9        | 0U            | ProtocolSequenceError | 1                      | 0xffffffffU      | radef_kNoError          | Normal operation   |
 * | 10       | 0U            | Max                   | 1                      | 0U               | radef_kInvalidParameter | Fatal error raised |
 * | 11       | 0U            | UserRequest           | 2                      | 0U               | radef_kNoError          | Normal operation   |
 * | 12       | 1U            | UserRequest           | 2                      | 0U               | radef_kNoError          | Normal operation   |
 * | 13       | 1U            | UserRequest           | 1                      | 0U               | radef_kInvalidParameter | Fatal error raised |
 * | 14       | 2U            | UserRequest           | 2                      | 0U               | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-585} Send DiscReq Message Function
 * @verifyReq{RASW-589} Set Disconnection Detailed Reason Function
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-650} Close Redundancy Channel Function
 * @verifyReq{RASW-368} Close Redundancy Channel Function Structure
 * @verifyReq{RASW-367} Redundancy Channel Id
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-651} Send Message Function
 * @verifyReq{RASW-364} Send Message Function Structure
 * @verifyReq{RASW-363} Redundancy Channel Id
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-422} Timer Value
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 * @verifyReq{RASW-489} Enum Disc Reason Structure
 */
TEST_P(SendDiscReqMessageTestWithParam, srcorTest011VerifySendDiscReqMessageFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config =     default_config;
  test_config.number_of_connections =                       configured_connections;
  uint16_t payload_size =                                   UT_SRCOR_PAYLOAD_LEN_MAX;
  uint8_t message_payload[payload_size] =                   {};
  const uint32_t timer_value =                              100;
  const uint32_t confirmed_sequence_number =                10;
  const uint32_t timestamp =                                50;
  const uint16_t detailed_disc_reason =                     200;

  // parameter values to check
  srtyp_SrMessageHeaderCreate message_header =              {};
  srtyp_SrMessageHeaderUpdate message_header_update =       {};

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  // set expectations
  if(expected_error == radef_kNoError)
  {
    // set confirmed timestamp and current and last confirmed seq number on connection
    srcor_rasta_connections[connection_id].time_stamp_rx = timestamp;
    srcor_rasta_connections[connection_id].sequence_number_tx = sequence_number;
    srcor_rasta_connections[connection_id].confirmed_sequence_number_tx = confirmed_sequence_number;

    // set disconnect reason to verify in call
    EXPECT_NO_THROW(srcor_SetDiscDetailedReason(connection_id, detailed_disc_reason));

    EXPECT_CALL(srmsg_mock, srmsg_CreateDiscReqMessage(_, detailed_disc_reason, disconnect_reason, _)).WillOnce(SaveArg<0>(&message_header));
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).WillOnce(Return(timer_value));
    EXPECT_CALL(srmsg_mock, srmsg_UpdateMessageHeader(_, _)).WillOnce(SaveArg<0>(&message_header_update));
    EXPECT_CALL(sradin_mock, sradin_SendMessage(connection_id, _, _)).Times(1);

    // expect buffer to be reset
    EXPECT_CALL(srsend_mock, srsend_InitBuffer(connection_id)).Times(1);

    // expect channel to be closed (1:1 mapping connection and redundancy channel id)
    EXPECT_CALL(sradin_mock, sradin_CloseRedundancyChannel(connection_id)).Times(1);
    // expect diagnostics notification to be sent at closing channel
    EXPECT_CALL(srdia_mock, srdia_SendDiagnosticNotification(connection_id)).Times(1);
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_SendDiscReqMessage(connection_id, disconnect_reason), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_SendDiscReqMessage(connection_id, disconnect_reason));

    // verify header which was used to create message
    EXPECT_EQ(default_config.connection_configurations[connection_id].sender_id, message_header.sender_id);
    EXPECT_EQ(default_config.connection_configurations[connection_id].receiver_id, message_header.receiver_id);
    EXPECT_EQ(sequence_number, message_header.sequence_number);
    EXPECT_EQ(srcor_rasta_connections[connection_id].time_stamp_rx, message_header.confirmed_time_stamp);

    // verify message header update
    EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_sequence_number_tx, message_header_update.confirmed_sequence_number);
    EXPECT_EQ(timer_value, message_header_update.time_stamp);

    // verify seq number gets incremented for next message
    EXPECT_EQ(sequence_number + 1, srcor_rasta_connections[connection_id].sequence_number_tx);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySendDiscReqMessageFunction,
  SendDiscReqMessageTestWithParam,
  ::testing::Values(
    // vary sequence number
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, sraty_kDiscReasonUserRequest, radef_kNoError),
    // vary disconnect reason
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, sraty_kDiscReasonNotInUse, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, sraty_kDiscReasonUnexpectedMessage, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, sraty_kDiscReasonSequenceNumberError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, sraty_kDiscReasonTimeout, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, sraty_kDiscReasonServiceNotAllowed, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, sraty_kDiscReasonProtocolVersionError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, sraty_kDiscReasonRetransmissionFailed, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, sraty_kDiscReasonProtocolSequenceError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, sraty_kDiscReasonMax, radef_kInvalidParameter),
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, 0, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, 0, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, 0, sraty_kDiscReasonUserRequest, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), 0, sraty_kDiscReasonUserRequest, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest012} Verify the SendHbMessage function
 *
 * @details     This test verifies the SendHbMessage function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Initialize the module
 * - Set any value to the time_stamp_rx
 * - Set expectations
 *   - For create and add message to buffer
 *   - For message allowed to send check
 *   - For send pending messages call
 * - Call the function
 * - Verify header (sender id, receiver id, seq number, confirmed timestamp) which gets used to create message
 * - Verify that sequence number TX is incremented for the next message to send
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Seq nbr: Sequence number of message
 * - Expected error: Expected error code when calling srcor_SendHbMessage function
 * .
 * | Test Run | Input parameter | Test config                         || Expected values                             ||
 * |----------|-----------------|------------------------|-------------|-------------------------|--------------------|
 * |          | Connection Id   | Configured connections | Seq nbr     | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | 0U          | radef_kNoError          | Normal operation   |
 * | 1        | 0U              | 1                      | 0xffffffffU | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 2                      | 0xffffffffU | radef_kNoError          | Normal operation   |
 * | 3        | 1U              | 2                      | 0xffffffffU | radef_kNoError          | Normal operation   |
 * | 4        | 1U              | 1                      | 0U          | radef_kInvalidParameter | Fatal error raised |
 * | 5        | 2U              | 2                      | 0U          | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-586} Send Heartbeat Message Function
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-651} Send Message Function
 * @verifyReq{RASW-364} Send Message Function Structure
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(SendHbMessageTestWithParam, srcorTest012VerifySendHbMessageFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint32_t confirmed_timestamp = 100;

  // parameter values to check
  srtyp_SrMessageHeaderCreate message_header = {};

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  // set expectations
  if(expected_error == radef_kNoError)
  {
    // set timestamp and seq nbr to verify in header
    srcor_rasta_connections[connection_id].time_stamp_rx = confirmed_timestamp;
    srcor_rasta_connections[connection_id].sequence_number_tx = sequence_number;

    EXPECT_CALL(srmsg_mock, srmsg_CreateHeartbeatMessage(_, _)).WillOnce(SaveArg<0>(&message_header));
    EXPECT_CALL(srsend_mock, srsend_AddToBuffer(connection_id, _)).Times(1);

    // set allow send message check expectations to allow one message to send
    EXPECT_CALL(srsend_mock, srsend_GetUsedBufferEntries(connection_id)).Times(2).WillRepeatedly(Return(1));

    {
      InSequence s;
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(1));
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(0));
    }

    // set expect calls for sending message
    EXPECT_CALL(srsend_mock, srsend_ReadMessageToSend(connection_id, _)).WillOnce(Return(radef_kNoError));
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_GetMessageHeader(_, _)).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_UpdateMessageHeader(_, _)).Times(1);
    EXPECT_CALL(sradin_mock, sradin_SendMessage(_, _, _)).Times(1);
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_SendHbMessage(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_SendHbMessage(connection_id));

    // verify header which was used to create message
    EXPECT_EQ(default_config.connection_configurations[connection_id].sender_id, message_header.sender_id);
    EXPECT_EQ(default_config.connection_configurations[connection_id].receiver_id, message_header.receiver_id);
    EXPECT_EQ(sequence_number, message_header.sequence_number);
    EXPECT_EQ(srcor_rasta_connections[connection_id].time_stamp_rx, message_header.confirmed_time_stamp);

    // verify seq number gets incremented for next message
    EXPECT_EQ(sequence_number + 1, srcor_rasta_connections[connection_id].sequence_number_tx);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySendHbMessageFunction,
  SendHbMessageTestWithParam,
  ::testing::Values(
    // vary configured connections and connection ids
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, 0, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), 0, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest013} Verify the SendRetrReqMessage function
 *
 * @details     This test verifies the SendRetrReqMessage function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Initialize the module
 * - Set any value to the time_stamp_rx
 * - Set expectations
 *   - For create and add message to buffer
 *   - For message allowed to send check
 *   - For send pending messages call
 * - Call the function
 * - Verify header (sender id, receiver id, seq number, confirmed timestamp) which gets used to create message
 * - Verify that sequence number TX is incremented for the next message to send
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Seq nbr: Sequence number of message
 * - Expected error: Expected error code when calling srcor_SendRetrReqMessage function
 * .
 * | Test Run | Input parameter | Test config                         || Expected values                             ||
 * |----------|-----------------|------------------------|-------------|-------------------------|--------------------|
 * |          | Connection Id   | Configured connections | Seq nbr     | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | 0U          | radef_kNoError          | Normal operation   |
 * | 1        | 0U              | 1                      | 0xffffffffU | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 2                      | 0xffffffffU | radef_kNoError          | Normal operation   |
 * | 3        | 1U              | 2                      | 0xffffffffU | radef_kNoError          | Normal operation   |
 * | 4        | 1U              | 1                      | 0U          | radef_kInvalidParameter | Fatal error raised |
 * | 5        | 2U              | 2                      | 0U          | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-588} Send RetrReq Message Function
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-651} Send Message Function
 * @verifyReq{RASW-364} Send Message Function Structure
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(SendRetrReqMessageTestWithParam, srcorTest013VerifySendRetrReqMessageFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint32_t confirmed_timestamp = 100;

  // parameter values to check
  srtyp_SrMessageHeaderCreate message_header = {};

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  // set expectations
  if(expected_error == radef_kNoError)
  {
    // set timestamp and seq nbr to verify in header
    srcor_rasta_connections[connection_id].time_stamp_rx = confirmed_timestamp;
    srcor_rasta_connections[connection_id].sequence_number_tx = sequence_number;

    EXPECT_CALL(srmsg_mock, srmsg_CreateRetrReqMessage(_, _)).WillOnce(SaveArg<0>(&message_header));
    EXPECT_CALL(srsend_mock, srsend_AddToBuffer(connection_id, _)).Times(1);

    // set allow send message check expectations to allow one message to send
    EXPECT_CALL(srsend_mock, srsend_GetUsedBufferEntries(connection_id)).Times(2).WillRepeatedly(Return(1));

    {
      InSequence s;
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(1));
      EXPECT_CALL(srsend_mock, srsend_GetNumberOfMessagesToSend(connection_id)).Times(2).WillRepeatedly(Return(0));
    }

    // set expect calls for sending message
    EXPECT_CALL(srsend_mock, srsend_ReadMessageToSend(connection_id, _)).WillOnce(Return(radef_kNoError));
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_GetMessageHeader(_, _)).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_UpdateMessageHeader(_, _)).Times(1);
    EXPECT_CALL(sradin_mock, sradin_SendMessage(_, _, _)).Times(1);
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_SendRetrReqMessage(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_SendRetrReqMessage(connection_id));

    // verify header which was used to create message
    EXPECT_EQ(default_config.connection_configurations[connection_id].sender_id, message_header.sender_id);
    EXPECT_EQ(default_config.connection_configurations[connection_id].receiver_id, message_header.receiver_id);
    EXPECT_EQ(sequence_number, message_header.sequence_number);
    EXPECT_EQ(srcor_rasta_connections[connection_id].time_stamp_rx, message_header.confirmed_time_stamp);

    // verify seq number gets incremented for next message
    EXPECT_EQ(sequence_number + 1, srcor_rasta_connections[connection_id].sequence_number_tx);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySendRetrReqMessageFunction,
  SendRetrReqMessageTestWithParam,
  ::testing::Values(
    // vary configured connections and connection ids
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, 0, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), 0, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest014} Verify the SendConnectionStateNotification function
 *
 * @details     This test verifies the SendConnectionStateNotification function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Initialize the module
 * - Set Disc Detailed Reason to verify in call
 * - Set expectations for getting receive and send buffer information
 * - Set expectations for srnot_ConnectionStateNotification
 * - Call the function
 * - Verify buffer utilisation data of connection state notification call
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Connection state: Connection state
 * - Disconnect reason: Disconnect reason
 * - Configured connections: Number of configured connections
 * - Expected error: Expected error code when calling srcor_SendConnectionStateNotification function
 * .
 * | Test Run | Input parameter                                        ||| Test config            | Expected values                             ||
 * |----------|---------------|------------------|-----------------------|------------------------|-------------------------|--------------------|
 * |          | Connection ID | Connection state | Disconnect reason     | Configured connections | Expected error          | Test Result        |
 * | 0        | 0U            | NotInitialized   | UserRequest           | 1                      | radef_kNoError          | Normal operation   |
 * | 1        | 0U            | Closed           | UserRequest           | 1                      | radef_kNoError          | Normal operation   |
 * | 2        | 0U            | Down             | UserRequest           | 1                      | radef_kNoError          | Normal operation   |
 * | 3        | 0U            | Start            | UserRequest           | 1                      | radef_kNoError          | Normal operation   |
 * | 4        | 0U            | Up               | UserRequest           | 1                      | radef_kNoError          | Normal operation   |
 * | 5        | 0U            | RetransRequest   | UserRequest           | 1                      | radef_kNoError          | Normal operation   |
 * | 6        | 0U            | RetransRunning   | UserRequest           | 1                      | radef_kNoError          | Normal operation   |
 * | 7        | 0U            | Max              | UserRequest           | 1                      | radef_kInvalidParameter | Fatal error raised |
 * | 8        | 0U            | Up               | NotInUse              | 1                      | radef_kNoError          | Normal operation   |
 * | 9        | 0U            | Up               | UnexpectedMessage     | 1                      | radef_kNoError          | Normal operation   |
 * | 10       | 0U            | Up               | SequenceNumberError   | 1                      | radef_kNoError          | Normal operation   |
 * | 11       | 0U            | Up               | Timeout               | 1                      | radef_kNoError          | Normal operation   |
 * | 12       | 0U            | Up               | ServiceNotAllowed     | 1                      | radef_kNoError          | Normal operation   |
 * | 13       | 0U            | Up               | ProtocolVersionError  | 1                      | radef_kNoError          | Normal operation   |
 * | 14       | 0U            | Up               | RetransmissionFailed  | 1                      | radef_kNoError          | Normal operation   |
 * | 15       | 0U            | Up               | ProtocolSequenceError | 1                      | radef_kNoError          | Normal operation   |
 * | 16       | 0U            | Up               | Max                   | 1                      | radef_kInvalidParameter | Fatal error raised |
 * | 17       | 1U            | Up               | UserRequest           | 1                      | radef_kInvalidParameter | Fatal error raised |
 * | 18       | 0U            | Up               | UserRequest           | 2                      | radef_kNoError          | Normal operation   |
 * | 19       | 1U            | Up               | UserRequest           | 2                      | radef_kNoError          | Normal operation   |
 * | 20       | 2U            | Up               | UserRequest           | 2                      | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-581} Send Connection State Notification Function
 * @verifyReq{RASW-589} Set Disconnection Detailed Reason Function
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-555} Connection State Notification
 * @verifyReq{RASW-296} Connection State Notification Structure
 * @verifyReq{RASW-299} Connection Identification
 * @verifyReq{RASW-298} Connection State
 * @verifyReq{RASW-293} Buffer Utilisation
 * @verifyReq{RASW-291} Opposite Buffer Size
 * @verifyReq{RASW-295} Disconnect Reason
 * @verifyReq{RASW-294} Detailed Disconnect Reason
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 * @verifyReq{RASW-461} Struct Buffer Utilisation Structure
 * @verifyReq{RASW-460} Used Send Buffer Entries
 * @verifyReq{RASW-465} Free Send Buffer Entries
 * @verifyReq{RASW-464} Used Receive Buffer Entries
 * @verifyReq{RASW-463} Free Receive Buffer Entries
 * @verifyReq{RASW-491} Enum Connection States Structure
 */
TEST_P(SendConnectionStateNotificationTestWithParam, srcorTest014VerifySendConnectionStateNotificationFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;

  // set send and receive buffer to half full
  const uint16_t send_buffer_used     = UT_SRCOR_NSENDMAX_MAX / 2;
  const uint16_t send_buffer_free     = UT_SRCOR_NSENDMAX_MAX / 2;
  const uint16_t receive_buffer_used  = UT_SRCOR_NSENDMAX_MAX / 2;
  const uint16_t receive_buffer_free  = UT_SRCOR_NSENDMAX_MAX / 2;

  const sraty_BufferUtilisation buffer_utilisation = {
    send_buffer_used,
    send_buffer_free,
    receive_buffer_used,
    receive_buffer_free
  };

  const uint16_t opposite_buffer_size = UT_SRCOR_SEND_BUFFER_SIZE_MAX;
  const uint16_t detailed_disconnect_reason = UINT16_MAX;

  // parameter values to check
  sraty_BufferUtilisation buffer_utilisation_to_check = {};

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  // set expectations
  if(expected_error == radef_kNoError)
  {
    // set detailed disconnect reason to verify in srnot_ConnectionStateNotification call
    EXPECT_NO_THROW(srcor_SetDiscDetailedReason(connection_id, detailed_disconnect_reason));

    // calls for GetBufferSizeAndUtilisation
    EXPECT_CALL(srrece_mock, srrece_GetFreeBufferEntries(connection_id)).WillOnce(Return(buffer_utilisation.receive_buffer_free));
    EXPECT_CALL(srrece_mock, srrece_GetUsedBufferEntries(connection_id)).WillOnce(Return(buffer_utilisation.receive_buffer_used));
    EXPECT_CALL(srsend_mock, srsend_GetFreeBufferEntries(connection_id)).WillOnce(Return(buffer_utilisation.send_buffer_free));
    EXPECT_CALL(srsend_mock, srsend_GetUsedBufferEntries(connection_id)).WillOnce(Return(buffer_utilisation.send_buffer_used));

    EXPECT_CALL(srnot_mock, srnot_ConnectionStateNotification(connection_id, connection_state, _,
      opposite_buffer_size, disconnect_reason, detailed_disconnect_reason)).WillOnce(SaveArg<2>(&buffer_utilisation_to_check));
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_SendConnectionStateNotification(connection_id, connection_state, disconnect_reason), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_SendConnectionStateNotification(connection_id, connection_state, disconnect_reason));

    // verify buffer utilisation parameter from srnot_ConnectionStateNotification call
    EXPECT_EQ(buffer_utilisation_to_check.send_buffer_used, buffer_utilisation.send_buffer_used);
    EXPECT_EQ(buffer_utilisation_to_check.send_buffer_free, buffer_utilisation.send_buffer_free);
    EXPECT_EQ(buffer_utilisation_to_check.receive_buffer_used, buffer_utilisation.receive_buffer_used);
    EXPECT_EQ(buffer_utilisation_to_check.receive_buffer_used, buffer_utilisation.receive_buffer_used);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySendConnectionStateNotificationFunction,
  SendConnectionStateNotificationTestWithParam,
  ::testing::Values(
    // vary connection states
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionNotInitialized, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionClosed, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionDown, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionStart, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionRetransRequest, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionRetransRunning, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionMax, sraty_kDiscReasonUserRequest, radef_kInvalidParameter),
    // vary disconnect reason
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonNotInUse, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonUnexpectedMessage, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonSequenceNumberError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonTimeout, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonServiceNotAllowed, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonProtocolVersionError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonRetransmissionFailed, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonProtocolSequenceError, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonMax, radef_kInvalidParameter),
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, sraty_kConnectionUp, sraty_kDiscReasonUserRequest, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, sraty_kConnectionUp, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, sraty_kConnectionUp, sraty_kDiscReasonUserRequest, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), sraty_kConnectionUp, sraty_kDiscReasonUserRequest, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest015} Verify the WriteMessagePayloadToTemporaryBuffer function
 *
 * @details     This test verifies the WriteMessagePayloadToTemporaryBuffer function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Add data to the test message payload
 * - Init the module
 * - Set message in buffer flag according to test parameter
 * - Call the WriteMessagePayloadToTemporaryBuffer function
 * - If no error occurred, verify the data from buffer
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Message payload size: Payload size of message in bytes (valid range: 1 - 1055)
 * - Configured connections: Number of configured connections
 * - Message in buffer: Indicates if a message is in temporary buffer
 * - Expected error: Expected error code when calling srcor_WriteMessagePayloadToTemporaryBuffer function
 * .
 * | Test Run | Input parameter                         || Test config                               || Expected values                             ||
 * |----------|---------------|--------------------------|------------------------|-------------------|-------------------------|--------------------|
 * |          | Connection Id | Message payload size     | Configured connections | Message in buffer | Expected error          | Test Result        |
 * | 0        | 0             | 0                        | 1                      | False             | radef_kInvalidParameter | Fatal error raised |
 * | 1        | 0             | 1                        | 1                      | False             | radef_kNoError          | Normal operation   |
 * | 2        | 0             | 1055                     | 1                      | False             | radef_kNoError          | Normal operation   |
 * | 3        | 0             | 1056                     | 1                      | False             | radef_kInvalidParameter | Fatal error raised |
 * | 4        | 0             | 1055                     | 1                      | True              | radef_kInternalError    | Fatal error raised |
 * | 5        | 0             | 1055                     | 2                      | False             | radef_kNoError          | Normal operation   |
 * | 6        | 1             | 1055                     | 2                      | False             | radef_kNoError          | Normal operation   |
 * | 7        | 1             | 1055                     | 1                      | False             | radef_kInvalidParameter | Fatal error raised |
 * | 8        | 2             | 1055                     | 2                      | False             | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-593} Write Message Payload to Temporary Buffer Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(WriteMessagePayloadToTemporaryBufferTestWithParam, srcorTest015VerifyWriteMessagePayloadToTemporaryBufferFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint8_t message_payload[message_payload_size] = {};

  // copy test data to payload
  for(uint16_t i = 0; i < message_payload_size; i++)
  {
    message_payload[i] = test_payload_data_u8[i];
  }

  // set expectations for init module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(test_config.safety_code_type, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(test_config.number_of_connections, test_config.t_max, test_config.n_diag_window, test_config.diag_timing_distr_intervals)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(test_config.number_of_connections)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(test_config.number_of_connections, test_config.n_send_max)).Times(1);

  // call the init function
  EXPECT_NO_THROW(srcor_Init(&test_config));

  if(expected_error != radef_kInvalidParameter)
  {
    // set message in buffer according test parameter
    srcor_rasta_connections[connection_id].temporary_send_buffer.message_in_buffer = message_in_buffer;
  }

  // execute the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_WriteMessagePayloadToTemporaryBuffer(connection_id, message_payload_size, &message_payload[0]), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_WriteMessagePayloadToTemporaryBuffer(connection_id, message_payload_size, &message_payload[0]));

    // verify buffer data
    EXPECT_TRUE(srcor_rasta_connections[connection_id].temporary_send_buffer.message_in_buffer);
    EXPECT_EQ(srcor_rasta_connections[connection_id].temporary_send_buffer.message_payload_buffer.payload_size, message_payload_size);

    for(uint16_t i = 0; i < message_payload_size; i++)
    {
      EXPECT_EQ(srcor_rasta_connections[connection_id].temporary_send_buffer.message_payload_buffer.payload[i], test_payload_data_u8[i]);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  VerifyWriteMessagePayloadToTemporaryBufferFunction,
  WriteMessagePayloadToTemporaryBufferTestWithParam,
  ::testing::Values(
    // vary payload size
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, (UT_SRCOR_PAYLOAD_LEN_MIN - 1), false, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_PAYLOAD_LEN_MIN, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_PAYLOAD_LEN_MAX, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, (UT_SRCOR_PAYLOAD_LEN_MAX + 1), false, radef_kInvalidParameter),
    // vary message in buffer
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_PAYLOAD_LEN_MAX, true, radef_kInternalError),
    // vary channel configuration and channel id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_PAYLOAD_LEN_MAX, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, UT_SRCOR_PAYLOAD_LEN_MAX, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, UT_SRCOR_PAYLOAD_LEN_MAX, false, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), UT_SRCOR_PAYLOAD_LEN_MAX, false, radef_kInvalidParameter)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
