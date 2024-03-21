/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srapi_send_read.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the api module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srapi.hh"

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
 * @brief Test class for value-parametrized SendData function test
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection id (uint32_t)
 * - message size (uint16_t)
 * - test data pointer (uint8_t *)
 * - free buffer entries (uint32_t)
 * - current state (sraty_ConnectionStates)
 * - expected return value (radef_RaStaReturnCode)
 * .
 */
class SendDataTestWithParam : public srapiTest,
                              public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint16_t, uint8_t *, uint32_t, sraty_ConnectionStates, radef_RaStaReturnCode>>
{
  public:
  uint32_t n_max_conn =               std::get<0>(GetParam());        ///< maximum number of connections
  uint32_t connection_id =            std::get<1>(GetParam());        ///< connection ID
  uint16_t message_size =             std::get<2>(GetParam());        ///< message size
  uint8_t * test_data_ptr =           std::get<3>(GetParam());        ///< pointer to the test data
  uint32_t free_buffer_entries =      std::get<4>(GetParam());        ///< free buffer entries to return from mock
  sraty_ConnectionStates state =      std::get<5>(GetParam());        ///< current state of the system
  radef_RaStaReturnCode return_code = std::get<6>(GetParam());        ///< expected return code
};

/**
 * @brief Test class for value-parametrized ReadData function test
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection id (uint32_t)
 * - buffer size (uint16_t)
 * - test data pointer (uint8_t *)
 * - payload size of next message (uint32_t)
 * - expected return value (radef_RaStaReturnCode)
 * .
 */
