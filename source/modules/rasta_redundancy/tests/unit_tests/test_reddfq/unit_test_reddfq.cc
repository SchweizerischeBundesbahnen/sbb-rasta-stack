/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_reddfq.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version b4d4bd413ce5fa54d373eb00890116763cac504a
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 *
 * @brief Unit test file for the defer queue module of the redundancy layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_reddfq.hh"

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
 * - Number of redundancy channels (uint32_t)
 * - Defer queue size (uint32_t)
 * - Tseq in miliseconds (uint32_t)
 * - Expect fatal error (bool)
 * .
 */
class InitTestWithParam :   public reddfqTest,
                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, bool>>
{
  public:
  uint32_t redundancy_channels =                  std::get<0>(GetParam());        ///< number of configured redundancy channels
  uint32_t queue_size =                           std::get<1>(GetParam());        ///< configured defer queue size
  uint32_t t_sequence =                           std::get<2>(GetParam());        ///< configured Tseq (ms)
  bool expect_fatal_error =                       std::get<3>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized InitDeferQueue test
 *
 * Parameter order:
 * - Number of redundancy channels (uint32_t)
 * - Channel id to init (uint32_t)
 * - Expect fatal error (bool)
 * .
 */
class InitDeferQueueTestWithParam :   public reddfqTest,
                                      public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>>
{
  public:
  uint32_t redundancy_channels =                  std::get<0>(GetParam());        ///< number of configured redundancy channels
  uint32_t channel_id =                           std::get<1>(GetParam());        ///< channel id to init
  bool expect_fatal_error =                       std::get<2>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - Channel Id (uint32_t)
 * - Message.message_size (uint16_t)
 * - Test data pointer (uint8_t *)
 * - Configured Queue size (uint32_t)
 * - Messages to add (uint32_t)
 * - Expect fatal error (bool)
 * .
 */
class AddMessageTestWithParam :   public reddfqTest,
                                  public testing::WithParamInterface<std::tuple<uint32_t, uint16_t, uint8_t *, uint32_t, uint32_t, bool>>
{
  public:
  uint32_t channel_id =                           std::get<0>(GetParam());        ///< channel id
  redtyp_RedundancyMessage message =             {std::get<1>(GetParam()),{}};    ///< message
  uint8_t * data_ptr =                            std::get<2>(GetParam());        ///< pointer to the test data
  uint32_t configured_queue_size =                std::get<3>(GetParam());        ///< configured defer queue size
  uint32_t messages_to_add =                      std::get<4>(GetParam());        ///< amount of messages to add
  bool expect_fatal_error =                       std::get<5>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - Channel Id (uint32_t)
 * - Message seq nbr of message to add (uint32_t)
 * - Message seq nbr of message to read (uint32_t)
 * - Indicates if a message gets added to the buffer (bool)
 * - Indicates if redmsg module returns correct seq nbr (bool)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class GetMessageTestWithParam :   public reddfqTest,
                                  public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, bool, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t channel_id =                           std::get<0>(GetParam());        ///< channel id
  uint32_t message_seq_to_add =                   std::get<1>(GetParam());        ///< seq of message to add
  uint32_t message_seq_to_read =                  std::get<2>(GetParam());        ///< seq of message to get
  bool add_message_to_buffer =                    std::get<3>(GetParam());        ///< indicates if a message gets added to the buffer
  bool return_correct_msg_seq_nbr =               std::get<4>(GetParam());        ///< indicates if redmsg module returns correct seq nbr
  radef_RaStaReturnCode expected_return_code =    std::get<5>(GetParam());        ///< expected rasta return code
};

/**
 * @brief Test class for value-parametrized IsTimeout test
 *
 * Parameter order:
 * - Configured timeout (uint32_t)
 * - Timer value to return on check (uint32_t)
 * - Add message (bool)
 * - Expect timeout (bool)
 * .
 */
class IsTimeoutTestWithParam :    public reddfqTest,
                                  public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool, bool>>
{
  public:
  uint32_t configured_timeout =                   std::get<0>(GetParam());        ///< configured timeout on init
  uint32_t timer_value_to_return =                std::get<1>(GetParam());        ///< timer value to return on timeoutcheck
  bool add_message =                              std::get<2>(GetParam());        ///< add message to queue before check
  bool expect_timeout =                           std::get<3>(GetParam());        ///< expect timeout
};

/**
 * @brief Test class for value-parametrized Contains test
 *
 * Parameter order:
 * - Add message (bool)
 * - Channel id to add message (uint32_t)
 * - Channel id to get message (uint32_t)
 * - Seq nbr of message to add (uint32_t)
 * - Seq nbr of message to get (uint32_t)
 * - Expect contains (bool)
 * - Expect fatal error (bool)
 * .
 */
class ContainsTestWithParam :     public reddfqTest,
                                  public testing::WithParamInterface<std::tuple<bool, uint32_t, uint32_t, uint32_t, uint32_t, bool, bool>>
{
  public:
  bool add_message =                              std::get<0>(GetParam());        ///< Add message to queue
  uint32_t add_message_channel_id =               std::get<1>(GetParam());        ///< Channel id to add message to
  uint32_t get_message_channel_id =               std::get<2>(GetParam());        ///< Channel id to get message from
  uint32_t seq_nbr_to_add =                       std::get<3>(GetParam());        ///< Sequence number to add to message
  uint32_t seq_nbr_to_get =                       std::get<4>(GetParam());        ///< Sequence number to try find
  bool expect_contains =                          std::get<5>(GetParam());        ///< Expect if queue contains message
  bool expect_fatal_error =                       std::get<6>(GetParam());        ///< Indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized GetOldestSequenceNumber test
 *
 * Parameter order:
 * - Channel id to add messages (uint32_t)
 * - Channel id to get min seq nbr (uint32_t)
 * - Seq nbr to start at (uint32_t)
 * - Seq nbr step (uint32_t)
 * - Messages to add (uint32_t)
 * - Read message (bool)
 * - Seq nbr of message to read (uint32_t)
 * - Expect seq nbr (uint32_t)
 * - Expect error code (radef_RaStaReturnCode)
 * .
 */
class GetOldestSequenceNumberTestWithParam :    public reddfqTest,
                                                public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool,  uint32_t, uint32_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t add_seq_nbr_channel_id =               std::get<0>(GetParam());        ///< Channel id to add messages with seq nbrs
  uint32_t get_seq_nbr_channel_id =               std::get<1>(GetParam());        ///< Channel id to get min seq nbr
  uint32_t seq_nbr_to_start =                     std::get<2>(GetParam());        ///< Sequence number to start at
  uint32_t seq_nbr_step =                         std::get<3>(GetParam());        ///< Seq nbr step to next nbr
  uint32_t messages_to_add =                      std::get<4>(GetParam());        ///< Amount of messages to add
  bool read_message =                             std::get<5>(GetParam());        ///< Indicates if a message gets read
  uint32_t seq_nbr_to_read =                      std::get<6>(GetParam());        ///< Seq nbr of message to read
  uint32_t expect_seq_nbr =                       std::get<7>(GetParam());        ///< Expect seq nbr to return
  radef_RaStaReturnCode exp_error_code =          std::get<8>(GetParam());        ///< Expected return code
};

/**
 * @brief Test class for value-parametrized GetUsedEntries test
 *
 * Parameter order:
 * - Channel id to add messages (uint32_t)
 * - Channel id get used entries (uint32_t)
 * - Messages to add (uint32_t)
 * - Expected used entries (uint32_t)
 * - Expect fatal error (bool)
 * .
 */
class GetUsedEntriesTestWithParam :     public reddfqTest,
                                        public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, bool>>
{
  public:
  uint32_t add_msg_channel_id =                   std::get<0>(GetParam());        ///< Channel id to add messages
  uint32_t get_used_entries_channel_id =          std::get<1>(GetParam());        ///< Channel id to get used entries of
  uint32_t messages_to_add =                      std::get<2>(GetParam());        ///< Amount of messages to add
  uint32_t expect_used_entries =                  std::get<3>(GetParam());        ///< Expected used entries to return
  bool expect_fatal_error =                       std::get<4>(GetParam());        ///< Indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup red_deferQueue
 * @{
 */

/**
 * @test        @ID{reddfqTest001} Verify the init function.
 *
 * @details     This test verifies that the init function sets the check code type
 *              parameter correctly.
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the init function with given test parameter
 * - verify if the values are set correctly
 * .
 *
 * @testParameter
 * - Redundancy channels: Number of configured redundancy channels
 * - Queue size: Configured defer queue size (in messages) (valid range: 4 - 10)
 * - Tseq: Configured Tseq (in ms) (valid range: 50 - 500)
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                           ||| Expected values                        ||
 * |----------|---------------------|----------------|------|--------------------|--------------------|
 * |          | Redundancy channels | Queue size     | Tseq | Expect Fatal Error | Test Result        |
 * | 0        | 1                   | 4U             | 50U  | false              | Normal operation   |
 * | 1        | 2                   | 10U            | 500U | false              | Normal operation   |
 * | 2        | 0                   | 4U             | 50U  | true               | Fatal error raised |
 * | 3        | 1                   | 3U             | 50U  | true               | Fatal error raised |
 * | 4        | 1                   | 4U             | 49U  | true               | Fatal error raised |
 * | 5        | 3                   | 10U            | 500U | true               | Fatal error raised |
 * | 6        | 2                   | 11U            | 500U | true               | Fatal error raised |
 * | 7        | 2                   | 10U            | 501U | true               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitTestWithParam, reddfqTest001VerifyInitFunction)
{
  extern uint32_t reddfq_number_of_red_channels;
  extern uint32_t reddfq_defer_queue_size;
  extern uint32_t reddfq_t_seq;

  // perform test
  if(expect_fatal_error)
  {
    // call the init function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &reddfqTest::invalidArgumentException));
    EXPECT_THROW(reddfq_Init(redundancy_channels, queue_size, t_sequence), std::invalid_argument);
    EXPECT_FALSE(reddfq_initialized);
  }
  else
  {
    // call the init function and verify if parameters are set correctly
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0U);
    EXPECT_NO_THROW(reddfq_Init(redundancy_channels, queue_size, t_sequence));

    EXPECT_EQ(reddfq_number_of_red_channels, redundancy_channels);
    EXPECT_EQ(reddfq_defer_queue_size, queue_size);
    EXPECT_EQ(reddfq_t_seq, t_sequence);
    EXPECT_TRUE(reddfq_initialized);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitFunction,
  InitTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MIN, UT_REDDFQ_QUEUE_SIZE_MIN, UT_REDDFQ_T_SEQ_MIN, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MAX, UT_REDDFQ_QUEUE_SIZE_MAX, UT_REDDFQ_T_SEQ_MAX, false),
    std::make_tuple((UT_REDDFQ_CHANNEL_NUM_MIN - 1U), UT_REDDFQ_QUEUE_SIZE_MIN, UT_REDDFQ_T_SEQ_MIN, true),
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MIN, (UT_REDDFQ_QUEUE_SIZE_MIN - 1U), UT_REDDFQ_T_SEQ_MIN, true),
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MIN, UT_REDDFQ_QUEUE_SIZE_MIN, (UT_REDDFQ_T_SEQ_MIN - 1U), true),
    std::make_tuple((UT_REDDFQ_CHANNEL_NUM_MAX + 1U), UT_REDDFQ_QUEUE_SIZE_MAX, UT_REDDFQ_T_SEQ_MAX , true),
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MAX, (UT_REDDFQ_QUEUE_SIZE_MAX + 1U), UT_REDDFQ_T_SEQ_MAX , true),
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MAX, UT_REDDFQ_QUEUE_SIZE_MAX, (UT_REDDFQ_T_SEQ_MAX  + 1U), true)
  )
);

/**
 * @test        @ID{reddfqTest002} Verify the number of init function calls.
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
TEST_F(reddfqTest, reddfqTest002VerifyInitFunctionCalls)
{
  // test variables
  uint32_t red_channels = UT_REDDFQ_CHANNEL_NUM_MIN;
  uint32_t queue_size = UT_REDDFQ_QUEUE_SIZE_MIN;
  uint32_t t_seq = UT_REDDFQ_T_SEQ_MIN;

  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &reddfqTest::invalidArgumentException));

  // call the init function and verify that it completes successful
  EXPECT_NO_THROW(reddfq_Init(red_channels, queue_size, t_seq));

  // call the init function again and verify that an assertion violations is raised as the module is already initialized
  EXPECT_THROW(reddfq_Init(red_channels, queue_size, t_seq), std::invalid_argument);
}

/**
 * @test        @ID{reddfqTest003} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - init module
 * - call the reddfq_AddMessage function with invalid payload pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the reddfq_GetMessage function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(reddfqTest, reddfqTest003VerifyNullPointerCheck)
{
  uint32_t red_channels               = UT_REDDFQ_CHANNEL_NUM_MIN;
  uint32_t queue_size                 = UT_REDDFQ_QUEUE_SIZE_MIN;
  uint32_t t_seq                      = UT_REDDFQ_T_SEQ_MIN;
  uint32_t channel_id                 = UT_REDDFQ_CHANNEL_ID_MIN;

  // test variables
  redtyp_RedundancyMessage message    = {};
  uint32_t sequence_number            = 0U;
  uint8_t nbr_of_function_calls       = 2U;                    // number of function calls

  // init module
  reddfq_Init(red_channels, queue_size, t_seq);

  // set expectations
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
    .Times(nbr_of_function_calls)
    .WillRepeatedly(Invoke(this, &reddfqTest::invalidArgumentException));

  // perform test
  EXPECT_THROW(reddfq_AddMessage(channel_id, (redtyp_RedundancyMessage *)NULL), std::invalid_argument);
  EXPECT_THROW(reddfq_GetMessage(channel_id, sequence_number, (redtyp_RedundancyMessage *)NULL), std::invalid_argument);
}

/**
 * @test        @ID{reddfqTest004} Initialization checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the reddfq_InitDeferQueue function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the reddfq_AddMessage function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the reddfq_Contains function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the reddfq_GetMessage function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the reddfq_GetOldestSequenceNumber function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the reddfq_GetUsedEntries function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the reddfq_IsTimeout function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(reddfqTest, reddfqTest004VerifyInitializationCheck)
{
  uint8_t nbr_of_function_calls       = 7U;        // number of function calls
  reddfq_initialized                  = false;

  // test variables
  uint32_t channel_id                 = UT_REDDFQ_CHANNEL_ID_MIN;
  uint32_t sequence_number            = 0U;
  redtyp_RedundancyMessage message    = {};

  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
    .Times(nbr_of_function_calls)
    .WillRepeatedly(Invoke(this, &reddfqTest::invalidArgumentException));

  // perform test
  EXPECT_THROW(reddfq_InitDeferQueue(channel_id), std::invalid_argument);
  EXPECT_THROW(reddfq_AddMessage(channel_id, &message), std::invalid_argument);
  EXPECT_THROW(reddfq_Contains(channel_id, sequence_number), std::invalid_argument);
  EXPECT_THROW(reddfq_GetMessage(channel_id, sequence_number, &message), std::invalid_argument);
  EXPECT_THROW(reddfq_GetOldestSequenceNumber(channel_id), std::invalid_argument);
  EXPECT_THROW(reddfq_GetUsedEntries(channel_id), std::invalid_argument);
  EXPECT_THROW(reddfq_IsTimeout(channel_id), std::invalid_argument);
}

/**
 * @test        @ID{reddfqTest005} verify the InitDeferQueue function.
 *
 * @details     This test verifies the correct behaviour of the InitDeferQueue function.
 *
 * Test steps:
 * - init module
 * - call the reddfq_AddMessage function and verify message is added
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the InitDeferQueue function for given channel
 * - verify no message in queue
 * .
 *
 * @testParameter
 * - Channel id: Redundancy channel identification
 * - Redundancy channels: Number of configured redundancy channels
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config         | Expected values                        ||
 * |----------|-----------------|---------------------|--------------------|--------------------|
 * |          | Channel id      | Redundancy channels | Expect Fatal Error | Test Result        |
 * | 0        | 0U              | 1                   | False              | Normal operation   |
 * | 1        | 1U              | 1                   | True               | Fatal error raised |
 * | 2        | 0U              | 2                   | False              | Normal operation   |
 * | 3        | 1U              | 2                   | False              | Normal operation   |
 * | 4        | 2U              | 2                   | True               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitDeferQueueTestWithParam, reddfqTest005VerifyInitDeferQueueFunction)
{
  // test variables
  uint32_t seq_nbr = 100;

  // create message and add seq nbr to it
  redtyp_RedundancyMessage message = {UT_REDDFQ_MSG_LEN_MAX, {}};
  setDataInBuffer_u32(&message.message, seq_nbr, expected_sequence_nbr_pos);

  // init module
  reddfq_Init(redundancy_channels, UT_REDDFQ_QUEUE_SIZE_MIN, UT_REDDFQ_T_SEQ_MIN);

  // perform test
  if(expect_fatal_error)
  {
    // call the InitDeferQueue function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &reddfqTest::invalidArgumentException));
    EXPECT_THROW(reddfq_InitDeferQueue(channel_id), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    // expects for Contains and AddMessage call
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(1);
    EXPECT_CALL(redmsg_mock, redmsg_GetMessageSequenceNumber(_)).WillOnce(Invoke(this, &reddfqTest::GetMessageSequenceNumber));

    // add message
    EXPECT_NO_THROW(reddfq_AddMessage(channel_id, &message));

    // verify message added
    EXPECT_EQ(1, reddfq_GetUsedEntries(channel_id));
    EXPECT_TRUE(reddfq_Contains(channel_id, seq_nbr));

    // call the InitDeferQueue function
    EXPECT_NO_THROW(reddfq_InitDeferQueue(channel_id));

    // verify defer queue was reset at init
    EXPECT_EQ(0, reddfq_GetUsedEntries(channel_id));
    EXPECT_FALSE(reddfq_Contains(channel_id, seq_nbr));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitDeferQueueFunction,
  InitDeferQueueTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MIN, UT_REDDFQ_CHANNEL_ID_MIN, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MIN, UT_REDDFQ_CHANNEL_ID_MAX, true),
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MAX, UT_REDDFQ_CHANNEL_ID_MIN, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MAX, UT_REDDFQ_CHANNEL_ID_MAX, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_NUM_MAX, (UT_REDDFQ_CHANNEL_ID_MAX + 1), true)
  )
);

/**
 * @test        @ID{reddfqTest006} Verify the AddMessage function.
 *
 * @details     This test verifies the correct behaviour of the AddMessage function.
 *
 * Test steps:
 * - Initialize module with minimal channel amount and queue size from given parameters and verify it
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Add messages according parameter
 * - Check if message is in Queue with its sequence number
 * - Check if used entries confirms added message
 * .
 *
 * @testParameter
 * - Channel Id: Redundancy channel identification
 * - Message size: Message size in bytes (valid range: 36 - 1113)
 * - Data pointer: Pointer to the test data
 * - Configured queue size: Configured defer queue size (messages)
 * - Messages to add: Number of messages to add
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter          || Test config                                                       ||| Expected values                        ||
 * |----------|------------|--------------|---------------------------|-----------------------|-----------------|--------------------|--------------------|
 * |          | Channel Id | Message size | Data pointer              | Configured queue size | Messages to add | Expect Fatal Error | Test Result        |
 * | 0        | 0          | 35U          | test_red_message_data_u8  | 4U                    | 1               | True               | Fatal error raised |
 * | 1        | 0          | 36U          | test_red_message_data_u8  | 4U                    | 1               | False              | Normal operation   |
 * | 2        | 0          | 1113U        | test_red_message_data_u8  | 4U                    | 4               | False              | Normal operation   |
 * | 3        | 0          | 1113U        | test_red_message_data_u8  | 4U                    | 5               | False              | Normal operation   |
 * | 4        | 1          | 1113U        | test_red_message_data_u8  | 10U                   | 10              | False              | Normal operation   |
 * | 5        | 1          | 1113U        | test_red_message_data_u8  | 10U                   | 11              | False              | Normal operation   |
 * | 6        | 1          | 1114U        | test_red_message_data_u8  | 4U                    | 4               | True               | Fatal error raised |
 * | 7        | 2          | 1113U        | test_red_message_data_u8  | 4U                    | 4               | True               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(AddMessageTestWithParam, reddfqTest006VerifyAddMessageFunction)
{
  extern uint32_t reddfq_defer_queue_size;

  uint16_t i                    = 0U;               // loop iterator
  uint32_t message_seq_nbr      = 1U;               // message sequence number to set and check in queue

  // expected values
  uint32_t expected_used_entries  = 0U;

  // configure used entries regarding on messages to add
  if(messages_to_add <= configured_queue_size)
  {
    expected_used_entries = messages_to_add;
  }
  else
  {
    expected_used_entries = configured_queue_size;
  }

  // initialize module
  reddfq_Init(UT_REDDFQ_CHANNEL_NUM_MAX, configured_queue_size, UT_REDDFQ_T_SEQ_MIN);

  // verify initialization
  EXPECT_TRUE(reddfq_initialized);
  EXPECT_EQ(configured_queue_size, reddfq_defer_queue_size);

  // perform test
  if(expect_fatal_error)
  {
    // call the AddMessage function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &reddfqTest::invalidArgumentException));
    EXPECT_THROW(reddfq_AddMessage(channel_id, &message), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(expected_used_entries);
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    for(i = 0U; i < messages_to_add; i++)
    {
      // set message sequence nbr
      setDataInBuffer_u32(&message.message, message_seq_nbr, expected_sequence_nbr_pos);

      // call the AddMessage function
      EXPECT_NO_THROW(reddfq_AddMessage(channel_id, &message));

      message_seq_nbr++;
    }

    EXPECT_EQ(expected_used_entries, reddfq_GetUsedEntries(channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyAddMessageFunction,
  AddMessageTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, (UT_REDDFQ_MSG_LEN_MIN - 1U), test_red_message_data_u8, UT_REDDFQ_QUEUE_SIZE_MIN, 1U, true),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_MSG_LEN_MIN, test_red_message_data_u8, UT_REDDFQ_QUEUE_SIZE_MIN, 1U, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_MSG_LEN_MAX, test_red_message_data_u8, UT_REDDFQ_QUEUE_SIZE_MIN, 4U, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_MSG_LEN_MAX, test_red_message_data_u8, UT_REDDFQ_QUEUE_SIZE_MIN, 5U, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_MSG_LEN_MAX, test_red_message_data_u8, UT_REDDFQ_QUEUE_SIZE_MAX, 10U, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_MSG_LEN_MAX, test_red_message_data_u8, UT_REDDFQ_QUEUE_SIZE_MAX, 11U, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, (UT_REDDFQ_MSG_LEN_MAX + 1U), test_red_message_data_u8, UT_REDDFQ_QUEUE_SIZE_MIN, 4U, true),
    std::make_tuple((UT_REDDFQ_CHANNEL_ID_MAX + 1), UT_REDDFQ_MSG_LEN_MAX, test_red_message_data_u8, UT_REDDFQ_QUEUE_SIZE_MIN, 4U, true)
  )
);

/**
 * @test        @ID{reddfqTest007} Verify the GetMessage function.
 *
 * @details     This test verifies the correct behaviour of the GetMessage function.
 *
 * Test steps:
 * - Initialize module
 * - Add message if given by test parameter
 * - Call GetMessage with given seq nbr parameter
 * - Verify message not in queue anymore
 * - Verify used entries
 * .
 *
 * @testParameter
 * - Channel Id: Redundancy channel identification
 * - Sequence nbr of msg to read: Sequence number of the message to read and remove from the queue
 * - Add message to buffer: Indicates if a message gets added to the buffer
 * - Sequence nbr of msg to add: Sequence number of the message to add to the queue
 * - Return correct seq nbr: Indicates if redmsg module returns correct seq nbr
 * - Expect return code: Expected return code when calling the function
 * .
 * | Test Run | Input parameter                         || Test config                                                               ||| Expected values                                  ||
 * |----------|------------|-----------------------------|-----------------------|----------------------------|------------------------|------------------------------|--------------------|
 * |          | Channel Id | Sequence nbr of msg to read | Add message to buffer | Sequence nbr of msg to add | Return correct seq nbr |  Expect return code          | Test Result        |
 * | 0        | 0          | 0U                          | True                  | 0U                         | True                   | radef_kNoError               | Normal operation   |
 * | 1        | 1          | 0xffffffffU                 | True                  | 0xffffffffU                | True                   | radef_kNoError               | Normal operation   |
 * | 2        | 2          | 10U                         | False                 | 10U                        | False                  | radef_kInvalidParameter      | Fatal error raised |
 * | 3        | 0          | 10U                         | True                  | 10U                        | False                  | radef_kInvalidSequenceNumber | Fatal error raised |
 * | 4        | 0          | 15U                         | True                  | 10U                        | True                   | radef_kInvalidSequenceNumber | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(GetMessageTestWithParam, reddfqTest007VerifyGetMessageFunction)
{
  extern uint32_t reddfq_defer_queue_size;

  uint16_t i                                    = 0U;                            // loop iterator
  uint8_t * data_ptr                            = test_red_message_data_u8;      // test message data
  redtyp_RedundancyMessage message_to_read      = {0U, {}};                      // test message to read from queue
  redtyp_RedundancyMessage message_to_add       = {UT_REDDFQ_MSG_LEN_MAX, {}};   // test message to add to queue

  // copy test message
  for (i = 0U; i < message_to_add.message_size; i++)
  {
    message_to_add.message[i] = data_ptr[i];
  }

  // set message to add sequence nbr
  setDataInBuffer_u32(&message_to_add.message, message_seq_to_add, expected_sequence_nbr_pos);

  // initialize module
  reddfq_Init(UT_REDDFQ_CHANNEL_NUM_MAX, UT_REDDFQ_QUEUE_SIZE_MIN, UT_REDDFQ_T_SEQ_MIN);

  // verify initialization
  EXPECT_TRUE(reddfq_initialized);

  if(expected_return_code != radef_kInvalidParameter)
  {
    if(return_correct_msg_seq_nbr)
    {
      EXPECT_CALL(redmsg_mock, redmsg_GetMessageSequenceNumber(_)).WillOnce(Invoke(this, &reddfqTest::GetMessageSequenceNumber));
    }
    else
    {
      EXPECT_CALL(redmsg_mock, redmsg_GetMessageSequenceNumber(_)).WillOnce(Return(message_seq_to_read + 1));
    }

    if(add_message_to_buffer)
    {
      EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(1);

      // add message to queue
      EXPECT_NO_THROW(reddfq_AddMessage(channel_id, &message_to_add));
    }
  }

  // perform test
  if(expected_return_code != radef_kNoError)
  {
    // call the GetMessage function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_return_code)).WillOnce(Invoke(this, &reddfqTest::invalidArgumentException));
    EXPECT_THROW(reddfq_GetMessage(channel_id, message_seq_to_read, &message_to_read), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    // call the GetMessage function
    EXPECT_NO_THROW(reddfq_GetMessage(channel_id, message_seq_to_read, &message_to_read));

    // verify message removed from queue
    EXPECT_FALSE(reddfq_Contains(channel_id, message_seq_to_read));

    EXPECT_EQ(0, reddfq_GetUsedEntries(channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetMessageFunction,
  GetMessageTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, 0U, 0U, true, true, radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UINT32_MAX, UINT32_MAX, true, true, radef_kNoError),
    std::make_tuple((UT_REDDFQ_CHANNEL_ID_MAX + 1), 10U, 10U, false, false, radef_kInvalidParameter),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, 10U, 10U, true, false, radef_kInvalidSequenceNumber),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, 10U, 15U, true, true, radef_kInvalidSequenceNumber)
  )
);

/**
 * @test        @ID{reddfqTest008} Verify the IsTimeout function.
 *
 * @details     This test verifies the correct behaviour of the IsTimeout function.
 *
 * Test steps:
 * - Initialize module with max channels, max queue size and given timeout
 * - Set timer return value to 0 for message to add
 * - Add message
 * - Set timer return value at check according to parameter
 * - Verify if timeout occured on channel 0
 * .
 *
 * @testParameter
 * - Configured timeout: Configured Tseq [ms]
 * - Time value to return: Return value of rasys_GetTimerValue call
 * - Add message: Indicates if a message get added to queue before check
 * - Expect timeout: Indicates if a timeout is expected
 * .
 * | Test Run | Test config                                           ||| Expected values                  ||
 * |----------|--------------------|----------------------|-------------|----------------|------------------|
 * |          | Configured timeout | Time value to return | Add message | Expect timeout | Test Result      |
 * | 0        | 50U                | 49U                  | True        | False          | Normal operation |
 * | 1        | 50U                | 50U                  | True        | False          | Normal operation |
 * | 2        | 50U                | 51U                  | True        | True           | Normal operation |
 * | 3        | 50U                | 51U                  | False       | False          | Normal operation |
 * | 4        | 500U               | 499U                 | True        | False          | Normal operation |
 * | 5        | 500U               | 500U                 | True        | False          | Normal operation |
 * | 6        | 500U               | 501U                 | True        | True           | Normal operation |
 * | 7        | 500U               | 501U                 | False       | False          | Normal operation |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(IsTimeoutTestWithParam, reddfqTest008VerifyIsTimeoutFunction)
{
  extern uint32_t reddfq_t_seq;
  uint16_t i                             = 0U;                            // loop iterator
  uint32_t start_timer_value             = 0U;                            // starting value of timer when adding message
  redtyp_RedundancyMessage message       = {UT_REDDFQ_MSG_LEN_MAX, {}};   // test message to add to queue
  uint32_t channel_id                    = UT_REDDFQ_CHANNEL_ID_MIN;

  // set expectation
  EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

  // init module
  reddfq_Init(UT_REDDFQ_CHANNEL_NUM_MIN, UT_REDDFQ_QUEUE_SIZE_MAX, configured_timeout);

  // Verify timeout set correctly
  EXPECT_EQ(reddfq_t_seq, configured_timeout);

  // copy test message
  for (i = 0U; i < message.message_size; i++)
  {
    message.message[i] = test_red_message_data_u8[i];
  }

  if(add_message){
    // Expecting get timer value call and set return value on add message and on check
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue())
      .WillOnce(Return(start_timer_value))
      .WillOnce(Return(timer_value_to_return));

    // add message
    reddfq_AddMessage(channel_id, &message);
  }

  // call the IsTimeout function and verify if parameters are set correctly
  EXPECT_EQ(expect_timeout, reddfq_IsTimeout(channel_id));
}

INSTANTIATE_TEST_SUITE_P(
  verifyIsTimeoutFunction,
  IsTimeoutTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDDFQ_T_SEQ_MIN, (UT_REDDFQ_T_SEQ_MIN - 1U), true, false),
    std::make_tuple(UT_REDDFQ_T_SEQ_MIN, UT_REDDFQ_T_SEQ_MIN, true, false),
    std::make_tuple(UT_REDDFQ_T_SEQ_MIN, (UT_REDDFQ_T_SEQ_MIN + 1U), true, true),
    std::make_tuple(UT_REDDFQ_T_SEQ_MIN, (UT_REDDFQ_T_SEQ_MIN + 1U), false, false),
    std::make_tuple(UT_REDDFQ_T_SEQ_MAX, (UT_REDDFQ_T_SEQ_MAX - 1U), true, false),
    std::make_tuple(UT_REDDFQ_T_SEQ_MAX, UT_REDDFQ_T_SEQ_MAX, true, false),
    std::make_tuple(UT_REDDFQ_T_SEQ_MAX, (UT_REDDFQ_T_SEQ_MAX + 1U), true, true),
    std::make_tuple(UT_REDDFQ_T_SEQ_MAX, (UT_REDDFQ_T_SEQ_MAX + 1U), false, false)
  )
);

/**
 * @test        @ID{reddfqTest009} Verify the Contains function.
 *
 * @details     This test verifies the correct behaviour of the Contains function.
 *
 * Test steps:
 * - Initialize module with max channels, max queue size and max timeout
 * - Add message with configured seq nbr to given channel if parameters set
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call contain function on channel with specified msg seq nr
 * - Verify if expectation of contain parameter is met
 * .
 *
 * @testParameter
 * - Channel id of msg to get: Redundancy channel identification
 * - Seq nbr of msg to get: Sequence number of the message to search
 * - Add msg: Indicates if a message gets added to queue
 * - Channel id of msg to add: Redundancy channel identification of message to add
 * - Seq nbr of msg to add: Sequence number of the message to add
 * - Expect contains: Indicates if Redundancy channel contains message
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                 || Test config                                              ||| Expected values                                         |||
 * |----------|--------------------------|-----------------------|---------|--------------------------|-----------------------|-----------------|--------------------|--------------------|
 * |          | Channel id of msg to get | Seq nbr of msg to get | Add msg | Channel id of msg to add | Seq nbr of msg to add | Expect contains | Expect fatal error | Test Result        |
 * | 0        | 0U                       | 0U                    | True    | 0U                       | 0U                    | True            | False              | Normal operation   |
 * | 1        | 0U                       | 0U                    | False   | 0U                       | 0U                    | False           | False              | Normal operation   |
 * | 2        | 0U                       | 1U                    | True    | 0U                       | 0U                    | False           | False              | Normal operation   |
 * | 3        | 1U                       | 0xffffffffU           | True    | 1U                       | 0xffffffffU           | True            | False              | Normal operation   |
 * | 4        | 1U                       | 0U                    | True    | 0U                       | 0U                    | False           | False              | Normal operation   |
 * | 5        | 2U                       | 0U                    | True    | 1U                       | 0U                    | True            | True               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(ContainsTestWithParam, reddfqTest009VerifyContainsFunction)
{
  // Test variables
  redtyp_RedundancyMessage message_to_add = {UT_REDDFQ_MSG_LEN_MAX,{}};

  // Init Module
  reddfq_Init(UT_REDDFQ_CHANNEL_NUM_MAX, UT_REDDFQ_QUEUE_SIZE_MAX, UT_REDDFQ_T_SEQ_MAX);

  if(add_message)
  {
    // expect timer to be called at adding message
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(1U);

    if(add_message_channel_id == get_message_channel_id)
    {
      // expect get sequence nbr call in contains function (only if queue contains message)
      EXPECT_CALL(redmsg_mock, redmsg_GetMessageSequenceNumber(_)).Times(1).WillOnce(Invoke(this, &reddfqTest::GetMessageSequenceNumber));
    }

    // set message sequence nbr and verify
    setDataInBuffer_u32(&message_to_add.message, seq_nbr_to_add, expected_sequence_nbr_pos);
    EXPECT_EQ(seq_nbr_to_add, getDataFromBuffer_u32(&message_to_add.message, expected_sequence_nbr_pos));

    // call the AddMessage function
    EXPECT_NO_THROW(reddfq_AddMessage(add_message_channel_id, &message_to_add));

  }

  // perform test
  if(expect_fatal_error)
  {
    // call the Contains function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &reddfqTest::invalidArgumentException));
    EXPECT_THROW(reddfq_Contains(get_message_channel_id, seq_nbr_to_get), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_EQ(expect_contains, reddfq_Contains(get_message_channel_id, seq_nbr_to_get));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyContainsFunction,
  ContainsTestWithParam,
  ::testing::Values(
    std::make_tuple(true, UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MIN, 0U, 0U, true, false),
    std::make_tuple(false, UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MIN, 0U, 0U, false, false),
    std::make_tuple(true, UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MIN, 0U, 1U, false, false),
    std::make_tuple(true, UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_CHANNEL_ID_MAX, UINT32_MAX, UINT32_MAX, true, false),
    std::make_tuple(true, UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MAX, 0U, 0U, false, false),
    std::make_tuple(true, UT_REDDFQ_CHANNEL_ID_MAX, (UT_REDDFQ_CHANNEL_ID_MAX + 1U), 0U, 0U, true, true)
  )
);

/**
 * @test        @ID{reddfqTest010} Verify the GetOldestSequenceNumber function.
 *
 * @details     This test verifies the correct behaviour of the GetOldestSequenceNumber function.
 *
 * Test steps:
 * - Initialize module with max channels, max queue size and max timeout
 * - Add message(s) with configured seq nbr to given channel if parameter set
 * - Read message if parameter given
 * - Call get oldest sequence number function on specified channel
 * - Verify if expectation of expected seq number parameter is met
 * .
 *
 * @testParameter
 * - Channel id of seq nbr to get: Redundancy channel identification
 * - Channel id of msg to add: Redundancy channel identification to add message
 * - Seq nbr start: Sequence number to start adding to messages
 * - Seq nbr step: Step to make for next sequence number
 * - Messages to add: Number of messages to add
 * - Read msg: Indicates if a message gets read
 * - Seq nbr of msg to read: Sequence number of the message to read
 * - Expect oldest seq nbr: Expected return value of GetOldestSequenceNumber call
 * - Expect error code: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter              | Test config                                                                                              |||||| Expected values                                                    |||
 * |----------|------------------------------|--------------------------|---------------|--------------|-----------------|----------|------------------------|-----------------------|-------------------------|--------------------|
 * |          | Channel id of seq nbr to get | Channel id of msg to add | Seq nbr start | Seq nbr step | Messages to add | Read msg | Seq nbr of msg to read | Expect oldest seq nbr | Expect error code       | Test Result        |
 * |        0 |                           0U |                       0U |             0 |            0 |               0 |    false |                      0 |                     0 | radef_kDeferQueueEmpty  | Fatal error raised |
 * |        1 |                           0U |                       0U |             0 |            0 |               0 |    false |                      0 |                     0 | radef_kDeferQueueEmpty  | Fatal error raised |
 * |        2 |                           0U |                       0U |             0 |            0 |               1 |    false |                      0 |                     0 | radef_kNoError          | Normal operation   |
 * |        3 |                           0U |                       0U |             0 |            1 |               2 |    false |                      0 |                     0 | radef_kNoError          | Normal operation   |
 * |        4 |                           0U |                       0U |             1 |            1 |               2 |    false |                      0 |                     1 | radef_kNoError          | Normal operation   |
 * |        5 |                           0U |                       0U |             0 |            1 |               2 |     true |                      0 |                     1 | radef_kNoError          | Normal operation   |
 * |        6 |                           1U |                       1U |             0 |            1 |              10 |    false |                      0 |                     0 | radef_kNoError          | Normal operation   |
 * |        7 |                           1U |                       1U |    0xffffffff |   1431655765 |               3 |    false |                      0 |            2863311529 | radef_kNoError          | Normal operation   |
 * |        8 |                           1U |                       1U |    0xffffffff |            1 |               3 |    false |                      0 |            0xffffffff | radef_kNoError          | Normal operation   |
 * |        9 |                           1U |                       1U |    2147483646 |            1 |               3 |    false |                      0 |            2147483646 | radef_kNoError          | Normal operation   |
 * |       10 |                           1U |                       1U |    2147483647 |            1 |               3 |    false |                      0 |            2147483647 | radef_kNoError          | Normal operation   |
 * |       11 |                           1U |                       1U |    2147483648 |            1 |               3 |    false |                      0 |            2147483648 | radef_kNoError          | Normal operation   |
 * |       12 |                           2U |                       1U |             0 |            1 |              10 |    false |                      0 |                     0 | radef_kInvalidParameter | Fatal error raised |
 * |       13 |                           0U |                       1U |             0 |            1 |              10 |    false |                      0 |                     0 | radef_kDeferQueueEmpty  | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(GetOldestSequenceNumberTestWithParam, reddfqTest010VerifyGetOldestSequenceNumberFunction)
{
  // Test variables
  uint16_t i                                  = 0U;               // loop iterator
  redtyp_RedundancyMessage message_to_add     = {UT_REDDFQ_MSG_LEN_MAX,{}};

  // Init Module
  reddfq_Init(UT_REDDFQ_CHANNEL_NUM_MAX, UT_REDDFQ_QUEUE_SIZE_MAX, UT_REDDFQ_T_SEQ_MAX);

  if(messages_to_add <= UT_REDDFQ_QUEUE_SIZE_MAX)
  {
    // expect timer to be called when add message
    EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(messages_to_add);
  }

  // expect get sequence nbr call for every message which was added
  EXPECT_CALL(redmsg_mock, redmsg_GetMessageSequenceNumber(_)).WillRepeatedly(Invoke(this, &reddfqTest::GetMessageSequenceNumber));

  for(i = 0U; i < messages_to_add; i++)
  {
    // set message sequence nbr
    setDataInBuffer_u32(&message_to_add.message, seq_nbr_to_start, expected_sequence_nbr_pos);

    // add message
    EXPECT_NO_THROW(reddfq_AddMessage(add_seq_nbr_channel_id, &message_to_add));
    EXPECT_TRUE(reddfq_Contains(add_seq_nbr_channel_id, seq_nbr_to_start));
    EXPECT_FALSE(reddfq_Contains(add_seq_nbr_channel_id, (seq_nbr_to_start + 1)));

    seq_nbr_to_start += seq_nbr_step;
  }

  if(read_message)
  {
    redtyp_RedundancyMessage message_to_read = {};
    reddfq_GetMessage(get_seq_nbr_channel_id, seq_nbr_to_read, &message_to_read);
  }

  // perform test
  if(exp_error_code != radef_kNoError)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(exp_error_code)).WillOnce(Invoke(this, &reddfqTest::invalidArgumentException));
    EXPECT_THROW(reddfq_GetOldestSequenceNumber(get_seq_nbr_channel_id), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0U);
    EXPECT_EQ(expect_seq_nbr, reddfq_GetOldestSequenceNumber(get_seq_nbr_channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetOldestSequenceNumberFunction,
  GetOldestSequenceNumberTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MIN, 0, 0, 0, false, 0, 0, radef_kDeferQueueEmpty),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MIN, 0, 0, 0, false, 0, 0, radef_kDeferQueueEmpty),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MIN, 0, 0, 1, false, 0, 0, radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MIN, 0, 1, 2, false, 0, 0, radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MIN, 1, 1, 2, false, 0, 1, radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MIN, 0, 1, 2, true, 0, 1, radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_CHANNEL_ID_MAX, 0, 1, 10, false, 0, 0, radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_CHANNEL_ID_MAX, UINT32_MAX, (UINT32_MAX / 3), 3, false, 0, ((UINT32_MAX / 3) * 2 - 1), radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_CHANNEL_ID_MAX, UINT32_MAX, 1, 3, false, 0, UINT32_MAX, radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_CHANNEL_ID_MAX, (UINT32_MAX / 2 - 1), 1, 3, false, 0, (UINT32_MAX / 2 - 1), radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_CHANNEL_ID_MAX, (UINT32_MAX / 2), 1, 3, false, 0, (UINT32_MAX / 2), radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_CHANNEL_ID_MAX, (UINT32_MAX / 2 + 1), 1, 3, false, 0, (UINT32_MAX / 2 + 1), radef_kNoError),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, (UT_REDDFQ_CHANNEL_ID_MAX + 1), 0, 1, 10, false, 0, 0, radef_kInvalidParameter),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_CHANNEL_ID_MIN, 0, 1, 10, false, 0, 0, radef_kDeferQueueEmpty)
  )
);

/**
 * @test        @ID{reddfqTest011} Verify the GetUsedEntries function.
 *
 * @details     This test verifies the correct behaviour of the GetUsedEntries function.
 *
 * Test steps:
 * - Initialize module with max channels, max queue size and max timeout
 * - Add message(s) to given channel if parameter set
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call get get used entries function on specified channel
 * - Verify if expectation of expected used entries parameter is met
 * .
 *
 * @testParameter
 * - Channel id to get used entries: Redundancy channel identification to get used entries of
 * - Channel id of msg to add: Redundancy channel identification to add messages
 * - Messages to add: Number of messages to add
 * - Expect used entries: Expected used entries to return
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                | Test config                               || Expected values                                             |||
 * |----------|--------------------------------|--------------------------|-----------------|---------------------|--------------------|--------------------|
 * |          | Channel id to get used entries | Channel id of msg to add | Messages to add | Expect used entries | Expect fatal error | Test Result        |
 * | 0        | 0U                             | 0U                       | 0               | 0                   | False              | Normal operation   |
 * | 1        | 1U                             | 1U                       | 10              | 10                  | False              | Normal operation   |
 * | 2        | 1U                             | 1U                       | 11              | 10                  | False              | Normal operation   |
 * | 3        | 2U                             | 1U                       | 0               | 0                   | True               | Fatal error raised |
 * | 4        | 1U                             | 0U                       | 10              | 0                   | False              | Normal operation   |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(GetUsedEntriesTestWithParam, reddfqTest011VerifyGetUsedEntriesFunction)
{
  // Test variables
  uint16_t i                                  = 0U;               // loop iterator
  redtyp_RedundancyMessage message_to_add     = {UT_REDDFQ_MSG_LEN_MAX,{}};
  uint32_t seq_nbr_to_start                   = 0U;

  // Init Module
  reddfq_Init(UT_REDDFQ_CHANNEL_NUM_MAX, UT_REDDFQ_QUEUE_SIZE_MAX, UT_REDDFQ_T_SEQ_MAX);

  uint32_t expected_gettimer_calls = messages_to_add;

  // set expected timer calls to max possible amount of adding messages
  if(messages_to_add > UT_REDDFQ_QUEUE_SIZE_MAX)
  {
    expected_gettimer_calls = UT_REDDFQ_QUEUE_SIZE_MAX;
  }

  // expect timer to be called on every successful add message call
  EXPECT_CALL(rasys_mock, rasys_GetTimerValue()).Times(expected_gettimer_calls);

  for(i = 0U; i < messages_to_add; i++)
  {
    // set message sequence nbr
    setDataInBuffer_u32(&message_to_add.message, seq_nbr_to_start, expected_sequence_nbr_pos);

    EXPECT_NO_THROW(reddfq_AddMessage(add_msg_channel_id, &message_to_add));

    seq_nbr_to_start++;
  }

  // perform test
  if(expect_fatal_error)
  {
    // call the Contains function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &reddfqTest::invalidArgumentException));
    EXPECT_THROW(reddfq_GetUsedEntries(get_used_entries_channel_id), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0U);
    EXPECT_EQ(expect_used_entries, reddfq_GetUsedEntries(get_used_entries_channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetUsedEntriesFunction,
  GetUsedEntriesTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MIN, 0U, 0U, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_CHANNEL_ID_MAX, 10U, 10U, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, UT_REDDFQ_CHANNEL_ID_MAX, 11U, 10U, false),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MAX, (UT_REDDFQ_CHANNEL_ID_MAX + 1U), 0U, 0U, true),
    std::make_tuple(UT_REDDFQ_CHANNEL_ID_MIN, UT_REDDFQ_CHANNEL_ID_MAX, 10U, 0U, false)
  )
);

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
