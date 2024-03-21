/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srapi_connection.cc
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
 * @brief Test class for value-parametrized OpenConnection function test
 *
 * Parameter order:
 * - sender id (uint32_t)
 * - receiver id (uint32_t)
 * - configured network id (uint32_t)
 * - network id (uint32_t)
 * - expected connection id (uint32_t)
 * - return value core (radef_RaStaReturnCode)
 * - expected return value (radef_RaStaReturnCode)
 * .
 */
class OpenConnectionTestWithParam : public srapiTest,
                                    public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, radef_RaStaReturnCode, radef_RaStaReturnCode>>
{
  public:
  uint32_t sender_id =                std::get<0>(GetParam());        ///< sender ID under test
  uint32_t receiver_id =              std::get<1>(GetParam());        ///< receiver ID under test
  uint32_t conf_network_id =          std::get<2>(GetParam());        ///< configured network id
  uint32_t network_id =               std::get<3>(GetParam());        ///< network ID under test
  uint32_t exp_connection_id =        std::get<4>(GetParam());        ///< expected connection ID
  radef_RaStaReturnCode ret_core =    std::get<5>(GetParam());        ///< return code of the core module (GetConnectionId)
  radef_RaStaReturnCode return_code = std::get<6>(GetParam());        ///< expected return code
};

/**
 * @brief Test class for value-parametrized CloseConnection function test
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection id (uint32_t)
 * - detailed reason (uint16_t)
 * - expected return value (radef_RaStaReturnCode)
 * .
 */
class CloseConnectionTestWithParam :  public srapiTest,
                                      public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint16_t, radef_RaStaReturnCode>>
{
  public:
  uint32_t n_max_conn =               std::get<0>(GetParam());        ///< maximum number of connections
  uint32_t connection_id =            std::get<1>(GetParam());        ///< connection ID
  uint16_t detailed_reason =          std::get<2>(GetParam());        ///< return code of the core module (GetConnectionId)
  radef_RaStaReturnCode return_code = std::get<3>(GetParam());        ///< expected return code
};

/**
 * @brief Test class for value-parametrized GetConnectionState function test
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection id (uint32_t)
 * - expected connection state (sraty_ConnectionStates)
 * - expected return value (radef_RaStaReturnCode)
 * .
 */
