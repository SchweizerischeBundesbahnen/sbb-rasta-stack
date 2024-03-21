/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redcor.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 0b41015832ec9608e983cf7fd23824621e67f895
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4969: Added channel id check before test setup,12.12.2022,N.Fricker}
 *
 * @brief Unit test file for the core module of the redundancy layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_redcor.hh"

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
// Test class definitons
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized Init function test
 *
 * Parameter order:
 * - Config valid (bool)
 * - Configured check code type (redcty_CheckCodeType)
 * - Time to buffer seq messages (uint32_t)
 * - Diagnosis window size (uint32_t)
 * - Defer queue size (uint32_t)
 * - Number of redundancy channels (uint32_t)
 * - Number of transport channels (uint32_t)
 * .
 */
class InitTestWithParam :   public redcorTest,
                            public testing::WithParamInterface<std::tuple<bool, redcty_CheckCodeType, uint32_t, uint32_t, uint32_t ,uint32_t, uint32_t>>
{
  public:
  bool config_valid                                                 = std::get<0>(GetParam());        ///< config valid
  redcty_CheckCodeType check_code_type                              = std::get<1>(GetParam());        ///< configured check code type
  uint32_t t_seq                                                    = std::get<2>(GetParam());        ///< time for out of sequence message buffering [ms]
  uint32_t n_diagnosis                                              = std::get<3>(GetParam());        ///< diagnosis window size [messages]
  uint32_t n_defer_queue_size                                       = std::get<4>(GetParam());        ///< size of defer queue [messages]
  uint32_t number_of_redundancy_channels                            = std::get<5>(GetParam());        ///< number of configured redundancy channels
  uint32_t number_of_transport_channels                             = std::get<6>(GetParam());        ///< number of configured transport channels per redundancy channel
};

/**
 * @brief Test class for value-parametrized IsConfigurationValid function test
 *
 * Parameter order:
 * - Expect configuration to be valid (bool)
 * .
 */
class IsConfigurationValidTestWithParam :   public redcorTest,
                                            public testing::WithParamInterface<std::tuple<bool>>
{
  public:
  bool expect_valid = std::get<0>(GetParam());        ///< expect if configuration should be valid
};

/**
 * @brief Test class for value-parametrized InitRedundancyChannelData function test
 *
 * Parameter order:
 * - Number of configured redundancy channels (uint32_t)
 * - Redundancy Channel id to init (uint32_t)
 * - Expect fatal error (bool)
 * .
 */
