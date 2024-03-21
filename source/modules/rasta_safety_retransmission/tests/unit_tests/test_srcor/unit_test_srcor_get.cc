/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srcor_get.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the functions which return a bool value from the core module of the safety and retransmission layer.
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
 * @brief Test class for value-parametrized IsProtocolVersionAccepted tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - if message gets added in buffer before (bool)
 * - message protocol value pos 1 (uint8_t)
 * - message protocol value pos 2 (uint8_t)
 * - message protocol value pos 3 (uint8_t)
 * - message protocol value pos 4 (uint8_t)
 * - indicates the expected return value of the function call (bool)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class IsProtocolVersionAcceptedTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool, uint8_t, uint8_t, uint8_t, uint8_t, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  bool message_in_buffer =                                std::get<2>(GetParam());    ///< indicates if message is in buffer
  uint8_t msg_protocol_pos_1 =                            std::get<3>(GetParam());    ///< message protocol value pos 1
  uint8_t msg_protocol_pos_2 =                            std::get<4>(GetParam());    ///< message protocol value pos 2
  uint8_t msg_protocol_pos_3 =                            std::get<5>(GetParam());    ///< message protocol value pos 3
  uint8_t msg_protocol_pos_4 =                            std::get<6>(GetParam());    ///< message protocol value pos 4
  bool expected_result =                                  std::get<7>(GetParam());    ///< indicates the expected return value of the function call
  radef_RaStaReturnCode expected_error =                  std::get<8>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized IsRetrReqSequenceNumberAvailable tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - if message gets added in buffer before (bool)
 * - return value of srsend_IsSequenceNumberInBuffer call (radef_RaStaReturnCode)
 * - message type (srtyp_SrMessageType)
 * - indicates the expected return value of the function call (bool)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class IsRetrReqSequenceNumberAvailableTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool, radef_RaStaReturnCode, srtyp_SrMessageType, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  bool message_in_buffer =                                std::get<2>(GetParam());    ///< indicates if message is in buffer
  radef_RaStaReturnCode seq_nbr_result =                  std::get<3>(GetParam());    ///< return value of srsend_IsSequenceNumberInBuffer call
  srtyp_SrMessageType message_type =                      std::get<4>(GetParam());    ///< message type
  bool expected_result =                                  std::get<5>(GetParam());    ///< indicates the expected return value of the function call
  radef_RaStaReturnCode expected_error =                  std::get<6>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized IsConnRoleServer tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - configured sender id for connection 1 (uint32_t)
 * - configured receiver id for connection 1 (uint32_t)
 * - configured sender id for connection 2 (uint32_t)
 * - configured receiver id for connection 2 (uint32_t)
 * - indicates the expected return value of the function call (bool)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class IsConnRoleServerTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  uint32_t conn_1_configured_sender_id =                  std::get<2>(GetParam());    ///< configured sender id for connection 1
  uint32_t conn_1_configured_receiver_id =                std::get<3>(GetParam());    ///< configured receiver id for connection 1
  uint32_t conn_2_configured_sender_id =                  std::get<4>(GetParam());    ///< configured sender id for connection 2
  uint32_t conn_2_configured_receiver_id =                std::get<5>(GetParam());    ///< configured receiver id for connection 2
  bool expected_result =                                  std::get<6>(GetParam());    ///< indicates the expected return value of the function call
  radef_RaStaReturnCode expected_error =                  std::get<7>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized IsMessageTimeoutTestWithParam tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - timer ti value (uint32_t)
 * - last confirmed timestamp (uint32_t)
 * - current timer value (uint32_t)
 * - indicates if the message is expected to be timed out (bool)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class IsMessageTimeoutTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  uint32_t timer_ti =                                     std::get<2>(GetParam());    ///< timer ti value
  uint32_t last_confirmed_timestamp =                     std::get<3>(GetParam());    ///< last confirmed timestamp on channel
  uint32_t current_timer_value =                          std::get<4>(GetParam());    ///< current timer value
  bool expect_timeout =                                   std::get<5>(GetParam());    ///< indicates if the message is expected to be timed out
  radef_RaStaReturnCode expected_error =                  std::get<6>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized IsHeartbeatInterval tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - configured heartbeat period (uint32_t)
 * - last timestamp sent (uint32_t)
 * - current timer value (uint32_t)
 * - indicates if the hb interval is expected to be elapsed (bool)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class IsHeartbeatIntervalTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  uint32_t configured_hb_period =                         std::get<2>(GetParam());    ///< configured heartbeat period
  uint32_t last_timestamp_sent =                          std::get<3>(GetParam());    ///< last timestamp sent
  uint32_t current_timer_value =                          std::get<4>(GetParam());    ///< current timer value
  bool expect_interval_elapsed =                          std::get<5>(GetParam());    ///< indicates if the hb interval is expected to be elapsed
  radef_RaStaReturnCode expected_error =                  std::get<6>(GetParam());    ///< expected error
};

/**
 * @brief Test class for value-parametrized GetConnectionId function test
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - configured sender id on channel 1 (uint32_t)
 * - configured receiver id on channel 1 (uint32_t)
 * - configured sender id on channel 2 (uint32_t)
 * - configured receiver id on channel 2 (uint32_t)
 * - sender id for function call (uint32_t)
 * - receiver id for function call (uint32_t)
 * - expected connection id for given sender and receiver id (uint32_t)
 * - expected return value (radef_RaStaReturnCode)
 * .
 */
class GetConnectionIdTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t,
                                                                                                    uint32_t, uint32_t,
                                                                                                    uint32_t, uint32_t,
                                                                                                    uint32_t, uint32_t,
                                                                                                    uint32_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t configured_sender_id_ch_1 =                    std::get<1>(GetParam());    ///< configured sender id on channel 1
  uint32_t configured_receiver_id_ch_1 =                  std::get<2>(GetParam());    ///< configured receiver id on channel 1
  uint32_t configured_sender_id_ch_2 =                    std::get<3>(GetParam());    ///< configured sender id on channel 2
  uint32_t configured_receiver_id_ch_2 =                  std::get<4>(GetParam());    ///< configured receiver id on channel 2
  uint32_t sender_id =                                    std::get<5>(GetParam());    ///< sender id for function call
  uint32_t receiver_id =                                  std::get<6>(GetParam());    ///< receiver id for function call
  uint32_t expected_connection_id =                       std::get<7>(GetParam());    ///< expected connection id
  radef_RaStaReturnCode expected_return_value =           std::get<8>(GetParam());    ///< expected return value
};