class ReadDataTestWithParam : public srapiTest,
                              public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint16_t, uint8_t *, uint32_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t n_max_conn =               std::get<0>(GetParam());        ///< maximum number of connections
  uint32_t connection_id =            std::get<1>(GetParam());        ///< connection ID
  uint16_t buffer_size =              std::get<2>(GetParam());        ///< buffer size
  uint8_t * test_data_ptr =           std::get<3>(GetParam());        ///< pointer to the test data
  uint32_t next_payload_size =        std::get<4>(GetParam());        ///< next payload size
  radef_RaStaReturnCode return_code = std::get<5>(GetParam());        ///< expected return code
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_api
 * @{
 */

/**
 * @test        @ID{srapiTest009} Verify the SendData function of the srapi module
 *
 * @details     This test verifies the behaviour of the SendData function.
 *
 * Test steps:
 * - create a SafetyRetransmissionConfiguration and set maximum number of connections
 * - initialize the module with this configuration
 * - set the mock function call expectation on the GetFreeBufferEntries function and return the value given as test parameter
 * - set the mock function call expectation on the srcor_WriteMessagePayloadToTemporaryBuffer function and verify the data
 * - set the mock function call expectations on the srstm_ProcessConnectionStateMachine function with event set to SendData
 * - call the SendData function and verify the return value
 * .
 *
 * @testParameter
 * - Conn. Id: RaSTA connection identification
 * - Data size: Size of the message data in bytes (valid range: 1 - 1055)
 * - Test data: Pointer to message data array
 * - Max. Num. Conn.: Number of maximum connection
 * - Free Buffer Entries: Free buffer entries (messages)
 * - Current State: Current state of connection
 * - Return Code: Expected return value of srapi_SendData function call
 * .
 * | Test Run | Input parameter                                 ||| Test config                                         ||| Expected values                                         ||
 * |----------|----------|-----------|----------------------------|-----------------|---------------------|---------------|---------------------------------------|------------------|
 * |          | Conn. Id | Data size | Test data                  | Max. Num. Conn. | Free Buffer Entries | Current State | Return Code                           | Test Result      |
 * | 0        | 0        | 1055      | test_payload_data_u8       | 0               | 20                  | Up            | radef_kInvalidParameter               | Normal Operation |
 * | 1        | 0        | 1055      | test_payload_data_u8       | 1               | 20                  | Up            | radef_kNoError                        | Normal operation |
 * | 2        | 1        | 1055      | test_payload_data_u8       | 1               | 20                  | Up            | radef_kInvalidParameter               | Normal operation |
 * | 3        | 2        | 1055      | test_payload_data_u8       | 2               | 20                  | Up            | radef_kInvalidParameter               | Normal operation |
 * | 4        | 0        | 0         | test_payload_data_u8       | 2               | 20                  | Up            | radef_kInvalidMessageSize             | Normal operation |
 * | 5        | 0        | 1         | test_payload_data_u8[1054] | 2               | 20                  | Up            | radef_kNoError                        | Normal operation |
 * | 6        | 0        | 1055      | test_payload_data_u8       | 2               | 20                  | Up            | radef_kNoError                        | Normal operation |
 * | 7        | 0        | 1056      | test_payload_data_u8       | 2               | 20                  | Up            | radef_kInvalidMessageSize             | Normal operation |
 * | 8        | 0        | 1055      | test_payload_data_u8       | 2               | 0                   | Up            | radef_kSendBufferFull                 | Normal operation |
 * | 9        | 0        | 1055      | test_payload_data_u8       | 2               | 20                  | Closed        | radef_kInvalidOperationInCurrentState | Normal operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-548} Send Data Function
 * @verifyReq{RASW-310} Send Data Function Structure
 * @verifyReq{RASW-275} Connection Identification
 * @verifyReq{RASW-277} Message Size
 * @verifyReq{RASW-276} Message Data
 * @verifyReq{RASW-272} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 * @verifyReq{RASW-491} Enum Connection States Structure
 */
TEST_P(SendDataTestWithParam, srapiTest009VerifySendDataFunction) {
  // create fake configuration
  const srcty_SafetyRetransmissionConfiguration config = {
    0,
    0,
    0,
    srcty_kSafetyCodeTypeLowerMd4,
    0,
    0,
    0,
    0,
    n_max_conn,
    {0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };

  // initialize the module
  EXPECT_CALL(srcor_mock, srcor_IsConfigurationValid(&config)).WillOnce(Return(true));
  EXPECT_CALL(srstm_mock, srstm_Init(_)).Times(1);
  EXPECT_CALL(srcor_mock, srcor_Init(_)).Times(1);
  EXPECT_EQ(radef_kNoError, srapi_Init(&config));

  // set expectations on srsend_GetFreeBufferEntries, srcor_WriteMessagePayloadToTemporaryBuffer and srstm_ProcessConnectionStateMachine
  if ((connection_id < n_max_conn) &&
      ((message_size >= UT_SRAPI_DATA_LEN_MIN) && (message_size <= UT_SRAPI_DATA_LEN_MAX))) {
    EXPECT_CALL(srstm_mock, srstm_GetConnectionState(connection_id)).WillOnce(Return(state));
    if (state != sraty_kConnectionClosed) {
      EXPECT_CALL(srsend_mock, srsend_GetFreeBufferEntries(connection_id)).WillOnce(Return(free_buffer_entries));
      if(free_buffer_entries > 0) {
        EXPECT_CALL(srcor_mock, srcor_WriteMessagePayloadToTemporaryBuffer(connection_id, message_size, _))
          .WillOnce(checkPayloadData(test_data_ptr));
        EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine(connection_id, srtyp_kConnEventSendData, false, false)).Times(1);
      }
    }
  }

  // call the SendData function and verify the return value
  EXPECT_EQ(return_code, srapi_SendData(connection_id, message_size, test_data_ptr));
}

INSTANTIATE_TEST_SUITE_P(
  verifySendDataFunction,
  SendDataTestWithParam,
  ::testing::Values(
        std::make_tuple((UT_SRAPI_MIN_NUM_CONN - 1),  0, UT_SRAPI_DATA_LEN_MAX,       test_payload_data_u8,                               UT_SRAPI_NSENDMAX_MAX,  sraty_kConnectionUp,      radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MIN_NUM_CONN,        0, UT_SRAPI_DATA_LEN_MAX,       test_payload_data_u8,                               UT_SRAPI_NSENDMAX_MAX,  sraty_kConnectionUp,      radef_kNoError),
        std::make_tuple(UT_SRAPI_MIN_NUM_CONN,        1, UT_SRAPI_DATA_LEN_MAX,       test_payload_data_u8,                               UT_SRAPI_NSENDMAX_MAX,  sraty_kConnectionUp,      radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN,        2, UT_SRAPI_DATA_LEN_MAX,       test_payload_data_u8,                               UT_SRAPI_NSENDMAX_MAX,  sraty_kConnectionUp,      radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN,        0, (UT_SRAPI_DATA_LEN_MIN - 1), test_payload_data_u8,                               UT_SRAPI_NSENDMAX_MAX,  sraty_kConnectionUp,      radef_kInvalidMessageSize),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN,        0, UT_SRAPI_DATA_LEN_MIN,       &(test_payload_data_u8[UT_SRAPI_DATA_LEN_MAX - 1]), UT_SRAPI_NSENDMAX_MAX,  sraty_kConnectionUp,      radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN,        0, UT_SRAPI_DATA_LEN_MAX,       test_payload_data_u8,                               UT_SRAPI_NSENDMAX_MAX,  sraty_kConnectionUp,      radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN,        0, (UT_SRAPI_DATA_LEN_MAX + 1), test_payload_data_u8,                               UT_SRAPI_NSENDMAX_MAX,  sraty_kConnectionUp,      radef_kInvalidMessageSize),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN,        0, UT_SRAPI_DATA_LEN_MAX,       test_payload_data_u8,                               0,                      sraty_kConnectionUp,      radef_kSendBufferFull),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN,        0, UT_SRAPI_DATA_LEN_MAX,       test_payload_data_u8,                               UT_SRAPI_NSENDMAX_MAX,  sraty_kConnectionClosed,  radef_kInvalidOperationInCurrentState)
  ));

