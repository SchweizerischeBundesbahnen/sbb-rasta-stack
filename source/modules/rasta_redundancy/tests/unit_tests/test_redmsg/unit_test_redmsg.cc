/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redmsg.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version b4d4bd413ce5fa54d373eb00890116763cac504a
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 *
 * @brief Unit test file for the messages module of the redundancy layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_redmsg.hh"

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
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - Configured check code type (redcty_CheckCodeType)
 * - Expect fatal error (bool)
 * .
 */
class InitTestWithParam :   public redmsgTest,
                            public testing::WithParamInterface<std::tuple<redcty_CheckCodeType, bool>>
{
  public:
  redcty_CheckCodeType configured_checkcode_type =         std::get<0>(GetParam());        ///< configured check code type
  bool expect_fatal_error =                                std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized message create test
 *
 * Parameter order:
 * - Sequence number (uint32_t)
 * - Check code type (redcty_CheckCodeType)
 * - MessagePayload.payload_size (uint16_t)
 * - Payload data pointer (uint8_t *)
 * - Expect fatal error (bool)
 * .
 */
class CreateMsgWithParam : public redmsgTest, public testing::WithParamInterface<std::tuple<uint32_t, redcty_CheckCodeType, uint16_t, uint8_t * , bool>>
{
    public:
    uint32_t sequence_number =                              std::get<0>(GetParam());        ///< sequence number
    redcty_CheckCodeType check_code_type =                  std::get<1>(GetParam());        ///< check code type
    redtyp_RedundancyMessagePayload message_payload =   {   std::get<2>(GetParam()),{}};    ///< message payload
    uint8_t * data_ptr =                                    std::get<3>(GetParam());        ///< pointer to the test data
    bool expect_fatal_error =                               std::get<4>(GetParam());        ///< expect fatal error
};

/**
 * @brief Test class for value-parametrized message crc check
 *
 * Parameter order:
 * - Check code type (redcty_CheckCodeType)
 * - Expected return code (radef_RaStaReturnCode)
 * - Expected data size (uint16_t)
 * - Payload data pointer (uint8_t *)
 * - Expect correct crc (bool)
 * - Expect fatal error (bool)
 * .
 */
class CheckMsgCrcWithParam : public redmsgTest, public testing::WithParamInterface<std::tuple<redcty_CheckCodeType, radef_RaStaReturnCode, uint16_t, uint8_t *, bool, bool>>
{
    public:

    redcty_CheckCodeType check_code_type =  std::get<0>(GetParam());        ///< check code type
    radef_RaStaReturnCode return_code =     std::get<1>(GetParam());        ///< expected return value (ignored if expecting fatal)
    uint16_t expected_data_size =           std::get<2>(GetParam());        ///< expected data size
    uint8_t * data_ptr =                    std::get<3>(GetParam());        ///< pointer to test data
    bool expect_correct_crc =               std::get<4>(GetParam());        ///< expect correct crc value from mock (ignored if expecting fatal)
    bool expect_fatal_error =               std::get<5>(GetParam());        ///< expect fatal error
};

/**
 * @brief Test class for sequence number check
 *
 * Parameter order:
 * - Expected sequence number (uint16_t)
 * - Configured sequence number (uint16_t)
 * - Payload size (uint16_t)
 * - Crc size (uint16_t)
 * - Payload data pointer (uint8_t *)
 * - Expect fatal error (bool)
 * .
 */
class getMsgSequenceNumber : public redmsgTest, public testing::WithParamInterface<std::tuple<uint16_t, uint16_t, uint16_t, uint16_t, uint8_t * , bool>>
{
    public:

    uint16_t expected_sequence_number =         std::get<0>(GetParam());        ///< expected sequence number
    uint16_t conf_sequence_number =             std::get<1>(GetParam());        ///< configured sequence number
    uint16_t payload_size =                     std::get<2>(GetParam());        ///< payload size
    uint16_t crc_size =                         std::get<3>(GetParam());        ///< expected crc length
    uint8_t * data_ptr =                        std::get<4>(GetParam());        ///< pointer to test data
    bool expect_fatal_error =                   std::get<5>(GetParam());        ///< expect fatal error
};

/**
 * @brief Test class for payload check
 *
 * Parameter order:
 * - Expected data size (uint16_t)
 * - Message Size (uint16_t)
 * - Configure correct message_length (bool)
 * - Check code type (redcty_CheckCodeType)
 * - Payload data pointer (uint8_t *)
 * - Expected return code (radef_RaStaReturnCode)
 * .
 */
class getMsgPayload : public redmsgTest, public testing::WithParamInterface<std::tuple<uint16_t, uint16_t, bool, redcty_CheckCodeType, uint8_t *, radef_RaStaReturnCode>>
{
    public:
    uint16_t expected_data_size =                 std::get<0>(GetParam());        ///< expected payload length
    uint16_t expected_message_size =              std::get<1>(GetParam());        ///< expected message length
    bool configure_correct_message_length =       std::get<2>(GetParam());        ///< configure stored message length correct
    redcty_CheckCodeType check_code_type =        std::get<3>(GetParam());        ///< check code type
    uint8_t * data_ptr =                          std::get<4>(GetParam());        ///< pointer to test data
    radef_RaStaReturnCode expected_return_code =  std::get<5>(GetParam());        ///< expected return value
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup red_messages
 * @{
 */

/**
 * @test        @ID{redmsgTest001} Verify the init function.
 *
 * @details     This test verifies that the init function sets the check code type
 *              parameter correctly.
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the init function with given test parameter
 * - verify if the check code type is set correctly
 * - verify that the module is set to initialized state
 * .
 *
 * @testParameter
 * - Check code type: Used check code type
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter      | Expected values                        ||
 * |----------|----------------------|--------------------|--------------------|
 * |          | Check code type      | Expect Fatal Error | Test Result        |
 * | 0        | redcty_kCheckCodeA   | False              | Normal operation   |
 * | 1        | redcty_kCheckCodeB   | False              | Normal operation   |
 * | 2        | redcty_kCheckCodeC   | False              | Normal operation   |
 * | 3        | redcty_kCheckCodeD   | False              | Normal operation   |
 * | 4        | redcty_kCheckCodeE   | False              | Normal operation   |
 * | 5        | 5                    | True               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitTestWithParam, redmsgTest001VerifyInitFunction)
{
  // perform test
  if (expect_fatal_error)
  {
    // call the init function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redmsgTest::invalidArgumentException));
    EXPECT_THROW(redmsg_Init(configured_checkcode_type), std::invalid_argument);
  }
  else
  {
    // Expect mock call to init redcrc where checkcode gets set
    EXPECT_CALL(redcrc_mock, redcrc_Init(_)).WillOnce(Invoke(this, &redmsgTest::crc_Init));

    // call the init function and verify if parameter are set correctly
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redmsg_Init(configured_checkcode_type));

    EXPECT_EQ(redmsg_check_code_type, configured_checkcode_type);
    EXPECT_TRUE(redmsg_initialized);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitFunction,
  InitTestWithParam,
  ::testing::Values(
      std::make_tuple(redcty_kCheckCodeA, false),
      std::make_tuple(redcty_kCheckCodeB, false),
      std::make_tuple(redcty_kCheckCodeC, false),
      std::make_tuple(redcty_kCheckCodeD, false),
      std::make_tuple(redcty_kCheckCodeE, false),
      std::make_tuple(redcty_kCheckCodeMax, true)
  )
);

/**
 * @test        @ID{redmsgTest002} Verify the number of init function calls.
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
TEST_F(redmsgTest, redmsgTest002VerifyInitFunctionCalls)
{
  redcty_CheckCodeType check_code = redcty_kCheckCodeA;

  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &redmsgTest::invalidArgumentException));

  // expect crc init call on first init
  EXPECT_CALL(redcrc_mock, redcrc_Init(check_code)).Times(1);

  // call the init function and verify that it completes successful
  EXPECT_NO_THROW(redmsg_Init(check_code));

  // call the init function again and verify that an assertion violations is raised as the module is already initialized
  EXPECT_THROW(redmsg_Init(check_code), std::invalid_argument);
}

/**
 * @test        @ID{redmsgTest003} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - init module
 * - call the redmsg_CreateMessage function with invalid payload pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the redmsg_CreateMessage function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the redmsg_CheckMessageCrc function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the redmsg_GetMessageSequenceNumber function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the redmsg_GetMessagePayload function with invalid message pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the redmsg_GetMessagePayload function with invalid payload pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redmsgTest, redmsgTest003VerifyNullPointerCheck)
{
  const uint8_t nbr_of_function_calls = 6U;        // number of function calls

  // Expect mock call to init redcrc where checkcode gets set
  EXPECT_CALL(redcrc_mock, redcrc_Init(_)).WillOnce(Invoke(this, &redmsgTest::crc_Init));

  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).Times(nbr_of_function_calls)
    .WillRepeatedly(Invoke(this, &redmsgTest::invalidArgumentException));

  // init module
  redmsg_Init(redcty_kCheckCodeA);

  // set test variables
  uint32_t sequence_number = 0U;
  redtyp_RedundancyMessagePayload payload = {UT_REDMSG_DATA_LEN_MIN, {}};
  redtyp_RedundancyMessage message = {};

  // perform test
  EXPECT_THROW(redmsg_CreateMessage(sequence_number, (redtyp_RedundancyMessagePayload *)NULL, &message), std::invalid_argument);
  EXPECT_THROW(redmsg_CreateMessage(sequence_number, &payload, (redtyp_RedundancyMessage *)NULL), std::invalid_argument);
  EXPECT_THROW(redmsg_CheckMessageCrc((redtyp_RedundancyMessage *)NULL), std::invalid_argument);
  EXPECT_THROW(redmsg_GetMessageSequenceNumber((redtyp_RedundancyMessage *)NULL), std::invalid_argument);
  EXPECT_THROW(redmsg_GetMessagePayload((redtyp_RedundancyMessage *)NULL, &payload), std::invalid_argument);
  EXPECT_THROW(redmsg_GetMessagePayload(&message, (redtyp_RedundancyMessagePayload *)NULL), std::invalid_argument);
}

/**
 * @test        @ID{redmsgTest004} Initialization checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the CreateMessage function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the CheckMessageCrc function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetMessageSequenceNumber function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetMessagePayload function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redmsgTest, redmsgTest004VerifyInitializationCheck)
{
  const uint8_t nbr_of_function_calls = 4U;        // number of function calls
  extern bool redmsg_initialized;
  redmsg_initialized = false;

  // set test variables
  const uint32_t sequence_number = 0U;
  redtyp_RedundancyMessagePayload payload = {UT_REDMSG_DATA_LEN_MIN, {}};
  redtyp_RedundancyMessage message = {};

  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
      .Times(nbr_of_function_calls)
      .WillRepeatedly(Invoke(this, &redmsgTest::invalidArgumentException));

  // perform test
  EXPECT_THROW(redmsg_CreateMessage(sequence_number, &payload, &message), std::invalid_argument);
  EXPECT_THROW(redmsg_CheckMessageCrc(&message), std::invalid_argument);
  EXPECT_THROW(redmsg_GetMessageSequenceNumber(&message), std::invalid_argument);
  EXPECT_THROW(redmsg_GetMessagePayload(&message, &payload), std::invalid_argument);
}

/**
 * @test        @ID{redmsgTest005} verify the createMessage function
 *
 * @details     This test verifies the correct behaviour of the createMessage function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Sequence Number: Sequence number of message to create
 * - Payload size: Size of message payload in bytes (valid range: 28 - 1101)
 * - Check code type: Configured check code type
 * - Payload data pointer: Pointer to data buffer containing the test message
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter               || Test config                                   || Expected values                   ||
 * |----------|-----------------|--------------|--------------------|---------------------------|---------------|--------------------|
 * |          | Sequence Number | Payload size | Check code type    | Payload data pointer      | Expect Fatal  | Test Result        |
 * | 0        | 0xffffffffU     | 28U          | redcty_kCheckCodeA | test_red_payload_data_u8  | False         | Normal operation   |
 * | 1        | 0               | 1101U        | redcty_kCheckCodeA | test_red_payload_data_u8  | False         | Normal operation   |
 * | 2        | 0               | 1102U        | redcty_kCheckCodeA | test_red_payload_data_u8  | True          | Fatal error raised |
 * | 3        | 0               | 27U          | redcty_kCheckCodeA | test_red_payload_data_u8  | True          | Fatal error raised |
 * | 4        | 0               | 28U          | redcty_kCheckCodeB | test_red_payload_data_u8  | False         | Normal operation   |
 * | 5        | 0               | 28U          | redcty_kCheckCodeC | test_red_payload_data_u8  | False         | Normal operation   |
 * | 6        | 0               | 28U          | redcty_kCheckCodeD | test_red_payload_data_u8  | False         | Normal operation   |
 * | 7        | 0               | 28U          | redcty_kCheckCodeE | test_red_payload_data_u8  | False         | Normal operation   |
 * | 8        | 0               | 1101U        | redcty_kCheckCodeE | test_red_payload_data_u8  | False         | Normal operation   |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(CreateMsgWithParam, redmsgTest005VerifyCreateMessage)
{
  // test variables
  redtyp_RedundancyMessage message   = {0U, {}};

  uint16_t message_size_no_crc       = message_payload.payload_size + UT_REDMSG_HEADER_SIZE;
  uint16_t expected_message_length   = message_size_no_crc;
  uint32_t expected_sequence_number  = sequence_number;

  switch(check_code_type)
  {
    case redcty_kCheckCodeA:
      expected_message_length += UT_REDMSG_CHECK_CODE_NONE_LEN;
      break;
    case redcty_kCheckCodeB:
    case redcty_kCheckCodeC:
      expected_message_length += UT_REDMSG_CHECK_CODE_MAX_LEN;
      break;
    case redcty_kCheckCodeD:
    case redcty_kCheckCodeE:
      expected_message_length += UT_REDMSG_CHECK_CODE_HALF_LEN;
      break;
  }

  // Expect mock call to init redcrc where checkcode gets set
  EXPECT_CALL(redcrc_mock, redcrc_Init(_)).WillOnce(Invoke(this, &redmsgTest::crc_Init));

  // init the module
  redmsg_Init(check_code_type);

  // copy test payload
  for (uint16_t i = 0U; i < message_payload.payload_size; i++)
  {
    message_payload.payload[i] = data_ptr[i];
  }

  if (expect_fatal_error)
  {
    // set expectations
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redmsgTest::invalidArgumentException));

    // call the function
    EXPECT_THROW(redmsg_CreateMessage(sequence_number, &message_payload, &message), std::invalid_argument);
  }
  else
  {
    // set mock expectations
    if (check_code_type != redcty_kCheckCodeA &&
        check_code_type != redcty_kCheckCodeMin &&
        check_code_type != redcty_kCheckCodeMax)
    {
      EXPECT_CALL(redcrc_mock, redcrc_CalculateCrc(message_size_no_crc, _, _)).Times(1);
    }
    else
    {
      EXPECT_CALL(redcrc_mock, redcrc_CalculateCrc(_, _, _)).Times(0);
    }

    // set expectations
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    // call the function
    redmsg_CreateMessage(sequence_number, &message_payload, &message);

    // verify the output of the function
    EXPECT_EQ(message.message_size, expected_message_length);
    EXPECT_EQ(getDataFromBuffer_u16(&message.message, expected_message_length_pos), expected_message_length);
    EXPECT_EQ(getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos), expected_sequence_number);
    EXPECT_THAT(getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos), InRange(0, UINT32_MAX));
    EXPECT_THAT(getDataFromBuffer_u32(&message.message, expected_message_length_pos), InRange(0, UINT32_MAX));
    EXPECT_THAT(getDataFromBuffer_u32(&message.message, expected_reserve_pos), InRange(0, UINT32_MAX));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyCreateMessage,
  CreateMsgWithParam,
  ::testing::Values(
      std::make_tuple(UINT32_MAX, redcty_kCheckCodeA, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, false),
      std::make_tuple(0U, redcty_kCheckCodeA, UT_REDMSG_DATA_LEN_MAX , test_red_payload_data_u8, false),
      std::make_tuple(0U, redcty_kCheckCodeA, (UT_REDMSG_DATA_LEN_MAX + 1U) , test_red_payload_data_u8, true),
      std::make_tuple(0U, redcty_kCheckCodeA, (UT_REDMSG_DATA_LEN_MIN - 1U) , test_red_payload_data_u8, true),
      std::make_tuple(0U, redcty_kCheckCodeB, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, false),
      std::make_tuple(0U, redcty_kCheckCodeC, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, false),
      std::make_tuple(0U, redcty_kCheckCodeD, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, false),
      std::make_tuple(0U, redcty_kCheckCodeE, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, false),
      std::make_tuple(0U, redcty_kCheckCodeE, UT_REDMSG_DATA_LEN_MAX, test_red_payload_data_u8, false)
  )
);

/**
 * @test        @ID{redmsgTest006} verify the checkMessageCrc function
 *
 * @details     This test verifies the correct behaviour of the checkMessageCrc function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module
 * - call the check message crc function with given test parameter
 * - verify the correctness of the return code
 * .
 *
 * @testParameter
 * - Payload size: Message payload size in bytes (valid range: 28 - 1101)
 * - Check code type: Configured check code type
 * - Payload data pointer: Pointer to data buffer containing the test message
 * - Expected return code: Expected return code from redmsg_CheckMessageCrc function call
 * - Expect correct crc: Indicates if redcrc_CalculateCrc check passes
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config                                   || Expected values                                                                  ||||
 * |----------|-----------------|---------------------|--------------------------|--------------------------|---------------------|---------------|--------------------|
 * |          | Payload size    | Check code type     | Payload data pointer     | Expected return code     | Expect correct crc  | Expect Fatal  | Test Result        |
 * | 0        | 28U             | redcty_kCheckCodeA  | test_red_payload_data_u8 | radef_kNoError           | True                | False         | Normal operation   |
 * | 1        | 1101U           | redcty_kCheckCodeA  | test_red_payload_data_u8 | radef_kNoError           | True                | False         | Normal operation   |
 * | 2        | 27U             | redcty_kCheckCodeA  | test_red_payload_data_u8 | radef_kInvalidMessageCrc | True                | True          | Fatal error raised |
 * | 3        | 28U             | redcty_kCheckCodeB  | test_red_payload_data_u8 | radef_kNoError           | True                | False         | Normal operation   |
 * | 4        | 28U             | redcty_kCheckCodeB  | test_red_payload_data_u8 | radef_kInvalidMessageCrc | False               | False         | Normal operation   |
 * | 5        | 1102U           | redcty_kCheckCodeB  | test_red_payload_data_u8 | radef_kInvalidMessageCrc | True                | True          | Fatal error raised |
 * | 6        | 28U             | redcty_kCheckCodeC  | test_red_payload_data_u8 | radef_kNoError           | True                | False         | Normal operation   |
 * | 7        | 28U             | redcty_kCheckCodeD  | test_red_payload_data_u8 | radef_kNoError           | True                | False         | Normal operation   |
 * | 8        | 28U             | redcty_kCheckCodeE  | test_red_payload_data_u8 | radef_kNoError           | True                | False         | Normal operation   |
 * | 9        | 1101U           | redcty_kCheckCodeE  | test_red_payload_data_u8 | radef_kNoError           | True                | False         | Normal operation   |
 * | 10       | 1101U           | redcty_kCheckCodeE  | test_red_payload_data_u8 | radef_kInvalidMessageCrc | False               | False         | Normal operation   |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(CheckMsgCrcWithParam, redmsgTest006VerifyCheckMsgCrc)
{
  // expected Values
  const uint8_t expected_payload_size_bytepos     = 0U;                // according to RASW-202
  const uint8_t expected_payload_bytepos          = 8U;                // according to RASW-202

  uint16_t expected_crc_bytepos                   = expected_payload_bytepos + expected_data_size;
  uint32_t message_crc                            = 100U;
  uint32_t wrong_message_crc                      = message_crc + 1U;

  uint16_t crc_length = 0U;

  switch(check_code_type)
  {
    case redcty_kCheckCodeA:
      crc_length += UT_REDMSG_CHECK_CODE_NONE_LEN;
      break;
    case redcty_kCheckCodeB:
    case redcty_kCheckCodeC:
      crc_length += UT_REDMSG_CHECK_CODE_MAX_LEN;
      break;
    case redcty_kCheckCodeD:
    case redcty_kCheckCodeE:
      crc_length += UT_REDMSG_CHECK_CODE_HALF_LEN;
      break;
  }

  uint16_t expected_message_size = expected_payload_bytepos + expected_data_size + crc_length;

  // test variables
  redtyp_RedundancyMessage message = {expected_message_size, {}};
  redtyp_RedundancyMessagePayload message_payload = {0U, {}};

  // set message size
  setDataInBuffer_u16(&message.message, expected_message_size, expected_message_length_pos);

  // Expect mock call to init redcrc where checkcode gets set
  EXPECT_CALL(redcrc_mock, redcrc_Init(_)).WillOnce(Invoke(this, &redmsgTest::crc_Init));

  // init the module
  redmsg_Init(check_code_type);

  if (check_code_type != redcty_kCheckCodeA)
  {
    setDataInBuffer_u32(&message.message, message_crc, expected_crc_bytepos);
  }

  for (uint16_t i = 0U; i < expected_data_size; i++)
  {
    setDataInBuffer_u8(&message.message, data_ptr[i], (expected_payload_bytepos + i));
  }

  if (expect_fatal_error)
  {
    // set expectations
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redmsgTest::invalidArgumentException));

    // call the function
    EXPECT_THROW(redmsg_CheckMessageCrc(&message), std::invalid_argument);
  }
  else
  {
    // set mock expectations
    if (check_code_type != redcty_kCheckCodeA &&
        check_code_type != redcty_kCheckCodeMin &&
        check_code_type != redcty_kCheckCodeMax)
    {
      if (!expect_correct_crc)
      {
        EXPECT_CALL(redcrc_mock, redcrc_CalculateCrc((message.message_size - crc_length), _, _)).Times(1).WillOnce(SetArgPointee<2>(wrong_message_crc));
      }
      else
      {
        EXPECT_CALL(redcrc_mock, redcrc_CalculateCrc((message.message_size - crc_length), _, _)).Times(1).WillOnce(SetArgPointee<2>(message_crc));
      }
    }
    else
    {
      EXPECT_CALL(redcrc_mock, redcrc_CalculateCrc(_, _, _)).Times(0);
    }

    // set expectations
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    // call the function and verify the output
    EXPECT_EQ(return_code, redmsg_CheckMessageCrc(&message));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyCheckMsgCrc,
  CheckMsgCrcWithParam,
  ::testing::Values(
      std::make_tuple(redcty_kCheckCodeA, radef_kNoError, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, true, false),
      std::make_tuple(redcty_kCheckCodeA, radef_kNoError, UT_REDMSG_DATA_LEN_MAX, test_red_payload_data_u8, true, false),
      std::make_tuple(redcty_kCheckCodeA, radef_kInvalidMessageCrc, (UT_REDMSG_DATA_LEN_MIN - 1), test_red_payload_data_u8, true, true),
      std::make_tuple(redcty_kCheckCodeB, radef_kNoError, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, true, false),
      std::make_tuple(redcty_kCheckCodeB, radef_kInvalidMessageCrc, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, false, false),
      std::make_tuple(redcty_kCheckCodeB, radef_kInvalidMessageCrc, (UT_REDMSG_DATA_LEN_MAX + 1), test_red_payload_data_u8, true, true),
      std::make_tuple(redcty_kCheckCodeC, radef_kNoError, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, true, false),
      std::make_tuple(redcty_kCheckCodeD, radef_kNoError, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, true, false),
      std::make_tuple(redcty_kCheckCodeE, radef_kNoError, UT_REDMSG_DATA_LEN_MIN, test_red_payload_data_u8, true, false),
      std::make_tuple(redcty_kCheckCodeE, radef_kNoError, UT_REDMSG_DATA_LEN_MAX, test_red_payload_data_u8, true, false),
      std::make_tuple(redcty_kCheckCodeE, radef_kInvalidMessageCrc, UT_REDMSG_DATA_LEN_MAX, test_red_payload_data_u8, false, false)
  )
);

/**
 * @test        @ID{redmsgTest007} verify the getMessageSequenceNumber function
 *
 * @details     This test verifies the correct behaviour of the getMessageSequenceNumber function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Payload size: Message payload size in bytes (valid range: 28 - 1101)
 * - Configured sequence number: Configured sequence number for message
 * - Crc size: Crc size in bytes
 * - Payload data pointer: Pointer to data buffer containing the test message
 * - Expected sequence number: Expected sequence number extracted from message
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                            || Test config                        || Expected values                                             |||
 * |----------|---------------|-----------------------------|----------|--------------------------|--------------------------|---------------|--------------------|
 * |          | Payload size  | Configured sequence number  | Crc size | Payload data pointer     | Expected sequence number | Expect Fatal  | Test Result        |
 * | 0        | 28U           | 0                           | 0        | test_red_payload_data_u8 | 0                        | False         | Normal operation   |
 * | 1        | 27U           | 0                           | 0        | test_red_payload_data_u8 | 0                        | True          | Fatal error raised |
 * | 2        | 28U           | 0xffffffffU                 | 0        | test_red_payload_data_u8 | 0xffffffffU              | False         | Normal operation   |
 * | 3        | 1101U         | 0                           | 4        | test_red_payload_data_u8 | 0                        | False         | Normal operation   |
 * | 4        | 1101U         | 0xffffffffU                 | 4        | test_red_payload_data_u8 | 0xffffffffU              | False         | Normal operation   |
 * | 5        | 1102U         | 0                           | 4        | test_red_payload_data_u8 | 0                        | True          | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(getMsgSequenceNumber, redmsgTest007VerifyGetMsgSeq)
{
  // expected Values
  const uint8_t expected_payload_bytepos          = 8U;               // according to RASW-202

  uint16_t expected_message_size = expected_payload_bytepos + payload_size + crc_size;

  // test variables
  redtyp_RedundancyMessage message = {expected_message_size, {}};

  // set message fields
  setDataInBuffer_u16(&message.message, expected_message_size, expected_message_length_pos);
  setDataInBuffer_u32(&message.message, conf_sequence_number, expected_sequence_nbr_pos);

  // copy test payload
  for (uint16_t i = 0U; i < payload_size; i++)
  {
    setDataInBuffer_u8(&message.message, data_ptr[i], (expected_payload_bytepos + i));
  }

  // Expect mock call to init redcrc where checkcode gets set
  EXPECT_CALL(redcrc_mock, redcrc_Init(_)).WillOnce(Invoke(this, &redmsgTest::crc_Init));

  // init the module
  redmsg_Init(redcty_kCheckCodeA);

  if (expect_fatal_error)
  {
    // set expectations
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redmsgTest::invalidArgumentException));

    // call the function
    EXPECT_THROW(redmsg_GetMessageSequenceNumber(&message), std::invalid_argument);
  }
  else
  {
    // set expectations
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);

    // call the function and verify the output
    EXPECT_EQ(expected_sequence_number, redmsg_GetMessageSequenceNumber(&message));
    EXPECT_EQ(expected_sequence_number, getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos));
    EXPECT_THAT(getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos), InRange(0, UINT32_MAX));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetMsgSeq,
  getMsgSequenceNumber,
  ::testing::Values(
      std::make_tuple(0, 0, UT_REDMSG_DATA_LEN_MIN, UT_REDMSG_CHECK_CODE_NONE_LEN, test_red_payload_data_u8, false),
      std::make_tuple(0, 0, (UT_REDMSG_DATA_LEN_MIN - 1), UT_REDMSG_CHECK_CODE_NONE_LEN, test_red_payload_data_u8, true),
      std::make_tuple(UINT32_MAX, UINT32_MAX, UT_REDMSG_DATA_LEN_MIN, UT_REDMSG_CHECK_CODE_NONE_LEN, test_red_payload_data_u8, false),
      std::make_tuple(0, 0, UT_REDMSG_DATA_LEN_MAX, UT_REDMSG_CHECK_CODE_MAX_LEN, test_red_payload_data_u8, false),
      std::make_tuple(UINT32_MAX, UINT32_MAX, UT_REDMSG_DATA_LEN_MAX, UT_REDMSG_CHECK_CODE_MAX_LEN, test_red_payload_data_u8, false),
      std::make_tuple(0, 0, (UT_REDMSG_DATA_LEN_MAX + 1), UT_REDMSG_CHECK_CODE_MAX_LEN, test_red_payload_data_u8, true)
  )
);

/**
 * @test        @ID{redmsgTest008}  verify the getMessageSequenceNumber function
 *
 * @details     This test verifies the correct behaviour of the getMessageSequenceNumber function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Payload size: Message payload size in bytes (valid range: 28 - 1101)
 * - Msg size: Message size in bytes (valid range: 36 - 1113)
 * - Set correct msg length: Indicate if correct crc gets set in message
 * - Check code type: Configured check code type
 * - Payload data pointer: Pointer to data buffer containing the test message
 * - Expected error code: Expected error code when calling redmsg_GetMessagePayload function
 * .
 * | Test Run | Input parameter         || Test config                                                          ||| Expected values                             ||
 * |----------|---------------|----------|------------------------|--------------------|--------------------------|-------------------------|--------------------|
 * |          | Payload size  | Msg size | Set correct msg length | Check code type    | Payload data pointer     | Expected error code     | Test Result        |
 * | 0        | 28U           | 36U      | false                  | redcty_kCheckCodeA | test_red_payload_data_u8 | radef_kInvalidParameter | Fatal error raised |
 * | 1        | 28U           | 35U      | true                   | redcty_kCheckCodeA | test_red_payload_data_u8 | radef_kInvalidParameter | Fatal error raised |
 * | 2        | 28U           | 36U      | true                   | redcty_kCheckCodeA | test_red_payload_data_u8 | radef_kNoError          | Fatal error raised |
 * | 3        | 1101U         | 1113U    | true                   | redcty_kCheckCodeB | test_red_payload_data_u8 | radef_kNoError          | Fatal error raised |
 * | 4        | 1101U         | 1114U    | true                   | redcty_kCheckCodeB | test_red_payload_data_u8 | radef_kInvalidParameter | Fatal error raised |
 * | 5        | 1101U         | 1113U    | false                  | redcty_kCheckCodeB | test_red_payload_data_u8 | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(getMsgPayload, redmsgTest008VerifyGetMsgPayload)
{
  // expected Values
  const uint8_t expected_payload_size_bytepos     = 0U;               // according to RASW-202
  const uint8_t expected_payload_bytepos          = 8U;               // according to RASW-202

  // test variables
  redtyp_RedundancyMessage message = {expected_message_size, {}};
  redtyp_RedundancyMessagePayload message_payload = {0, {}};

  if (configure_correct_message_length)
  {
    // set message size
    setDataInBuffer_u16(&message.message, expected_message_size, expected_message_length_pos);
  }
  else
  {
    // set wrong message size
    setDataInBuffer_u16(&message.message, (expected_message_size - 1), expected_message_length_pos);
  }

  // set payload size
  setDataInBuffer_u16(&message_payload.payload, expected_data_size, expected_payload_size_bytepos);

  for (uint16_t i = 0U; i < expected_data_size; i++)
  {
    setDataInBuffer_u8(&message.message, data_ptr[i], (expected_payload_bytepos + i));
  }

  // Expect mock call to init redcrc where checkcode gets set
  EXPECT_CALL(redcrc_mock, redcrc_Init(_)).WillOnce(Invoke(this, &redmsgTest::crc_Init));

  // init the module
  redmsg_Init(check_code_type);

  if (expected_return_code != radef_kNoError)
  {
    // set expectations
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_return_code)).WillOnce(Invoke(this, &redmsgTest::invalidArgumentException));

    // call the function
    EXPECT_THROW(redmsg_GetMessagePayload(&message, &message_payload), std::invalid_argument);
  }
  else
  {
    // set expectations
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    redmsg_GetMessagePayload(&message, &message_payload);

    // verify that the values are set correctly
    EXPECT_EQ(expected_data_size, message_payload.payload_size);
    for (uint16_t i = 0U; i < expected_data_size; i++)
    {
      EXPECT_EQ(data_ptr[i], message_payload.payload[i]);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetMsgPayload,
  getMsgPayload,
  ::testing::Values(
      std::make_tuple(UT_REDMSG_DATA_LEN_MIN, UT_REDMSG_MESSAGE_LEN_MIN, false, redcty_kCheckCodeA, test_red_payload_data_u8, radef_kInvalidParameter),
      std::make_tuple(UT_REDMSG_DATA_LEN_MIN, (UT_REDMSG_MESSAGE_LEN_MIN - 1), true, redcty_kCheckCodeA, test_red_payload_data_u8, radef_kInvalidParameter),
      std::make_tuple(UT_REDMSG_DATA_LEN_MIN, UT_REDMSG_MESSAGE_LEN_MIN, true, redcty_kCheckCodeA, test_red_payload_data_u8, radef_kNoError),
      std::make_tuple(UT_REDMSG_DATA_LEN_MAX, UT_REDMSG_MESSAGE_LEN_MAX, true, redcty_kCheckCodeB, test_red_payload_data_u8, radef_kNoError),
      std::make_tuple(UT_REDMSG_DATA_LEN_MAX, (UT_REDMSG_MESSAGE_LEN_MAX + 1), true, redcty_kCheckCodeB, test_red_payload_data_u8, radef_kInvalidParameter),
      std::make_tuple(UT_REDMSG_DATA_LEN_MAX, UT_REDMSG_MESSAGE_LEN_MAX, false, redcty_kCheckCodeB, test_red_payload_data_u8, radef_kInvalidParameter)
  )
);

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