/**
 * @brief Test class for value-parametrized GetReceivedMessagePendingFlag tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - set pending flag (bool)
 * - expect fatal error (bool)
 * .
 */
class GetReceivedMessagePendingFlagTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool, bool>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  bool set_flag =                                         std::get<2>(GetParam());    ///< set pending flag
  bool expect_fatal_error =                               std::get<3>(GetParam());    ///< expect fatal error
};

/**
 * @brief Test class for value-parametrized GetBufferSizeAndUtilisation tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class GetBufferSizeAndUtilisationTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  bool expect_fatal_error =                               std::get<2>(GetParam());    ///< expect fatal error
};

/**
 * @brief Test class for value-parametrized IsReceivedMsgPendingAndBuffersNotFull tests
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - set message pending flag (bool)
 * - free receive buffer entries (uint32_t)
 * - free send buffer entries (uint32_t)
 * - expect return value (bool)
 * - expect fatal error (bool)
 * .
 */
class IsReceivedMsgPendingAndBuffersNotFullTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool, uint32_t, uint32_t, bool, bool>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  bool message_pending_flag_set =                         std::get<2>(GetParam());    ///< set message pending flag
  uint32_t free_receive_buffer_entries =                  std::get<3>(GetParam());    ///< free receive buffer entries
  uint32_t free_send_buffer_entries =                     std::get<4>(GetParam());    ///< free send buffer entries
  bool expect_return_value =                              std::get<5>(GetParam());    ///< expect return value
  bool expect_fatal_error =                               std::get<6>(GetParam());    ///< expect fatal error
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_core
 * @{
 */

/**
 * @test        @ID{srcorTest016} Verify the IsProtocolVersionAccepted function
 *
 * @details     This test verifies the IsProtocolVersionAccepted function of the rasta safety and
 *              retransmission layer core module. Test parameters are chosen for protocol version 03.03
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Set expected calls for initialize module
 * - Initialize the module
 * - Set message in input buffer flag if test parameter true
 * - Call function and verify return value with given test parameter
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Add msg to buffer: Indicates if a message gets added to buffer
 * - Version in message: The version written in message
 * - Expect accepted: Indicates if version is accepted or not
 * - Expected error: Expected error code when calling function
 * .
 * | Test Run | Input parameter | Test config                                                   ||| Expected values                                                |||
 * |----------|-----------------|------------------------|-------------------|--------------------|-------------------|-------------------------|--------------------|
 * |          | Connection Id   | Configured connections | Add msg to buffer | Version in message | Expect accepted   | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | True              | 00.00              | False             | radef_kNoError          | Normal operation   |
 * | 1        | 0U              | 1                      | True              | 03.02              | False             | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 1                      | True              | 02.03              | False             | radef_kNoError          | Normal operation   |
 * | 3        | 0U              | 1                      | True              | 03.03              | True              | radef_kNoError          | Normal operation   |
 * | 4        | 0U              | 1                      | True              | 03.04              | True              | radef_kNoError          | Normal operation   |
 * | 5        | 0U              | 1                      | True              | 04.03              | True              | radef_kNoError          | Normal operation   |
 * | 6        | 0U              | 1                      | True              | 99.99              | True              | radef_kNoError          | Normal operation   |
 * | 7        | 0U              | 2                      | False             | 03.03              | False             | radef_kInternalError    | Fatal error thrown |
 * | 8        | 0U              | 2                      | True              | 03.03              | True              | radef_kNoError          | Normal operation   |
 * | 9        | 1U              | 2                      | True              | 03.03              | True              | radef_kNoError          | Normal operation   |
 * | 10       | 1U              | 1                      | False             | 03.03              | False             | radef_kInvalidParameter | Fatal error thrown |
 * | 11       | 2U              | 2                      | False             | 03.03              | False             | radef_kInvalidParameter | Fatal error thrown |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-577} Is Protocol Version Accepted Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(IsProtocolVersionAcceptedTestWithParam, srcorTest016VerifyIsProtocolVersionAcceptedFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config =  default_config;
  test_config.number_of_connections = configured_connections;
  const srtyp_ProtocolVersion message_protocol_version = {
    msg_protocol_pos_1,
    msg_protocol_pos_2,
    msg_protocol_pos_3,
    msg_protocol_pos_4
  };

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  if(expected_error == radef_kNoError)
  {
    // set message in buffer on connection according to parameter
    srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = message_in_buffer;

    // set expectations
    if(message_in_buffer)
    {
      EXPECT_CALL(srmsg_mock, srmsg_GetConnMessageData(&srcor_rasta_connections[connection_id].input_buffer.message_buffer, _, _)).WillOnce(SetArgPointee<1>(message_protocol_version));
    }
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_IsProtocolVersionAccepted(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_EQ(expected_result, srcor_IsProtocolVersionAccepted(connection_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyIsProtocolVersionAcceptedFunction,
  IsProtocolVersionAcceptedTestWithParam,
  ::testing::Values(
    // vary message protocol version
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, 48, 48, 48, 48, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, 48, 51, 48, 50, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, 48, 50, 48, 51, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, 48, 51, 48, 51, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, 48, 51, 48, 52, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, 48, 52, 48, 51, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, 57, 57, 57, 57, true, radef_kNoError),
    // vary message in buffer
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, false, 48, 51, 48, 51, true, radef_kInternalError),
    // vary connection configuration and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, true, 48, 51, 48, 51, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, true, 48, 51, 48, 51, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, false, 48, 51, 48, 51, false, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), false, 48, 51, 48, 51, false, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest017} Verify the IsRetrReqSequenceNumberAvailable function
 *
 * @details     This test verifies the IsRetrReqSequenceNumberAvailable function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Set expected calls for initialize module
 * - Initialize the module
 * - Set message in input buffer flag if test parameter true
 * - Expect calls if message in buffer
 *   - For get message header function which returns message header with type according to test parameter
 *   - For is sequence number in buffer call which returns given test parameter when message type srtyp_kSrMessageRetrReq
 * - Call function and verify return value with given test parameter
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Add msg to buffer: Indicates if a message gets added to buffer
 * - Is Seq nbr in buffer return value: Return value of srsend_IsSequenceNumberInBuffer function call
 * - Message type: Message type of message in buffer
 * - Expect available: The expected return value of the srcor_IsRetrReqSequenceNumberAvailable function call
 * - Expected error code: Expected error code when calling function
 * .
 * | Test Run | Input parameter | Test config                                                                                            |||| Expected values                                                |||
 * |----------|-----------------|------------------------|-------------------|-----------------------------------|--------------------------|-------------------|-------------------------|--------------------|
 * |          | Connection Id   | Configured connections | Add msg to buffer | Is Seq nbr in buffer return value | Message type             | Expect available  | Expected error code     | Test Result        |
 * | 0        | 0U              | 1                      | False             | radef_kNoError                    | srtyp_kSrMessageRetrReq  | False             | radef_kInternalError    | Fatal error raised |
 * | 1        | 0U              | 1                      | True              | radef_kNoError                    | srtyp_kSrMessageRetrReq  | True              | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 1                      | True              | radef_kNoError                    | srtyp_kSrMessageConnReq  | False             | radef_kNoError          | Normal operation   |
 * | 3        | 0U              | 1                      | True              | radef_kNoError                    | srtyp_kSrMessageConnResp | False             | radef_kNoError          | Normal operation   |
 * | 4        | 0U              | 1                      | True              | radef_kNoError                    | srtyp_kSrMessageRetrResp | False             | radef_kNoError          | Normal operation   |
 * | 5        | 0U              | 1                      | True              | radef_kNoError                    | srtyp_kSrMessageDiscReq  | False             | radef_kNoError          | Normal operation   |
 * | 6        | 0U              | 1                      | True              | radef_kNoError                    | srtyp_kSrMessageHb       | False             | radef_kNoError          | Normal operation   |
 * | 7        | 0U              | 1                      | True              | radef_kNoError                    | srtyp_kSrMessageData     | False             | radef_kNoError          | Normal operation   |
 * | 8        | 0U              | 1                      | True              | radef_kNoError                    | srtyp_kSrMessageRetrData | False             | radef_kNoError          | Normal operation   |
 * | 9        | 0U              | 1                      | True              | radef_kInvalidSequenceNumber      | srtyp_kSrMessageRetrReq  | False             | radef_kNoError          | Normal operation   |
 * | 10       | 0U              | 2                      | True              | radef_kNoError                    | srtyp_kSrMessageRetrReq  | True              | radef_kNoError          | Normal operation   |
 * | 11       | 1U              | 1                      | True              | radef_kNoError                    | srtyp_kSrMessageRetrReq  | False             | radef_kInvalidParameter | Fatal error raised |
 * | 12       | 1U              | 2                      | True              | radef_kNoError                    | srtyp_kSrMessageRetrReq  | True              | radef_kNoError          | Normal operation   |
 * | 13       | 2U              | 2                      | True              | radef_kNoError                    | srtyp_kSrMessageRetrReq  | False             | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-578} Is Retransmission Request Sequence Number Available Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(IsRetrReqSequenceNumberAvailableTestWithParam, srcorTest017VerifyIsRetrReqSequenceNumberAvailableFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint32_t confirmed_seq_nr = 123;
  srtyp_SrMessageHeader message_header = {
    0,
    message_type,
    0,
    0,
    confirmed_seq_nr,
    0,
    0,
  };  // only message type is relevant for this test

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  if(expected_error == radef_kNoError)
  {
    // set message in buffer on connection according to parameter
    srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = message_in_buffer;

    // set expectations
    if(message_in_buffer)
    {
      srcor_rasta_connections[connection_id].input_buffer.message_header = message_header;
      if(message_header.message_type == srtyp_kSrMessageRetrReq)
      {
        EXPECT_CALL(srsend_mock, srsend_IsSequenceNumberInBuffer(connection_id, (message_header.confirmed_sequence_number + 1))).WillOnce(Return(seq_nbr_result));
      }
    }
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_IsRetrReqSequenceNumberAvailable(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_EQ(expected_result, srcor_IsRetrReqSequenceNumberAvailable(connection_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyIsRetrReqSequenceNumberAvailableFunction,
  IsRetrReqSequenceNumberAvailableTestWithParam,
  ::testing::Values(
    // vary message in buffer
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, false, radef_kNoError, srtyp_kSrMessageRetrReq, false, radef_kInternalError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kNoError, srtyp_kSrMessageRetrReq, true, radef_kNoError),
    // vary message type
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kNoError, srtyp_kSrMessageConnReq, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kNoError, srtyp_kSrMessageConnResp, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kNoError, srtyp_kSrMessageRetrResp, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kNoError, srtyp_kSrMessageDiscReq, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kNoError, srtyp_kSrMessageHb, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kNoError, srtyp_kSrMessageData, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kNoError, srtyp_kSrMessageRetrData, false, radef_kNoError),
    // vary seq nbr check return value
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kInvalidSequenceNumber, srtyp_kSrMessageRetrReq, false, radef_kNoError),
    // vary connection configuration and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kNoError, srtyp_kSrMessageRetrReq, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, true, radef_kNoError, srtyp_kSrMessageRetrReq, false, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, true, radef_kNoError, srtyp_kSrMessageRetrReq, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), true, radef_kNoError, srtyp_kSrMessageRetrReq, false, radef_kInvalidParameter)
  )
);

// srcor_IsConnRoleServer
/**
 * @test        @ID{srcorTest018} Verify the IsConnRoleServer function
 *
 * @details     This test verifies the IsConnRoleServer function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections and sender and receiver id of each of them (from parameter)
 * - Set expected calls for initialize module
 * - Initialize the module
 * - Call function and verify return value with given test parameter
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Sender id conn 1: Sender id on connection 1
 * - Receiver id conn 1: Receiver id on connection 1
 * - Sender id conn 2: Sender id on connection 2
 * - Receiver id conn 2: Receiver id on connection 1
 * - Expect is conn server: Expected return value of srcor_IsConnRoleServer function call
 * - Expected error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config                                                                                        ||||| Expected values                                                     |||
 * |----------|-----------------|------------------------|------------------|--------------------|------------------|--------------------|------------------------|-------------------------|--------------------|
 * |          | Connection Id   | Configured connections | Sender id conn 1 | Receiver id conn 1 | Sender id conn 2 | Receiver id conn 2 | Expect is conn server  | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | 1U               | 0U                 | 0U               | 0U                 | True                   | radef_kNoError          | Normal operation   |
 * | 1        | 0U              | 1                      | 0xffffffffU      | 0U                 | 0U               | 0U                 | True                   | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 1                      | 0U               | 1U                 | 0U               | 0U                 | False                  | radef_kNoError          | Normal operation   |
 * | 3        | 0U              | 1                      | 0U               | 0xffffffffU        | 0U               | 0U                 | False                  | radef_kNoError          | Normal operation   |
 * | 4        | 1U              | 2                      | 1U               | 0U                 | 1U               | 0U                 | True                   | radef_kNoError          | Normal operation   |
 * | 5        | 1U              | 2                      | 0xffffffffU      | 0U                 | 0xffffffffU      | 0U                 | True                   | radef_kNoError          | Normal operation   |
 * | 6        | 1U              | 2                      | 0U               | 1U                 | 0U               | 1U                 | False                  | radef_kNoError          | Normal operation   |
 * | 7        | 1U              | 2                      | 0U               | 0xffffffffU        | 0U               | 0xffffffffU        | False                  | radef_kNoError          | Normal operation   |
 * | 8        | 0U              | 1                      | 1U               | 0U                 | 1U               | 2U                 | True                   | radef_kNoError          | Normal operation   |
 * | 9        | 1U              | 2                      | 1U               | 2U                 | 1U               | 0U                 | True                   | radef_kNoError          | Normal operation   |
 * | 10       | 0U              | 2                      | 1U               | 0U                 | 1U               | 0U                 | True                   | radef_kNoError          | Normal operation   |
 * | 11       | 1U              | 1                      | 1U               | 0U                 | 1U               | 0U                 | False                  | radef_kInvalidParameter | Fatal error raised |
 * | 12       | 2U              | 2                      | 1U               | 0U                 | 1U               | 0U                 | False                  | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-574} Is Connection Role Server Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(IsConnRoleServerTestWithParam, srcorTest018VerifyIsConnRoleServerFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  test_config.connection_configurations[0].sender_id = conn_1_configured_sender_id;
  test_config.connection_configurations[0].receiver_id = conn_1_configured_receiver_id;
  test_config.connection_configurations[1].sender_id = conn_2_configured_sender_id;
  test_config.connection_configurations[1].receiver_id = conn_2_configured_receiver_id;

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module
  srcor_Init(&test_config);

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_IsConnRoleServer(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_EQ(expected_result, srcor_IsConnRoleServer(connection_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyIsConnRoleServerFunction,
  IsConnRoleServerTestWithParam,
  ::testing::Values(
    // vary sender and client id of connection 1 and call for connection 1
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 1, 0, 0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX,  0, 0, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, 1, 0, 0, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, UINT32_MAX, 0, 0, false, radef_kNoError),
    // vary sender and client id of connection 2 and call for connection 2
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, 1, 0, 1, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, UINT32_MAX,  0, UINT32_MAX, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, 0, 1, 0, 1, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, 0, UINT32_MAX, 0, UINT32_MAX, false, radef_kNoError),
    // vary sender and receiver on connections
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 1, 0, 1, 2, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, 1, 2, 1, 0, true, radef_kNoError),
    // vary connection configuration and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, 1, 0, 1, 0, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, 1, 0, 1, 0, false, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), 1, 0, 1, 0, false, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest019} Verify the IsMessageTimeout function
 *
 * @details     This test verifies the IsMessageTimeout function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Set expected call for timer and return given test parameter
 * - Initialize the module
 * - Set last confirmed timestamp and timer ti value according to test parameter
 * - Call the function and verify expected return value from test parameter
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Timer ti value: Timer_ti value on connection
 * - last confirmed rx timestamp: Last confirmed received timestamp on connection
 * - Timer value: Value returned by rasys_GetTimerValue function call
 * - Expect timeout: Expected return value of srcor_IsMessageTimeout function call
 * - Expected error: Expected error code when calling srcor_IsMessageTimeout function
 * .
 * | Test Run | Input parameter | Test config                                                                      |||| Expected values                                             |||
 * |----------|-----------------|------------------------|----------------|-----------------------------|-------------|----------------|-------------------------|--------------------|
 * |          | Connection Id   | Configured connections | Timer ti value | last confirmed rx timestamp | Timer value | Expect timeout | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | 0U             | 0U                          | 0U          | False          | radef_kNoError          | Normal operation   |
 * | 1        | 0U              | 1                      | 750U           | 10000U                      | 10750U      | False          | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 1                      | 750U           | 10000U                      | 10751U      | True           | radef_kNoError          | Normal operation   |
 * | 3        | 0U              | 1                      | 1000U          | 10000U                      | 11000U      | False          | radef_kNoError          | Normal operation   |
 * | 4        | 0U              | 1                      | 1000U          | 10000U                      | 11001U      | True           | radef_kNoError          | Normal operation   |
 * | 5        | 0U              | 2                      | 750U           | 10000U                      | 10750U      | False          | radef_kNoError          | Normal operation   |
 * | 6        | 1U              | 2                      | 750U           | 10000U                      | 10750U      | False          | radef_kNoError          | Normal operation   |
 * | 7        | 1U              | 1                      | 750U           | 10000U                      | 10750U      | True           | radef_kInvalidParameter | Fatal error raised |
 * | 8        | 2U              | 2                      | 750U           | 10000U                      | 10750U      | True           | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-576} Is Message Timeout Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-422} Timer Value
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(IsMessageTimeoutTestWithParam, srcorTest019VerifyIsMessageTimeoutFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // set expectations
  if(expected_error == radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).WillOnce(Return(current_timer_value));
  }

  // initialize the module
  srcor_Init(&test_config);

  if(expected_error == radef_kNoError)
  {
    // set last confirmed timestamp and timer ti value
    srcor_rasta_connections[connection_id].confirmed_time_stamp_rx = last_confirmed_timestamp;
    srcor_rasta_connections[connection_id].timer_t_i = timer_ti;
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_IsMessageTimeout(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_EQ(expect_timeout, srcor_IsMessageTimeout(connection_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyIsMessageTimeoutFunction,
  IsMessageTimeoutTestWithParam,
  ::testing::Values(
    // vary timeout value, confirmed timestamp current timer value
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, 0, 0, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 750, 10000, 10750, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 750, 10000, 10751, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 1000, 10000, 11000, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 1000, 10000, 11001, true, radef_kNoError),
    // vary connection configuration and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, 750, 10000, 10750, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, 750, 10000, 10750, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, 750, 10000, 10750, true, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), 750, 10000, 10750, true, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest020} Verify the IsHeartbeatInterval function
 *
 * @details     This test verifies the IsHeartbeatInterval function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections and heartbeat period(from parameter)
 * - Set expected call for timer and return given test parameter
 * - Initialize the module
 * - Set last sent timestamp according to test parameter
 * - Call the function and verify expected return value from test parameter
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - HB interval: Configured heartbeat interval
 * - last timestamp sent: Last timestamp sent on connection
 * - Current timer value: Return value of rasys_GetTimerValue function call
 * - Expect HB elapsed: Expected return value of srcor_IsHeartbeatInterval function call
 * - Expected error: Expected error code when calling srcor_IsHeartbeatInterval function
 * .
 * | Test Run | Input parameter | Test config                                                                   |||| Expected values                                                |||
 * |----------|-----------------|------------------------|-------------|---------------------|---------------------|-------------------|-------------------------|--------------------|
 * |          | Connection Id   | Configured connections | HB interval | last timestamp sent | Current timer value | Expect HB elapsed | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | 300U        | 10000U              | 10000U              | False             | radef_kNoError          | Normal operation   |
 * | 1        | 0U              | 1                      | 300U        | 10000U              | 10299U              | False             | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 1                      | 300U        | 10000U              | 10300U              | True              | radef_kNoError          | Normal operation   |
 * | 3        | 0U              | 1                      | 750U        | 10000U              | 10000U              | False             | radef_kNoError          | Normal operation   |
 * | 4        | 0U              | 1                      | 750U        | 10000U              | 10749U              | False             | radef_kNoError          | Normal operation   |
 * | 5        | 0U              | 1                      | 750U        | 10000U              | 10750U              | True              | radef_kNoError          | Normal operation   |
 * | 6        | 0U              | 2                      | 750U        | 10000U              | 10749U              | False             | radef_kNoError          | Normal operation   |
 * | 7        | 1U              | 2                      | 750U        | 10000U              | 10749U              | False             | radef_kNoError          | Normal operation   |
 * | 8        | 1U              | 1                      | 750U        | 10000U              | 10749U              | False             | radef_kInvalidParameter | Fatal error raised |
 * | 9        | 2U              | 2                      | 750U        | 10000U              | 10749U              | False             | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-575} Is Heartbeat Interval Function
 * @verifyReq{RASW-807} Timer Th
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-422} Timer Value
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 *
 */
TEST_P(IsHeartbeatIntervalTestWithParam, srcorTest020VerifyIsHeartbeatIntervalFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;

  test_config.number_of_connections = configured_connections;
  test_config.t_h = configured_hb_period;

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // set expectations
  if(expected_error == radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).WillOnce(Return(current_timer_value));
  }

  // initialize the module
  srcor_Init(&test_config);

  if(expected_error == radef_kNoError)
  {
    // set last confirmed timestamp and timer ti value
    srcor_rasta_connections[connection_id].time_stamp_tx = last_timestamp_sent;
  }

  // perform the test
  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_IsHeartbeatInterval(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_EQ(expect_interval_elapsed, srcor_IsHeartbeatInterval(connection_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyIsHeartbeatIntervalFunction,
  IsHeartbeatIntervalTestWithParam,
  ::testing::Values(
    // vary configured heartbeat period value, last send timestamp and current timer value
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_HB_MIN, 10000, 10000, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_HB_MIN, 10000, 10299, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_HB_MIN, 10000, 10300, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_HB_MAX, 10000, 10000, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_HB_MAX, 10000, 10749, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_HB_MAX, 10000, 10750, true, radef_kNoError),
    // vary connection configuration and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, UT_SRCOR_HB_MAX, 10000, 10749, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, UT_SRCOR_HB_MAX, 10000, 10749, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, UT_SRCOR_HB_MAX, 10000, 10749, false, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), UT_SRCOR_HB_MAX, 10000, 10749, false, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{srcorTest021} Verify the GetConnectionId function
 *
 * @details     This test verifies the GetConnectionId function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections and with each sender and receiver id (from test parameter)
 * - Set expected calls for init call
 * - Initialize the module
 * - Call the GetConnectionId function with given test parameter
 * - Verify the return value and if no error expected verify the connection id
 * .
 *
 * @testParameter
 * - Sender id: Sender identification
 * - Receiver id: Receiver identification
 * - Configured connections: Number of configured connections
 * - Conf sender CH 1: Configured sender id on channel 1
 * - Conf receiver CH 1: Configured receiver id on channel 1
 * - Conf sender CH 2: Configured sender id on channel 2
 * - Conf receiver CH 2: Configured sender id on channel 2
 * - Expected connection id: Expected connection id given by srcor_GetConnectionId
 * - Expected return value: Expected return value of srcor_GetConnectionId function call
 * .
 * | Test Run | Input parameter          || Test config                                                                                        ||||| Expected values                                                   |||
 * |----------|-------------|-------------|------------------------|------------------|--------------------|------------------|--------------------|------------------------|-------------------------|------------------|
 * |          | Sender id   | Receiver id | Configured connections | Conf sender CH 1 | Conf receiver CH 1 | Conf sender CH 2 | Conf receiver CH 2 | Expected connection id | Expected return value   | Test Result      |
 * | 0        | 0U          | 1U          | 1                      | 0U               | 1U                 | 0U               | 0U                 | 0U                     | radef_kNoError          | Normal operation |
 * | 1        | 1U          | 0U          | 1                      | 1U               | 0U                 | 0U               | 0U                 | 0U                     | radef_kNoError          | Normal operation |
 * | 2        | 0xfffffffeU | 0xffffffffU | 1                      | 0xfffffffeU      | 0xffffffffU        | 0U               | 0U                 | 0U                     | radef_kNoError          | Normal operation |
 * | 3        | 0xffffffffU | 0xfffffffeU | 1                      | 0xffffffffU      | 0xfffffffeU        | 0U               | 0U                 | 0U                     | radef_kNoError          | Normal operation |
 * | 4        | 1U          | 1U          | 1                      | 1U               | 2U                 | 0U               | 0U                 | 0U                     | radef_kInvalidParameter | Normal operation |
 * | 5        | 2U          | 3U          | 2                      | 0U               | 1U                 | 2U               | 3U                 | 1U                     | radef_kNoError          | Normal operation |
 * | 6        | 3U          | 2U          | 2                      | 0U               | 1U                 | 3U               | 2U                 | 1U                     | radef_kNoError          | Normal operation |
 * | 7        | 0xfffffffeU | 0xffffffffU | 2                      | 0U               | 1U                 | 0xfffffffeU      | 0xffffffffU        | 1U                     | radef_kNoError          | Normal operation |
 * | 8        | 0xffffffffU | 0xfffffffeU | 2                      | 0U               | 1U                 | 0xffffffffU      | 0xfffffffeU        | 1U                     | radef_kNoError          | Normal operation |
 * | 9        | 4U          | 4U          | 2                      | 1U               | 2U                 | 3U               | 4U                 | 0U                     | radef_kInvalidParameter | Normal operation |
 * | 10       | 4U          | 4U          | 2                      | 0U               | 1U                 | 2U               | 3U                 | 0U                     | radef_kInvalidParameter | Normal operation |
 * | 11       | 1U          | 4U          | 2                      | 0U               | 1U                 | 2U               | 3U                 | 0U                     | radef_kInvalidParameter | Normal operation |
 * | 12       | 4U          | 1U          | 2                      | 0U               | 1U                 | 2U               | 3U                 | 0U                     | radef_kInvalidParameter | Normal operation |
 * | 13       | 0U          | 3U          | 2                      | 0U               | 1U                 | 2U               | 3U                 | 0U                     | radef_kInvalidParameter | Normal operation |
 * | 14       | 2U          | 1U          | 2                      | 0U               | 1U                 | 2U               | 3U                 | 0U                     | radef_kInvalidParameter | Normal operation |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-568} Get Connection ID Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(GetConnectionIdTestWithParam, srcorTest021VerifyGetConnectionIdFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  test_config.connection_configurations[0].sender_id = configured_sender_id_ch_1;
  test_config.connection_configurations[0].receiver_id = configured_receiver_id_ch_1;
  test_config.connection_configurations[1].sender_id = configured_sender_id_ch_2;
  test_config.connection_configurations[1].receiver_id = configured_receiver_id_ch_2;
  uint32_t connection_id = 0;

  // set expectations for init module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(test_config.safety_code_type, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(test_config.number_of_connections, test_config.t_max, test_config.n_diag_window, test_config.diag_timing_distr_intervals)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(test_config.number_of_connections)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(test_config.number_of_connections, test_config.n_send_max)).Times(1);

  // call the init function
  EXPECT_NO_THROW(srcor_Init(&test_config));

  // call the GetConnectionId function
  EXPECT_EQ(expected_return_value, srcor_GetConnectionId(sender_id, receiver_id, &connection_id));

  if(expected_return_value == radef_kNoError)
  {
    // verify the connection id
    EXPECT_EQ(connection_id, expected_connection_id);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetConnectionIdFunction,
  GetConnectionIdTestWithParam,
  ::testing::Values(
    // vary configured sender and receiver id on channel 1
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, 0, 1, 0, 0, 0, 1, UT_SRCOR_CONNECTION_ID_MIN, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, 1, 0, 0, 0, 1, 0, UT_SRCOR_CONNECTION_ID_MIN, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, (UINT32_MAX - 1), UINT32_MAX, 0, 0, (UINT32_MAX - 1), UINT32_MAX, UT_SRCOR_CONNECTION_ID_MIN, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UINT32_MAX, (UINT32_MAX - 1), 0, 0, UINT32_MAX, (UINT32_MAX - 1), UT_SRCOR_CONNECTION_ID_MIN, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, 1, 2, 0, 0, 1, 1, UT_SRCOR_CONNECTION_ID_MIN, radef_kInvalidParameter),
    // vary configured sender and receiver id on channel 2
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, 0, 1, 2, 3, 2, 3, UT_SRCOR_CONNECTION_ID_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, 0, 1, 3, 2, 3, 2, UT_SRCOR_CONNECTION_ID_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, 0, 1, (UINT32_MAX - 1), UINT32_MAX, (UINT32_MAX - 1), UINT32_MAX, UT_SRCOR_CONNECTION_ID_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, 0, 1, UINT32_MAX, (UINT32_MAX - 1), UINT32_MAX, (UINT32_MAX - 1), UT_SRCOR_CONNECTION_ID_MAX, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, 1, 2, 3, 4, 4, 4, UT_SRCOR_CONNECTION_ID_MIN, radef_kInvalidParameter),
    // vary configured and queried sender and receiver id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, 0, 1, 2, 3, 4, 4, UT_SRCOR_CONNECTION_ID_MIN , radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, 0, 1, 2, 3, 1, 4, UT_SRCOR_CONNECTION_ID_MIN , radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, 0, 1, 2, 3, 4, 1, UT_SRCOR_CONNECTION_ID_MIN , radef_kInvalidParameter),
    // query sender id from channel 1 and receiver id from channel 2
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, 0, 1, 2, 3, 0, 3, UT_SRCOR_CONNECTION_ID_MIN , radef_kInvalidParameter),
    // query sender id from channel 2 and receiver id from channel 1
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, 0, 1, 2, 3, 2, 1, UT_SRCOR_CONNECTION_ID_MIN , radef_kInvalidParameter)
  ));

