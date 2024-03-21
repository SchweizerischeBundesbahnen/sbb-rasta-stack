/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redrbf.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the received buffer module of the redundancy layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_redrbf.hh"

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
 * - configured channels (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class InitTestWithParam :   public redrbfTest,
                            public testing::WithParamInterface<std::tuple<uint32_t, bool>>
{
  public:
    uint32_t configured_channels =      std::get<0>(GetParam());        ///< number of configured channels
    bool expect_fatal_error =           std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized init buffer test
 *
 * Parameter order:
 * - channel id index (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class InitBufferTestWithParam : public redrbfTest,
                                public testing::WithParamInterface<std::tuple<uint32_t, bool>>
{
  public:
    uint32_t channel_id =               std::get<0>(GetParam());        ///< channel id
    bool expect_fatal_error =           std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized AddToBuffer test
 *
 * Parameter order:
 * - do init buffer (bool)
 * - channel ID (uint32_t)
 * - payload size (uint16_t)
 * - payload data pointer (uint8_t *)
 * - expected buffer length after write (uint8_t)
 * - do readback check (bool)
 * - expected error code (radef_RaStaReturnCode)
 * .
 */
class AddToBufferTestWithParam :    public redrbfTest,
                                    public testing::WithParamInterface<std::tuple<bool, uint32_t, uint16_t,  uint8_t * , uint8_t, bool, radef_RaStaReturnCode>>
{
  public:
    bool do_initialize =                        std::get<0>(GetParam());        ///< initialize module
    uint32_t channel_id =                       std::get<1>(GetParam());        ///< channel ID
    redtyp_RedundancyMessagePayload payload =   {std::get<2>(GetParam()), {}};  ///< message payload
    uint8_t * payload_data_ptr =                std::get<3>(GetParam());        ///< pointer to payload data
    uint8_t exp_buffer_length =                 std::get<4>(GetParam());        ///< expected buffer length after write
    bool do_readback_check =                    std::get<5>(GetParam());        ///< indicates if a read back check is performed
    radef_RaStaReturnCode error =               std::get<6>(GetParam());        ///< expected error code
};

/**
 * @brief Test class for value-parametrized ReadFromBuffer test
 *
 * Parameter order:
 * - channel ID (uint32_t)
 * - payload size (uint16_t)
 * - payload data pointer (uint8_t *)
 * - message buffer size (uint16_t)
 * - amount of message to add (uint32_t)
 * - amount of message to read (uint32_t)
 * - expectFatalError (bool)
 * - expected error code (radef_RaStaReturnCode)
 * .
 */
class ReadFromBufferTestWithParam : public redrbfTest,
                                    public testing::WithParamInterface<std::tuple<uint32_t, uint16_t, uint8_t *, uint16_t, uint32_t, uint32_t, bool, radef_RaStaReturnCode>>
{
  public:
    uint32_t channel_id =                       std::get<0>(GetParam());        ///< channel ID
    redtyp_RedundancyMessagePayload payload =   {std::get<1>(GetParam()), {}};  ///< message payload
    uint8_t * payload_data_ptr =                std::get<2>(GetParam());        ///< pointer to payload data
    uint16_t buffer_size =                      std::get<3>(GetParam());        ///< message buffer size
    uint32_t messages_to_add =                  std::get<4>(GetParam());        ///< amount of messages to add to buffer
    uint32_t messages_to_read =                 std::get<5>(GetParam());        ///< amount of messages to read from buffer
    bool expect_fatal_error =                   std::get<6>(GetParam());        ///< indicates if a fatal error is expected
    radef_RaStaReturnCode exp_retval =          std::get<7>(GetParam());        ///< expected return value
};

/**
 * @brief Test class for value-parametrized GetFreeBufferEntries test
 *
 * Parameter order:
 * - channel ID (uint32_t)
 * - buffer items (uint32_t)
 * - expRetVal (uint32_t)
 * - expectFatalError (bool)
 * .
 */
class GetFreeBufferEntriesTestWithParam :   public redrbfTest,
                                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, bool>>
{
  public:
    uint32_t channel_id =               std::get<0>(GetParam());        ///< channel ID
    uint32_t messages_to_add =          std::get<1>(GetParam());        ///< amount of messages to add to buffer
    uint32_t exp_retval =               std::get<2>(GetParam());        ///< expected return value
    bool expect_fatal_error =           std::get<3>(GetParam());        ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup red_receiveBuf
 * @{
 */

/**
 * @test        @ID{redrbfTest001} Verify the init function.
 *
 * @details     This test verifies that the init function sets the number of configured
 *              channels correctly.
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
 * | 0        | 2                    | False              | Normal operation   |
 * | 1        | 3                    | True               | Fatal error raised |
 * | 2        | 1                    | False              | Normal operation   |
 * | 3        | 0                    | True               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitTestWithParam, redrbfTest001VerifyInitFunction)
{
  extern uint32_t redrbf_number_of_red_channels;

  if (expect_fatal_error)
  {
    // call the init function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redrbfTest::invalidArgumentException));
    EXPECT_THROW(redrbf_Init(configured_channels), std::invalid_argument);
  }
  else
  {
    // call the init function and verify if parameter are set correctly
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redrbf_Init(configured_channels));

    EXPECT_EQ(redrbf_number_of_red_channels, configured_channels);
    EXPECT_TRUE(redrbf_initialized);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitFunction,
  InitTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDRBF_MAX_NUM_OF_RED_CHANNELS, false),
    std::make_tuple((UT_REDRBF_MAX_NUM_OF_RED_CHANNELS + 1), true),
    std::make_tuple(UT_REDRBF_MIN_NUM_OF_RED_CHANNELS, false),
    std::make_tuple((UT_REDRBF_MIN_NUM_OF_RED_CHANNELS - 1), true)
  ));

/**
 * @test        @ID{redrbfTest002} Verify the init buffer function.
 *
 * @details     This test verifies that the init buffer function initializes
 *              the dedicated receive buffer correctly.
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - add a message to the buffer
 * - verify that the buffer size is set to 1
 * - call the init buffer function with given test parameter and verify that it completes successfully
 * - verify that the buffer size is reset to 0
 * .
 *
 * @testParameter
 * - Channel Id: Redundancy channel identification
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Expected values                        ||
 * |----------|-----------------|--------------------|--------------------|
 * |          | Channel Id      | Expect Fatal Error | Test Result        |
 * | 0        | 0U              | False              | Normal operation   |
 * | 1        | 1U              | False              | Normal operation   |
 * | 2        | 2U              | True               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitBufferTestWithParam, redrbfTest002VerifyInitBufferFunction)
{
  redtyp_RedundancyMessagePayload dummy_payload = {UT_REDRBF_DATA_LEN_MIN, {0}};
  redtyp_RedundancyMessagePayload payload_to_read = {};
  const uint16_t buffer_size = UT_REDRBF_BUFFER_LEN_MIN;
  uint16_t message_size = 0U;
  uint8_t message_buffer[buffer_size] = {};

  // initialize the module
  redrbf_Init(UT_REDRBF_MAX_NUM_OF_RED_CHANNELS);

  if (expect_fatal_error)
  {
    // call the init function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redrbfTest::invalidArgumentException));
    EXPECT_THROW(redrbf_InitBuffer(channel_id), std::invalid_argument);
  }
  else
  {
    // add a message to the buffer to increment buffer size
    EXPECT_CALL(rednot_mock, rednot_MessageReceivedNotification(channel_id)).Times(1);

    // Expect no error to be thrown
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    redrbf_AddToBuffer(channel_id, &dummy_payload);
    EXPECT_EQ(radef_kNoError, redrbf_ReadFromBuffer(channel_id, buffer_size, &message_size, &message_buffer[0]));

    EXPECT_NO_THROW(redrbf_InitBuffer(channel_id));

    // verify that the buffer contains no message
    EXPECT_EQ(radef_kNoMessageReceived, redrbf_ReadFromBuffer(channel_id, buffer_size, &message_size, &message_buffer[0]));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitBufferFunction,
  InitBufferTestWithParam,
  ::testing::Values(
    std::make_tuple(0, false),
    std::make_tuple(1, false),
    std::make_tuple(2, true)
  ));

/**
 * @test        @ID{redrbfTest003} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - call the AddToBuffer function with invalid payload pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the ReadFromBuffer function with invalid message size pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the ReadFromBuffer function with invalid message buffer pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redrbfTest, redrbfTest003VerifyNullPointerCheck)
{
  const uint8_t nbr_of_function_calls = 3U;          // number of function calls

  // Test variables
  const uint32_t channel_id = 0U;
  const uint16_t buffer_size = 0U;
  uint16_t message_size[0U] = {};
  uint8_t message_buffer[0U] = {};

  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
    .Times(nbr_of_function_calls)
    .WillRepeatedly(Invoke(this, &redrbfTest::invalidArgumentException));

  // initialize the module
  redrbf_Init(UT_REDRBF_MIN_NUM_OF_RED_CHANNELS);

  // call the functions
  EXPECT_THROW(redrbf_AddToBuffer(channel_id, (redtyp_RedundancyMessagePayload *)NULL), std::invalid_argument);
  EXPECT_THROW(redrbf_ReadFromBuffer(channel_id, buffer_size, (uint16_t *)NULL, message_buffer), std::invalid_argument);
  EXPECT_THROW(redrbf_ReadFromBuffer(channel_id, buffer_size, message_size, (uint8_t *)NULL), std::invalid_argument);
}

/**
 * @test        @ID{redrbfTest004} Verify the AddToBuffer function.
 *
 * @details     This test verifies that correct behaviour of the AddToBuffer function.
 *
 * Test steps:
 * - initialize the module with maximum number of channels
 * - set the fatal error expectation based on the test parameter
 * - verify the free entries before the add
 * - call the AddToBuffer function
 * - verify the free entries after the add
 * - verify that the message received notification is sent
 * - perform a readback check using the ReadFromBuffer function and verify the data correctness
 * - verify the free entries after the read
 * .
 *
 * @testParameter
 * - Channel Id: Redundancy channel identification
 * - Payload Size: Size of payload in bytes (valid range: 28 - 1101)
 * - Do init: Indicates if module gets initialized before function call
 * - Data Pointer: Pointer to data buffer containing the test message
 * - Readback check: Indicates if a read back check is performed after redrbf_AddToBuffer function call
 * - Exp. Buf. Length: Expected buffer length (in messages) after write
 * - Exp. Error Code: Expected error code when calling redrbf_AddToBuffer function
 * .
 * | Test Run | Input parameter          || Test config                                       ||| Expected values                                                |||
 * |----------|------------|--------------|---------|--------------------------|----------------|------------------|--------------------------|--------------------|
 * |          | Channel Id | Payload Size | Do init | Data Pointer             | Readback check | Exp. Buf. Length | Exp. Error Code          | Test Result        |
 * | 0        | 0U         | 27U          | true    | test_red_payload_data_u8 | false          | 0U               | radef_kInvalidParameter  | Fatal error raised |
 * | 1        | 0U         | 1102U        | true    | test_red_payload_data_u8 | false          | 0U               | radef_kInvalidParameter  | Fatal error raised |
 * | 2        | 0U         | 1101U        | true    | test_red_payload_data_u8 | true           | 1U               | radef_kNoError           | Normal operation   |
 * | 3        | 0U         | 1101U        | true    | test_red_payload_data_u8 | false          | 1U               | radef_kNoError           | Normal operation   |
 * | 4        | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 2U               | radef_kNoError           | Normal operation   |
 * | 5        | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 3U               | radef_kNoError           | Normal operation   |
 * | 6        | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 4U               | radef_kNoError           | Normal operation   |
 * | 7        | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 5U               | radef_kNoError           | Normal operation   |
 * | 8        | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 6U               | radef_kNoError           | Normal operation   |
 * | 9        | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 7U               | radef_kNoError           | Normal operation   |
 * | 10       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 8U               | radef_kNoError           | Normal operation   |
 * | 11       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 9U               | radef_kNoError           | Normal operation   |
 * | 12       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 10U              | radef_kNoError           | Normal operation   |
 * | 13       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 11U              | radef_kNoError           | Normal operation   |
 * | 14       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 12U              | radef_kNoError           | Normal operation   |
 * | 15       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 13U              | radef_kNoError           | Normal operation   |
 * | 16       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 14U              | radef_kNoError           | Normal operation   |
 * | 17       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 15U              | radef_kNoError           | Normal operation   |
 * | 18       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 16U              | radef_kNoError           | Normal operation   |
 * | 19       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 17U              | radef_kNoError           | Normal operation   |
 * | 20       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 18U              | radef_kNoError           | Normal operation   |
 * | 21       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 19U              | radef_kNoError           | Normal operation   |
 * | 22       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 20U              | radef_kNoError           | Normal operation   |
 * | 23       | 0U         | 1101U        | false   | test_red_payload_data_u8 | false          | 21U              | radef_kReceiveBufferFull | Fatal error raised |
 * | 24       | 0U         | 1101U        | true    | test_red_payload_data_u8 | true           | 1U               | radef_kNoError           | Normal operation   |
 * | 25       | 1U         | 1101U        | true    | test_red_payload_data_u8 | true           | 1U               | radef_kNoError           | Normal operation   |
 * | 26       | 1U         | 1101U        | true    | test_red_payload_data_u8 | false          | 1U               | radef_kNoError           | Normal operation   |
 * | 27       | 1U         | 1101U        | false   | test_red_payload_data_u8 | false          | 2U               | radef_kNoError           | Normal operation   |
 * | 28       | 1U         | 1101U        | false   | test_red_payload_data_u8 | false          | 3U               | radef_kNoError           | Normal operation   |
 * | 29       | 2U         | 1101U        | false   | test_red_payload_data_u8 | false          | 0U               | radef_kInvalidParameter  | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(AddToBufferTestWithParam, redrbfTest004VerifyAddToBufferFunction)
{
  redtyp_RedundancyMessagePayload read_payload = {0, {}};         // payload read from the buffer
  const uint16_t buffer_size = UT_REDRBF_BUFFER_LEN_MAX;
  uint16_t message_size = 0U;
  uint8_t message_buffer[buffer_size] = {};

  // copy payload data
  for (uint16_t i = 0U; i < payload.payload_size; i++)
  {
    payload.payload[i] = payload_data_ptr[i];
  }

  // initialize module
  if(do_initialize)
  {
    redrbf_Init(UT_REDRBF_MAX_NUM_OF_RED_CHANNELS);
  }
  else
  {
    redrbf_initialized = true;
  }

  if (error != radef_kNoError)
  {
    // call the addToBuffer function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(error)).WillOnce(Invoke(this, &redrbfTest::invalidArgumentException));
    EXPECT_THROW(redrbf_AddToBuffer(channel_id, &payload), std::invalid_argument);
  }
  else
  {
    // verify the number of free entries before the add
    EXPECT_EQ((UT_REDRBF_N_SEND_MAX - exp_buffer_length + 1), redrbf_GetFreeBufferEntries(channel_id));

    // expect the message received notification to be sent and no error thrown
    EXPECT_CALL(rednot_mock, rednot_MessageReceivedNotification(channel_id)).Times(1);
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    // call the AddToBuffer function
    EXPECT_NO_THROW(redrbf_AddToBuffer(channel_id, &payload));

    // verify the number of free entries after the add
    EXPECT_EQ((UT_REDRBF_N_SEND_MAX - exp_buffer_length), redrbf_GetFreeBufferEntries(channel_id));

    if (do_readback_check)
    {
      // call the readFromBuffer function
      EXPECT_EQ(radef_kNoError, redrbf_ReadFromBuffer(channel_id, buffer_size, &message_size, &message_buffer[0]));

      // verify the free entries after the read
      EXPECT_EQ((UT_REDRBF_N_SEND_MAX - exp_buffer_length + 1), redrbf_GetFreeBufferEntries(channel_id));

      // verify the data
      ASSERT_EQ(payload.payload_size, message_size);

      for (uint16_t i = 0U; i < payload.payload_size; i++)
      {
        EXPECT_EQ(payload.payload[i], getDataFromBuffer_u8(&message_buffer, i));
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyAddToBufferFunction,
  AddToBufferTestWithParam,
  ::testing::Values(
    std::make_tuple(true, 0U, (UT_REDRBF_DATA_LEN_MIN - 1), test_red_payload_data_u8, 0, false, radef_kInvalidParameter),
    std::make_tuple(true, 0U, (UT_REDRBF_DATA_LEN_MAX + 1), test_red_payload_data_u8, 0, false, radef_kInvalidParameter),
    std::make_tuple(true, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 1, true, radef_kNoError),
    std::make_tuple(true, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 1, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 2, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 3, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 4, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 5, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 6, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 7, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 8, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 9, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 10, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 11, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 12, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 13, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 14, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 15, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 16, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 17, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 18, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 19, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 20, false, radef_kNoError),
    std::make_tuple(false, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 21, false, radef_kReceiveBufferFull),
    std::make_tuple(true, 0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 1, true, radef_kNoError),
    std::make_tuple(true, 1U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 1, true, radef_kNoError),
    std::make_tuple(true, 1U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 1, false, radef_kNoError),
    std::make_tuple(false, 1U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 2, false, radef_kNoError),
    std::make_tuple(false, 1U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 3, false, radef_kNoError),
    std::make_tuple(false, 2U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, 0, false, radef_kInvalidParameter)
  ));

/**
 * @test        @ID{redrbfTest005} Verify the ReadFromBuffer function.
 *
 * @details     This test verifies that correct behaviour of the ReadFromBuffer function.
 *
 * Test steps:
 * - initialize the module with maximum number of connections
 * - set the fatal error expectation based on the test parameter
 * - add a message
 * - call the ReadFromBuffer function
 * - verify the free entries after the read
 * - verify the data correctness (if message has been added)
 * .
 *
 * @testParameter
 * - Channel Id: Redundancy channel identification
 * - Buffer size: Size of the external memory buffer in bytes to store the read message data (valid range: 28 - 1101)
 * - Payload Size: Size of the message payload which gets added to buffer
 * - Data Pointer: Pointer to data buffer containing the test payload
 * - Messages to add: Number of messages to add
 * - Messages to read:  Number of messages to read
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * - Last Exp. Return Value: Last expected return value from redrbf_ReadFromBuffer function call
 * .
 * | Test Run | Input parameter                                                 |||| Test config                       || Expected values                                                  |||
 * |----------|------------|-------------|--------------|--------------------------|-----------------|------------------|--------------------|--------------------------|--------------------|
 * |          | Channel Id | Buffer size | Payload Size | Data Pointer             | Messages to add | Messages to read | Expect Fatal Error | Last Exp. Return Value   | Test Result        |
 * | 0        | 0U         | 1101U       | 1101U        | test_red_payload_data_u8 | 1               | 1                | false              | radef_kNoError           | Normal operation   |
 * | 1        | 0U         | 28U         | 28U          | test_red_payload_data_u8 | 1               | 1                | false              | radef_kNoError           | Normal operation   |
 * | 2        | 0U         | 1101U       | 1101U        | test_red_payload_data_u8 | 0               | 1                | false              | radef_kNoMessageReceived | Normal operation   |
 * | 3        | 0U         | 1101U       | 1101U        | test_red_payload_data_u8 | 2               | 1                | false              | radef_kNoError           | Normal operation   |
 * | 4        | 0U         | 1101U       | 1101U        | test_red_payload_data_u8 | 20              | 20               | false              | radef_kNoError           | Normal operation   |
 * | 5        | 0U         | 1101U       | 1101U        | test_red_payload_data_u8 | 20              | 21               | false              | radef_kNoMessageReceived | Normal operation   |
 * | 6        | 1U         | 1101U       | 1101U        | test_red_payload_data_u8 | 1               | 1                | false              | radef_kNoError           | Normal operation   |
 * | 7        | 1U         | 1101U       | 1101U        | test_red_payload_data_u8 | 0               | 1                | false              | radef_kNoMessageReceived | Normal operation   |
 * | 8        | 1U         | 1100U       | 1101U        | test_red_payload_data_u8 | 0               | 1                | true               | radef_kInvalidBufferSize | Fatal error raised |
 * | 9        | 2U         | 1101U       | 1101U        | test_red_payload_data_u8 | 1               | 1                | true               | radef_kInvalidParameter  | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 */
TEST_P(ReadFromBufferTestWithParam, redrbfTest005VerifyReadFromBufferFunction)
{
  extern uint32_t redrbf_number_of_red_channels;

  // redtyp_RedundancyMessagePayload read_payload = {0U, {}};                                      // payload read from the buffer
  uint32_t expected_free_buffer_entries_before_read = RADEF_MAX_N_SEND_MAX - messages_to_add;      // expected free entries in buffer before reading messages
  uint32_t expected_free_buffer_entries_after_read = RADEF_MAX_N_SEND_MAX;                         // expected free entries in buffer after reading messages
  uint16_t message_size = 0U;
  uint8_t message_buffer[buffer_size] = {};

  // set expected free entries in buffer when adding and reading messages
  if(messages_to_add > 0U)
  {
    expected_free_buffer_entries_after_read = expected_free_buffer_entries_before_read + messages_to_read;
  }

  // set expected free entries in buffer when reading more messages than buffer size
  if(messages_to_read > UT_REDRBF_N_SEND_MAX)
  {
    expected_free_buffer_entries_after_read = UT_REDRBF_N_SEND_MAX;
  }

  // free entries stay the same when message could not be read
  if(exp_retval == radef_kInvalidBufferSize)
  {
    expected_free_buffer_entries_after_read = expected_free_buffer_entries_before_read;
  }

  // copy payload data
  for (uint16_t i = 0U; i < payload.payload_size; i++)
  {
    payload.payload[i] = payload_data_ptr[i];
  }

  // initialize module
  redrbf_Init(UT_REDRBF_MAX_NUM_OF_RED_CHANNELS);

  // verify initialization
  EXPECT_EQ(redrbf_initialized, true);
  EXPECT_EQ(redrbf_number_of_red_channels, UT_REDRBF_MAX_NUM_OF_RED_CHANNELS);

  if (exp_retval == radef_kInvalidParameter)
  {
    // call the addToBuffer function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redrbfTest::invalidArgumentException));
    EXPECT_THROW(redrbf_ReadFromBuffer(channel_id, buffer_size, &message_size, &message_buffer[0]), std::invalid_argument);
  }
  else
  {
    for (uint16_t i = 0U; i < messages_to_add; i++)
    {
      // add a message to the buffer
      EXPECT_CALL(rednot_mock, rednot_MessageReceivedNotification(channel_id)).Times(1);
      redrbf_AddToBuffer(channel_id, &payload);
    }

    // verify the number of free entries before the read
    EXPECT_EQ((expected_free_buffer_entries_before_read), redrbf_GetFreeBufferEntries(channel_id));

    if(messages_to_read > 0U)
    {
      radef_RaStaReturnCode result = radef_kNoError;

      for(uint16_t read_message_i = 0U; read_message_i < messages_to_read; read_message_i++)
      {
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

        result = redrbf_ReadFromBuffer(channel_id, buffer_size, &message_size, &message_buffer[0]);

        if(exp_retval == radef_kNoError)
        {
          // verify the data
          ASSERT_EQ(payload.payload_size, message_size);
          for (uint16_t i = 0U; i < payload.payload_size; i++)
          {
            EXPECT_EQ(payload.payload[i], getDataFromBuffer_u8(&message_buffer, i));
          }
        }
      }

      // verify result with expected last return value
      EXPECT_EQ(exp_retval, result);
    }

    // verify the number of free entries after the read
    EXPECT_EQ((expected_free_buffer_entries_after_read), redrbf_GetFreeBufferEntries(channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyReadFromBufferFunction,
  ReadFromBufferTestWithParam,
  ::testing::Values(
    std::make_tuple(0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, UT_REDRBF_BUFFER_LEN_MAX, 1U, 1U, false, radef_kNoError),
    std::make_tuple(0U, UT_REDRBF_DATA_LEN_MIN, test_red_payload_data_u8, UT_REDRBF_BUFFER_LEN_MIN, 1U, 1U, false, radef_kNoError),
    std::make_tuple(0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, UT_REDRBF_BUFFER_LEN_MAX, 0U, 1U, false, radef_kNoMessageReceived),
    std::make_tuple(0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, UT_REDRBF_BUFFER_LEN_MAX, 2U, 1U, false, radef_kNoError),
    std::make_tuple(0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, UT_REDRBF_BUFFER_LEN_MAX, 20U, 20U, false, radef_kNoError),
    std::make_tuple(0U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, UT_REDRBF_BUFFER_LEN_MAX, 20U, 21U, false, radef_kNoMessageReceived),
    std::make_tuple(1U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, UT_REDRBF_BUFFER_LEN_MAX, 1U, 1U, false, radef_kNoError),
    std::make_tuple(1U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, UT_REDRBF_BUFFER_LEN_MAX, 0U, 1U, false, radef_kNoMessageReceived),
    std::make_tuple(1U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, (UT_REDRBF_BUFFER_LEN_MAX -1), 1U, 1U, true, radef_kInvalidBufferSize),
    std::make_tuple(2U, UT_REDRBF_DATA_LEN_MAX, test_red_payload_data_u8, UT_REDRBF_BUFFER_LEN_MAX, 1U, 1U, true, radef_kInvalidParameter)
  ));

/**
 * @test        @ID{redrbfTest006} Verify the GetFreeBufferEntries function.
 *
 * @details     This test verifies that correct behaviour of the GetFreeBufferEntries function.
 *
 * Test steps:
 * - initialize the module
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - add amount of given messages to buffer
 * - call the GetFreeBufferEntries function
 * - verify that the number of free buffer entries returned is correct
 * .
 *
 * @testParameter
 * - Channel Id: Redundancy channel identification
 * - Configured buffer items: Number of configured items in buffer (messages)
 * - Expected Return Value: Expected return value from redrbf_GetFreeBufferEntries function call
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config             | Expected values                                               |||
 * |----------|-----------------|-------------------------|-----------------------|--------------------|--------------------|
 * |          | Channel Id      | Configured buffer items | Expected Return Value | Expect Fatal Error | Test Result        |
 * | 1        | 0U              | 0U                      | 20U                   | false              | Normal operation   |
 * | 2        | 0U              | 1U                      | 19U                   | false              | Normal operation   |
 * | 3        | 1U              | 20U                     | 0U                    | false              | Normal operation   |
 * | 4        | 2U              | 0U                      | 20U                   | true               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(GetFreeBufferEntriesTestWithParam, redrbfTest006VerifyGetFreeBufferEntries)
{
  redtyp_RedundancyMessagePayload payload = {UT_REDRBF_DATA_LEN_MIN, 0};     // empty payload

  // initialize module
  redrbf_Init(UT_REDRBF_MAX_NUM_OF_RED_CHANNELS);

  if (expect_fatal_error)
  {
    // call the GetBufferSize function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redrbfTest::invalidArgumentException));
    EXPECT_THROW(redrbf_GetFreeBufferEntries(channel_id), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    for(uint16_t i = 0U; i < messages_to_add; i++)
    {
      // add a message to the buffer
      EXPECT_CALL(rednot_mock, rednot_MessageReceivedNotification(channel_id)).Times(1);
      redrbf_AddToBuffer(channel_id, &payload);
    }

    // call the GetBufferSize function
    EXPECT_EQ(exp_retval, redrbf_GetFreeBufferEntries(channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetFreeBufferEntries,
  GetFreeBufferEntriesTestWithParam,
  ::testing::Values(
    std::make_tuple(0U, 0U, UT_REDRBF_N_SEND_MAX, false),
    std::make_tuple(0U, 1U, 19U, false),
    std::make_tuple(1U, UT_REDRBF_N_SEND_MAX, 0U, false),
    std::make_tuple(2U, 0U, UT_REDRBF_N_SEND_MAX, true)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
