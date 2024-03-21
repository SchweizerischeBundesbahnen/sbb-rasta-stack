/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redstm.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the state machine module of the redundancy layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_redstm.hh"

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
 * @brief initializes the module and sets the current state
 *
 * This function calls the init function of the State Machine module with a maximum
 * number of 1 channel. Then it sets the state of the channel with given
 *  ID to the desired value.
 *
 * @param channel_id     channel id
 * @param state          state to be set
 */
static void InitModuleAndSetState(uint32_t channel_id, redstm_RedundancyChannelStates state);

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
 * - configured channels (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class InitTestWithParam :   public redstmTest,
                            public testing::WithParamInterface<std::tuple<uint32_t, bool>>
{
  public:
    uint32_t configured_channels =      std::get<0>(GetParam());        ///< number of configured channels
    bool expect_fatal_error =           std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized GetChannelState test
 *
 * Parameter order:
 * - channel ID (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class GetChannelStateTestWithParam :   public redstmTest,
                                       public testing::WithParamInterface<std::tuple<uint32_t, bool>>
{
  public:
    uint32_t channel_id =               std::get<0>(GetParam());        ///< channel id
    bool expect_fatal_error =           std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized State Closed test
 *
 * The channel ID is set to zero.
 *
 * Parameter order:
 * - event (redstm_RedundancyChannelEvents)
 * - next state (redstm_RedundancyChannelStates)
 * .
 */
class StateClosedTestWithParam :   public redstmTest,
                                   public testing::WithParamInterface<std::tuple<redstm_RedundancyChannelEvents, redstm_RedundancyChannelStates>>
{
  public:
    uint32_t channel_id = 0U;                                                       ///< channel id
    redstm_RedundancyChannelEvents event =          std::get<0>(GetParam());        ///< event to be processed
    redstm_RedundancyChannelStates next_state =     std::get<1>(GetParam());        ///< expected state after transition
};

/**
 * @brief Test class for value-parametrized State Up test
 *
 * The channel ID is set to zero.
 *
 * Parameter order:
 * - event (redstm_RedundancyChannelEvents)
 * - next state (redstm_RedundancyChannelStates)
 * .
 */
class StateUpTestWithParam :   public redstmTest,
                               public testing::WithParamInterface<std::tuple<redstm_RedundancyChannelEvents, redstm_RedundancyChannelStates>>
{
  public:
    uint32_t channel_id = 0U;                                                       ///< channel id
    redstm_RedundancyChannelEvents event =          std::get<0>(GetParam());        ///< event to be processed
    redstm_RedundancyChannelStates next_state =     std::get<1>(GetParam());        ///< expected state after transition
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup red_sm
 * @{
 */

/**
 * @test        @ID{redstmTest001} Verify the init function.
 *
 * @details     This test verifies that the init function sets the number of configured
 *              channels parameter correctly.
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the init function with given test parameter
 * - verify if the number of channels is set correctly
 * - verify that the module is set to initialized state
 * .
 *
 * @testParameter
 * - Amount of channels: Number of configured redundancy channels
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Test config          | Expected values                        ||
 * |----------|----------------------|--------------------|--------------------|
 * |          | Amount of channels   | Expect Fatal Error | Test Result        |
 * | 0        | 0                    | True               | Fatal error raised |
 * | 1        | 1                    | False              | Normal operation   |
 * | 2        | 2                    | False              | Normal operation   |
 * | 3        | 3                    | True               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitTestWithParam, redstmTest001VerifyInitFunction)
{
  extern uint32_t redstm_number_of_red_channels;

  // perform test
  if(expect_fatal_error)
  {
    // call the init function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redstmTest::invalidArgumentException));
    EXPECT_THROW(redstm_Init(configured_channels), std::invalid_argument);
  }
  else
  {
    // call the init function and verify if parameter are set correctly
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redstm_Init(configured_channels));

    EXPECT_EQ(redstm_number_of_red_channels, configured_channels);
    EXPECT_TRUE(redstm_initialized);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitFunction,
  InitTestWithParam,
  ::testing::Values(
    std::make_tuple((UT_REDSTM_MIN_NUMBER_OF_RED_CHANNELS - 1), true),
    std::make_tuple(UT_REDSTM_MIN_NUMBER_OF_RED_CHANNELS, false),
    std::make_tuple(UT_REDSTM_MAX_NUMBER_OF_RED_CHANNELS, false),
    std::make_tuple((UT_REDSTM_MAX_NUMBER_OF_RED_CHANNELS + 1), true)
  ));

/**
 * @test        @ID{redstmTest002} Verify the number of init function calls.
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
TEST_F(redstmTest, redstmTest002VerifyInitFunctionCalls)
{
  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &redstmTest::invalidArgumentException));

  // call the init function and verify that it completes successful
  EXPECT_NO_THROW(redstm_Init(UT_REDSTM_MIN_NUMBER_OF_RED_CHANNELS));

  // call the init function again and verify that an assertion violations is raised as the module is already initialized
  EXPECT_THROW(redstm_Init(UT_REDSTM_MIN_NUMBER_OF_RED_CHANNELS), std::invalid_argument);
}

/**
 * @test        @ID{redstmTest003} Initialization checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the ProcessChannelStateMachine function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetChannelState function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redstmTest, redstmTest003VerifyInitializationCheck)
{
  const uint8_t nbr_of_function_calls = 2U;      // number of function calls

  // Test variables
  const uint32_t channel_id = UT_REDSTM_MIN_ID_CHANNEL;

  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
    .Times(nbr_of_function_calls)
    .WillRepeatedly(Invoke(this, &redstmTest::invalidArgumentException));

  // call the functions without initializing the module
  EXPECT_THROW(redstm_ProcessChannelStateMachine(channel_id, redstm_kRedundancyChannelEventOpen), std::invalid_argument);
  EXPECT_THROW(redstm_GetChannelState(channel_id), std::invalid_argument);
}

/**
 * @test        @ID{redstmTest004} Verify the GetChannelState function.
 *
 * @details     This test verifies the behaviour of the GetChannelState function.
 *
 * Test steps:
 * - initialize the module for maximum number of channels
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the GetChannelState function with given test parameter
 * - verify if the state is set to Closed
 * .
 *
 * @testParameter
 * - Channel id: Redundancy channel identification
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Expected values                        ||
 * |----------|-----------------|--------------------|--------------------|
 * |          | Channel id      | Expect Fatal Error | Test Result        |
 * | 0        | 0               | False              | Normal operation   |
 * | 1        | 1               | False              | Normal operation   |
 * | 2        | 2               | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(GetChannelStateTestWithParam, redstmTest004VerifyGetChannelStateFunction)
{
  // initialize the module
  redstm_Init(UT_REDSTM_MAX_NUMBER_OF_RED_CHANNELS);

  if (expect_fatal_error)
  {
    // call the GetChannelState function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redstmTest::invalidArgumentException));
    EXPECT_THROW(redstm_GetChannelState(channel_id), std::invalid_argument);
  }
  else
  {
    // call the GetChannelState function and verify if state Closed is returned (first state after init)
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_EQ(redstm_kRedundancyChannelStateClosed, redstm_GetChannelState(channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetChannelStateFunction,
  GetChannelStateTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDSTM_MIN_ID_CHANNEL, false),
    std::make_tuple(UT_REDSTM_MAX_ID_CHANNEL, false),
    std::make_tuple((UT_REDSTM_MAX_ID_CHANNEL + 1), true)
  ));

/**
 * @test        @ID{redstmTest005} Verify the State NotInitialized.
 *
 * @details     This test verifies the state NotInitialized.
 *
 * Test steps:
 * - initialize the module for one channel
 * - set the current state to notInizialized
 * - verify that the current state is set correctly
 * - call the ProcessChannelStateMachine function with an open event
 * - verify that the state machine throws
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redstmTest, redstmTest005VerifyStateNotInitialized)
{
  const uint32_t channel_id                 = UT_REDSTM_MIN_ID_CHANNEL;
  redstm_RedundancyChannelEvents event      = redstm_kRedundancyChannelEventOpen;

  // initialize the module and set the state to NotInitialized
  InitModuleAndSetState(channel_id, redstm_kRedundancyChannelStateNotInitialized);

  // verify if the state is set correctly
  EXPECT_EQ(redstm_kRedundancyChannelStateNotInitialized, redstm_GetChannelState(channel_id));

  // process state machine and expect a fatal error
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInternalError)).WillOnce(Invoke(this, &redstmTest::invalidArgumentException));
  EXPECT_THROW(redstm_ProcessChannelStateMachine(channel_id, event), std::invalid_argument);
}

/**
 * @test        @ID{redstmTest006} Verify the State Closed.
 *
 * @details     This test verifies the state Closed and its transitions.
 *
 * Test steps:
 * - initialize the module for one channel
 * - set the current state to Closed
 * - verify that the current state is set correctly
 * - set expectations for open event
 * - call the ProcessChannelStateMachine function
 * - verify that the new state matches the expectation
 * .
 *
 * @testParameter
 * - Event: Event to be processed
 * - Expected state: State in what system is expected to be after processing event
 * .
 * | Test Run | Input parameter       | Expected values                     ||
 * |----------|-----------------------|----------------|---------------------|
 * |          | Event                 | Expected state | Test Result        |
 * | 0        | Close                 | Closed         | No state transition |
 * | 1        | Receive Data          | Closed         | No state transition |
 * | 2        | Send Data             | Closed         | No state transition |
 * | 3        | Timeout               | Closed         | No state transition |
 * | 4        | Open                  | Up             | Transition -> Up    |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(StateClosedTestWithParam, redstmTest006VerifyStateClosed)
{
  // initialize the module and set the state to Closed
  InitModuleAndSetState(channel_id, redstm_kRedundancyChannelStateClosed);

  // verify if the state is set correctly
  EXPECT_EQ(redstm_kRedundancyChannelStateClosed, redstm_GetChannelState(channel_id));

  if(event == redstm_kRedundancyChannelEventOpen)
  {
    // expect core mock to be called
    EXPECT_CALL(redcor_mock, redcor_InitRedundancyChannelData(channel_id)).Times(1);
  }

  // process state machine
  redstm_ProcessChannelStateMachine(channel_id, event);

  // verify if the state transition is correct
  EXPECT_EQ(next_state, redstm_GetChannelState(channel_id));
}

INSTANTIATE_TEST_SUITE_P(
  verifyStateClosed,
  StateClosedTestWithParam,
  ::testing::Values(
    std::make_tuple(redstm_kRedundancyChannelEventClose, redstm_kRedundancyChannelStateClosed),
    std::make_tuple(redstm_kRedundancyChannelEventReceiveData, redstm_kRedundancyChannelStateClosed),
    std::make_tuple(redstm_kRedundancyChannelEventSendData, redstm_kRedundancyChannelStateClosed),
    std::make_tuple(redstm_kRedundancyChannelEventDeferTimeout, redstm_kRedundancyChannelStateClosed),
    std::make_tuple(redstm_kRedundancyChannelEventOpen, redstm_kRedundancyChannelStateUp)
  ));

/**
 * @test        @ID{redstmTest007} Verify the State Up.
 *
 * @details     This test verifies the state Up and its transitions.
 *
 * Test steps:
 * - initialize the module for one channel
 * - set the current state to Up
 * - verify that the current state is set correctly
 * - set expectations for different events
 * - call the ProcessChannelStateMachine function
 * - verify that the new state matches the expectation
 * .
 *
 * @testParameter
 * - Event: Event to be processed
 * - Expected state: State in what system is expected to be after processing event
 * .
 * | Test Run | Input parameter       | Expected values                      ||
 * |----------|-----------------------|----------------|----------------------|
 * |          | Event                 | Expected state | Test Result          |
 * | 0        | Close                 | Closed         | Transition -> Closed |
 * | 1        | Receive Data          | Up             | No state transition  |
 * | 2        | Send Data             | Up             | No state transition  |
 * | 3        | Timeout               | Up             | No state transition  |
 * | 4        | Open                  | Up             | No state transition  |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(StateUpTestWithParam, redstmTest007VerifyStateUp)
{
  // initialize the module and set the state to Closed
  InitModuleAndSetState(channel_id, redstm_kRedundancyChannelStateUp);

  // verify if the state is set correctly
  EXPECT_EQ(redstm_kRedundancyChannelStateUp, redstm_GetChannelState(channel_id));

  // expect core mock to be called
  switch (event)
  {
    case redstm_kRedundancyChannelEventClose:
      EXPECT_CALL(redcor_mock, redcor_InitRedundancyChannelData(channel_id)).Times(1);
      break;
    case redstm_kRedundancyChannelEventOpen:
      // mock not called
      break;
    case redstm_kRedundancyChannelEventReceiveData:
      EXPECT_CALL(redcor_mock, redcor_ProcessReceivedMessage(channel_id)).Times(1);
      break;
    case redstm_kRedundancyChannelEventSendData:
      EXPECT_CALL(redcor_mock, redcor_SendMessage(channel_id)).Times(1);
      break;
    case redstm_kRedundancyChannelEventDeferTimeout:
      EXPECT_CALL(redcor_mock, redcor_DeferQueueTimeout(channel_id)).Times(1);
      break;
  }

  // process state machine
  redstm_ProcessChannelStateMachine(channel_id, event);

  // verify if the state transition is correct
  EXPECT_EQ(next_state, redstm_GetChannelState(channel_id));
}

INSTANTIATE_TEST_SUITE_P(
  verifyStateUp,
  StateUpTestWithParam,
  ::testing::Values(
    std::make_tuple(redstm_kRedundancyChannelEventClose, redstm_kRedundancyChannelStateClosed),
    std::make_tuple(redstm_kRedundancyChannelEventReceiveData, redstm_kRedundancyChannelStateUp),
    std::make_tuple(redstm_kRedundancyChannelEventSendData, redstm_kRedundancyChannelStateUp),
    std::make_tuple(redstm_kRedundancyChannelEventDeferTimeout, redstm_kRedundancyChannelStateUp),
    std::make_tuple(redstm_kRedundancyChannelEventOpen, redstm_kRedundancyChannelStateUp)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

void InitModuleAndSetState(uint32_t channel_id, redstm_RedundancyChannelStates state)
{
  extern redstm_RedundancyChannelStates redstm_redundancy_channel_states[RADEF_MAX_NUMBER_OF_RED_CHANNELS];

  redstm_Init(UT_REDSTM_MIN_NUMBER_OF_RED_CHANNELS);
  redstm_redundancy_channel_states[channel_id] = state;
}
