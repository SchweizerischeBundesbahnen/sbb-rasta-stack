/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srcor.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the core module of the safety and retransmission layer.
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
 * @brief Test class for value-parametrized init function test
 *
 * Parameter order:
 * - use valid config (bool)
 * .
 */
class InitTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<bool>>
{
  public:
  bool use_valid_config = std::get<0>(GetParam());     ///< indicates if valid config gets used
};

/**
 * @brief Test class for value-parametrized InitRastaConnData function test
 *
 * Parameter order:
 * - amount of configured connections (uint32_t)
 * - connection id (uint32_t)
 * - indicates if the connection is in role server (bool)
 * - expected error (radef_RaStaReturnCode)
 * .
 */
class InitRastaConnDataTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =                       std::get<0>(GetParam());    ///< configured connections
  uint32_t connection_id =                                std::get<1>(GetParam());    ///< connection id
  bool is_conn_role_server =                              std::get<2>(GetParam());    ///< indicates if the connection is in role server
  radef_RaStaReturnCode expected_error =                  std::get<3>(GetParam());    ///< expected error
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_core
 * @{
 */

/**
 * @test        @ID{srcorTest002} Verify the Init function
 *
 * @details     This test verifies the initialization of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Set expectations for initializing other modules when using valid config (given by parameter)
 * - Call init function with invalid or valid config
 * - Verify radef_kInvalidConfiguration exception gets thrown when using invalid config
 * - Verify initialization state
 * .
 *
 * @testParameter
 * - Use valid config: Indicates if valid config gets used for srcor_Init call
 * .
 * | Test Run | Input parameter  | Expected values    |
 * |----------|------------------|--------------------|
 * |          | Use valid config | Test Result        |
 * | 0        | true             | Normal operation   |
 * | 1        | false            | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(InitTestWithParam, srcorTest002VerifyInitFunction)
{
  // test variables
  const srcty_SafetyRetransmissionConfiguration invalid_test_config = {
    0,
    0,
    0,
    srcty_kSafetyCodeTypeNone,
    0,
    0,
    0,
    0,
    0,
    {0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };

  if(use_valid_config)
  {
    // set expectations
    EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
    EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
    EXPECT_CALL(srmsg_mock, srmsg_Init(default_config.safety_code_type, _)).Times(1).WillOnce(Invoke(this, &srcorTest::checkDefaultMd4InitValues));
    EXPECT_CALL(srdia_mock, srdia_Init(default_config.number_of_connections, default_config.t_max, default_config.n_diag_window, default_config.diag_timing_distr_intervals)).Times(1);
    EXPECT_CALL(srsend_mock, srsend_Init(default_config.number_of_connections)).Times(1);
    EXPECT_CALL(srrece_mock, srrece_Init(default_config.number_of_connections, default_config.n_send_max)).Times(1);

    // call the init function
    EXPECT_NO_THROW(srcor_Init(&default_config));

    // check initialization state
    EXPECT_TRUE(srcor_initialized);
  }
  else
  {
    // expect error to be raised on init function call with invalid config
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidConfiguration)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_Init(&invalid_test_config), std::invalid_argument);

    // check initialization state
    EXPECT_FALSE(srcor_initialized);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitFunction,
  InitTestWithParam,
  ::testing::Values(
        std::make_tuple(true),
        std::make_tuple(false)
  ));

/**
 * @test        @ID{srcorTest003} Verify the number of init function calls
 *
 * @details     This test verifies that the initialization of the rasta safety and
 *              retransmission layer core module is done only once.
 *
 * Test steps:
 * - call the init function and verify that it completes successfully
 * - call the init function again and verify that a radef_kAlreadyInitialized error gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_F(srcorTest, srcorTest003VerifyInitFunctionCalls)
{
  // set expectations for successful init call
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(default_config.safety_code_type, _)).Times(1).WillOnce(Invoke(this, &srcorTest::checkDefaultMd4InitValues));
  EXPECT_CALL(srdia_mock, srdia_Init(default_config.number_of_connections, default_config.t_max, default_config.n_diag_window, default_config.diag_timing_distr_intervals)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(default_config.number_of_connections)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(default_config.number_of_connections, default_config.n_send_max)).Times(1);

  // call the init function and verify that it completes successfully
  EXPECT_NO_THROW(srcor_Init(&default_config));

  // call the init function again and verify that a error is thrown
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
  EXPECT_THROW(srcor_Init(&default_config), std::invalid_argument);
}

/**
 * @test        @ID{srcorTest004} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - call the srcor_Init function with invalid configuration pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - init the core module with the default config
 * - call the srcor_IsConfigurationValid function with invalid configuration pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srcor_GetConnectionId function with invalid connection id pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srcor_ReceiveMessage function with invalid connection event pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srcor_ReceiveMessage function with invalid seq number in sequence pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srcor_ReceiveMessage function with invalid confirmed timestamp in seq pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srcor_WriteMessagePayloadToTemporaryBuffer function with invalid message payload pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srcor_GetBufferSizeAndUtilisation function with invalid buffer utilization pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the srcor_GetBufferSizeAndUtilisation function with invalid buffer size pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-573} Is Configuration Valid Function
 * @verifyReq{RASW-568} Get Connection ID Function
 * @verifyReq{RASW-580} Receive Message Function
 * @verifyReq{RASW-593} Write Message Payload to Temporary Buffer Function
 * @verifyReq{RASW-567} Get Buffer Size and Utilisation Function
 * @verifyReq{RASW-520} Error Handling
 * @verifyReq{RASW-521} Input Parameter Check
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 * @verifyReq{RASW-461} Struct Buffer Utilisation Structure
 */
TEST_F(srcorTest, srcorTest004VerifyNullPointerCheck)
{
  uint8_t nbr_of_function_calls = 9U;        // number of function calls

  // test variables
  uint32_t sender_id = 0U;
  uint32_t receiver_id = 0U;
  uint32_t connection_id = 0U;
  uint32_t message_payload_size = 0U;
  uint16_t opposite_buffer_size = 0U;
  bool sequence_number_in_seq = false;
  bool confirmed_time_stamp_in_seq = false;
  srtyp_ConnectionEvents connection_event = srtyp_kConnEventMin;
  sraty_BufferUtilisation buffer_utilisation = {};

  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
    .Times(nbr_of_function_calls)
    .WillRepeatedly(Invoke(this, &srcorTest::invalidArgumentException));

  // perform init null pointer check test
  EXPECT_THROW(srcor_Init((srcty_SafetyRetransmissionConfiguration *)NULL), std::invalid_argument);

  // expect calls for init core module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(_, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(_, _, _, _)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(_)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(_, _)).Times(1);

  // initialize the module for rest of tests
  EXPECT_NO_THROW(srcor_Init(&default_config));

  // call the functions
  EXPECT_THROW(srcor_IsConfigurationValid((srcty_SafetyRetransmissionConfiguration *)NULL), std::invalid_argument);
  EXPECT_THROW(srcor_GetConnectionId(sender_id, receiver_id, (uint32_t *)NULL), std::invalid_argument);
  EXPECT_THROW(srcor_ReceiveMessage(connection_id, (srtyp_ConnectionEvents *)NULL, &sequence_number_in_seq, &confirmed_time_stamp_in_seq), std::invalid_argument);
  EXPECT_THROW(srcor_ReceiveMessage(connection_id, &connection_event, (bool *)NULL, &confirmed_time_stamp_in_seq), std::invalid_argument);
  EXPECT_THROW(srcor_ReceiveMessage(connection_id, &connection_event, &sequence_number_in_seq, (bool *)NULL), std::invalid_argument);
  EXPECT_THROW(srcor_WriteMessagePayloadToTemporaryBuffer(connection_id, message_payload_size, (uint8_t *)NULL), std::invalid_argument);
  EXPECT_THROW(srcor_GetBufferSizeAndUtilisation(connection_id, (sraty_BufferUtilisation *)NULL, &opposite_buffer_size), std::invalid_argument);
  EXPECT_THROW(srcor_GetBufferSizeAndUtilisation(connection_id, &buffer_utilisation, (uint16_t *)NULL), std::invalid_argument);
}

/**
 * @test        @ID{srcorTest005} Initialization checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the GetConnectionId function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the InitRaStaConnData function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the CloseRedundancyChannel function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the ReceiveMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the ProcessReceivedMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the UpdateConfirmedTxSequenceNumber function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the UpdateConfirmedRxSequenceNumber function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the IsProtocolVersionAccepted function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SetReceivedMessagePendingFlag function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetReceivedMessagePendingFlag function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the WriteMessagePayloadToTemporaryBuffer function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the ClearInputBufferMessagePendingFlag function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SendDataMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SendConnReqMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SendConnRespMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SendDiscReqMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SetDiscDetailedReason function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SendHbMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SendRetrReqMessage function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the HandleRetrReq function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the IsRetrReqSequenceNumberAvailable function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the IsConnRoleServer function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the IsMessageTimeout function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the IsHeartbeatInterval function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SendPendingMessages function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SendConnectionStateNotification function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetBufferSizeAndUtilisation function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-528} Fatal Error Function
 * @verifyReq{RASW-565} Component sr_core Overview
 * @verifyReq{RASW-568} Get Connection ID Function
 * @verifyReq{RASW-572} Init RaSTA Connection Data Function
 * @verifyReq{RASW-826} Close Redundancy Channel Function
 * @verifyReq{RASW-580} Receive Message Function
 * @verifyReq{RASW-579} Process Received Messages Function
 * @verifyReq{RASW-592} Update Confirmed Tx Sequence Number Function
 * @verifyReq{RASW-591} Update Confirmed Rx Sequence Number Function
 * @verifyReq{RASW-577} Is Protocol Version Accepted Function
 * @verifyReq{RASW-590} Set Received Message Pending Flag Function
 * @verifyReq{RASW-569} Get Received Message Pending Flag Function
 * @verifyReq{RASW-593} Write Message Payload to Temporary Buffer Function
 * @verifyReq{RASW-566} Clear Input Buffer Message Pending Flag Function
 * @verifyReq{RASW-584} Send Data Message Function
 * @verifyReq{RASW-582} Send ConnReq Message Function
 * @verifyReq{RASW-583} Send ConnResp Message Function
 * @verifyReq{RASW-585} Send DiscReq Message Function
 * @verifyReq{RASW-589} Set Disconnection Detailed Reason Function
 * @verifyReq{RASW-586} Send Heartbeat Message Function
 * @verifyReq{RASW-588} Send RetrReq Message Function
 * @verifyReq{RASW-570} Handle Retransmission Request Function
 * @verifyReq{RASW-578} Is Retransmission Request Sequence Number Available Function
 * @verifyReq{RASW-574} Is Connection Role Server Function
 * @verifyReq{RASW-576} Is Message Timeout Function
 * @verifyReq{RASW-575} Is Heartbeat Interval Function
 * @verifyReq{RASW-587} Send Pending Messages Function
 * @verifyReq{RASW-581} Send Connection State Notification Function
 * @verifyReq{RASW-567} Get Buffer Size and Utilisation Function
 * @verifyReq{RASW-461} Struct Buffer Utilisation Structure
 * @verifyReq{RASW-489} Enum Disc Reason Structure
 */
TEST_F(srcorTest, srcorTest005VerifyInitializationCheck)
{
  uint8_t nbr_of_function_calls = 27U;      // number of function calls

  // Test variables
  uint32_t sender_id = 0U;
  uint32_t receiver_id = 0U;
  uint32_t connection_id = 0U;
  uint32_t message_payload_size = 0U;
  uint16_t opposite_buffer_size = 0U;
  uint16_t detailed_disconnect_reason = 0U;
  uint8_t message_payload[message_payload_size] = {};
  bool sequence_number_in_seq = false;
  bool confirmed_time_stamp_in_seq = false;
  srtyp_ConnectionEvents connection_event = srtyp_kConnEventMin;
  sraty_DiscReason disconnect_reason = sraty_kDiscReasonMin;
  sraty_ConnectionStates connection_state = sraty_kConnectionMin;
  sraty_BufferUtilisation buffer_utilisation = {};

  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
    .Times(nbr_of_function_calls)
    .WillRepeatedly(Invoke(this, &srcorTest::invalidArgumentException));

  // call the functions without initializing the module
  EXPECT_THROW(srcor_GetConnectionId(sender_id, receiver_id, &connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_InitRaStaConnData(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_CloseRedundancyChannel(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_ReceiveMessage(connection_id, &connection_event, &sequence_number_in_seq, &confirmed_time_stamp_in_seq), std::invalid_argument);
  EXPECT_THROW(srcor_ProcessReceivedMessage(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_UpdateConfirmedTxSequenceNumber(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_UpdateConfirmedRxSequenceNumber(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_IsProtocolVersionAccepted(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_SetReceivedMessagePendingFlag(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_GetReceivedMessagePendingFlag(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_WriteMessagePayloadToTemporaryBuffer(connection_id, message_payload_size, message_payload), std::invalid_argument);
  EXPECT_THROW(srcor_ClearInputBufferMessagePendingFlag(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_SendDataMessage(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_SendConnReqMessage(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_SendConnRespMessage(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_SendDiscReqMessage(connection_id, disconnect_reason), std::invalid_argument);
  EXPECT_THROW(srcor_SetDiscDetailedReason(connection_id, detailed_disconnect_reason), std::invalid_argument);
  EXPECT_THROW(srcor_SendHbMessage(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_SendRetrReqMessage(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_HandleRetrReq(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_IsRetrReqSequenceNumberAvailable(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_IsConnRoleServer(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_IsMessageTimeout(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_IsHeartbeatInterval(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_SendPendingMessages(connection_id), std::invalid_argument);
  EXPECT_THROW(srcor_SendConnectionStateNotification(connection_id, connection_state, disconnect_reason), std::invalid_argument);
  EXPECT_THROW(srcor_GetBufferSizeAndUtilisation(connection_id, &buffer_utilisation, &opposite_buffer_size), std::invalid_argument);
}

/**
 * @test        @ID{srcorTest006} Verify the InitRastaConnData function
 *
 * @details     This test verifies the InitRastaConnData function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from parameter)
 * - Set expected calls
 *   - for rasys_GetTimerValue when in client role
 *   - for init diagnostics, init send and receive buffer and open redundancy channel call
 * - Set sender and receiver according to server or client role
 * - Initialize the module
 * - Call the InitRaStaConnData and verify values
 *   - SN_T set to random number and t_i = configured Tmax
 *   - If connection role client verify CS_T = 0 and CTS_R = current time
 * .
 *
 * @testParameter
 * - Connection id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Connection role server: Indicates if connection to init is in server role
 * - Expected error: Expected error code when calling function
 * .
 * | Test Run | Input parameter | Test config                                    || Expected values                             ||
 * |----------|-----------------|------------------------|------------------------|-------------------------|--------------------|
 * |          | Connection id   | Configured connections | Connection role server | Expected error          | Test Result        |
 * | 0        | 0U              | 1                      | True                   | radef_kNoError          | Normal operation   |
 * | 1        | 0U              | 1                      | False                  | radef_kNoError          | Normal operation   |
 * | 2        | 0U              | 2                      | False                  | radef_kNoError          | Normal operation   |
 * | 3        | 1U              | 2                      | False                  | radef_kNoError          | Normal operation   |
 * | 4        | 1U              | 1                      | False                  | radef_kInvalidParameter | Fatal error raised |
 * | 5        | 2U              | 2                      | False                  | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-572} Init RaSTA Connection Data Function
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-649} Open Redundancy Channel Function
 * @verifyReq{RASW-369} Open Redundancy Channel Function Structure
 * @verifyReq{RASW-367} Redundancy Channel Id
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-531} Get Timer Value Function
 * @verifyReq{RASW-410} Get Timer Value Function Structure
 * @verifyReq{RASW-422} Timer Value
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 * @verifyReq{RASW-529} Get Random Number Function
 * @verifyReq{RASW-414} Get Random Number Function Structure
 * @verifyReq{RASW-413} Random Number
 */
TEST_P(InitRastaConnDataTestWithParam, srcorTest006VerifyInitRastaConnDataFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint32_t timer_value = 1000;  // timer value to verify confirmed timestamp
  uint32_t random_number = 1234; // "random" number for rasys mock call to verify
  uint32_t sender_id = 0;
  uint32_t receiver_id = 1;

  // set expectations for init module
  EXPECT_CALL(srdia_mock, srdia_AreDiagnosticTimingIntervalsValid(_, _)).WillOnce(Return(true));
  EXPECT_CALL(rasys_mock, rasys_GetTimerGranularity()).Times(1);
  EXPECT_CALL(srmsg_mock, srmsg_Init(test_config.safety_code_type, _)).Times(1);
  EXPECT_CALL(srdia_mock, srdia_Init(test_config.number_of_connections, test_config.t_max, test_config.n_diag_window, test_config.diag_timing_distr_intervals)).Times(1);
  EXPECT_CALL(srsend_mock, srsend_Init(test_config.number_of_connections)).Times(1);
  EXPECT_CALL(srrece_mock, srrece_Init(test_config.number_of_connections, test_config.n_send_max)).Times(1);

  if(expected_error == radef_kNoError)
  {
    if(is_conn_role_server)
    {
      // The communication partner with the higher value is server, the one with the lower value client.
      sender_id = receiver_id + 1;
    }
    else
    {
      EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).WillOnce(Return(timer_value));
    }

    EXPECT_CALL(rasys_mock, rasys_GetRandomNumber()).WillOnce(Return(random_number));
    EXPECT_CALL(srdia_mock, srdia_InitConnectionDiagnostics(connection_id));
    EXPECT_CALL(srsend_mock, srsend_InitBuffer(connection_id));
    EXPECT_CALL(srrece_mock, srrece_InitBuffer(connection_id));
    EXPECT_CALL(sradin_mock, sradin_OpenRedundancyChannel(connection_id)); // connection and redundancy channel are the same (1:1 mapping)

    // set sender and receiver id
    test_config.connection_configurations[connection_id].sender_id = sender_id;
    test_config.connection_configurations[connection_id].receiver_id = receiver_id;

    srcor_rasta_connections[connection_id].confirmed_sequence_number_tx = 123; // set to any value to verify if reset when not in server role
  }

  // call the init function
  EXPECT_NO_THROW(srcor_Init(&test_config));

  if(expected_error != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_InitRaStaConnData(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_InitRaStaConnData(connection_id));

    // verify: SN_T set to random number and, t_i to configured t_max
    ASSERT_EQ(srcor_rasta_connections[connection_id].sequence_number_tx, random_number);
    ASSERT_EQ(srcor_rasta_connections[connection_id].timer_t_i, test_config.t_max);

    if(!is_conn_role_server)
    {
      // verify CS_T is set to 0 and CTS_R to current time
      EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_sequence_number_tx, 0);
      EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_time_stamp_rx, timer_value);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitRastaConnDataFunction,
  InitRastaConnDataTestWithParam,
  ::testing::Values(
    // vary if server or client role in connection
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, true, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, false, radef_kNoError),
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, false, radef_kNoError),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, false, radef_kInvalidParameter),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), false, radef_kInvalidParameter)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
