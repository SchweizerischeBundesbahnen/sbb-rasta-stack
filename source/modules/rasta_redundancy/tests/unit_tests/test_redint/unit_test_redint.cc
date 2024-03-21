/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redint.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the redundancy layer interface.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_redint.hh"

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
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - Use valid config (bool)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class InitTestWithParam :   public redintTest,
                            public testing::WithParamInterface<std::tuple<bool, radef_RaStaReturnCode>>
{
  public:
  bool use_valid_config                       = std::get<0>(GetParam());        ///< indicates if a valid config is used
  radef_RaStaReturnCode expected_return_code  = std::get<1>(GetParam());        ///< expected return code
};

/**
 * @brief Test class for GetInitializationState test
 *
 * Parameter order:
 * - Initialize Module (bool)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class GetInitializationStateTestWithParam : public redintTest,
                                            public testing::WithParamInterface<std::tuple<bool, radef_RaStaReturnCode>>
{
  public:
  bool initialize                             = std::get<0>(GetParam());        ///< initialize module
  radef_RaStaReturnCode expected_return_code  = std::get<1>(GetParam());        ///< expected return code
};

/**
 * @brief Test class for OpenRedundancyChannel test
 *
 * Parameter order:
 * - Amount of configured redundancy channels (uint32_t)
 * - Redundancy channel ID (uint32_t)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class OpenRedundancyChannelTestWithParam :  public redintTest,
                                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_redundancy_channels     = std::get<0>(GetParam());        ///< amount of redundancy channels
  uint32_t redundancy_channel_id              = std::get<1>(GetParam());        ///< redundancy channel id
  radef_RaStaReturnCode expected_return_code  = std::get<2>(GetParam());        ///< expected return code
};

/**
 * @brief Test class for OpenRedundancyChannel test which inherits from OpenRedundancyChannelTestWithParam due to same parameters
 */
class CloseRedundancyChannelTestWithParam : public OpenRedundancyChannelTestWithParam
{

};

/**
 * @brief Test class for SendMessage test
 *
 * Parameter order:
 * - Amount of configured redundancy channels (uint32_t)
 * - Redundancy channel ID (uint32_t)
 * - Message size (uint16_t)
 * - Indicates if channel state up (bool)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class SendMessageTestWithParam :  public redintTest,
                                  public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint16_t, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_redundancy_channels       = std::get<0>(GetParam());        ///< amount of redundancy channels
  uint32_t redundancy_channel_id                = std::get<1>(GetParam());        ///< redundancy channel id
  uint16_t message_size                         = std::get<2>(GetParam());        ///< message size
  bool channel_state_up                         = std::get<3>(GetParam());        ///< indicates if channel state up
  radef_RaStaReturnCode expected_return_code    = std::get<4>(GetParam());        ///< expected return code
};

/**
 * @brief Test class for ReadMessage test
 *
 * Parameter order:
 * - Amount of configured redundancy channels (uint32_t)
 * - Redundancy channel ID (uint32_t)
 * - Buffer size (uint16_t)
 * - Buffer read return code (radef_RaStaReturnCode)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class ReadMessageTestWithParam :  public redintTest,
                                  public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint16_t, radef_RaStaReturnCode, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_redundancy_channels           = std::get<0>(GetParam());        ///< amount of redundancy channels
  uint32_t redundancy_channel_id                    = std::get<1>(GetParam());        ///< redundancy channel id
  uint16_t buffer_size                              = std::get<2>(GetParam());        ///< buffer size
  radef_RaStaReturnCode buffer_read_return_code     = std::get<3>(GetParam());        ///< return code of read buffer action
  radef_RaStaReturnCode expected_return_code        = std::get<4>(GetParam());        ///< expected return code
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup red_api
 * @{
 */