/**
 * @test        @ID{srcorTest022} Verify the GetReceivedMessagePendingFlag function
 *
 * @details     This test verifies the GetReceivedMessagePendingFlag function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Init the module
 * - Set message pending flag on channel according to test parameter
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call the GetReceivedMessagePendingFlag function
 * - Verify message pending flag
 * .
 *
 * @testParameter
 * - Connection id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Set pending flag: Indicates if pending flag on connection gets set
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config                              || Expected values                        ||
 * |----------|-----------------|------------------------|------------------|--------------------|--------------------|
 * |          | Connection id   | Configured connections | Set pending flag | Expect fatal error | Test Result        |
 * | 0        | 0               | 1                      | False            | False              | Normal operation   |
 * | 1        | 0               | 1                      | True             | False              | Normal operation   |
 * | 2        | 0               | 2                      | True             | False              | Normal operation   |
 * | 3        | 1               | 2                      | True             | False              | Normal operation   |
 * | 4        | 1               | 1                      | False            | True               | Fatal error raised |
 * | 5        | 2               | 2                      | False            | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-569} Get Received Message Pending Flag Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(GetReceivedMessagePendingFlagTestWithParam, srcorTest022VerifyGetReceivedMessagePendingFlagFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;

  // set expectations for init module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(test_config.safety_code_type, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(test_config.number_of_connections, test_config.t_max, test_config.n_diag_window, test_config.diag_timing_distr_intervals)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(test_config.number_of_connections)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(test_config.number_of_connections, test_config.n_send_max)).Times(1);

  // call the init function
  EXPECT_NO_THROW(srcor_Init(&test_config));

  if(!expect_fatal_error)
  {
    // set flag according to test parameter
    srcor_rasta_connections[connection_id].received_data_pending = set_flag;
  }

  // execute the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_GetReceivedMessagePendingFlag(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_GetReceivedMessagePendingFlag(connection_id));

    // verify pending flag
    EXPECT_EQ(set_flag, srcor_rasta_connections[connection_id].received_data_pending);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetReceivedMessagePendingFlagFunction,
  GetReceivedMessagePendingFlagTestWithParam,
  ::testing::Values(
    // vary set pending flag
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, false, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, false),
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, true, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, true, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, false, true),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), false, true)
  ));

/**
 * @test        @ID{srcorTest023} Verify the GetBufferSizeAndUtilisation function
 *
 * @details     This test verifies the GetBufferSizeAndUtilisation function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Set expectations for receive and send buffer calls
 * - Init the module
 * - Set opposite buffer size to channel connection data
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call the GetBufferSizeAndUtilisation function
 * - Verify buffer utilisation and buffer size with return values from expected call
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config            | Expected values                         ||
 * |----------|-----------------|------------------------|---------------------|--------------------|
 * |          | Connection Id   | Configured connections | Expect fatal error  | Test Result        |
 * | 0        | 0               | 1                      | False               | Normal operation   |
 * | 1        | 0               | 2                      | False               | Normal operation   |
 * | 2        | 1               | 2                      | False               | Normal operation   |
 * | 3        | 1               | 1                      | True                | Fatal error raised |
 * | 4        | 2               | 2                      | True                | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-567} Get Buffer Size and Utilisation Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 * @verifyReq{RASW-461} Struct Buffer Utilisation Structure
 * @verifyReq{RASW-460} Used Send Buffer Entries
 * @verifyReq{RASW-465} Free Send Buffer Entries
 * @verifyReq{RASW-464} Used Receive Buffer Entries
 * @verifyReq{RASW-463} Free Receive Buffer Entries
 */
