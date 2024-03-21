/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srcor_set_update.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for set and update specific functions from the core module of the safety and retransmission layer.
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
 * @brief Test class for value-parametrized CloseRedundancyChannel function test
 *
 * Parameter order:
 * - amount of configured channels (uint32_t)
 * - channel id (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class CloseRedundancyChannelTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>>
{
  public:
  uint32_t configured_connections =  std::get<0>(GetParam());     ///< amount of configured connections
  uint32_t connection_id =           std::get<1>(GetParam());     ///< channel id
  bool expect_fatal_error =          std::get<2>(GetParam());     ///< expect fatal error
};

/**
 * @brief Test class for value-parametrized UpdateConfirmedTxSequenceNumber function test
 *
 * Parameter order:
 * - amount of configured channels (uint32_t)
 * - channel id to update confirmed tx seq nbr (uint32_t)
 * - Confirmed seq nbr (uint32_t)
 * - indicates if a message is in input buffer (bool)
 * - expect fatal error (bool)
 * .
 */
class UpdateConfirmedTxSequenceNumberTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, bool, bool>>
{
  public:
  uint32_t configured_connections =  std::get<0>(GetParam());     ///< amount of configured connections
  uint32_t connection_id =           std::get<1>(GetParam());     ///< channel id to update confirmed tx seq nbr
  uint32_t seq_nbr =                 std::get<2>(GetParam());     ///< Confirmed sequence number
  bool message_in_input_buffer =     std::get<3>(GetParam());     ///< indicates if a message is in input buffer
  bool expect_fatal_error =          std::get<4>(GetParam());     ///< expect fatal error
};

/**
 * @brief Test class for value-parametrized UpdateConfirmedRxSequenceNumber function test
 *
 * Parameter order:
 * - amount of configured channels (uint32_t)
 * - channel id to update confirmed rx seq nbr (uint32_t)
 * - Confirmed seq nbr (uint32_t)
 * - indicates if a message is in input buffer (bool)
 * - expect fatal error (bool)
 * .
 */
class UpdateConfirmedRxSequenceNumberTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, bool, bool>>
{
  public:
  uint32_t configured_connections =  std::get<0>(GetParam());     ///< amount of configured connections
  uint32_t connection_id =           std::get<1>(GetParam());     ///< channel id to update confirmed rx seq nbr
  uint32_t seq_nbr =                 std::get<2>(GetParam());     ///< Confirmed sequence number
  bool message_in_input_buffer =     std::get<3>(GetParam());     ///< indicates if a message is in input buffer
  bool expect_fatal_error =          std::get<4>(GetParam());     ///< expect fatal error
};

/**
 * @brief Test class for value-parametrized SetReceivedMessagePendingFlag function test
 *
 * This class simply inherits from CloseRedundancyChannelTestWithParam class due to same parameter
 */
class SetReceivedMessagePendingFlagTestWithParam : public CloseRedundancyChannelTestWithParam{};

/**
 * @brief Test class for value-parametrized ClearInputBufferMessagePendingFlag function test
 *
 * This class simply inherits from CloseRedundancyChannelTestWithParam class due to same parameter
 */
class ClearInputBufferMessagePendingFlagTestWithParam : public CloseRedundancyChannelTestWithParam{};

/**
 * @brief Test class for value-parametrized SetDiscDetailedReason function test
 *
 * Parameter order:
 * - amount of configured channels (uint32_t)
 * - connection id to set detailed reason (uint32_t)
 * - disc detailed reason (uint16_t)
 * - expect fatal error (bool)
 * .
 */
class SetDiscDetailedReasonTestWithParam : public srcorTest, public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint16_t, bool>>
{
  public:
  uint32_t configured_connections =  std::get<0>(GetParam());     ///< amount of configured connections
  uint32_t connection_id =           std::get<1>(GetParam());     ///< connection id to set detailed reason
  uint16_t disc_detailed_reason  =   std::get<2>(GetParam());     ///< Disc detailed reason
  bool expect_fatal_error =          std::get<3>(GetParam());     ///< expect fatal error
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_core
 * @{
 */

/**
 * @test        @ID{srcorTest024} Verify the CloseRedundancyChannel function
 *
 * @details     This test verifies the CloseRedundancyChannel function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Set expectations for adapter interface and diagnostics module calls
 * - Init the module
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call the CloseRedundancyChannel function
 * .
 *
 * @testParameter
 * - Channel Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config            | Expected values                        ||
 * |----------|-----------------|------------------------|--------------------|--------------------|
 * |          | Channel Id      | Configured connections | Expect fatal error | Test Result        |
 * | 0        | 0               | 1                      | False              | Normal operation   |
 * | 1        | 0               | 2                      | False              | Normal operation   |
 * | 2        | 1               | 2                      | False              | Normal operation   |
 * | 3        | 1               | 1                      | True               | Fatal error raised |
 * | 4        | 2               | 2                      | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-826} Close Redundancy Channel Function
 * @verifyReq{RASW-647} Component sr_adapter_interface Overview
 * @verifyReq{RASW-650} Close Redundancy Channel Function
 * @verifyReq{RASW-368} Close Redundancy Channel Function Structure
 * @verifyReq{RASW-367} Redundancy Channel Id
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(CloseRedundancyChannelTestWithParam, srcorTest024VerifyCloseRedundancyChannelFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint32_t expected_channel_id = connection_id;

  // expect calls for adapter interface and diagnostics module
  if(!expect_fatal_error)
  {
    EXPECT_CALL(sradin_mock, sradin_CloseRedundancyChannel(expected_channel_id));
    EXPECT_CALL(srdia_mock, srdia_SendDiagnosticNotification(connection_id));
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

  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_CloseRedundancyChannel(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_CloseRedundancyChannel(connection_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyCloseRedundancyChannelFunction,
  CloseRedundancyChannelTestWithParam,
  ::testing::Values(
    // vary configured sender and receiver id on channel 1
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, true),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), true)
  ));

/**
 * @test        @ID{srcorTest025} Verify the UpdateConfirmedTxSequenceNumber function
 *
 * @details     This test verifies the UpdateConfirmedTxSequenceNumber function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Init the module
 * - Set any value to confirmed seq nbr tx
 * - Set message in buffer flag according to test parameter
 * - Set scratch header seq nbr according to test parameter
 * - Set seq nbr to scratch msg header according to test parameter
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call the UpdateConfirmedTxSequenceNumber function
 * - Verify that the confirmed sequence number of the given connection is updated correctly
 * - Verify that message in buffer flag is false
 * .
 *
 * @testParameter
 * - Channel Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Confirmed Seq nbr: Configured confirmed sequence number on connection
 * - Message in input buffer: Indicates if a message is in the input buffer
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config                                                        ||| Expected values                        ||
 * |----------|-----------------|------------------------|-------------------|-------------------------|--------------------|--------------------|
 * |          | Connection id   | Configured connections | Confirmed Seq nbr | Message in input buffer | Expect fatal error | Test Result        |
 * | 0        | 0               | 1                      | 0                 | False                   | False              | Normal operation   |
 * | 1        | 0               | 1                      | 0                 | True                    | False              | Normal operation   |
 * | 2        | 0               | 1                      | 0xffffffffU       | True                    | False              | Normal operation   |
 * | 3        | 0               | 2                      | 0xffffffffU       | True                    | False              | Normal operation   |
 * | 4        | 1               | 2                      | 0xffffffffU       | True                    | False              | Normal operation   |
 * | 5        | 1               | 1                      | 0xffffffffU       | True                    | True               | Fatal error raised |
 * | 6        | 2               | 2                      | 0xffffffffU       | True                    | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-592} Update Confirmed Tx Sequence Number Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(UpdateConfirmedTxSequenceNumberTestWithParam, srcorTest025VerifyUpdateConfirmedTxSequenceNumberFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint32_t previous_seq_nbr = seq_nbr + 1;  ///< to verify if nr gets updated

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
    // give seq nbr a value to verify if updated
    srcor_rasta_connections[connection_id].confirmed_sequence_number_tx = previous_seq_nbr;

    // set message in buffer property to test parameter
    srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = message_in_input_buffer;

    // set given seq nbr to msg header in input buffer
    srcor_rasta_connections[connection_id].input_buffer.message_header.sequence_number = seq_nbr;
  }

  // execute the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_UpdateConfirmedTxSequenceNumber(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_UpdateConfirmedTxSequenceNumber(connection_id));

    // verify seq nbr
    if(message_in_input_buffer)
    {
      EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_sequence_number_tx, seq_nbr);
    }
    else
    {
      EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_sequence_number_tx, previous_seq_nbr);
    }

    // verify message in buffer flag
    EXPECT_FALSE(srcor_rasta_connections[connection_id].input_buffer.message_in_buffer);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyUpdateConfirmedTxSequenceNumberFunction,
  UpdateConfirmedTxSequenceNumberTestWithParam,
  ::testing::Values(
    // vary message in input buffer
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, false, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, true, false),
    // vary seq nbr
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, true, false),
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, true, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, UINT32_MAX, true, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, UINT32_MAX,  true, true),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), UINT32_MAX, true, true)
  ));

/**
 * @test        @ID{srcorTest026} Verify the UpdateConfirmedRxSequenceNumber function
 *
 * @details     This test verifies the UpdateConfirmedRxSequenceNumber function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Init the module
 * - Set any value to confirmed seq nbr rx
 * - Set message in buffer flag according to test parameter
 * - Set scratch header seq nbr according to test parameter
 * - Set confirmed seq nbr to scratch msg header according to test parameter
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call the UpdateConfirmedRxSequenceNumber function
 * - Verify that the confirmed sequence number of the given connection is updated correctly
 * - Verify that message in buffer flag is false
 * .
 *
 * @testParameter
 * - Channel Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Confirmed Seq nbr: Configured confirmed sequence number on connection
 * - Message in input buffer: Indicates if a message is in the input buffer
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config                                                        ||| Expected values                        ||
 * |----------|-----------------|------------------------|-------------------|-------------------------|--------------------|--------------------|
 * |          | Connection id   | Configured connections | Confirmed Seq nbr | Message in input buffer | Expect fatal error | Test Result        |
 * | 0        | 0               | 1                      | 0                 | False                   | False              | Normal operation   |
 * | 1        | 0               | 1                      | 0                 | True                    | False              | Normal operation   |
 * | 2        | 0               | 1                      | 0xffffffffU       | True                    | False              | Normal operation   |
 * | 3        | 0               | 2                      | 0xffffffffU       | True                    | False              | Normal operation   |
 * | 4        | 1               | 2                      | 0xffffffffU       | True                    | False              | Normal operation   |
 * | 5        | 1               | 1                      | 0xffffffffU       | True                    | True               | Fatal error raised |
 * | 6        | 2               | 2                      | 0xffffffffU       | True                    | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-591} Update Confirmed Rx Sequence Number Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(UpdateConfirmedRxSequenceNumberTestWithParam, srcorTest026VerifyUpdateConfirmedRxSequenceNumberFunction)
{
  // test variables
  srcty_SafetyRetransmissionConfiguration test_config = default_config;
  test_config.number_of_connections = configured_connections;
  uint32_t previous_seq_nbr = seq_nbr + 1;  ///< to verify if nr gets updated

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
    // give seq nbr a value to verify if updated
    srcor_rasta_connections[connection_id].confirmed_sequence_number_rx = previous_seq_nbr;

    // set message in buffer property to test parameter
    srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = message_in_input_buffer;

    // set given seq nbr to msg header in input buffer
    srcor_rasta_connections[connection_id].input_buffer.message_header.confirmed_sequence_number = seq_nbr;

    if(message_in_input_buffer)
    {
      EXPECT_CALL(srsend_mock, srsend_RemoveFromBuffer(connection_id, seq_nbr)).Times(1);
    }
  }

  // execute the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_UpdateConfirmedRxSequenceNumber(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_UpdateConfirmedRxSequenceNumber(connection_id));

    // verify seq nbr
    if(message_in_input_buffer)
    {
      EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_sequence_number_rx, seq_nbr);
    }
    else
    {
      EXPECT_EQ(srcor_rasta_connections[connection_id].confirmed_sequence_number_rx, previous_seq_nbr);
    }

    // verify message in buffer flag
    EXPECT_FALSE(srcor_rasta_connections[connection_id].input_buffer.message_in_buffer);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyUpdateConfirmedRxSequenceNumberFunction,
  UpdateConfirmedRxSequenceNumberTestWithParam,
  ::testing::Values(
    // vary message in input buffer
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, false, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, true, false),
    // vary seq nbr
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, true, false),
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, UINT32_MAX, true, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, UINT32_MAX, true, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, UINT32_MAX,  true, true),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), UINT32_MAX, true, true)
  ));

/**
 * @test        @ID{srcorTest027} Verify the SetReceivedMessagePendingFlag function
 *
 * @details     This test verifies the SetReceivedMessagePendingFlag function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Init the module
 * - Set message pending flag on channel to false
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call the SetReceivedMessagePendingFlag function
 * - Verify message pending flag is set to true
 * .
 *
 * @testParameter
 * - Channel Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config            | Expected values                        ||
 * |----------|-----------------|------------------------|--------------------|--------------------|
 * |          | Connection Id   | Configured connections | Expect fatal error | Test Result        |
 * | 0        | 0               | 1                      | False              | Normal operation   |
 * | 1        | 0               | 2                      | False              | Normal operation   |
 * | 2        | 1               | 2                      | False              | Normal operation   |
 * | 3        | 1               | 1                      | True               | Fatal error raised |
 * | 4        | 2               | 2                      | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-590} Set Received Message Pending Flag Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(SetReceivedMessagePendingFlagTestWithParam, srcorTest027VerifySetReceivedMessagePendingFlagFunction)
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
    // set message pending flag to false
    srcor_rasta_connections[connection_id].received_data_pending = false;
  }

  // execute the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_SetReceivedMessagePendingFlag(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_SetReceivedMessagePendingFlag(connection_id));

    // verify pending flag true
    EXPECT_TRUE(srcor_rasta_connections[connection_id].received_data_pending);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySetReceivedMessagePendingFlagFunction,
  SetReceivedMessagePendingFlagTestWithParam,
  ::testing::Values(
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, true),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), true)
  ));

/**
 * @test        @ID{srcorTest028} Verify the ClearInputBufferMessagePendingFlag function
 *
 * @details     This test verifies the ClearInputBufferMessagePendingFlag function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Init the module
 * - Set message in buffer flag on channel
 * - Call the ClearInputBufferMessagePendingFlag function
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - When no error expected verify message in buffer flag is reset
 * .
 *
 * @testParameter
 * - Channel Id: RaSTA connection identification
 * - Configured connections: Number of configured connections
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config            | Expected values                        ||
 * |----------|-----------------|------------------------|--------------------|--------------------|
 * |          | Connection Id   | Configured connections | Expect fatal error | Test Result        |
 * | 0        | 0               | 1                      | False              | Normal operation   |
 * | 1        | 0               | 2                      | False              | Normal operation   |
 * | 2        | 1               | 2                      | False              | Normal operation   |
 * | 3        | 1               | 1                      | True               | Fatal error raised |
 * | 4        | 2               | 2                      | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-566} Clear Input Buffer Message Pending Flag Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(ClearInputBufferMessagePendingFlagTestWithParam, srcorTest028VerifyClearInputBufferMessagePendingFlagFunction)
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
    // set message in buffer flag to true
    srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = true;
  }

  // execute the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_ClearInputBufferMessagePendingFlag(connection_id), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_ClearInputBufferMessagePendingFlag(connection_id));

    // verify input buffer message pending flag reset
    EXPECT_FALSE(srcor_rasta_connections[connection_id].input_buffer.message_in_buffer);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyClearInputBufferMessagePendingFlagFunction,
  ClearInputBufferMessagePendingFlagTestWithParam,
  ::testing::Values(
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, true),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), true)
  ));

/**
 * @test        @ID{srcorTest029} Verify the SetDiscDetailedReason function
 *
 * @details     This test verifies the SetDiscDetailedReason function of the rasta safety and
 *              retransmission layer core module.
 *
 * Test steps:
 * - Create a config with given amount of connections (from test parameter)
 * - Init the module
 * - Set message in buffer flag on channel
 * - Call the SetDiscDetailedReason function according to test parameter
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - When no error expected Verify disc detailed reason set correctly
 * .
 *
 * @testParameter
 * - Channel Id: RaSTA connection identification
 * - Disc detailed reason: Detailed disconnect reason to set
 * - Configured connections: Number of configured connections
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                      || Test config            | Expected values                        ||
 * |----------|------------|--------------------------|------------------------|--------------------|--------------------|
 * |          | Channel Id | Disc detailed reason     | Configured connections | Expect fatal error | Test Result        |
 * | 0        | 0          | 0                        | 1                      | False              | Normal operation   |
 * | 1        | 0          | 65535                    | 1                      | False              | Normal operation   |
 * | 2        | 0          | 6200                     | 2                      | False              | Normal operation   |
 * | 3        | 1          | 6200                     | 2                      | False              | Normal operation   |
 * | 4        | 1          | 6200                     | 1                      | True               | Fatal error raised |
 * | 5        | 2          | 6200                     | 2                      | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-571} Init sr_core Function
 * @verifyReq{RASW-589} Set Disconnection Detailed Reason Function
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-530} Get Timer Granularity Function
 * @verifyReq{RASW-420} Get Timer Granularity Function Structure
 */
TEST_P(SetDiscDetailedReasonTestWithParam, srcorTest029VerifySetDiscDetailedReasonFunction)
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

  // execute the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srcorTest::invalidArgumentException));
    EXPECT_THROW(srcor_SetDiscDetailedReason(connection_id, disc_detailed_reason), std::invalid_argument);
  }
  else
  {
    EXPECT_NO_THROW(srcor_SetDiscDetailedReason(connection_id, disc_detailed_reason));

    // verify disc detailed reason set correctly
    EXPECT_EQ(disc_detailed_reason, srcor_rasta_connections[connection_id].detailed_disconnect_reason);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySetDiscDetailedReasonFunction,
  SetDiscDetailedReasonTestWithParam,
  ::testing::Values(
    // vary disc detailed reason
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, 0, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MIN, UINT16_MAX, false),
    // vary configured connections and connection id
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MIN, srtyp_kSrMessageConnReq, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, UT_SRCOR_CONNECTION_ID_MAX, srtyp_kSrMessageConnReq, false),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MIN, UT_SRCOR_CONNECTION_ID_MAX, srtyp_kSrMessageConnReq, true),
    std::make_tuple(UT_SRCOR_CONNECTION_NUM_MAX, (UT_SRCOR_CONNECTION_ID_MAX + 1), srtyp_kSrMessageConnReq, true)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