class InitRedundancyChannelDataTestWithParam:   public redcorTest,
                                                public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>>
{
  public:
  uint32_t number_of_redundancy_channels           = std::get<0>(GetParam());        ///< number of configured redundancy channels
  uint32_t channel_id                              = std::get<1>(GetParam());        ///< Channel id to init
  bool expect_fatal_error                          = std::get<2>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized DeferQueueTimeout function test
 *
 * Parameter order:
 * - Redundancy Channel id (uint32_t)
 * - Message in defer queue (bool)
 * - Expect fatal error (bool)
 * .
 */
class DeferQueueTimeoutTestWithParam :    public redcorTest,
                                          public testing::WithParamInterface<std::tuple<uint32_t, bool, bool>>
{
  public:
  uint32_t channel_id                              = std::get<0>(GetParam());        ///< Redundancy channel id
  bool message_in_queue                            = std::get<1>(GetParam());        ///< indicates if a message is already in queue
  bool expect_fatal_error                          = std::get<2>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized GetAssociatedRedundancyChannel function test
 *
 * Parameter order:
 * - Transport channel id (uint32_t)
 * - Expected Redundancy Channel id (uint32_t)
 * - Expect fatal error (bool)
 * .
 */
class GetAssociatedRedundancyChannelTestWithParam:   public redcorTest,
                                                public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>>
{
  public:
  uint32_t transport_channel_id               = std::get<0>(GetParam());        ///< transport channel id
  uint32_t expected_redundancy_channel_id     = std::get<1>(GetParam());        ///< expected associated redundancy channel id
  bool expect_fatal_error                     = std::get<2>(GetParam());        ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup red_core
 * @{
 */

/**
 * @test        @ID{redcorTest001} Verify the init function.
 *
 * @details     This test verifies that the init function sets the configuration correctly
 *
 * Test steps:
 * - Create config with given parameter
 * - Expect call for diagnostics configuration validation and return given test parameter
 * - If no error expected
 *   - Expect Initialization of the diagnostics and message module
 *   - Expect Initialization of defer queue, buffer and channel diagnostics module for each redundancy channel
 *   - Init module
 *   - Verify if configuration set correct
 * - If config not valid expect radef_kInvalidConfiguration on init call
 * .
 *
 * @testParameter
 * - Check code type: Type of check code
 * - T seq: Time for out of sequence message buffering in ms
 * - N diagnosis: Diagnosis window size
 * - Defer queue size: Size of defer queue
 * - Redundancy channels: Number of configured redundancy channels
 * - Transport channels: Number of configured transport channels per redundancy channel
 * - Config valid: Indicates if given config is expected to be valid
 * .
 * | Test Run | Input parameter                                                                                   |||||| Test config  | Expected values    |
 * |----------|--------------------|-------|-------------|------------------|---------------------|--------------------|--------------|--------------------|
 * |          | Check code type    | T seq | N diagnosis | Defer queue size | Redundancy channels | Transport channels | Config valid | Test Result        |
 * |        0 | redcty_kCheckCodeA |   50U |         10U |               4U |                  1U |                 1U | true         | Normal Operation   |
 * |        1 | redcty_kCheckCodeE |  500U |       1000U |              10U |                  2U |                 2U | true         | Normal Operation   |
 * |        2 | redcty_kCheckCodeA |   50U |         10U |               4U |                  1U |                 1U | false        | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitTestWithParam, redcorTest001VerifyInit)
{
  redcty_RedundancyLayerConfiguration  config = CreateConfig(check_code_type, t_seq, n_diagnosis, n_defer_queue_size,number_of_redundancy_channels, 0, number_of_transport_channels, 0);

  // return configuration validation call according test parameter
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(config_valid));

  if(config_valid)
  {
    EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
    EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

    // expect calls for init of each redundancy channel
    EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(number_of_redundancy_channels);
    EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(number_of_redundancy_channels);
    EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(number_of_redundancy_channels);
  }

  if(!config_valid)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidConfiguration)).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));

    // call the function
    EXPECT_THROW(redcor_Init(&config), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    // call the function
    EXPECT_NO_THROW(redcor_Init(&config));
    EXPECT_TRUE(redcor_initialized);

    // verify configuration set correct
    EXPECT_EQ(redcor_redundancy_configuration->check_code_type, config.check_code_type);
    EXPECT_EQ(redcor_redundancy_configuration->n_defer_queue_size, config.n_defer_queue_size);
    EXPECT_EQ(redcor_redundancy_configuration->n_diagnosis, config.n_diagnosis);
    EXPECT_EQ(redcor_redundancy_configuration->number_of_redundancy_channels, config.number_of_redundancy_channels);
    EXPECT_EQ(redcor_redundancy_configuration->t_seq, config.t_seq);

    // verify each redundancy channel config
    for(uint16_t i = 0; i < number_of_redundancy_channels; i++)
    {
      EXPECT_EQ(redcor_redundancy_configuration->redundancy_channel_configurations[i].num_transport_channels, config.redundancy_channel_configurations[i].num_transport_channels);
      EXPECT_EQ(redcor_redundancy_configuration->redundancy_channel_configurations[i].red_channel_id, config.redundancy_channel_configurations[i].red_channel_id);

      // verify transport channels
      for(uint16_t t = 0; t < number_of_transport_channels; t++)
      {
        EXPECT_EQ(redcor_redundancy_configuration->redundancy_channel_configurations[i].transport_channel_ids[t], config.redundancy_channel_configurations[i].transport_channel_ids[t]);
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInit,
  InitTestWithParam,
  ::testing::Values(
    std::make_tuple(true, redcty_kCheckCodeA, UT_REDCOR_T_SEQ_MIN, UT_REDCOR_N_DIAGNOSIS_MIN, UT_REDCOR_QUEUE_SIZE_MIN, UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDCOR_TRANSPORT_CHANNEL_NUM_MIN),
    std::make_tuple(true, redcty_kCheckCodeE, UT_REDCOR_T_SEQ_MAX, UT_REDCOR_N_DIAGNOSIS_MAX, UT_REDCOR_QUEUE_SIZE_MAX, UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDCOR_TRANSPORT_CHANNEL_NUM_MAX),
    std::make_tuple(false, redcty_kCheckCodeA, UT_REDCOR_T_SEQ_MIN, UT_REDCOR_N_DIAGNOSIS_MIN, UT_REDCOR_QUEUE_SIZE_MIN, UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDCOR_TRANSPORT_CHANNEL_NUM_MIN)
  )
);

/**
 * @test        @ID{redcorTest002} Verify the number of init function calls.
 *
 * @details     This test verifies that the init function can only be called once.
 *
 * Test steps:
 * - call the init function and verify that it completes successfully
 * - call the init function again and verify that a radef_kAlreadyInitialized error is raised
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redcorTest, redcorTest002VerifyInitFunctionCalls)
{
  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));

  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);

  // call for every redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(UT_REDCOR_TRANSPORT_CHANNEL_NUM_MAX);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(UT_REDCOR_TRANSPORT_CHANNEL_NUM_MAX);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(UT_REDCOR_TRANSPORT_CHANNEL_NUM_MAX);

  // call the init function and verify that it completes successfully
  EXPECT_NO_THROW(redcor_Init(&default_config));

  // call the init function again and verify that an assertion violation is raised as the module is already initialized
  EXPECT_THROW(redcor_Init(&default_config), std::invalid_argument);
}

/**
 * @test        @ID{redcorTest003} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - init module
 * - call the Init function with invalid configuration pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the IsConfigurationValid function with invalid configuration pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the WriteReceivedMessageToInputBuffer function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the WriteMessagePayloadToSendBuffer function with invalid payload pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the GetAssociatedRedundancyChannel function with invalid payload pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redcorTest, redcorTest003VerifyNullPointerCheck)
{
  const uint8_t nbr_of_function_calls = 5U;        // number of function calls

  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).Times(nbr_of_function_calls)
    .WillRepeatedly(Invoke(this, &redcorTest::invalidArgumentException));

  // expect calls for init
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);

  // call for every redundancy channel at init
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(2);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(2);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(2);

  // expect call for transport channel id validation on WriteReceivedMessageToInputBuffer call
  EXPECT_CALL(reddia_mock, reddia_IsTransportChannelIdValid(_, _)).WillOnce(Return(true));

  // perform nullpointer test for init function
  EXPECT_THROW(redcor_Init((redcty_RedundancyLayerConfiguration *)NULL), std::invalid_argument);

  // init module
  redcor_Init(&default_config);

  // perform tests
  EXPECT_THROW(redcor_IsConfigurationValid((redcty_RedundancyLayerConfiguration *)NULL), std::invalid_argument);
  EXPECT_THROW(redcor_WriteReceivedMessageToInputBuffer(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX, (redtyp_RedundancyMessage *)NULL), std::invalid_argument);
  EXPECT_THROW(redcor_WriteMessagePayloadToSendBuffer(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, UT_REDCOR_DATA_LEN_MAX, (uint8_t *)NULL), std::invalid_argument);
  EXPECT_THROW(redcor_GetAssociatedRedundancyChannel(UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX, (uint32_t *)NULL), std::invalid_argument);
}

/**
 * @test        @ID{redcorTest004} Initialization checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the InitRedundancyChannelData function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the DeferQueueTimeout function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the WriteReceivedMessageToInputBuffer function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the ClearInputBufferMessagePendingFlag function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the ProcessReceivedMessage function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SetMessagePendingFlag function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetMessagePendingFlag function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the ClearMessagePendingFlag function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the WriteMessagePayloadToSendBuffer function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the ClearSendBufferMessagePendingFlag function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the SendMessage function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetAssociatedRedundancyChannel function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 */
TEST_F(redcorTest, redcorTest004VerifyInitializationCheck)
{
  uint8_t nbr_of_function_calls             = 12;     // number of function calls

  // Test variables
  uint32_t redundancy_channel_id            = 0;
  uint32_t transport_channel_id             = 0;
  uint32_t associated_redundancy_channel_id = 0;
  redtyp_RedundancyMessage message          = {0, {}};
  uint16_t payload_size                     = 0;
  uint8_t payload                           = 0;

  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
    .Times(nbr_of_function_calls)
    .WillRepeatedly(Invoke(this, &redcorTest::invalidArgumentException));

  // call the functions without initializing the module
  EXPECT_THROW(redcor_InitRedundancyChannelData(redundancy_channel_id), std::invalid_argument);
  EXPECT_THROW(redcor_DeferQueueTimeout(redundancy_channel_id), std::invalid_argument);
  EXPECT_THROW(redcor_WriteReceivedMessageToInputBuffer(redundancy_channel_id, transport_channel_id, &message), std::invalid_argument);
  EXPECT_THROW(redcor_ClearInputBufferMessagePendingFlag(redundancy_channel_id), std::invalid_argument);
  EXPECT_THROW(redcor_ProcessReceivedMessage(redundancy_channel_id), std::invalid_argument);
  EXPECT_THROW(redcor_SetMessagePendingFlag(redundancy_channel_id, transport_channel_id), std::invalid_argument);
  EXPECT_THROW(redcor_GetMessagePendingFlag(redundancy_channel_id, transport_channel_id), std::invalid_argument);
  EXPECT_THROW(redcor_ClearMessagePendingFlag(redundancy_channel_id, transport_channel_id), std::invalid_argument);
  EXPECT_THROW(redcor_WriteMessagePayloadToSendBuffer(redundancy_channel_id, payload_size, &payload), std::invalid_argument);
  EXPECT_THROW(redcor_ClearSendBufferMessagePendingFlag(redundancy_channel_id), std::invalid_argument);
  EXPECT_THROW(redcor_SendMessage(redundancy_channel_id), std::invalid_argument);
  EXPECT_THROW(redcor_GetAssociatedRedundancyChannel(transport_channel_id, &associated_redundancy_channel_id), std::invalid_argument);
}

/**
 * @test        @ID{redcorTest005} Verify the IsConfigurationValid function.
 *
 * @details     This test verifies that the IsConfigurationValid function
 *
 * Test steps:
 * - Call isConfigurationValid Function and compare with value which gets returned by diagnostics module call
 * .
 *
 * @testParameter
 * - Expect config valid: The return value of the reddia_IsConfigurationValid call
 * .
 * | Test Run | Expected values                       ||
 * |----------|---------------------|------------------|
 * |          | Expect config valid | Test Result      |
 * |        0 | true                | Normal Operation |
 * |        1 | false               | Normal Operation |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(IsConfigurationValidTestWithParam, redcorTest005VerifyIsConfigurationValid)
{
  // set expectations
  EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(&default_config)).WillOnce(Return(expect_valid));

  // call the function and verify the output
  EXPECT_EQ(expect_valid, redcor_IsConfigurationValid(&default_config));
}

INSTANTIATE_TEST_SUITE_P(
  verifyIsConfigurationValid,
  IsConfigurationValidTestWithParam,
  ::testing::Values(
    true,
    false
  )
);

/**
 * @test        @ID{redcorTest006} Verify the InitRedundancyChannelData function.
 *
 * @details     This test verifies the correct behaviour of the InitRedundancyChannelData function.
 *
 * Test steps:
 * - Init module with given parameter
 * - Add random values to redundancy channel config and buffers
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call InitRedundancyChannel function
 * - verify values of redundancy channel config and buffers are reset
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Conf redundancy channels: Amount of configured redundancy channels
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter       | Test config              | Expected values                  ||
 * |----------|-----------------------|--------------------------|--------------|--------------------|
 * |          | Redundancy channel id | Conf redundancy channels | Expect Fatal | Test Result        |
 * | 0        | 0U                    | 1U                       | false        | Normal operation   |
 * | 1        | 1U                    | 1U                       | true         | Fatal error raised |
 * | 2        | 0U                    | 2U                       | false        | Normal operation   |
 * | 3        | 1U                    | 2U                       | false        | Normal operation   |
 * | 4        | 2U                    | 2U                       | true         | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitRedundancyChannelDataTestWithParam, redcorTest006VerifyInitRedundancyChannelData)
{
  redcty_RedundancyLayerConfiguration config = default_config;
  config.number_of_redundancy_channels = number_of_redundancy_channels;

  // expect calls for init module
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  if(expect_fatal_error)
  {
    // expect calls only for init module
    EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(number_of_redundancy_channels);
    EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(number_of_redundancy_channels);
    EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(number_of_redundancy_channels);
  }
  else
  {
    // expect calls for init module and init of each redundancy channel
    EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(number_of_redundancy_channels + 1);
    EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(number_of_redundancy_channels + 1);
    EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(number_of_redundancy_channels + 1);
  }

  // init the module
  redcor_Init(&config);

  if(channel_id <= UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX)
  {
    // set values to verify reset after InitRedundancyChannelData
    redcor_redundancy_channels[channel_id].seq_rx = 10U;
    redcor_redundancy_channels[channel_id].seq_tx = 20U;

    for (uint32_t tr_channel_index = 0U; tr_channel_index < config.number_of_redundancy_channels * config.redundancy_channel_configurations->num_transport_channels; ++tr_channel_index) {
      redcor_redundancy_channels[channel_id].received_data_pending[tr_channel_index] = true;
    }

    redcor_redundancy_channels[channel_id].input_buffer.message_in_buffer = true;
    redcor_redundancy_channels[channel_id].input_buffer.message_buffer.message_size = 1000U;
    redcor_redundancy_channels[channel_id].input_buffer.transport_channel_id = 2U;
    redcor_redundancy_channels[channel_id].send_buffer.message_in_buffer = true;
    redcor_redundancy_channels[channel_id].send_buffer.message_payload_buffer.payload_size = 900U;
  }

  if(expect_fatal_error)
  {
    // expect fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));

    // call the function and expect throw
    EXPECT_THROW(redcor_InitRedundancyChannelData(channel_id), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    // perform test
    EXPECT_NO_THROW(redcor_InitRedundancyChannelData(channel_id));

    // verify redundancy channel is reset
    EXPECT_EQ(redcor_redundancy_channels[channel_id].seq_rx, 0U);
    EXPECT_EQ(redcor_redundancy_channels[channel_id].seq_tx, 0U);

    // verify transport channel does not contain pending data
    for (uint32_t tr_channel_index = 0U; tr_channel_index < config.number_of_redundancy_channels * config.redundancy_channel_configurations->num_transport_channels; ++tr_channel_index) {
      EXPECT_EQ(redcor_redundancy_channels[channel_id].received_data_pending[tr_channel_index], false);
    }

    // verify input buffer of redundancy channel is reset
    EXPECT_EQ(redcor_redundancy_channels[channel_id].input_buffer.message_in_buffer, false);
    EXPECT_EQ(redcor_redundancy_channels[channel_id].input_buffer.message_buffer.message_size, 0U);
    EXPECT_EQ(redcor_redundancy_channels[channel_id].input_buffer.transport_channel_id, 0U);

    // verify send buffer of redundancy channel is reset
    EXPECT_EQ(redcor_redundancy_channels[channel_id].send_buffer.message_in_buffer, false);
    EXPECT_EQ(redcor_redundancy_channels[channel_id].send_buffer.message_payload_buffer.payload_size, 0U);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitRedundancyChannelData,
  InitRedundancyChannelDataTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, true),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_NUM_MAX, (UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), true)
  )
);

/**
 * @test        @ID{redcorTest007} Verify the DeferQueueTimeout function.
 *
 * @details     This test verifies the correct behaviour of the DeferQueueTimeout function.
 *
 * Test steps:
 * - Init module with default configuration
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call DeferQueueTimeout function
 * - Verify expected function calls when message(s) in queue:
 *   - Contains function
 *   - GetMessage
 *   - GetMessagePayload
 *   - AddToBuffer
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Message in defer queue: Indicates if a message is in queue
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter       | Test config            | Expected values                  ||
 * |----------|-----------------------|------------------------|--------------|--------------------|
 * |          | Redundancy channel id | Message in defer queue | Expect Fatal | Test Result        |
 * | 0        | 0U                    | false                  | false        | Normal operation   |
 * | 1        | 0U                    | true                   | false        | Normal operation   |
 * | 2        | 1U                    | false                  | false        | Normal operation   |
 * | 3        | 1U                    | true                   | false        | Normal operation   |
 * | 4        | 2U                    | false                  | true         | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(DeferQueueTimeoutTestWithParam, redcorTest007VerifyDeferQueueTimeout)
{
  // expect calls for init module
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  // expect calls for init of each redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(default_config.number_of_redundancy_channels);

  // init the module
  redcor_Init(&default_config);

  // perform the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));
    EXPECT_THROW(redcor_DeferQueueTimeout(channel_id), std::invalid_argument);
  }
  else
  {
    // expect calls for GetOldestSequenceNumber function
    EXPECT_CALL(reddfq_mock, reddfq_GetOldestSequenceNumber(_)).Times(1);

    // set expected actions if messages in queue
    if(message_in_queue)
    {
      EXPECT_CALL(reddfq_mock, reddfq_Contains(channel_id, _)).Times(2).WillOnce(Return(true)).WillOnce(Return(false));

      EXPECT_CALL(reddfq_mock, reddfq_GetMessage(channel_id, _, _)).Times(1);
      EXPECT_CALL(redmsg_mock, redmsg_GetMessagePayload(_, _)).Times(1);
      EXPECT_CALL(redrbf_mock, redrbf_AddToBuffer(channel_id, _)).Times(1);;
    }
    else
    {
      EXPECT_CALL(reddfq_mock, reddfq_Contains(channel_id, _)).Times(1).WillOnce(Return(false));
    }

    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcor_DeferQueueTimeout(channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyDeferQueueTimeout,
  DeferQueueTimeoutTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, true, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, false, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, true, false),
    std::make_tuple((UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), false, true)
  )
);

/**
 * @test        @ID{redcorTest017} Verify the GetAssociatedRedundancyChannel function.
 *
 * @details     This test verifies the correct behaviour of the GetAssociatedRedundancyChannel function.
 *
 * Test steps:
 * - Init the module with default configuration
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call GetAssociatedRedundancyChannel function for given transport channel id
 * - Verify the associated redundancy channel
 * .
 *
 * @testParameter
 * - Transport channel id: Transport channel identification
 * - Expected redundancy channel id: Expected associated redundancy channel identification
 * - Expect fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter       | Expected values                                                   |||
 * |----------|-----------------------|--------------------------------|---------------|--------------------|
 * |          | Transport channel id  | Expected redundancy channel id | Expect fatal  | Test Result        |
 * | 0        | 0U                    | 0U                             | false         | Normal operation   |
 * | 1        | 1U                    | 0U                             | false         | Normal operation   |
 * | 2        | 2U                    | 1U                             | false         | Normal operation   |
 * | 3        | 3U                    | 1U                             | false         | Normal operation   |
 * | 4        | 4U                    | 0U                             | true          | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(GetAssociatedRedundancyChannelTestWithParam, redcorTest017VerifyGetAssociatedRedundancyChannel)
{
  // test variables
  uint32_t redundancy_channel_id_result = 0;

  // expect calls for init module
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  // expect calls for init of each redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(default_config.number_of_redundancy_channels);

  // init the module
  redcor_Init(&default_config);

  // perform the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));
    EXPECT_THROW(redcor_GetAssociatedRedundancyChannel(transport_channel_id, &redundancy_channel_id_result), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcor_GetAssociatedRedundancyChannel(transport_channel_id, &redundancy_channel_id_result));
    EXPECT_EQ(expected_redundancy_channel_id, redundancy_channel_id_result);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetAssociatedRedundancyChannel,
  GetAssociatedRedundancyChannelTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN, UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, false),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX, UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX + 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, true)
  )
);

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