TEST_P(GetBufferSizeAndUtilisationTestWithParam, srcorTest023VerifyGetBufferSizeAndUtilisationFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint16_t free_received_buffer_entries_to_return   = 15;
  uint16_t used_received_buffer_entries_to_return   = 5;
  uint16_t free_send_buffer_entries_to_return       = 12;
  uint16_t used_send_buffer_entries_to_return       = 8;
  uint16_t opposite_buffer_size_to_return           = 10;

  // variables to verify values
  sraty_BufferUtilisation buffer_utilisation = {};
  uint16_t opposite_buffer_size = 0;

  if(!expect_fatal_error)
  {
    // set expectations for receive and send buffer calls
    EXPECT_CALL(srrece_mock, srrece_GetFreeBufferEntries(connection_id)).WillOnce(Return(free_received_buffer_entries_to_return));
    EXPECT_CALL(srrece_mock, srrece_GetUsedBufferEntries(connection_id)).WillOnce(Return(used_received_buffer_entries_to_return));
    EXPECT_CALL(srsend_mock, srsend_GetFreeBufferEntries(connection_id)).WillOnce(Return(free_send_buffer_entries_to_return));
    EXPECT_CALL(srsend_mock, srsend_GetUsedBufferEntries(connection_id)).WillOnce(Return(used_send_buffer_entries_to_return));
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

  if(!expect_fatal_error)
  {
    // set opposite buffer size
    srcor_rasta_connections[connection_id].opposite_receive_buffer_size = opposite_buffer_size_to_return;
  }

  // execute the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_GetBufferSizeAndUtilisation(connection_id, &buffer_utilisation, &opposite_buffer_size), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_GetBufferSizeAndUtilisation(connection_id, &buffer_utilisation, &opposite_buffer_size));

    // verify buffer values
    EXPECT_EQ(free_received_buffer_entries_to_return, buffer_utilisation.receive_buffer_free);
    EXPECT_EQ(used_received_buffer_entries_to_return, buffer_utilisation.receive_buffer_used);
    EXPECT_EQ(free_send_buffer_entries_to_return, buffer_utilisation.send_buffer_free);
    EXPECT_EQ(used_send_buffer_entries_to_return, buffer_utilisation.send_buffer_used);
    EXPECT_EQ(opposite_buffer_size_to_return, opposite_buffer_size);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetBufferSizeAndUtilisationFunction,
  GetBufferSizeAndUtilisationTestWithParam,
  ::testing::Values(
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, true),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), true)
  ));