class GetConnectionStateTestWithParam : public srapiTest,
                                        public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, sraty_ConnectionStates, radef_RaStaReturnCode>>
{
  public:
  uint32_t n_max_conn =               std::get<0>(GetParam());        ///< maximum number of connections
  uint32_t connection_id =            std::get<1>(GetParam());        ///< connection ID
  sraty_ConnectionStates exp_state =  std::get<2>(GetParam());        ///< expected connection state
  radef_RaStaReturnCode return_code = std::get<3>(GetParam());        ///< expected return code
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_api
 * @{
 */

/**
 * @test        @ID{srapiTest006} Verify the OpenConnection function of the srapi module
 *
 * @details     This test verifies the behaviour of the OpenConnection function.
 *
 * Test steps:
 * - create a SafetyRetransmissionConfiguration and set the network id
 * - initialize the module with this configuration
 * - set the mock function call expectations according to the test parameter
 * - call the OpenConnection function and verify the return value
 * .
 *
 *  @testParameter
 * - Tx Id: RaSTA sender identification
 * - Rx Id: RaSTA receiver identification
 * - Network Id: RaSTA network identification
 * - Conf. Network Id: Configured RaSTA network identification
 * - Return Code Core: Return value of srcor_GetConnectionId function call
 * - Expected Connection Id: Expected connection identification
 * - Expected return Code: Expected return code of srapi_OpenConnection function call
 * .
 * | Test Run | Input parameter                    ||| Test config                               || Expected values                                                   |||
 * |----------|------------|------------|------------|------------------|-------------------------|------------------------|-------------------------|------------------|
 * |          | Tx Id      | Rx Id      | Network Id | Conf. Network Id | Return Code Core        | Expected Connection Id | Expected return Code    | Test Result      |
 * | 0        | 0          | 0          | 0          | 0                | radef_kNoError          | 0                      | radef_kNoError          | Normal Operation |
 * | 1        | 0xffffffff | 0          | 0          | 0                | radef_kNoError          | 0                      | radef_kNoError          | Normal operation |
 * | 2        | 0          | 0xffffffff | 0          | 0                | radef_kNoError          | 0                      | radef_kNoError          | Normal operation |
 * | 3        | 0          | 0          | 0          | 0xffffffff       | radef_kNoError          | 0                      | radef_kInvalidParameter | Normal operation |
 * | 4        | 0          | 0          | 0xffffffff | 0xffffffff       | radef_kNoError          | 0                      | radef_kNoError          | Normal operation |
 * | 5        | 0          | 0          | 0xffffffff | 0                | radef_kNoError          | 0                      | radef_kInvalidParameter | Normal operation |
 * | 6        | 0          | 0          | 0          | 0                | radef_kNoError          | 0xffffffff             | radef_kNoError          | Normal operation |
 * | 7        | 0          | 0          | 0          | 0                | radef_kInvalidParameter | 0                      | radef_kInvalidParameter | Normal operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-546} Open Connection Function
 * @verifyReq{RASW-303} Open Connection Function Structure
 * @verifyReq{RASW-301} Sender Identification
 * @verifyReq{RASW-300} Receiver Identification
 * @verifyReq{RASW-324} Network Identification
 * @verifyReq{RASW-322} Connection Identification
 * @verifyReq{RASW-314} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_P(OpenConnectionTestWithParam, srapiTest006VerifyOpenConnectionFunction) {
  uint32_t connection_id = 0;

  // create fake configuration
  const srcty_SafetyRetransmissionConfiguration config = {
    conf_network_id,
    0,
    0,
    srcty_kSafetyCodeTypeLowerMd4,
    0,
    0,
    0,
    0,
    0,
    {0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };

  // initialize the module
  EXPECT_CALL(srcor_mock, srcor_IsConfigurationValid(&config)).WillOnce(Return(true));
  EXPECT_CALL(srstm_mock, srstm_Init(_)).Times(1);
  EXPECT_CALL(srcor_mock, srcor_Init(_)).Times(1);
  EXPECT_EQ(radef_kNoError, srapi_Init(&config));

  // set expectations on GetConnectionId and ProcessConnectionStateMachine
  if (network_id == conf_network_id) {
    EXPECT_CALL(srcor_mock, srcor_GetConnectionId(sender_id, receiver_id, &connection_id))
        .WillOnce(DoAll(SetArgPointee<2>(exp_connection_id), Return(ret_core)));
    if (ret_core == radef_kNoError) {
      EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine(exp_connection_id, srtyp_kConnEventOpen, false, false));
    }
  }

  // call the OpenConnection function and verify the return value
  EXPECT_EQ(return_code, srapi_OpenConnection(sender_id, receiver_id, network_id, &connection_id));
  EXPECT_EQ(connection_id, exp_connection_id);
}

INSTANTIATE_TEST_SUITE_P(
  verifyOpenConnectionFunction,
  OpenConnectionTestWithParam,
  ::testing::Values(
        std::make_tuple(0, 0, 0, 0, 0, radef_kNoError, radef_kNoError),
        std::make_tuple(UINT32_MAX, 0, 0, 0, 0, radef_kNoError, radef_kNoError),
        std::make_tuple(0, UINT32_MAX, 0, 0, 0, radef_kNoError, radef_kNoError),
        std::make_tuple(0, 0, UINT32_MAX, 0, 0, radef_kNoError, radef_kInvalidParameter),
        std::make_tuple(0, 0, UINT32_MAX, UINT32_MAX, 0, radef_kNoError, radef_kNoError),
        std::make_tuple(0, 0, 0, UINT32_MAX, 0, radef_kNoError, radef_kInvalidParameter),
        std::make_tuple(0, 0, 0, 0, UINT32_MAX, radef_kNoError, radef_kNoError),
        std::make_tuple(0, 0, 0, 0, 0, radef_kInvalidParameter, radef_kInvalidParameter)
  ));

/**
 * @test        @ID{srapiTest007} Verify the CloseConnection function of the srapi module
 *
 * @details     This test verifies the behaviour of the CloseConnection function.
 *
 * Test steps:
 * - create a SafetyRetransmissionConfiguration and set the maximum number of connections
 * - initialize the module with this configuration
 * - set the mock function call expectations according to the test parameter
 * - call the CloseConnection function and verify the return value as well as the detailed reason
 * .
 *
 *  @testParameter
 * - Connection Id: RaSTA connection identification
 * - Detailed Reason: Detailed reason for disconnection
 * - Max. Num. Conn.: Maximum number of connection
 * - Return Code: Expected return code from srapi_CloseConnection function call
 * .
 * | Test Run | Input parameter                || Test config     | Expected values                           ||
 * |----------|---------------|-----------------|-----------------|-------------------------|------------------|
 * |          | Connection Id | Detailed Reason | Max. Num. Conn. | Return Code             | Test Result      |
 * | 0        | 0             | 0               | 0               | radef_kInvalidParameter | Normal Operation |
 * | 1        | 0             | 0               | 1               | radef_kNoError          | Normal operation |
 * | 2        | 1             | 0               | 1               | radef_kInvalidParameter | Normal operation |
 * | 3        | 0             | 0               | 2               | radef_kNoError          | Normal operation |
 * | 4        | 1             | 65535           | 2               | radef_kNoError          | Normal operation |
 * | 5        | 2             | 0               | 2               | radef_kInvalidParameter | Normal operation |
 *
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-547} Close Connection Function
 * @verifyReq{RASW-318} Close Connection Function Structure
 * @verifyReq{RASW-316} Connection Identification
 * @verifyReq{RASW-309} Detailed Reason
 * @verifyReq{RASW-308} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_P(CloseConnectionTestWithParam, srapiTest007VerifyCloseConnectionFunction) {
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

  // set expectations on SetDiscDetailedReason and ProcessConnectionStateMachine
  if (connection_id < n_max_conn) {
    EXPECT_CALL(srcor_mock, srcor_SetDiscDetailedReason(connection_id, detailed_reason)).Times(1);
    EXPECT_CALL(srstm_mock, srstm_ProcessConnectionStateMachine(connection_id, srtyp_kConnEventClose, false, false));
  }

  // call the CloseConnection function and verify the return value
  EXPECT_EQ(return_code, srapi_CloseConnection(connection_id, detailed_reason));
}

INSTANTIATE_TEST_SUITE_P(
  verifyCloseConnectionFunction,
  CloseConnectionTestWithParam,
  ::testing::Values(
        std::make_tuple((UT_SRAPI_MIN_NUM_CONN - 1), 0, 0, radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MIN_NUM_CONN, 0, 0, radef_kNoError),
        std::make_tuple(UT_SRAPI_MIN_NUM_CONN, 1, 0, radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, 0, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 1, UINT16_MAX, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 2, 0, radef_kInvalidParameter)
  ));

/**
 * @test        @ID{srapiTest008} Verify the GetConnectionState function of the srapi module
 *
 * @details     This test verifies the behaviour of the GetConnectionState function.
 *
 * Test steps:
 * - create a SafetyRetransmissionConfiguration and set the maximum number of connections
 * - initialize the module with this configuration
 * - set the mock function call expectation for the srstm_GetConnectionState function and return the expected state
 * - set the mock function call expectation for the srcor_GetBufferSizeAndUtilisation function
 * - call the GetConnectionState function and verify the return value as well as the state returned
 * .
 *
 *  @testParameter
 * - Connection Id: RaSTA connection identification
 * - Max. Num. Conn.: Maximum number of connections
 * - Expected State: Expected connection state
 * - Return Code: Expected return value of srapi_GetConnectionState function call
 * .
 * | Test Run | Input parameter | Test config     | Expected values                                                            |||
 * |----------|-----------------|-----------------|---------------------------------|-------------------------|------------------|
 * |          | Connection Id   | Max. Num. Conn. | Expected State                  | Return Code             | Test Result      |
 * | 0        | 0               | 0               | sraty_kConnectionNotInitialized | radef_kInvalidParameter | Normal Operation |
 * | 1        | 0               | 1               | sraty_kConnectionNotInitialized | radef_kNoError          | Normal operation |
 * | 2        | 1               | 1               | sraty_kConnectionNotInitialized | radef_kInvalidParameter | Normal operation |
 * | 3        | 0               | 2               | sraty_kConnectionNotInitialized | radef_kNoError          | Normal operation |
 * | 4        | 1               | 2               | sraty_kConnectionNotInitialized | radef_kNoError          | Normal operation |
 * | 5        | 2               | 2               | sraty_kConnectionNotInitialized | radef_kInvalidParameter | Normal operation |
 * | 6        | 0               | 2               | sraty_kConnectionClosed         | radef_kNoError          | Normal operation |
 * | 7        | 0               | 2               | sraty_kConnectionDown           | radef_kNoError          | Normal operation |
 * | 8        | 0               | 2               | sraty_kConnectionStart          | radef_kNoError          | Normal operation |
 * | 9        | 0               | 2               | sraty_kConnectionUp             | radef_kNoError          | Normal operation |
 * | 10       | 0               | 2               | sraty_kConnectionRetransRequest | radef_kNoError          | Normal operation |
 * | 11       | 0               | 2               | sraty_kConnectionRetransRunning | radef_kNoError          | Normal operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-550} Get Connection State Function
 * @verifyReq{RASW-288} Get Connection State Function Structure
 * @verifyReq{RASW-287} Connection Identification
 * @verifyReq{RASW-282} Connection State
 * @verifyReq{RASW-281} Buffer Utilisation
 * @verifyReq{RASW-284} Opposite Buffer Size
 * @verifyReq{RASW-283} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 * @verifyReq{RASW-461} Struct Buffer Utilisation Structure
 */
TEST_P(GetConnectionStateTestWithParam, srapiTest008VerifyGetConnectionStateFunction)
{
  // test variables
  sraty_ConnectionStates returned_state = sraty_kConnectionMin;
  sraty_BufferUtilisation buffer_utilisation = {};
  uint16_t opposite_buffer_size = 0;

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

  // set expectations on SetDiscDetailedReason and ProcessConnectionStateMachine
  if (connection_id < n_max_conn) {
    EXPECT_CALL(srstm_mock, srstm_GetConnectionState(connection_id)).WillOnce(Return(exp_state));
    EXPECT_CALL(srcor_mock, srcor_GetBufferSizeAndUtilisation(connection_id, &buffer_utilisation, &opposite_buffer_size)).Times(1);
  }

  // call the GetConnectionState function and verify the return value
  EXPECT_EQ(return_code, srapi_GetConnectionState(connection_id, &returned_state, &buffer_utilisation, &opposite_buffer_size));

  // verify the state returned
  EXPECT_EQ(returned_state, exp_state);
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetConnectionStateFunction,
  GetConnectionStateTestWithParam,
  ::testing::Values(
        std::make_tuple((UT_SRAPI_MIN_NUM_CONN - 1), 0, sraty_kConnectionNotInitialized, radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MIN_NUM_CONN, 0, sraty_kConnectionNotInitialized, radef_kNoError),
        std::make_tuple(UT_SRAPI_MIN_NUM_CONN, 1, sraty_kConnectionNotInitialized, radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, sraty_kConnectionNotInitialized, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 1, sraty_kConnectionNotInitialized, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 2, sraty_kConnectionNotInitialized, radef_kInvalidParameter),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, sraty_kConnectionClosed, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, sraty_kConnectionDown, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, sraty_kConnectionStart, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, sraty_kConnectionUp, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, sraty_kConnectionRetransRequest, radef_kNoError),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, 0, sraty_kConnectionRetransRunning, radef_kNoError)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