/**
 * @test        @ID{srapiTest010} Verify the ReadData function of the srapi module
 *
 * @details     This test verifies the behaviour of the ReadData function.
 *
 * Test steps:
 * - create a SafetyRetransmissionConfiguration and set maximum number of connections
 * - initialize the module with this configuration
 * - set the mock function call expectation on the srrece_GetPayloadSizeOfNextMessageToRead function and return the value given as test parameter
 * - set the mock function call expectation on the srrece_ReadFromBuffer function and provide data as well as a return value
 * - call the ReadData function and verify the return value
 * - verify the read data
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Buffer size: Size of the buffer in bytes available in the application layer (valid range: 1 - 1055)
 * - Max. Num. Conn.: Number of maximum connection
 * - Test data: Pointer to message data array
 * - Data size next msg.: Return value of srrece_GetPayloadSizeOfNextMessageToRead function call
 * - Return Code: Expected return code of srapi_ReadData function call
 * .
 * | Test Run | Input parameter            || Test config                                                      ||| Expected values                            ||
 * |----------|---------------|-------------|-----------------|----------------------------|---------------------|--------------------------|------------------|
 * |          | Connection Id | Buffer size | Max. Num. Conn. | Test data                  | Data size next msg. | Return Code              | Test Result      |
 * | 0        | 0             | 1055        | 0               | test_payload_data_u8       | 1055                | radef_kInvalidParameter  | Normal Operation |
 * | 1        | 0             | 1055        | 1               | test_payload_data_u8       | 1055                | radef_kNoError           | Normal operation |
 * | 2        | 1             | 1055        | 1               | test_payload_data_u8       | 1055                | radef_kInvalidParameter  | Normal operation |
 * | 3        | 2             | 1055        | 2               | test_payload_data_u8       | 1055                | radef_kInvalidParameter  | Normal operation |
 * | 4        | 0             | 0           | 2               | test_payload_data_u8       | 1055                | radef_kInvalidBufferSize | Normal operation |
 * | 5        | 0             | 1           | 2               | test_payload_data_u8[1054] | 1                   | radef_kNoError           | Normal operation |
 * | 6        | 0             | 1055        | 2               | test_payload_data_u8       | 1055                | radef_kNoError           | Normal operation |
 * | 7        | 0             | 1056        | 2               | test_payload_data_u8       | 1055                | radef_kInvalidBufferSize | Normal operation |
 * | 8        | 0             | 1055        | 2               | test_payload_data_u8       | 0                   | radef_kNoMessageReceived | Normal operation |
 * | 9        | 0             | 1           | 2               | test_payload_data_u8       | 2                   | radef_kInvalidBufferSize | Normal operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-549} Read Data Function
 * @verifyReq{RASW-274} Read Data Function Structure
 * @verifyReq{RASW-273} Connection Identification
 * @verifyReq{RASW-269} Buffer Size
 * @verifyReq{RASW-268} Message Size
 * @verifyReq{RASW-270} Message Buffer
 * @verifyReq{RASW-286} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_P(ReadDataTestWithParam, srapiTest010VerifyReadDataFunction)
{
  // test variables
  uint16_t message_size = 0;
  uint8_t message_data[UT_SRAPI_DATA_LEN_MAX] = {0};

  // create fake configuration
  const srcty_SafetyRetransmissionConfiguration config = {
    0,
    0,
    0,
    srcty_kSafetyCodeTypeLowerMd4,
    0,
    0,
    0,
    0,
    n_max_conn,
    {0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };

  // initialize the module
  EXPECT_CALL(srcor_mock, srcor_IsConfigurationValid(&config)).WillOnce(Return(true));
  EXPECT_CALL(srstm_mock, srstm_Init(_)).Times(1);
  EXPECT_CALL(srcor_mock, srcor_Init(_)).Times(1);
  EXPECT_EQ(radef_kNoError, srapi_Init(&config));

  // set expectations on srrece_GetPayloadSizeOfNextMessageToRead and srrece_ReadFromBuffer
  if ((buffer_size >= UT_SRAPI_DATA_LEN_MIN) && (buffer_size <= UT_SRAPI_DATA_LEN_MAX)) {
    EXPECT_CALL(srrece_mock, srrece_GetPayloadSizeOfNextMessageToRead(connection_id)).WillOnce(Return(next_payload_size));
    if ((next_payload_size <= buffer_size) && (connection_id < n_max_conn)) {
      EXPECT_CALL(srrece_mock, srrece_ReadFromBuffer(connection_id, _))
        .WillOnce(provideDataAndReturn(next_payload_size, test_data_ptr));
    }
  }

  // call the ReadData function and verify the return value
  EXPECT_EQ(return_code, srapi_ReadData(connection_id, buffer_size, &message_size, message_data));

  // verify the data
  if (radef_kNoError == return_code) {
    EXPECT_EQ(message_size, next_payload_size);
    for (uint16_t i = 0; i < next_payload_size; i++) {
      EXPECT_EQ(message_data[i], test_data_ptr[i]);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyReadDataFunction,
  ReadDataTestWithParam,
  ::testing::Values(
        std::make_tuple((UT_SRAPI_MIN_NUM_CONN - 1), 0, UT_SRAPI_DATA_LEN_MAX, test_payload_data_u8, UT_SRAPI_DATA_LEN_MAX, radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MIN_NUM_CONN, 0, UT_SRAPI_DATA_LEN_MAX, test_payload_data_u8, UT_SRAPI_DATA_LEN_MAX, radef_kNoError),
        std::make_tuple(UT_SRAPI_MIN_NUM_CONN, 1, UT_SRAPI_DATA_LEN_MAX, test_payload_data_u8, UT_SRAPI_DATA_LEN_MAX, radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 2, UT_SRAPI_DATA_LEN_MAX, test_payload_data_u8, UT_SRAPI_DATA_LEN_MAX, radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, (UT_SRAPI_DATA_LEN_MIN - 1), test_payload_data_u8, UT_SRAPI_DATA_LEN_MAX, radef_kInvalidBufferSize),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, UT_SRAPI_DATA_LEN_MIN, &(test_payload_data_u8[UT_SRAPI_DATA_LEN_MAX - 1]), UT_SRAPI_DATA_LEN_MIN, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, UT_SRAPI_DATA_LEN_MAX, test_payload_data_u8, UT_SRAPI_DATA_LEN_MAX, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, (UT_SRAPI_DATA_LEN_MAX + 1), test_payload_data_u8, UT_SRAPI_DATA_LEN_MAX, radef_kInvalidBufferSize),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, UT_SRAPI_DATA_LEN_MAX, test_payload_data_u8, 0, radef_kNoMessageReceived),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, UT_SRAPI_DATA_LEN_MIN, test_payload_data_u8, (UT_SRAPI_DATA_LEN_MIN + 1), radef_kInvalidBufferSize)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