/**
 * @test        @ID{srcorTest033} Verify the IsReceivedMsgPendingAndBuffersNotFull function
 *
 * @details     This test verifies the IsReceivedMsgPendingAndBuffersNotFull function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections according test parameter
 * - Set return values for Receive and Send buffer call according test parameter
 * - Init the module
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call the IsReceivedMsgPendingAndBuffersNotFull function and verify the return value
 * .
 *
 * @testParameter
 * - Connection id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Set pending flag: Indicates if pending flag gets set before srcor_IsReceivedMsgPendingAndBuffersNotFull function call
 * - Free receive bfr entries: Configured return value from srrece_GetFreeBufferEntries function call
 * - Free send bfr entries: Configured return value from srsend_GetFreeBufferEntries function call
 * - Exp. return value: Expected return value from srcor_IsReceivedMsgPendingAndBuffersNotFull function call
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config                                                                               |||| Expected values                                           |||
 * |----------|-----------------|------------------------|------------------|--------------------------|-----------------------|-------------------|--------------------|--------------------|
 * |          | Connection id   | Configured connections | Set pending flag | Free receive bfr entries | Free send bfr entries | Exp. return value | Expect fatal error | Test Result        |
 * | 0        | 0               | 1                      | false            | 0                        | 0                     | false             | false              | Normal operation   |
 * | 1        | 0               | 1                      | true             | 0                        | 0                     | false             | false              | Normal operation   |
 * | 2        | 0               | 1                      | false            | 1                        | 3                     | false             | false              | Normal operation   |
 * | 3        | 0               | 1                      | true             | 1                        | 0                     | false             | false              | Normal operation   |
 * | 4        | 0               | 1                      | true             | 0                        | 3                     | false             | false              | Normal operation   |
 * | 5        | 0               | 1                      | true             | 1                        | 3                     | true              | false              | Normal operation   |
 * | 6        | 0               | 1                      | true             | 20                       | 20                    | true              | false              | Normal operation   |
 * | 7        | 1               | 1                      | true             | 20                       | 20                    | false             | true               | Fatal error raised |
 * | 8        | 0               | 2                      | true             | 20                       | 20                    | true              | false              | Normal operation   |
 * | 9        | 1               | 2                      | true             | 20                       | 20                    | true              | false              | Normal operation   |
 * | 10       | 2               | 2                      | true             | 20                       | 20                    | false             | true               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-830} Is Received Message Pending And Buffers Not Full Function
 */
