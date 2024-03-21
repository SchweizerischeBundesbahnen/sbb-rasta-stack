/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_reddia.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 0d5e35164e062b280cdced7615ef10c3300de6c9
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4957: Fixed index for verifying timestamp of received message,12.12.2022,N. Fricker}
 *
 * @brief Unit test file for the diagnostics module of the redundancy layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_reddia.hh"

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

/**
 * @brief gets the transport channel index in a redundancy channel
 *
 * This function gets the index of a specified transport channel in a
 * specified redundancy channel from the given config
 *
 * @param config                  configuration
 * @param red_channel_id          redundancy channel id to get transport channel
 * @param transport_channel_id    transport channel id to get index of
 */
uint32_t GetTransportChannelIndex(const redcty_RedundancyLayerConfiguration * config, const uint32_t red_channel_id, const uint32_t transport_channel_id);

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
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - Expect config to be valid (bool)
 * - Expect fatal error (bool)
 * .
 */
class InitTestWithParam :   public reddiaTest,
                            public testing::WithParamInterface<std::tuple<bool, bool>>
{
  public:
  bool use_valid_config =       std::get<0>(GetParam());        ///< indicates if valid config is used
  bool expect_fatal_error =     std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized InitRedundancyChannelDiagnostics test
 *
 * Parameter order:
 * - Amount of configured redundancy channels (uint32_t)
 * - Redundancy channel ID (uint32_t)
 * - Expect fatal error (bool)
 * .
 */
class InitRedundancyChannelDiagnosticsTestWithParam :   public reddiaTest,
                                                        public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>>
{
  public:
  uint32_t configured_redundancy_channels =   std::get<0>(GetParam());        ///< configured redundancy channels
  uint32_t redundancy_channel_id =            std::get<1>(GetParam());        ///< redundancy channel id
  bool expect_fatal_error =                   std::get<2>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized UpdateRedundancyChannelDiagnostics test
 *
 * Parameter order:
 * - Indicates if module gets initialized (if test has dependencies to prior one or not) (bool)
 * - Amount of configured redundancy channels (uint32_t)
 * - Redundancy channel ID (uint32_t)
 * - Transport channel ID (uint32_t)
 * - Messages in current diagnosis window (uint32_t)
 * - Message sequence number (uint32_t)
 * - Message timestamp (uint32_t)
 * - Message already received (bool)
 * - Timestamp of message which was already received (uint32_t)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class UpdateRedundancyChannelDiagnosticsTestWithParam :   public reddiaTest,
                                                          public testing::WithParamInterface<std::tuple<bool, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool, uint32_t, radef_RaStaReturnCode>>
{
  public:
  bool do_init =                                  std::get<0>(GetParam());        ///< init module
  uint32_t configured_redundancy_channels =       std::get<1>(GetParam());        ///< configured redundancy channels
  uint32_t redundancy_channel_id =                std::get<2>(GetParam());        ///< redundancy channel id
  uint32_t transport_channel_id =                 std::get<3>(GetParam());        ///< transport channel id
  uint32_t messages_in_diagnosis_window =         std::get<4>(GetParam());        ///< messages in current diagnosis window
  uint32_t message_sequence_number =              std::get<5>(GetParam());        ///< message sequence number
  uint32_t message_timestamp =                    std::get<6>(GetParam());        ///< timestamp of message
  bool message_already_received =                 std::get<7>(GetParam());        ///< indicates if the message was already received
  uint32_t already_received_message_timestamp =   std::get<8>(GetParam());        ///< timestamp of already received message
  radef_RaStaReturnCode expected_return_code =    std::get<9>(GetParam());        ///< expected return code
};

/**
 * @brief Test class for value-parametrized IsConfigurationValid function test
 *
 * Parameter order:
 * - Configured check code type (redcty_CheckCodeType)
 * - Time to buffer seq messages (uint32_t)
 * - Diagnosis window size (uint32_t)
 * - Defer queue size (uint32_t)
 * - Number of redundancy channels (uint32_t)
 * - Redundancy channel start id (uint32_t)
 * - Number of transport channels (uint32_t)
 * - Transport channel start id (uint32_t)
 * - Expect configuration to be valid (bool)
 * .
 */
class IsConfigurationValidTestWithParam :   public reddiaTest,
                                            public testing::WithParamInterface<std::tuple<redcty_CheckCodeType, uint32_t, uint32_t, uint32_t ,uint32_t, uint32_t, uint32_t, uint32_t, bool>>
{
  public:
  redcty_CheckCodeType check_code_type                              = std::get<0>(GetParam());        ///< configured check code type
  uint32_t t_seq                                                    = std::get<1>(GetParam());        ///< time for out of sequence message buffering [ms]
  uint32_t n_diagnosis                                              = std::get<2>(GetParam());        ///< diagnosis window size [messages]
  uint32_t n_defer_queue_size                                       = std::get<3>(GetParam());        ///< size of defer queue [messages]
  uint32_t number_of_redundancy_channels                            = std::get<4>(GetParam());        ///< number of configured redundancy channels
  uint32_t red_channel_id_start                                     = std::get<5>(GetParam());        ///< redundancy channel id to start with
  uint32_t number_of_transport_channels                             = std::get<6>(GetParam());        ///< number of configured transport channels per redundancy channel
  uint32_t transport_channel_id_start                               = std::get<7>(GetParam());        ///< transport channel id to start with
  bool expect_valid                                                 = std::get<8>(GetParam());        ///< expect if configuration should be valid
};

/**
 * @brief Test class for value-parametrized IsTransportChannelIdValid test
 *
 * Parameter order:
 * - Amount of configured redundancy channels (uint32_t)
 * - Redundancy channel ID (uint32_t)
 * - Amount of configured transport channels (uint32_t)
 * - Transport channel ID (uint32_t)
 * - Expect transport channel id to be valid (bool)
 * - Expect fatal error (bool)
 * .
 */
class IsTransportChannelIdValidTestWithParam :  public reddiaTest,
                                                public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, bool, bool>>
{
  public:
  uint32_t configured_redundancy_channels =   std::get<0>(GetParam());        ///< configured redundancy channels
  uint32_t redundancy_channel_id =            std::get<1>(GetParam());        ///< redundancy channel id
  uint32_t configured_transport_channels =    std::get<2>(GetParam());        ///< configured transport channels
  uint32_t transport_channel_id =             std::get<3>(GetParam());        ///< transport channel id
  bool expect_transport_channel_id_valid =    std::get<4>(GetParam());        ///< indicates if transport channel is valid
  bool expect_fatal_error =                   std::get<5>(GetParam());        ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup red_diagnostics
 * @{
 */

/**
 * @test        @ID{reddiaTest001} Verify the InitRedundancyLayerDiagnostics function.
 *
 * @details     This test verifies that the init function sets the check code type
 *              parameter correctly.
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidConfiguration based on the test parameter
 * - call the init function with valid oder invalid config, given by parameter
 * - verify internal set config with used test config
 * .
 *
 * @testParameter
 * - Use valid config: Indicates if valid config gets used
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Test config      | Expected values                        ||
 * |----------|------------------|--------------------|--------------------|
 * |          | Use valid config | Expect Fatal Error | Test Result        |
 * | 0        | true             | false              | Normal operation   |
 * | 1        | false            | true               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitTestWithParam, reddiaTest001VerifyInitFunction)
{
  // test variables
  redcty_RedundancyLayerConfiguration config = default_config;

  if(!use_valid_config)
  {
    // invalidate the config
    config.number_of_redundancy_channels = (UT_REDDIA_NUM_OF_RED_CHANNELS_MAX + 1);
  }

  // perform test
  if (expect_fatal_error)
  {
    // call the init function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidConfiguration)).WillOnce(Invoke(this, &reddiaTest::invalidArgumentException));
    EXPECT_THROW(reddia_InitRedundancyLayerDiagnostics(&config), std::invalid_argument);
  }
  else
  {
    // call the init function and verify initialized set to true
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(reddia_InitRedundancyLayerDiagnostics(&default_config));
    EXPECT_TRUE(reddia_initialized);

    // verify configuration set correct
    EXPECT_EQ(reddia_redundancy_configuration->check_code_type, default_config.check_code_type);
    EXPECT_EQ(reddia_redundancy_configuration->n_defer_queue_size, default_config.n_defer_queue_size);
    EXPECT_EQ(reddia_redundancy_configuration->n_diagnosis, default_config.n_diagnosis);
    EXPECT_EQ(reddia_redundancy_configuration->number_of_redundancy_channels, default_config.number_of_redundancy_channels);
    EXPECT_EQ(reddia_redundancy_configuration->t_seq, default_config.t_seq);

    // verify each redundancy channel config
    for(uint16_t i = 0; i < default_config.number_of_redundancy_channels; i++)
    {
      EXPECT_EQ(reddia_redundancy_configuration->redundancy_channel_configurations[i].num_transport_channels, default_config.redundancy_channel_configurations[i].num_transport_channels);
      EXPECT_EQ(reddia_redundancy_configuration->redundancy_channel_configurations[i].red_channel_id, default_config.redundancy_channel_configurations[i].red_channel_id);

      // verify transport channels
      for(uint16_t t = 0; t < default_config.redundancy_channel_configurations[i].num_transport_channels; t++)
      {
        EXPECT_EQ(reddia_redundancy_configuration->redundancy_channel_configurations[i].transport_channel_ids[t], default_config.redundancy_channel_configurations[i].transport_channel_ids[t]);
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitFunction,
  InitTestWithParam,
  ::testing::Values(
      std::make_tuple(true, false),
      std::make_tuple(false, true)
  )
);

/**
 * @test        @ID{reddiaTest002} Verify the number of init function calls.
 *
 * @details     This test verifies that the init function can only be called once.
 *
 * Test steps:
 * - call the init function with the default config and verify that it completes successfully
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
TEST_F(reddiaTest, reddiaTest002VerifyInitFunctionCalls)
{
  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &reddiaTest::invalidArgumentException));

  // call the init function and verify that it completes successfully
  EXPECT_NO_THROW(reddia_InitRedundancyLayerDiagnostics(&default_config));

  // call the init function again and verify that an assertion violation is raised as the module is already initialized
  EXPECT_THROW(reddia_InitRedundancyLayerDiagnostics(&default_config), std::invalid_argument);
}

/**
 * @test        @ID{reddiaTest003} Initialization checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the InitRedundancyChannelDiagnostics function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the UpdateRedundancyChannelDiagnostics function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(reddiaTest, reddiaTest003VerifyInitializationCheck)
{
  const uint8_t nbr_of_function_calls       = 2U;        // number of function calls

  // set test variables
  const uint32_t redundancy_channel_id      = 0U;
  const uint32_t transport_channel_id       = 0U;
  const uint32_t message_sequence_number    = 0U;

  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
      .Times(nbr_of_function_calls)
      .WillRepeatedly(Invoke(this, &reddiaTest::invalidArgumentException));

  // perform test
  EXPECT_THROW(reddia_InitRedundancyChannelDiagnostics(redundancy_channel_id), std::invalid_argument);
  EXPECT_THROW(reddia_UpdateRedundancyChannelDiagnostics(redundancy_channel_id, transport_channel_id, message_sequence_number), std::invalid_argument);
}

/**
 * @test        @ID{reddiaTest004} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - init module
 * - call the reddia_InitRedundancyLayerDiagnostics function with invalid configuration pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - init the module with default config
 * - call the reddia_IsConfigurationValid function with invalid configuration pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(reddiaTest, reddiaTest004VerifyNullPointerCheck)
{
  const uint8_t nbr_of_function_calls = 2U;        // number of function calls

  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).Times(nbr_of_function_calls).WillRepeatedly(Invoke(this, &reddiaTest::invalidArgumentException));

  // perform init test
  EXPECT_THROW(reddia_InitRedundancyLayerDiagnostics((redcty_RedundancyLayerConfiguration *)NULL), std::invalid_argument);

  // init module for next test
  reddia_InitRedundancyLayerDiagnostics(&default_config);

  EXPECT_THROW(reddia_IsConfigurationValid((redcty_RedundancyLayerConfiguration *)NULL), std::invalid_argument);
}

/**
 * @test        @ID{reddiaTest005} Verify the InitRedundancyChannelDiagnostics function.
 *
 * @details     This test verifies the correct behaviour of the InitRedundancyChannelDiagnostics function
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - init the module with given config
 * - set properties which should be reset by function to any value
 * - call the function for the given redundancy channel
 * - verify properties are reset
 * .
 *
 * @testParameter
 * - Redundancy channel Id: Redundancy channel identification
 * - Configured redundancy channels: Number of configured Redundancy channels
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter       | Test config                    | Expected values                        ||
 * |----------|-----------------------|--------------------------------|--------------------|--------------------|
 * |          | Redundancy channel Id | Configured redundancy channels | Expect fatal error | Test Result        |
 * | 0        | 0U                    | 1U                             | false              | Normal operation   |
 * | 1        | 1U                    | 1U                             | true               | Fatal error raised |
 * | 2        | 0U                    | 2U                             | false              | Normal operation   |
 * | 3        | 1U                    | 2U                             | false              | Normal operation   |
 * | 4        | 2U                    | 2U                             | true               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitRedundancyChannelDiagnosticsTestWithParam, reddiaTest005verifyInitRedundancyChannelDiagnosticsFunction)
{
  // test variables
  redcty_RedundancyLayerConfiguration config = CreateConfig(
    default_config.check_code_type,
    default_config.t_seq,
    default_config.n_diagnosis,
    default_config.n_defer_queue_size,
    configured_redundancy_channels,         // given parameter
    UT_REDDIA_RED_CHANNEL_ID_MIN,
    UT_REDDIA_NUM_OF_TR_CHANNELS_MIN,
    UT_REDDIA_TR_CHANNEL_ID_MIN
  );

  // init module
  EXPECT_NO_THROW(reddia_InitRedundancyLayerDiagnostics(&config));

  // perform test
  if (expect_fatal_error)
  {
    // call the init function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &reddiaTest::invalidArgumentException));
    EXPECT_THROW(reddia_InitRedundancyChannelDiagnostics(redundancy_channel_id), std::invalid_argument);
  }
  else
  {
    // set values to properties which are getting reset by function call
    for (uint32_t i = 0U; i < config.n_diagnosis; i++) {
      reddia_received_messages_timestamps[redundancy_channel_id][i].sequence_number = 1U;
      reddia_received_messages_timestamps[redundancy_channel_id][i].received_time_stamp = 1U;
      for (uint32_t tr = 0U; tr < config.redundancy_channel_configurations[redundancy_channel_id].num_transport_channels; tr++) {
        reddia_received_messages_timestamps[redundancy_channel_id][i].message_received_flag[tr] = true;
      }
    }

    for(uint16_t i = 0U; i < config.redundancy_channel_configurations[redundancy_channel_id].num_transport_channels; i++)
    {
      reddia_transport_channel_diagnostic_data[redundancy_channel_id][i].n_diagnosis = 1U;
      reddia_transport_channel_diagnostic_data[redundancy_channel_id][i].n_missed = 1U;
      reddia_transport_channel_diagnostic_data[redundancy_channel_id][i].t_drift = 1U;
      reddia_transport_channel_diagnostic_data[redundancy_channel_id][i].t_drift2 = 1U;
    }

    // call the function
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(reddia_InitRedundancyChannelDiagnostics(redundancy_channel_id));

    // verify properties are reset
    for (uint32_t i = 0; i < config.n_diagnosis; i++) {
      EXPECT_EQ(reddia_received_messages_timestamps[redundancy_channel_id][i].sequence_number, 0U);
      EXPECT_EQ(reddia_received_messages_timestamps[redundancy_channel_id][i].received_time_stamp, 0U);
      for (uint32_t tr = 0; tr < config.redundancy_channel_configurations[redundancy_channel_id].num_transport_channels; tr++) {
        EXPECT_FALSE(reddia_received_messages_timestamps[redundancy_channel_id][i].message_received_flag[tr]);
      }
    }

    EXPECT_EQ(reddia_current_n_diagnosis[redundancy_channel_id], 0U);

    for(uint16_t i = 0; i < config.redundancy_channel_configurations[redundancy_channel_id].num_transport_channels; i++)
    {
      EXPECT_EQ(reddia_transport_channel_diagnostic_data[redundancy_channel_id][i].n_diagnosis, 0U);
      EXPECT_EQ(reddia_transport_channel_diagnostic_data[redundancy_channel_id][i].n_missed, 0U);
      EXPECT_EQ(reddia_transport_channel_diagnostic_data[redundancy_channel_id][i].t_drift, 0U);
      EXPECT_EQ(reddia_transport_channel_diagnostic_data[redundancy_channel_id][i].t_drift2, 0U);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitRedundancyChannelDiagnosticsFunction,
  InitRedundancyChannelDiagnosticsTestWithParam,
  ::testing::Values(
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MAX, true),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MIN, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, (UT_REDDIA_RED_CHANNEL_ID_MAX + 1), true)
  )
);

/**
 * @test        @ID{reddiaTest006} Verify the UpdateRedundancyChannelDiagnostics function.
 *
 * @details     This test verifies the correct behaviour of the UpdateRedundancyChannelDiagnostics function
 *
 * Test steps:
 * - create config with given parameter for redundancy channels
 * - check if diagnosis window full or more than configured
 * - set return value of timer to given parameter
 * - init module
 * - set amount of messages in diagnosis window according parameter
 * - call UpdateRedundancyChannelDiagnostics function
 * - verify channel diagnostics reset if diagnosis window overflows
 * - if no error expected verify if properties updated (when message already received) or set when message new
 * .
 *
 * @testParameter
 * - Red CH Id: Redundancy channel identification
 * - Tr CH Id: Transport channel identification
 * - Msq seq number: Message sequence number
 * - Init: Indicates if module gets initialized
 * - Conf red CHs: Number of configured Redundancy channels
 * - diagnosis window msgs: Current messages in diagnosis window
 * - Msg timestamp: Timestamp of message
 * - Msg already received: Indicates if the message was already received
 * - Timestamp of received msg: Timestamp of already received message
 * - Expected return code: Expected return code when calling the function
 * .
 * | Test Run | Input parameter                     ||| Test config                                                                                                |||||| Expected values                             ||
 * |----------|-----------|----------|----------------|-------|--------------|-----------------------|---------------|----------------------|---------------------------|-------------------------|--------------------|
 * |          | Red CH ID | Tr CH ID | Msq seq number | Init  | Conf red CHs | diagnosis window msgs | Msg timestamp | Msg already received | Timestamp of received msg | Expected return code    | Test Result        |
 * | 0        | 0U        | 0U       | 0U             | true  | 1U           | 0U                    | 0U            | false                | 0U                        | radef_kNoError          | Normal operation   |
 * | 1        | 0U        | 1U       | 0U             | true  | 1U           | 0U                    | 0U            | 0U                   | 0U                        | radef_kNoError          | Normal operation   |
 * | 2        | 0U        | 0U       | 1U             | true  | 1U           | 0U                    | 0U            | false                | 0U                        | radef_kNoError          | Normal operation   |
 * | 3        | 0U        | 0U       |  1U            | true  | 1U           | 0U                    | 0U            | true                 | 0U                        | radef_kNoError          | Normal operation   |
 * | 4        | 0U        | 0U       | 1U             | true  | 1U           | 1U                    | 149U          | true                 | 100U                      | radef_kNoError          | Normal operation   |
 * | 5        | 0U        | 0U       | 2U             | false | 1U           | 200U                  | 0U            | false                | 0U                        | radef_kNoError          | Normal operation   |
 * | 6        | 0U        | 0U       | 1U             | true  | 1U           | 1U                    | 150U          | true                 | 100U                      | radef_kNoError          | Normal operation   |
 * | 7        | 0U        | 0U       | 1U             | true  | 1U           | 1U                    | 151U          | true                 | 100U                      | radef_kNoError          | Normal operation   |
 * | 8        | 0U        | 0U       | 0U             | true  | 1U           | 201U                  | 0U            | false                | 0U                        | radef_kInternalError    | Fatal error raised |
 * | 9        | 0U        | 0U       | 1U             | true  | 1U           | 200U                  | 0U            | false                | 0U                        | radef_kNoError          | Normal operation   |
 * | 10       | 1U        | 2U       | 0xffffffffU    | true  | 2U           | 0U                    | 0U            | false                | 0U                        | radef_kNoError          | Normal operation   |
 * | 11       | 1U        | 3U       | 0xffffffffU    | true  | 2U           | 0U                    | 0U            | false                | 0U                        | radef_kNoError          | Normal operation   |
 * | 12       | 1U        | 0U       | 0U             | true  | 1U           | 1U                    | 0U            | false                | 0U                        | radef_kInvalidParameter | Fatal error raised |
 * | 13       | 0U        | 2U       | 0U             | true  | 1U           | 1U                    | 0U            | false                | 0U                        | radef_kInvalidParameter | Fatal error raised |
 * | 14       | 0U        | 3U       | 0U             | true  | 1U           | 1U                    | 0U            | false                | 0U                        | radef_kInvalidParameter | Fatal error raised |
 * | 15       | 2U        | 3U       | 0U             | true  | 2U           | 1U                    | 0U            | false                | 0U                        | radef_kInvalidParameter | Fatal error raised |
 * | 16       | 1U        | 0U       | 0U             | true  | 2U           | 1U                    | 0U            | false                | 0U                        | radef_kInvalidParameter | Fatal error raised |
 * | 17       | 1U        | 1U       | 0U             | true  | 2U           | 1U                    | 0U            | false                | 0U                        | radef_kInvalidParameter | Fatal error raised |
 * | 18       | 1U        | 4U       | 0U             | true  | 2U           | 1U                    | 0U            | false                | 0U                        | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(UpdateRedundancyChannelDiagnosticsTestWithParam, reddiaTest006verifyUpdateRedundancyChannelDiagnosticsFunction)
{
  // test variables
  bool diagnosis_window_reached = false;                                                    // indicates if the diagnosis window is fully reached
  bool diagnosis_window_overflow = false;                                                   // indicates if the diagnosis window is greater than configured window size
  bool expect_tdrift_updated = false;                                                       // indicates if message already received and tdrift values getting updated
  uint32_t expected_message_diagnosis_index = messages_in_diagnosis_window;                 // index of message in diagnosis
  uint32_t transport_channel_index = 0U;                                                    // transport channel index in redundancy channel
  uint32_t expected_t_drift = message_timestamp - already_received_message_timestamp;       // expected tdrift when message was already received

  redcty_RedundancyLayerConfiguration config = CreateConfig(
    default_config.check_code_type,
    default_config.t_seq,
    default_config.n_diagnosis,
    default_config.n_defer_queue_size,
    configured_redundancy_channels,         // given parameter
    UT_REDDIA_RED_CHANNEL_ID_MIN,
    UT_REDDIA_NUM_OF_TR_CHANNELS_MAX,
    UT_REDDIA_TR_CHANNEL_ID_MIN
  );

  if(expected_return_code == radef_kNoError)
  {
    // get transport channel index to verify properties set
    transport_channel_index = GetTransportChannelIndex(&config, redundancy_channel_id, transport_channel_id);
  }

  // check if more messages in diagnosis window than configured
  if(messages_in_diagnosis_window > config.n_diagnosis)
  {
    diagnosis_window_overflow = true;
    expected_message_diagnosis_index = 0;
  }
  else if(messages_in_diagnosis_window == config.n_diagnosis)
  {
    diagnosis_window_reached = true;
    expected_message_diagnosis_index = 0;

    // if diagnosis window reached diagnostics data for this channel gets reset and notification is sent to every transport channel configured
    EXPECT_CALL(rednot_mock, rednot_DiagnosticNotification(redundancy_channel_id, config.redundancy_channel_configurations[redundancy_channel_id].transport_channel_ids[0], _)).Times(1);

    if(config.redundancy_channel_configurations[redundancy_channel_id].num_transport_channels > 1)
    {
      EXPECT_CALL(rednot_mock, rednot_DiagnosticNotification(redundancy_channel_id, config.redundancy_channel_configurations[redundancy_channel_id].transport_channel_ids[1], _)).Times(1);
    }
  }

  // expect get timer value called when no error or diagnosis window reached (error gets thrown later)
  if (expected_return_code == radef_kNoError || diagnosis_window_overflow)
  {
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).WillOnce(Return(message_timestamp));
  }

  if(do_init){
    // init module
    EXPECT_NO_THROW(reddia_InitRedundancyLayerDiagnostics(&config));
  }
  else
  {
    reddia_initialized = true;
  }

  // set current messages in diagnosis window
  reddia_current_n_diagnosis[redundancy_channel_id] = messages_in_diagnosis_window;

  if(message_already_received)
  {
    // set msg seq nbr and timestamp to message which was already received
    reddia_received_messages_timestamps[redundancy_channel_id][0].sequence_number = message_sequence_number;
    reddia_received_messages_timestamps[redundancy_channel_id][0].received_time_stamp = already_received_message_timestamp;

    // if message already received, difference (tdrift) between the messages timestamps is calculated
    if(config.t_seq >= expected_t_drift && !diagnosis_window_reached)
    {
      expect_tdrift_updated = true;
    }
  }

  // perform test
  if (expected_return_code != radef_kNoError)
  {
    // call the UpdateRedundancyChannelDiagnostics function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_return_code)).WillOnce(Invoke(this, &reddiaTest::invalidArgumentException));
    EXPECT_THROW(reddia_UpdateRedundancyChannelDiagnostics(redundancy_channel_id, transport_channel_id, message_sequence_number), std::invalid_argument);

    if(diagnosis_window_overflow)
    {
      EXPECT_EQ(reddia_transport_channel_diagnostic_data[redundancy_channel_id][transport_channel_index].n_diagnosis, 0U);
      EXPECT_EQ(reddia_transport_channel_diagnostic_data[redundancy_channel_id][transport_channel_index].n_missed, 0U);
      EXPECT_EQ(reddia_transport_channel_diagnostic_data[redundancy_channel_id][transport_channel_index].t_drift, 0U);
      EXPECT_EQ(reddia_transport_channel_diagnostic_data[redundancy_channel_id][transport_channel_index].t_drift2, 0U);
    }
  }
  else
  {
    // call the UpdateRedundancyChannelDiagnostics function
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(reddia_UpdateRedundancyChannelDiagnostics(redundancy_channel_id, transport_channel_id, message_sequence_number));

    if(message_already_received && expect_tdrift_updated)
    {
      EXPECT_EQ(reddia_transport_channel_diagnostic_data[redundancy_channel_id][transport_channel_index].t_drift, expected_t_drift);
      EXPECT_EQ(reddia_transport_channel_diagnostic_data[redundancy_channel_id][transport_channel_index].t_drift2, (expected_t_drift * expected_t_drift));
      EXPECT_TRUE(reddia_received_messages_timestamps[redundancy_channel_id][0].message_received_flag[transport_channel_index]);
    }

    if(!message_already_received)
    {
      EXPECT_EQ(reddia_received_messages_timestamps[redundancy_channel_id][expected_message_diagnosis_index].received_time_stamp, message_timestamp);
      EXPECT_EQ(reddia_received_messages_timestamps[redundancy_channel_id][expected_message_diagnosis_index].sequence_number, message_sequence_number);
      EXPECT_TRUE(reddia_received_messages_timestamps[redundancy_channel_id][expected_message_diagnosis_index].message_received_flag[transport_channel_index]);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyUpdateRedundancyChannelDiagnosticsFunction,
  UpdateRedundancyChannelDiagnosticsTestWithParam,
  ::testing::Values(
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, 0, 0, 0, false, 0, radef_kNoError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, 1, 0, 0, 0, false, 0, radef_kNoError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, 0, 1, 0, false, 0, radef_kNoError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, 0, 1, 0, true, 0, radef_kNoError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, 1, 1, 149, true, 100, radef_kNoError),
      std::make_tuple(false, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, default_config.n_diagnosis, 2, 0, false, 0, radef_kNoError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, 1, 1, 150, true, 100, radef_kNoError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, 1, 1, 151, true, 100, radef_kNoError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, (default_config.n_diagnosis + 1), 0, 0, false, 0, radef_kInternalError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, default_config.n_diagnosis, 1, 0, false, 0, radef_kNoError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, 2, 0, UINT32_MAX, 0, false, 0, radef_kNoError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, UT_REDDIA_TR_CHANNEL_ID_MAX, 0, UINT32_MAX, 0, false, 0, radef_kNoError),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MAX, UT_REDDIA_TR_CHANNEL_ID_MIN, 1, 0, 0, false, 0, radef_kInvalidParameter),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, 2, 1, 0, 0, false, 0, radef_kInvalidParameter),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_TR_CHANNEL_ID_MAX, 1, 0, 0, false, 0, radef_kInvalidParameter),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, (UT_REDDIA_RED_CHANNEL_ID_MAX + 1), UT_REDDIA_TR_CHANNEL_ID_MAX, 1, 0, 0, false, 0, radef_kInvalidParameter),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, UT_REDDIA_TR_CHANNEL_ID_MIN, 1, 0, 0, false, 0, radef_kInvalidParameter),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, 1, 1, 0, 0, false, 0, radef_kInvalidParameter),
      std::make_tuple(true, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, (UT_REDDIA_TR_CHANNEL_ID_MAX + 1), 1, 0, 0, false, 0, radef_kInvalidParameter)
  )
);

/**
 * @test        @ID{reddiaTest007} Verify the IsConfigurationValid function.
 *
 * @details     This test verifies the IsConfigurationValid function
 *
 * Test steps:
 * - Create config with given parameter
 * - Call isConfigurationValid Function and compare with expected valid value
 * .
 *
 * @testParameter
 * - Check code type: Type of check code
 * - T Seq: Time for out of sequence message buffering in ms (valid range: 50 - 500)
 * - N diagnosis: diagnosis window size (valid range: 10 - 1000)
 * - Defer queue size: Size of defer queue (valid range: 4 - 10)
 * - Red channels: Number of configured redundancy channels (valid range: 1 -2)
 * - Red channel Start Id: Redundancy Channel identification start value (+1 on every channel)
 * - Tr channels: Number of configured transport channels per Redundancy channel (valid range: 1 - 2)
 * - Tr channel Start Id: Transport Channel identification start value (+1 on every channel)
 * - Expect config valid: Indicates if configuration expected to be valid
 * .
 * | Test Run | Input parameter                                                                                                                  |||||||| Expected values                         ||
 * |----------|----------------------|-------|-------------|------------------|--------------|----------------------|-------------|---------------------|---------------------|--------------------|
 * |          | Check code type      | T Seq | N diagnosis | Defer queue size | Red channels | Red channel Start Id | Tr channels | Tr channel Start Id | Expect config valid | Test Result        |
 * |        0 | redcty_kCheckCodeA   |   50U |         10U |               4U |           1U |                   0U |          1U |                  0U |                true | Normal Operation   |
 * |        1 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           2U |                   0U |          1U |                  1U |                true | Normal Operation   |
 * |        2 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           2U |                   0U |          1U |                  2U |                true | Normal Operation   |
 * |        3 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           2U |                   0U |          1U |                  3U |               false | Fatal error raised |
 * |        4 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           2U |                   0U |          2U |                  0U |                true | Normal Operation   |
 * |        5 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           2U |                   0U |          2U |                  1U |               false | Fatal error raised |
 * |        6 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           2U |                   0U |          2U |                  2U |               false | Fatal error raised |
 * |        7 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           2U |                   0U |          2U |                  3U |               false | Fatal error raised |
 * |        8 | redcty_kCheckCodeA   |   49U |         10U |               4U |           1U |                   0U |          1U |                  0U |               false | Fatal error raised |
 * |        9 | redcty_kCheckCodeA   |   50U |          9U |               4U |           1U |                   0U |          1U |                  0U |               false | Fatal error raised |
 * |       10 | redcty_kCheckCodeA   |   50U |         10U |               3U |           1U |                   0U |          1U |                  0U |               false | Fatal error raised |
 * |       11 | redcty_kCheckCodeA   |   50U |         10U |               4U |           0U |                   0U |          1U |                  0U |               false | Fatal error raised |
 * |       12 | redcty_kCheckCodeA   |   50U |         10U |               4U |           1U |                   0U |          0U |                  0U |               false | Fatal error raised |
 * |       13 | redcty_kCheckCodeMax |  500U |       1000U |              10U |           2U |                   0U |          2U |                  0U |               false | Fatal error raised |
 * |       14 | redcty_kCheckCodeE   |  501U |       1000U |              10U |           2U |                   0U |          2U |                  0U |               false | Fatal error raised |
 * |       15 | redcty_kCheckCodeE   |  500U |       1001U |              10U |           2U |                   0U |          2U |                  0U |               false | Fatal error raised |
 * |       16 | redcty_kCheckCodeE   |  500U |       1000U |              11U |           2U |                   0U |          2U |                  0U |               false | Fatal error raised |
 * |       17 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           3U |                   0U |          2U |                  0U |               false | Fatal error raised |
 * |       18 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           2U |                   1U |          2U |                  0U |               false | Fatal error raised |
 * |       19 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           2U |                   0U |          3U |                  0U |               false | Fatal error raised |
 * |       20 | redcty_kCheckCodeE   |  500U |       1000U |              10U |           2U |                   0U |          2U |                  4U |               false | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(IsConfigurationValidTestWithParam, reddiaTest007VerifyIsConfigurationValid)
{
  redcty_RedundancyLayerConfiguration  config = CreateConfig(check_code_type, t_seq, n_diagnosis, n_defer_queue_size, number_of_redundancy_channels,
                                                             red_channel_id_start, number_of_transport_channels, transport_channel_id_start);

  // set expectations
  EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

  // call the function and verify the output
  EXPECT_EQ(expect_valid, reddia_IsConfigurationValid(&config));
}

INSTANTIATE_TEST_SUITE_P(
  verifyIsConfigurationValid,
  IsConfigurationValidTestWithParam,
  ::testing::Values(
      std::make_tuple(redcty_kCheckCodeA, UT_REDDIA_T_SEQ_MIN, UT_REDDIA_N_DIAGNOSIS_MIN, UT_REDDIA_DFQ_SIZE_MIN, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, 0, true),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, 1, true),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, 2, true),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, 3, false),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 0, true),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 1, false),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 2, false),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 3, false),
      std::make_tuple(redcty_kCheckCodeA, (UT_REDDIA_T_SEQ_MIN - 1), UT_REDDIA_N_DIAGNOSIS_MIN, UT_REDDIA_DFQ_SIZE_MIN, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, 0, false),
      std::make_tuple(redcty_kCheckCodeA, UT_REDDIA_T_SEQ_MIN, (UT_REDDIA_N_DIAGNOSIS_MIN - 1), UT_REDDIA_DFQ_SIZE_MIN, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, 0, false),
      std::make_tuple(redcty_kCheckCodeA, UT_REDDIA_T_SEQ_MIN, UT_REDDIA_N_DIAGNOSIS_MIN, (UT_REDDIA_DFQ_SIZE_MIN - 1), UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, 0, false),
      std::make_tuple(redcty_kCheckCodeA, UT_REDDIA_T_SEQ_MIN, UT_REDDIA_N_DIAGNOSIS_MIN, UT_REDDIA_DFQ_SIZE_MIN, (UT_REDDIA_NUM_OF_RED_CHANNELS_MIN - 1), 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, 0, false),
      std::make_tuple(redcty_kCheckCodeA, UT_REDDIA_T_SEQ_MIN, UT_REDDIA_N_DIAGNOSIS_MIN, UT_REDDIA_DFQ_SIZE_MIN, UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, 0, (UT_REDDIA_NUM_OF_TR_CHANNELS_MIN - 1), 0, false),
      std::make_tuple(redcty_kCheckCodeMax, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 0, false),
      std::make_tuple(redcty_kCheckCodeE, (UT_REDDIA_T_SEQ_MAX + 1), UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 0, false),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, (UT_REDDIA_N_DIAGNOSIS_MAX + 1), UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 0, false),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, (UT_REDDIA_DFQ_SIZE_MAX + 1), UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 0, false),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, (UT_REDDIA_NUM_OF_RED_CHANNELS_MAX + 1), 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 0, false),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 1, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 0, false),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, (UT_REDDIA_NUM_OF_TR_CHANNELS_MAX + 1), 0, false),
      std::make_tuple(redcty_kCheckCodeE, UT_REDDIA_T_SEQ_MAX, UT_REDDIA_N_DIAGNOSIS_MAX, UT_REDDIA_DFQ_SIZE_MAX, UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, 0, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, (UT_REDDIA_TR_CHANNEL_ID_MAX + 1), false)
  )
);

/**
 * @test        @ID{reddiaTest008} Verify the IsTransportChannelIdValid function.
 *
 * @details     This test verifies the correct behaviour of the IsTransportChannelIdValid function
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - init the module with given config
 * - call the IsTransportChannelIdValid function and verify its return value according test parameter
 * .
 *
 * @testParameter
 * - Red channel Id: Redundancy channel identification
 * - Tr channel Id: Transport channel identification
 * - Conf red channels: Number of configured Redundancy channels
 * - Conf tr channels: Number of configured Transport channels
 * - Expect tr channel valid: Indicates if transport channel id found in redundancy channel
 * - Expect fatal error: Indicates if a fatal error is expected
 * | Test Run | Input parameter               || Test config                         || Expected values                                                 |||
 * |----------|----------------|---------------|-------------------|------------------|-------------------------|--------------------|--------------------|
 * |          | Red channel Id | Tr channel Id | Conf red channels | Conf tr channels | Expect tr channel valid | Expect fatal error | Test Result        |
 * |        0 |             0U |            0U |                1U |               1U | true                    | false              | normal operation   |
 * |        1 |             0U |            1U |                1U |               1U | false                   | false              | normal operation   |
 * |        2 |             0U |            0U |                1U |               2U | true                    | false              | normal operation   |
 * |        3 |             0U |            1U |                1U |               2U | true                    | false              | normal operation   |
 * |        4 |             0U |            2U |                1U |               2U | false                   | false              | normal operation   |
 * |        5 |             0U |            3U |                1U |               2U | false                   | false              | normal operation   |
 * |        6 |             0U |            0U |                2U |               2U | true                    | false              | normal operation   |
 * |        7 |             0U |            1U |                2U |               2U | true                    | false              | normal operation   |
 * |        8 |             0U |            2U |                2U |               2U | false                   | false              | normal operation   |
 * |        9 |             0U |            3U |                2U |               2U | false                   | false              | normal operation   |
 * |       10 |             1U |            0U |                2U |               1U | false                   | false              | normal operation   |
 * |       11 |             1U |            1U |                2U |               1U | true                    | false              | normal operation   |
 * |       12 |             1U |            0U |                2U |               2U | false                   | false              | normal operation   |
 * |       13 |             1U |            1U |                2U |               2U | false                   | false              | normal operation   |
 * |       14 |             1U |            2U |                2U |               2U | true                    | false              | normal operation   |
 * |       15 |             1U |            3U |                2U |               2U | true                    | false              | normal operation   |
 * |       16 |             1U |            0U |                1U |               1U | false                   | true               | fatal error raised |
 * |       17 |             2U |            0U |                2U |               1U | false                   | true               | fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(IsTransportChannelIdValidTestWithParam, reddiaTest008verifyIsTransportChannelIdValidFunction)
{
  // test config
  redcty_RedundancyLayerConfiguration config = CreateConfig(
    default_config.check_code_type,
    default_config.t_seq,
    default_config.n_diagnosis,
    default_config.n_defer_queue_size,
    configured_redundancy_channels,         // given parameter
    UT_REDDIA_RED_CHANNEL_ID_MIN,
    configured_transport_channels,          // given parameter
    UT_REDDIA_TR_CHANNEL_ID_MIN
  );

  // init module
  EXPECT_NO_THROW(reddia_InitRedundancyLayerDiagnostics(&config));

  // perform test
  if (expect_fatal_error)
  {
    // call the init function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &reddiaTest::invalidArgumentException));
    EXPECT_THROW(reddia_IsTransportChannelIdValid(redundancy_channel_id, transport_channel_id), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_EQ(reddia_IsTransportChannelIdValid(redundancy_channel_id, transport_channel_id), expect_transport_channel_id_valid);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyIsTransportChannelIdValidFunction,
  IsTransportChannelIdValidTestWithParam,
  ::testing::Values(
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, true, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, 1, false, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, UT_REDDIA_TR_CHANNEL_ID_MIN, true, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 1, true, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 2, false, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, UT_REDDIA_TR_CHANNEL_ID_MAX, false, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, UT_REDDIA_TR_CHANNEL_ID_MIN, true, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 1, true, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 2, false, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MIN, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, UT_REDDIA_TR_CHANNEL_ID_MAX, false, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, false, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, 1, true, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, UT_REDDIA_TR_CHANNEL_ID_MIN, false, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 1, false, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, 2, true, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, UT_REDDIA_RED_CHANNEL_ID_MAX, UT_REDDIA_NUM_OF_TR_CHANNELS_MAX, UT_REDDIA_TR_CHANNEL_ID_MAX, true, false),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MIN, UT_REDDIA_RED_CHANNEL_ID_MAX, UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, false, true),
      std::make_tuple(UT_REDDIA_NUM_OF_RED_CHANNELS_MAX, (UT_REDDIA_RED_CHANNEL_ID_MAX + 1), UT_REDDIA_NUM_OF_TR_CHANNELS_MIN, UT_REDDIA_TR_CHANNEL_ID_MIN, false, true)
  )
);

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

uint32_t GetTransportChannelIndex(const redcty_RedundancyLayerConfiguration * config, const uint32_t red_channel_id, const uint32_t transport_channel_id) {
  uint32_t tr_channel_index = 0U;
  bool tr_channel_id_found = false;

  while ((tr_channel_index < config->redundancy_channel_configurations[red_channel_id].num_transport_channels) &&
         !tr_channel_id_found) {
    if (transport_channel_id == config->redundancy_channel_configurations[red_channel_id].transport_channel_ids[tr_channel_index]) {
      tr_channel_id_found = true;
    } else {
      ++tr_channel_index;
    }
  }

  return tr_channel_index;
}