/**
 * @test        @ID{redintTest001} Verify the init function.
 *
 * @details     This test verifies the init function
 *
 * Test steps:
 * - set a mock function call expectation for the isConfigurationValid function and return true or false according to test parameter
 * - call the init function and verify the return value
 * .
 *
 * @testParameter
 * - Use valid config: Indicates if redcor_IsConfigurationValid check passes
 * - Expected return code: Expected return code from redint_Init function call
 * .
 * | Test Run | Input parameter  | Expected values                                 ||
 * |----------|------------------|-----------------------------|--------------------|
 * |          | Use valid config | Expected return code        | Test Result        |
 * | 0        | true             | radef_kNoError              | Normal operation   |
 * | 1        | false            | radef_kInvalidConfiguration | Normal operation   |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitTestWithParam, redintTest001VerifyInitFunction)
{
  // Set expectations
  if(use_valid_config)
  {
    EXPECT_CALL(redcor_mock, redcor_IsConfigurationValid(&default_config)).WillOnce(Return(true));
    EXPECT_CALL(redcor_mock, redcor_Init(&default_config)).Times(1);
    EXPECT_CALL(redstm_mock, redstm_Init(default_config.number_of_redundancy_channels)).Times(1);
    EXPECT_CALL(redrbf_mock, redrbf_Init(default_config.number_of_redundancy_channels)).Times(1);
    EXPECT_CALL(reddfq_mock, reddfq_Init(default_config.number_of_redundancy_channels, default_config.n_defer_queue_size, default_config.t_seq)).Times(1);
  }
  else
  {
    // Configuration gets checked before initializing other modules
    EXPECT_CALL(redcor_mock, redcor_IsConfigurationValid(&default_config)).WillOnce(Return(false));
  }

  // perform test
  EXPECT_EQ(expected_return_code, redint_Init(&default_config));
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitFunction,
  InitTestWithParam,
  ::testing::Values(
    std::make_tuple(true, radef_kNoError),
    std::make_tuple(false, radef_kInvalidConfiguration)
  )
);

/**
 * @test        @ID{redintTest002} Verify the number of init function calls.
 *
 * @details     This test verifies that the init function can only be called once.
 *
 * Test steps:
 * - call the init function and verify that it completes successfully
 * - call the init function again and verify radef_kAlreadyInitialized gets returned
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redintTest, redintTest002VerifyInitFunctionCalls)
{
  // set expectations for first init call
  EXPECT_CALL(redcor_mock, redcor_IsConfigurationValid(&default_config)).WillOnce(Return(true));
  EXPECT_CALL(redcor_mock, redcor_Init(&default_config)).Times(1);
  EXPECT_CALL(redstm_mock, redstm_Init(default_config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(redrbf_mock, redrbf_Init(default_config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(reddfq_mock, reddfq_Init(default_config.number_of_redundancy_channels, default_config.n_defer_queue_size, default_config.t_seq)).Times(1);

  // call the init function and verify no error return code
  EXPECT_EQ(radef_kNoError, redint_Init(&default_config));

  // call the init function again and verify already initialized return code
  EXPECT_EQ(radef_kAlreadyInitialized, redint_Init(&default_config));
}

/**
 * @test        @ID{redintTest003} Null pointer checks
 *
 * @details     This test verifies that the radef_kNotInitialized code is returned whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - init module with default config
 * - call the redint_SendMessage with invalid message data pointer and verify return code
 * - call the redint_ReadMessage with invalid message size pointer and verify return code
 * - call the redint_ReadMessage with invalid message buffer pointer and verify return code
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redintTest, redintTest003VerifyNullPointerCheck)
{
  // test variables
  uint32_t redundancy_channel_id    = 0;
  uint16_t message_size             = UT_REDINT_MSG_LEN_MAX;
  uint16_t buffer_size              = UT_REDINT_MSG_BUFFER_SIZE_MAX;
  uint8_t message_buffer            = {};

  // perform test for init function
  EXPECT_EQ(radef_kInvalidParameter, redint_Init((redcty_RedundancyLayerConfiguration *)NULL));

  // set expectations for init call
  EXPECT_CALL(redcor_mock, redcor_IsConfigurationValid(&default_config)).WillOnce(Return(true));
  EXPECT_CALL(redcor_mock, redcor_Init(&default_config)).Times(1);
  EXPECT_CALL(redstm_mock, redstm_Init(default_config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(redrbf_mock, redrbf_Init(default_config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(reddfq_mock, reddfq_Init(default_config.number_of_redundancy_channels, default_config.n_defer_queue_size, default_config.t_seq)).Times(1);

  // init module
  redint_Init(&default_config);

  // expect state check before parameter check of sendmessage
  EXPECT_CALL(redstm_mock, redstm_GetChannelState(redundancy_channel_id)).Times(1).WillOnce(Return(redstm_kRedundancyChannelStateUp));

  // perform rest of the tests
  EXPECT_EQ(radef_kInvalidParameter, redint_SendMessage(redundancy_channel_id, message_size, (uint8_t *)NULL));
  EXPECT_EQ(radef_kInvalidParameter, redint_ReadMessage(redundancy_channel_id, buffer_size, (uint16_t *)NULL, &message_buffer));
  EXPECT_EQ(radef_kInvalidParameter, redint_ReadMessage(redundancy_channel_id, buffer_size, &message_size, (uint8_t *)NULL));
}

/**
 * @test        @ID{redintTest004} Initialization checks
 *
 * @details     This test verifies that the radef_kNotInitialized code is returned whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the redint_GetInitializationState function without prior initialization and verify return code
 * - call the redint_OpenRedundancyChannel function without prior initialization and verify return code
 * - call the redint_CloseRedundancyChannel function without prior initialization and verify return code
 * - call the redint_SendMessage function without prior initialization and verify return code
 * - call the redint_ReadMessage function without prior initialization and verify return code
 * - call the redint_CheckTimings function without prior initialization and verify return code
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redintTest, redintTest004VerifyInitializationCheck)
{
  // test variables
  uint32_t redundancy_channel_id    = 0;
  uint16_t message_size             = 0;
  uint16_t buffer_size              = 0;
  uint8_t message_buffer            = {};

  // perform test
  EXPECT_EQ(radef_kNotInitialized, redint_GetInitializationState());
  EXPECT_EQ(radef_kNotInitialized, redint_OpenRedundancyChannel(redundancy_channel_id));
  EXPECT_EQ(radef_kNotInitialized, redint_CloseRedundancyChannel(redundancy_channel_id));
  EXPECT_EQ(radef_kNotInitialized, redint_SendMessage(redundancy_channel_id, message_size, &message_buffer));
  EXPECT_EQ(radef_kNotInitialized, redint_ReadMessage(redundancy_channel_id, buffer_size, &message_size, &message_buffer));
  EXPECT_EQ(radef_kNotInitialized, redint_CheckTimings());
}

/**
 * @test        @ID{redintTest005} Verify the GetInitializationState function.
 *
 * @details     This test verifies the GetInitializationState function
 *
 * Test steps:
 * - init module with default config if test parameter is set to true
 * - call GetInitializationState and verify return code
 * .
 *
 * @testParameter
 * - Init module: Indicates if module gets initialized
 * - Expected return code: Expected return code from redint_GetInitializationState function call
 * .
 * | Test Run | Test config | Expected values                           ||
 * |----------|-------------|-----------------------|--------------------|
 * |          | Init module | Expected return code  | Test Result        |
 * | 0        | true        | radef_kNoError        | Normal operation   |
 * | 1        | false       | radef_kNotInitialized | Normal operation   |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(GetInitializationStateTestWithParam, redintTest005VerifyGetInitializationState)
{
  if(initialize)
  {
    // set init call expectations
    EXPECT_CALL(redcor_mock, redcor_IsConfigurationValid(&default_config)).WillOnce(Return(true));
    EXPECT_CALL(redcor_mock, redcor_Init(&default_config)).Times(1);
    EXPECT_CALL(redstm_mock, redstm_Init(default_config.number_of_redundancy_channels)).Times(1);
    EXPECT_CALL(redrbf_mock, redrbf_Init(default_config.number_of_redundancy_channels)).Times(1);
    EXPECT_CALL(reddfq_mock, reddfq_Init(default_config.number_of_redundancy_channels, default_config.n_defer_queue_size, default_config.t_seq)).Times(1);

    // init module
    redint_Init(&default_config);
  }

  // perform test
  EXPECT_EQ(expected_return_code, redint_GetInitializationState());
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetInitializationState,
  GetInitializationStateTestWithParam,
  ::testing::Values(
    std::make_tuple(true, radef_kNoError),
    std::make_tuple(false, radef_kNotInitialized)
  )
);

/**
 * @test        @ID{redintTest006} Verify the OpenRedundancyChannel function.
 *
 * @details     This test verifies the OpenRedundancyChannel function
 *
 * Test steps:
 * - call the init function with the number of redundancy channels given as test parameter
 * - set expectations for state machine when channel gets opened
 * - call the OpenRedundancyChannel and verify return code
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Configured redundancy channels: Number of configured redundancy channels
 * - Expected return code: Expected return code from redint_OpenRedundancyChannel function call
 * .
 * | Test Run | Input parameter       | Test config                    | Expected values                           ||
 * |----------|-----------------------|--------------------------------|-------------------------|------------------|
 * |          | Redundancy channel id | Configured redundancy channels | Expected return code    | Test Result      |
 * | 0        | 0U                    | 1U                             | radef_kNoError          | Normal operation |
 * | 1        | 1U                    | 1U                             | radef_kInvalidParameter | Normal operation |
 * | 2        | 0U                    | 2U                             | radef_kNoError          | Normal operation |
 * | 3        | 1U                    | 2U                             | radef_kNoError          | Normal operation |
 * | 4        | 2U                    | 2U                             | radef_kInvalidParameter | Normal operation |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(OpenRedundancyChannelTestWithParam, redintTest006VerifyOpenRedundancyChannel)
{
  // test variables
  redcty_RedundancyLayerConfiguration config = CreateConfig(
    default_config.check_code_type,
    default_config.t_seq,
    default_config.n_diagnosis,
    default_config.n_defer_queue_size,
    configured_redundancy_channels,         // given parameter
    UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN,
    UT_REDINT_TRANSPORT_CHANNEL_NUM_MAX,
    UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN
  );

  // Set init call expectations
  EXPECT_CALL(redcor_mock, redcor_IsConfigurationValid(&config)).WillOnce(Return(true));
  EXPECT_CALL(redcor_mock, redcor_Init(&config)).Times(1);
  EXPECT_CALL(redstm_mock, redstm_Init(config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(redrbf_mock, redrbf_Init(config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(reddfq_mock, reddfq_Init(config.number_of_redundancy_channels, config.n_defer_queue_size, config.t_seq)).Times(1);

  if(expected_return_code == radef_kNoError)
  {
    EXPECT_CALL(redstm_mock, redstm_ProcessChannelStateMachine(redundancy_channel_id, redstm_kRedundancyChannelEventOpen)).Times(1);
  }

  // init module
  redint_Init(&config);

  // perform test
  EXPECT_EQ(expected_return_code, redint_OpenRedundancyChannel(redundancy_channel_id));
}

INSTANTIATE_TEST_SUITE_P(
  verifyOpenRedundancyChannel,
  OpenRedundancyChannelTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, (UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN + 1), radef_kInvalidParameter),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, (UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX + 1), radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{redintTest007} Verify the CloseRedundancyChannel function.
 *
 * @details     This test verifies the CloseRedundancyChannel function
 *
 * Test steps:
 * - call the init function with the number of redundancy channels given as test parameter
 * - open redundancy channel if id valid
 * - call the CloseRedundancyChannel and verify the return code
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Configured redundancy channels: Number of configured redundancy channels
 * - Expected return code: Expected return code from redint_OpenRedundancyChannel function call
 * .
 * | Test Run | Input parameter       | Test config                    | Expected values                           ||
 * |----------|-----------------------|--------------------------------|-------------------------|------------------|
 * |          | Redundancy channel id | Configured redundancy channels | Expected return code    | Test Result      |
 * | 0        | 0U                    | 1U                             | radef_kNoError          | Normal operation |
 * | 1        | 1U                    | 1U                             | radef_kInvalidParameter | Normal operation |
 * | 2        | 0U                    | 2U                             | radef_kNoError          | Normal operation |
 * | 3        | 1U                    | 2U                             | radef_kNoError          | Normal operation |
 * | 4        | 2U                    | 2U                             | radef_kInvalidParameter | Normal operation |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(CloseRedundancyChannelTestWithParam, redintTest007VerifyCloseRedundancyChannel)
{
  // test variables
  redcty_RedundancyLayerConfiguration config = CreateConfig(
    default_config.check_code_type,
    default_config.t_seq,
    default_config.n_diagnosis,
    default_config.n_defer_queue_size,
    configured_redundancy_channels,         // given parameter
    UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN,
    UT_REDINT_TRANSPORT_CHANNEL_NUM_MAX,
    UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN
  );

  // Set init call expectations
  EXPECT_CALL(redcor_mock, redcor_IsConfigurationValid(&config)).WillOnce(Return(true));
  EXPECT_CALL(redcor_mock, redcor_Init(&config)).Times(1);
  EXPECT_CALL(redstm_mock, redstm_Init(config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(redrbf_mock, redrbf_Init(config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(reddfq_mock, reddfq_Init(config.number_of_redundancy_channels, config.n_defer_queue_size, config.t_seq)).Times(1);

  // init module
  redint_Init(&config);

  // open redundancy channel if config valid
  if(expected_return_code == radef_kNoError)
  {
    EXPECT_CALL(redstm_mock, redstm_ProcessChannelStateMachine(redundancy_channel_id, redstm_kRedundancyChannelEventOpen)).Times(1);
    EXPECT_EQ(radef_kNoError, redint_OpenRedundancyChannel(redundancy_channel_id));

    // expect state machine call on closing
    EXPECT_CALL(redstm_mock, redstm_ProcessChannelStateMachine(redundancy_channel_id, redstm_kRedundancyChannelEventClose)).Times(1);
  }

  // perform test
  EXPECT_EQ(expected_return_code, redint_CloseRedundancyChannel(redundancy_channel_id));
}

INSTANTIATE_TEST_SUITE_P(
  verifyCloseRedundancyChannel,
  CloseRedundancyChannelTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, (UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN + 1), radef_kInvalidParameter),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, (UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX + 1), radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{redintTest008} Verify the SendMessage function.
 *
 * @details     This test verifies the SendMessage function
 *
 * Test steps:
 * - call the init function with the number of redundancy channels given as test parameter
 * - expect state machine call to get state and return state up if test parameter is set to true
 * - set core and state machine call expectations when sending message
 * - call the SendMessage function with given test parameter and verify the return code
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Message size: Size of message to send in bytes (valid range: 28 - 1101)
 * - Configured redundancy channels: Number of configured redundancy channels
 * - Is state up: Indicates if state is up (closed if not)
 * - Expected return code: Expected return code from redint_SendMessage function call
 * .
 * | Test Run | Input parameter       | Test config                                               ||| Expected values                                         ||
 * |----------|-----------------------|--------------|--------------------------------|-------------|---------------------------------------|------------------|
 * |          | Redundancy channel id | Message size | Configured redundancy channels | Is state up | Expected return code                  | Test Result      |
 * | 0        | 0U                    | 28U          | 1U                             | true        | radef_kNoError                        | Normal operation |
 * | 1        | 1U                    | 28U          | 1U                             | true        | radef_kInvalidParameter               | Normal operation |
 * | 2        | 0U                    | 1101U        | 2U                             | true        | radef_kNoError                        | Normal operation |
 * | 3        | 1U                    | 1101U        | 2U                             | true        | radef_kNoError                        | Normal operation |
 * | 4        | 2U                    | 1101U        | 2U                             | true        | radef_kInvalidParameter               | Normal operation |
 * | 5        | 1U                    | 1101U        | 2U                             | false       | radef_kInvalidOperationInCurrentState | Normal operation |
 * | 6        | 1U                    | 27U          | 2U                             | true        | radef_kInvalidMessageSize             | Normal operation |
 * | 7        | 1U                    | 1102U        | 2U                             | true        | radef_kInvalidMessageSize             | Normal operation |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(SendMessageTestWithParam, redintTest008VerifySendMessage)
{
  // test variables
  uint8_t message_data = {};
  redcty_RedundancyLayerConfiguration config = CreateConfig(
    default_config.check_code_type,
    default_config.t_seq,
    default_config.n_diagnosis,
    default_config.n_defer_queue_size,
    configured_redundancy_channels,         // given parameter
    UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN,
    UT_REDINT_TRANSPORT_CHANNEL_NUM_MAX,
    UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN
  );

  // Set init call expectations
  EXPECT_CALL(redcor_mock, redcor_IsConfigurationValid(&config)).WillOnce(Return(true));
  EXPECT_CALL(redcor_mock, redcor_Init(&config)).Times(1);
  EXPECT_CALL(redstm_mock, redstm_Init(config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(redrbf_mock, redrbf_Init(config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(reddfq_mock, reddfq_Init(config.number_of_redundancy_channels, config.n_defer_queue_size, config.t_seq)).Times(1);

  // init module
  redint_Init(&config);

  if(channel_state_up)
  {
    EXPECT_CALL(redstm_mock, redstm_GetChannelState(redundancy_channel_id)).WillOnce(Return(redstm_kRedundancyChannelStateUp));
  }
  else
  {
    EXPECT_CALL(redstm_mock, redstm_GetChannelState(redundancy_channel_id)).WillOnce(Return(redstm_kRedundancyChannelStateClosed));
  }

  if(expected_return_code == radef_kNoError)
  {
    EXPECT_CALL(redcor_mock, redcor_WriteMessagePayloadToSendBuffer(redundancy_channel_id, message_size, &message_data));
    EXPECT_CALL(redcor_mock, redcor_ClearSendBufferMessagePendingFlag(redundancy_channel_id));
    EXPECT_CALL(redstm_mock, redstm_ProcessChannelStateMachine(redundancy_channel_id, redstm_kRedundancyChannelEventSendData));
  }

  // perform test
  EXPECT_EQ(expected_return_code, redint_SendMessage(redundancy_channel_id, message_size, &message_data));
}

INSTANTIATE_TEST_SUITE_P(
  verifySendMessage,
  SendMessageTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, UT_REDINT_MSG_LEN_MIN, true, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX, UT_REDINT_MSG_LEN_MIN, true, radef_kInvalidParameter),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, UT_REDINT_MSG_LEN_MAX, true, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX, UT_REDINT_MSG_LEN_MAX, true, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, (UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX + 1), UT_REDINT_MSG_LEN_MAX, true, radef_kInvalidParameter),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX, UT_REDINT_MSG_LEN_MAX, false, radef_kInvalidOperationInCurrentState),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX, (UT_REDINT_MSG_LEN_MIN - 1), true, radef_kInvalidMessageSize),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX, (UT_REDINT_MSG_LEN_MAX + 1), true, radef_kInvalidMessageSize)
  )
);

/**
 * @test        @ID{redintTest009} Verify the ReadMessage function.
 *
 * @details     This test verifies the ReadMessage function
 *
 * Test steps:
 * - call the init function with the number of redundancy channels given as test parameter
 * - set return value given by test parameter to redrbf_ReadFromBuffer call
 * - call the ReadMessage function with given test parameter and verify the return code
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Buffer size: Size of buffer in bytes (valid range: 36 - 1093)
 * - Configured redundancy channels: Number of configured redundancy channels
 * - Buffer read return code: Return code of redrbf_ReadFromBuffer function call
 * - Expected return code: Expected return code from redint_ReadMessage function call
 * .
 * | Test Run | Input parameter                    || Test config                                              || Expected values                            ||
 * |----------|-----------------------|-------------|--------------------------------|--------------------------|--------------------------|------------------|
 * |          | Redundancy channel id | Buffer size | Configured redundancy channels | Buffer read return code  | Expected return code     | Test Result      |
 * | 0        | 0U                    | 36U         | 1U                             | radef_kNoError           | radef_kNoError           | Normal operation |
 * | 1        | 1U                    | 36U         | 1U                             | radef_kNoError           | radef_kInvalidParameter  | Normal operation |
 * | 2        | 0U                    | 36U         | 1U                             | radef_kInvalidParameter  | radef_kInvalidParameter  | Normal operation |
 * | 3        | 0U                    | 35U         | 1U                             | radef_kNoError           | radef_kInvalidParameter  | Normal operation |
 * | 4        | 0U                    | 1094U       | 1U                             | radef_kNoError           | radef_kInvalidParameter  | Normal operation |
 * | 5        | 0U                    | 1093U       | 2U                             | radef_kNoError           | radef_kNoError           | Normal operation |
 * | 6        | 1U                    | 1093U       | 2U                             | radef_kNoError           | radef_kNoError           | Normal operation |
 * | 7        | 2U                    | 1093U       | 2U                             | radef_kNoError           | radef_kInvalidParameter  | Normal operation |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(ReadMessageTestWithParam, redintTest009VerifyReadMessage)
{
  // test variables
  uint8_t buffer = {};
  uint16_t message_size = UT_REDINT_MSG_LEN_MAX;

  redcty_RedundancyLayerConfiguration config = CreateConfig(
    default_config.check_code_type,
    default_config.t_seq,
    default_config.n_diagnosis,
    default_config.n_defer_queue_size,
    configured_redundancy_channels,         // given parameter
    UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN,
    UT_REDINT_TRANSPORT_CHANNEL_NUM_MAX,
    UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN
  );

  // Set init call expectations
  EXPECT_CALL(redcor_mock, redcor_IsConfigurationValid(&config)).WillOnce(Return(true));
  EXPECT_CALL(redcor_mock, redcor_Init(&config)).Times(1);
  EXPECT_CALL(redstm_mock, redstm_Init(config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(redrbf_mock, redrbf_Init(config.number_of_redundancy_channels)).Times(1);
  EXPECT_CALL(reddfq_mock, reddfq_Init(config.number_of_redundancy_channels, config.n_defer_queue_size, config.t_seq)).Times(1);

  // if ReadMessage call return code not invalid error or invalid param return code is expected from ReadFromBuffer function, expect call
  if(expected_return_code != radef_kInvalidParameter || (expected_return_code == radef_kInvalidParameter && buffer_read_return_code == radef_kInvalidParameter))
  {
    EXPECT_CALL(redrbf_mock, redrbf_ReadFromBuffer(redundancy_channel_id, buffer_size, &message_size, &buffer)).WillOnce(Return(buffer_read_return_code));
  }

  // init module
  redint_Init(&config);

  // perform test
  EXPECT_EQ(expected_return_code, redint_ReadMessage(redundancy_channel_id, buffer_size, &message_size, &buffer));
}

INSTANTIATE_TEST_SUITE_P(
  verifyReadMessage,
  ReadMessageTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, UT_REDINT_MSG_BUFFER_SIZE_MIN, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX, UT_REDINT_MSG_BUFFER_SIZE_MIN, radef_kNoError, radef_kInvalidParameter),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, UT_REDINT_MSG_BUFFER_SIZE_MIN, radef_kInvalidParameter, radef_kInvalidParameter),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, (UT_REDINT_MSG_BUFFER_SIZE_MIN - 1), radef_kNoError, radef_kInvalidParameter),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MIN, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, (UT_REDINT_MSG_BUFFER_SIZE_MAX + 1), radef_kNoError, radef_kInvalidParameter),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MIN, UT_REDINT_MSG_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX, UT_REDINT_MSG_BUFFER_SIZE_MAX, radef_kNoError, radef_kNoError),
    std::make_tuple(UT_REDINT_REDUNDANCY_CHANNEL_NUM_MAX, (UT_REDINT_REDUNDANCY_CHANNEL_ID_MAX + 1 ), UT_REDINT_MSG_BUFFER_SIZE_MAX, radef_kNoError, radef_kInvalidParameter)
  )
);

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