TEST_P(IsReceivedMsgPendingAndBuffersNotFullTestWithParam, srcorTest033VerifyIsReceivedMsgPendingAndBuffersNotFullFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;

  // set expectations for init module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(test_config.safety_code_type, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(test_config.number_of_connections, test_config.t_max, test_config.n_diag_window, test_config.diag_timing_distr_intervals)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(test_config.number_of_connections)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(test_config.number_of_connections, test_config.n_send_max)).Times(1);

  if(!expect_fatal_error)
  {
    EXPECT_CALL(srrece_mock, srrece_GetFreeBufferEntries(connection_id)).WillOnce(Return(free_receive_buffer_entries));
    EXPECT_CALL(srsend_mock, srsend_GetFreeBufferEntries(connection_id)).WillOnce(Return(free_send_buffer_entries));
  }

  // call the init function
  EXPECT_NO_THROW(srcor_Init(&test_config));

  // execute the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_IsReceivedMsgPendingAndBuffersNotFull(connection_id), std::invalid_argument);
  }
  else
  {
    // set message pending flag according test parameter
    if(message_pending_flag_set)
    {
      EXPECT_NO_THROW(srcor_SetReceivedMessagePendingFlag(connection_id));
    }

    EXPECT_EQ(expect_return_value, srcor_IsReceivedMsgPendingAndBuffersNotFull(connection_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyIsReceivedMsgPendingAndBuffersNotFullFunction,
  IsReceivedMsgPendingAndBuffersNotFullTestWithParam,
  ::testing::Values(
    // vary set pending flag
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       false, 0,                                            0,                                      false, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0,                                            0,                                      false, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       false, UT_SRCOR_RECEIVE_MIN_FREE_ENTRIES_TO_RECEIVE, UT_SRCOR_SEND_MIN_FREE_ENTRIES_TO_SEND, false, false),
    // vary free receive buffer size
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  UT_SRCOR_RECEIVE_MIN_FREE_ENTRIES_TO_RECEIVE, 0,                                      false, false),
    // vary free send buffer size
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  0,                                            UT_SRCOR_SEND_MIN_FREE_ENTRIES_TO_SEND, false, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  UT_SRCOR_RECEIVE_MIN_FREE_ENTRIES_TO_RECEIVE, UT_SRCOR_SEND_MIN_FREE_ENTRIES_TO_SEND, true,  false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN,       true,  UT_SRCOR_RECEIVE_BUFFER_SIZE_MAX,             UT_SRCOR_SEND_BUFFER_SIZE_MAX,          true,  false),
    // vary connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX,       true,  UT_SRCOR_RECEIVE_BUFFER_SIZE_MAX,             UT_SRCOR_SEND_BUFFER_SIZE_MAX,          false, true),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN,       true,  UT_SRCOR_RECEIVE_BUFFER_SIZE_MAX,             UT_SRCOR_SEND_BUFFER_SIZE_MAX,          true,  false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX,       true,  UT_SRCOR_RECEIVE_BUFFER_SIZE_MAX,             UT_SRCOR_SEND_BUFFER_SIZE_MAX,          true,  false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), true,  UT_SRCOR_RECEIVE_BUFFER_SIZE_MAX,             UT_SRCOR_SEND_BUFFER_SIZE_MAX,          false, true)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

