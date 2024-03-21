/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srapi.cc
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
 * @brief Test class for value-parametrized init function test
 *
 * Parameter order:
 * - configured connections (uint32_t)
 * - is config valid (bool)
 * - expected return code (radef_RaStaReturnCode)
 * .
 */
class InitTestWithParam : public srapiTest,
                          public testing::WithParamInterface<std::tuple<uint32_t, bool, radef_RaStaReturnCode>>
{
  public:
  uint32_t configured_connections =   std::get<0>(GetParam());        ///< number of configured connections
  bool is_config_valid =              std::get<1>(GetParam());        ///< sets the return value of the configuration check
  radef_RaStaReturnCode return_code = std::get<2>(GetParam());        ///< expected return code
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_api
 * @{
 */

/**
 * @test        @ID{srapiTest001} Verify the init function of the srapi module
 *
 * @details     This test verifies that the init function initializes the module and
 *              also calls the initialization functions of the srstm and srcor modules.
 *
 * Test steps:
 * - expect a call to the IsConfigurationValid function and return true or false
 * - expect the initialization of srstm and srcor if the config is valid
 * - call the Init function and verify the return value
 * - verify that the initialization state is set correctly
 * .
 *
 *  @testParameter
 * - Configured Connections: Number of configured connections
 * - Config valid: Return value of srcor_IsConfigurationValid function call
 * - Return Code: Expected return code from srapi_Init function call
 * .
 * | Test Run | Test config                          || Expected values                               ||
 * |----------|------------------------|--------------|-----------------------------|------------------|
 * |          | Configured Connections | Config valid | Return Code                 | Test Result      |
 * | 0        | 0                      | true         | radef_kNoError              | Normal Operation |
 * | 1        | 0                      | false        | radef_kInvalidConfiguration | Normal operation |
 * | 2        | 2                      | true         | radef_kNoError              | Normal operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-544} Init sr_api Function
 * @verifyReq{RASW-267} Initialization Function Structure
 * @verifyReq{RASW-292} Configuration SafRetL
 * @verifyReq{RASW-290} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_P(InitTestWithParam, srapiTest001VerifyInitFunction) {
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
    configured_connections,
    {0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };

  // expect configuration check and return true or false (depending on the test parameter)
  EXPECT_CALL(srcor_mock, srcor_IsConfigurationValid(&config)).WillOnce(Return(is_config_valid));

  // expect state machine and core module to be initialized if config is valid
  if (is_config_valid) {
    EXPECT_CALL(srstm_mock, srstm_Init(configured_connections)).Times(1);
    EXPECT_CALL(srcor_mock, srcor_Init(&config)).Times(1);
  }

  // call the init function and verify the return value
  EXPECT_EQ(return_code, srapi_Init(&config));

  // check the initialization state
  if (is_config_valid) {
    EXPECT_TRUE(srapi_initialized);
  }
  else {
    EXPECT_FALSE(srapi_initialized);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitFunction,
  InitTestWithParam,
  ::testing::Values(
        std::make_tuple(0, true, radef_kNoError),
        std::make_tuple(0, false, radef_kInvalidConfiguration),
        std::make_tuple(UT_SRAPI_MAX_NUM_CONN, true, radef_kNoError)
  ));

/**
 * @test        @ID{srapiTest002} Verify the number of init function calls
 *
 * @details     This test verifies that the initialization is done only once.
 *
 * Test steps:
 * - call the init function and verify that it completes successfully
 * - call the init function again and verify that the return value is set to AlreadyInitialized
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-544} Init sr_api Function
 */
TEST_F(srapiTest, srapiTest002VerifyInitFunctionCalls) {
  const srcty_SafetyRetransmissionConfiguration dummy_config = {};

  // call the init function and verify that it completes successfully
  EXPECT_CALL(srcor_mock, srcor_IsConfigurationValid(&dummy_config)).WillOnce(Return(true));
  EXPECT_CALL(srstm_mock, srstm_Init(_)).Times(1);
  EXPECT_CALL(srcor_mock, srcor_Init(_)).Times(1);
  EXPECT_EQ(radef_kNoError, srapi_Init(&dummy_config));

  // call the init function again and verify that the return value is set to AlreadyInitialized
  EXPECT_EQ(radef_kAlreadyInitialized, srapi_Init(&dummy_config));
}

/**
 * @test        @ID{srapiTest003} Null pointer checks
 *
 * @details     This test verifies that every function returns InvalidParameter whenever it is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - initialize the module
 * - call the Init function with invalid configuration pointer and check the return value
 * - call the OpenConnection function with invalid connection_id pointer and check the return value
 * - call the SendData function with invalid data pointer and check the return value
 * - call the ReadData function with invalid message_size pointer and check the return value
 * - call the ReadData function with invalid data pointer and check the return value
 * - call the GetConnectionState function with invalid connection_state pointer and check the return value
 * - call the GetConnectionState function with invalid buffer_utilisation pointer and check the return value
 * - call the GetConnectionState function with invalid buffer_size pointer and check the return value
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-520} Error Handling
 * @verifyReq{RASW-521} Input Parameter Check
 * @verifyReq{RASW-544} Init sr_api Function
 * @verifyReq{RASW-546} Open Connection Function
 * @verifyReq{RASW-548} Send Data Function
 * @verifyReq{RASW-549} Read Data Function
 * @verifyReq{RASW-550} Get Connection State Function
 * @verifyReq{RASW-461} Struct Buffer Utilisation Structure
 */
TEST_F(srapiTest, srapiTest003VerifyNullPointerCheck) {
  const srcty_SafetyRetransmissionConfiguration dummy_config = {};

  uint32_t sender_id = 0;
  uint32_t receiver_id = 0;
  uint32_t network_id = 0;
  uint32_t connection_id = 0;
  uint16_t message_size = 0;
  uint16_t buffer_size = 1;
  uint8_t message_buffer = 0;
  sraty_ConnectionStates state = sraty_kConnectionMin;
  sraty_BufferUtilisation buffer_utilisation = {};

  // initialize the module
  EXPECT_CALL(srcor_mock, srcor_IsConfigurationValid(&dummy_config)).WillOnce(Return(true));
  EXPECT_CALL(srstm_mock, srstm_Init(_)).Times(1);
  EXPECT_CALL(srcor_mock, srcor_Init(_)).Times(1);
  EXPECT_EQ(radef_kNoError, srapi_Init(&dummy_config));

  // call the functions
  EXPECT_EQ(radef_kInvalidParameter, srapi_Init((srcty_SafetyRetransmissionConfiguration *)NULL));
  EXPECT_EQ(radef_kInvalidParameter, srapi_OpenConnection(sender_id, receiver_id, network_id, (uint32_t *)NULL));
  EXPECT_EQ(radef_kInvalidParameter, srapi_SendData(connection_id, message_size, (uint8_t *)NULL));
  EXPECT_EQ(radef_kInvalidParameter, srapi_ReadData(connection_id, buffer_size, (uint16_t *)NULL, &message_buffer));
  EXPECT_EQ(radef_kInvalidParameter, srapi_ReadData(connection_id, buffer_size, &message_size, (uint8_t *)NULL));
  EXPECT_EQ(radef_kInvalidParameter, srapi_GetConnectionState(connection_id, (sraty_ConnectionStates *)NULL, &buffer_utilisation, &buffer_size));
  EXPECT_EQ(radef_kInvalidParameter, srapi_GetConnectionState(connection_id, &state, (sraty_BufferUtilisation *)NULL, &buffer_size));
  EXPECT_EQ(radef_kInvalidParameter, srapi_GetConnectionState(connection_id, &state, &buffer_utilisation, (uint16_t *)NULL));
}

/**
 * @test        @ID{srapiTest004} Initialization checks
 *
 * @details     This test verifies that NotInitialized is returned whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the OpenConnection function without prior initialization and verify that NotInitialized is returned
 * - call the CloseConnection function without prior initialization and verify that NotInitialized is returned
 * - call the SendData function without prior initialization and verify that NotInitialized is returned
 * - call the ReadData function without prior initialization and verify that NotInitialized is returned
 * - call the GetConnectionState function without prior initialization and verify that NotInitialized is returned
 * - call the CheckTimings function without prior initialization and verify that NotInitialized is returned
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-543} Component sr_api Overview
 * @verifyReq{RASW-546} Open Connection Function
 * @verifyReq{RASW-547} Close Connection Function
 * @verifyReq{RASW-548} Send Data Function
 * @verifyReq{RASW-549} Read Data Function
 * @verifyReq{RASW-550} Get Connection State Function
 * @verifyReq{RASW-551} Check Timings Function
 */
TEST_F(srapiTest, srapiTest004VerifyInitializationCheck) {
  uint32_t sender_id = 0;
  uint32_t receiver_id = 0;
  uint32_t network_id = 0;
  uint32_t connection_id = 0;
  uint16_t detailed_reason = 0;
  uint16_t message_size = srcty_kMinSrLayerPayloadDataSize;
  uint16_t buffer_size = 1;
  uint8_t message_buffer = 0;
  sraty_ConnectionStates state = sraty_kConnectionMin;
  sraty_BufferUtilisation buffer_utilisation = {};

  // call the functions without initializing the module
  EXPECT_EQ(radef_kNotInitialized, srapi_OpenConnection(sender_id, receiver_id, network_id, &connection_id));
  EXPECT_EQ(radef_kNotInitialized, srapi_CloseConnection(connection_id, detailed_reason));
  EXPECT_EQ(radef_kNotInitialized, srapi_SendData(connection_id, message_size, &message_buffer));
  EXPECT_EQ(radef_kNotInitialized, srapi_ReadData(connection_id, buffer_size, &message_size, &message_buffer));
  EXPECT_EQ(radef_kNotInitialized, srapi_GetConnectionState(connection_id, &state, &buffer_utilisation, &buffer_size));
  EXPECT_EQ(radef_kNotInitialized, srapi_CheckTimings());
}

/**
 * @test        @ID{srapiTest005} Verify GetInitializationState
 *
 * @details     This test verifies the GetInitializationState function of the srapi module
 *
 * Test steps:
 * - call the GetInitializationState function without prior initialization and verify that NotInitialized is returned
 * - initialize the module
 * - call the GetInitializationState function again and verify that it returns NoError
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-545} Get Initialization State Function
 * @verifyReq{RASW-306} Get Initialization State Function Structure
 * @verifyReq{RASW-305} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_F(srapiTest, srapiTest005VerifyGetInitializationState) {
  const srcty_SafetyRetransmissionConfiguration dummy_config = {};

  // call the GetInitializationState function without prior initialization and verify that it returns NotInitialized
  EXPECT_EQ(radef_kNotInitialized, srapi_GetInitializationState());

  // initialize the module
  EXPECT_CALL(srcor_mock, srcor_IsConfigurationValid(&dummy_config)).WillOnce(Return(true));
  EXPECT_CALL(srstm_mock, srstm_Init(_)).Times(1);
  EXPECT_CALL(srcor_mock, srcor_Init(_)).Times(1);
  EXPECT_EQ(radef_kNoError, srapi_Init(&dummy_config));

  // call the GetInitializationState function again and verify that it returns NoError
  EXPECT_EQ(radef_kNoError, srapi_GetInitializationState());
}

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
