/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redcor_flags.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version dc5cb6cccc23cd7e19bfaa5a9692e5abe0414803
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4937: Fixed out of boundary access for array in redcor unit tests,05.12.2022,N. Fricker}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4967: Added channel id check before setting message in buffer flag,12.12.2022,N. Fricker}
 *
 * @brief Unit test file for core module flag tests of the redundancy layer.
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
 * @brief Test class for value-parametrized SetMessagePendingFlag function test
 *
 * Parameter order:
 * - Redundancy channel ID (uint32_t)
 * - Transport channel ID (uint32_t)
 * - Indicates if transport channel id is valid (bool)
 * - Expect Fatal error (bool)
 * .
 */
class SetMessagePendingFlagTestWithParam :    public redcorTest,
                                              public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool, bool>>
{
  public:
  uint32_t redundancy_channel_id                = std::get<0>(GetParam());        ///< redundancy channel id
  uint32_t transport_channel_id                 = std::get<1>(GetParam());        ///< transport channel id
  bool transport_channel_id_valid               = std::get<2>(GetParam());        ///< indicates if transport channel id is valid
  bool expect_fatal_error                       = std::get<3>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized GetMessagePendingFlag function test
 *
 * Parameter order:
 * - Transport channel id to set pending flag (uint32_t)
 * - Redundancy channel ID to get flag (uint32_t)
 * - Transport channel ID to get flag (uint32_t)
 * - Indicates if transport channel id is valid (bool)
 * - Expect pending flag (bool)
 * - Expect Fatal error (bool)
 * .
 */
class GetMessagePendingFlagTestWithParam :    public redcorTest,
                                              public testing::WithParamInterface<std::tuple<uint32_t, uint32_t , uint32_t, bool, bool, bool>>
{
  public:
  uint32_t transport_channel_id_to_set_pending                      = std::get<0>(GetParam());        ///< transport channel id to set pending flag
  uint32_t redundancy_channel_id                                    = std::get<1>(GetParam());        ///< redundancy channel id
  uint32_t transport_channel_id                                     = std::get<2>(GetParam());        ///< transport channel id
  bool transport_channel_id_valid                                   = std::get<3>(GetParam());        ///< indicates if transport channel id is valid
  bool expect_pending                                               = std::get<4>(GetParam());        ///< expect pending flag
  bool expect_fatal_error                                           = std::get<5>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized ClearMessagePendingFlag function test
 *
 * Parameter order:
 * - Transport channel ID to set pending (uint32_t)
 * - Redundancy channel ID (uint32_t)
 * - Transport channel ID to clear pending (uint32_t)
 * - Indicates if transport channel id is valid (bool)
 * - Expect Fatal error (bool)
 * .
 */
class ClearMessagePendingFlagTestWithParam :  public redcorTest,
                                              public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, bool, bool>>
{
  public:
  uint32_t transport_channel_id_to_set_pending                    = std::get<0>(GetParam());        ///< transport channel id to clear pending flag
  uint32_t redundancy_channel_id                                  = std::get<1>(GetParam());        ///< redundancy channel id
  uint32_t transport_channel_id_to_clear_pending                  = std::get<2>(GetParam());        ///< transport channel id to clear pending flag
  bool transport_channel_id_valid                                 = std::get<3>(GetParam());        ///< indicates if transport channel id is valid
  bool expect_fatal_error                                         = std::get<4>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized ClearSendBufferMessagePendingFlag function test
 *
 * Parameter order:
 * - Redundancy channel ID (uint32_t)
 * - Expect Fatal error (bool)
 * .
 */
class ClearSendBufferMessagePendingFlagTestWithParam :  public redcorTest,
                                                        public testing::WithParamInterface<std::tuple<uint32_t, bool>>
{
  public:
  uint32_t redundancy_channel_id                       = std::get<0>(GetParam());        ///< redundancy channel id to clear pending flag
  bool expect_fatal_error                              = std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized ClearInputBufferMessagePendingFlag function test
 *
 * Parameter order:
 * - Redundancy channel ID (uint32_t)
 * - Expect Fatal error (bool)
 * .
 */
class ClearInputBufferMessagePendingFlagTestWithParam :  public redcorTest,
                                                        public testing::WithParamInterface<std::tuple<uint32_t, bool>>
{
  public:
  uint32_t redundancy_channel_id                       = std::get<0>(GetParam());        ///< redundancy channel id to clear pending flag
  bool expect_fatal_error                              = std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup red_core
 * @{
 */

/**
 * @test        @ID{redcorTest010} Verify the SetMessagePendingFlag function.
 *
 * @details     This test verifies the correct behaviour of the SetMessagePendingFlag function.
 *
 * Test steps:
 * - Init module with given config for transport channel ids
 * - Verify message pending flag set to false
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call SetMessagePendingFlag function for given redundancy and transport channel
 * - Verify message pending flag set to true if no error expected
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Transport channel id: Transport channel identification
 * - Transport channel id valid: Return value of IsTransportChannelIdValid function
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                             || Test config                | Expected values                  ||
 * |----------|-----------------------|----------------------|----------------------------|--------------|--------------------|
 * |          | Redundancy channel id | Transport channel id | Transport channel id valid | Expect Fatal | Test Result        |
 * | 0        | 0U                    | 0U                   | true                       | false        | Normal operation   |
 * | 1        | 0U                    | 1U                   | true                       | false        | Normal operation   |
 * | 2        | 1U                    | 2U                   | true                       | false        | Normal operation   |
 * | 3        | 1U                    | 3U                   | true                       | false        | Normal operation   |
 * | 4        | 2U                    | 3U                   | false                      | true         | Fatal error raised |
 * | 5        | 1U                    | 4U                   | false                      | true         | Fatal error raised |
 * | 6        | 0U                    | 2U                   | false                      | true         | Fatal error raised |
 * | 7        | 0U                    | 3U                   | false                      | true         | Fatal error raised |
 * | 8        | 1U                    | 0U                   | false                      | true         | Fatal error raised |
 * | 9        | 1U                    | 1U                   | false                      | true         | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(SetMessagePendingFlagTestWithParam, redcorTest010VerifySetMessagePendingFlag)
{
  // expect calls for configuration validation and init of both redundancy channel
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  // expect calls for init of each redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(default_config.number_of_redundancy_channels);

  if(redundancy_channel_id <= UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX)
  {
    // expect call for transport channel validation when no error thrown at redundancy channel validation
    EXPECT_CALL(reddia_mock, reddia_IsTransportChannelIdValid(redundancy_channel_id, _)).Times(1).WillRepeatedly(Return(transport_channel_id_valid));
  }

  // init the module
  redcor_Init(&default_config);

  // perform the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));
    EXPECT_THROW(redcor_SetMessagePendingFlag(redundancy_channel_id, transport_channel_id), std::invalid_argument);
  }
  else
  {
    // verify pending flag not set
    EXPECT_FALSE(redcor_redundancy_channels[redundancy_channel_id].received_data_pending[transport_channel_id]);

    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcor_SetMessagePendingFlag(redundancy_channel_id, transport_channel_id));

    // verify pending flag set
    EXPECT_TRUE(redcor_redundancy_channels[redundancy_channel_id].received_data_pending[transport_channel_id]);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySetMessagePendingFlag,
  SetMessagePendingFlagTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN, true, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, (UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), true, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), true, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX, true, false),
    std::make_tuple((UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX, false, true),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX + 1), false, true),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), false, true),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX, false, true),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN, false, true),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, (UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), false, true)
  )
);

/**
 * @test        @ID{redcorTest011} Verify the GetMessagePendingFlag function.
 *
 * @details     This test verifies the correct behaviour of the GetMessagePendingFlag function.
 *
 * Test steps:
 * - Init module with given default config
 * - Set pending flag to transport channel given by test parameter
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call GetMessagePendingFlag function for given redundancy and transport channel and verify pending flag according test parameter
 * .
 *
 * @testParameter
 * - Red channel id to get flag: Redundancy channel identification to get flag from
 * - Tr channel id to get flag: Transport channel identification to get flag from
 * - Tr channel id to set flag: Transport channel identification to set flag
 * - Tr channel id valid: Return value of IsTransportChannelIdValid function
 * - Expect pending flag: Indicates the expected value of pending flag
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                       || Test config                                    || Expected values                                       |||
 * |----------|----------------------------|---------------------------|---------------------------|---------------------|---------------------|--------------|--------------------|
 * |          | Red channel id to get flag | Tr channel id to get flag | Tr channel id to set flag | Tr channel id valid | Expect pending flag | Expect Fatal | Test Result        |
 * | 0        | 0                          | 0                         | 0                         | true                | true                | false        | Normal operation   |
 * | 1        | 0                          | 1                         | 1                         | true                | true                | false        | Normal operation   |
 * | 2        | 0                          | 1                         | 0                         | true                | false               | false        | Normal operation   |
 * | 3        | 0                          | 0                         | 1                         | true                | false               | false        | Normal operation   |
 * | 4        | 1                          | 3                         | 3                         | true                | true                | false        | Normal operation   |
 * | 5        | 1                          | 2                         | 2                         | true                | true                | false        | Normal operation   |
 * | 6        | 1                          | 2                         | 3                         | true                | false               | false        | Normal operation   |
 * | 7        | 1                          | 3                         | 2                         | true                | false               | false        | Normal operation   |
 * | 8        | 2                          | 3                         | 3                         | false               | false               | true         | Fatal error raised |
 * | 9        | 1                          | 4                         | 3                         | false               | false               | true         | Fatal error raised |
 * | 10       | 0                          | 2                         | 0                         | false               | false               | true         | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(GetMessagePendingFlagTestWithParam, redcorTest011VerifyGetMessagePendingFlag)
{
  // expect calls for configuration validation and init of both redundancy channel
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  // expect calls for init of each redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(default_config.number_of_redundancy_channels);

  if(redundancy_channel_id <= UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX)
  {
    if(transport_channel_id_valid)
    {
      // expect call for transport channel validation on set and get
      EXPECT_CALL(reddia_mock, reddia_IsTransportChannelIdValid(redundancy_channel_id, _)).Times(2).WillRepeatedly(Return(transport_channel_id_valid));
    }
    else
    {
      // expect call for transport channel validation only on get
      EXPECT_CALL(reddia_mock, reddia_IsTransportChannelIdValid(redundancy_channel_id, transport_channel_id)).WillOnce(Return(transport_channel_id_valid));
    }
  }

  // init the module
  redcor_Init(&default_config);

  if(transport_channel_id_valid)
  {
    // set pending flag
    redcor_SetMessagePendingFlag(redundancy_channel_id, transport_channel_id_to_set_pending);
  }

  // perform the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));
    EXPECT_THROW(redcor_GetMessagePendingFlag(redundancy_channel_id, transport_channel_id), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_EQ(expect_pending, redcor_GetMessagePendingFlag(redundancy_channel_id, transport_channel_id));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyGetMessagePendingFlag,
  GetMessagePendingFlagTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN,       UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN,       true,  true,  false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), true,  true,  false),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), true,  false, false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN,       UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN,       true,  false, false),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX,       UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       true,  true,  false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), true,  true,  false),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), true,  false, false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX,       UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       true,  false, false),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       (UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       false, false, true),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX + 1), false, false, true),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 2), false, false, true)
  )
);

/**
 * @test        @ID{redcorTest012} Verify the ClearMessagePendingFlag function.
 *
 * @details     This test verifies the correct behaviour of the ClearMessagePendingFlag function.
 *
 * Test steps:
 * - Init module with given default config
 * - Set Message pending flag for given redundancy and transport channel
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call ClearMessagePendingFlag function for given redundancy and transport channel
 * - Verify message pending flag set to expected value
 * .
 *
 * @testParameter
 * - Red channel id: Redundancy channel identification to clear flag
 * - Tr channel id to clear flag: Transport channel identification to clear flag
 * - Tr channel id to set flag: Transport channel identification to set flag
 * - Tr channel valid: Return value of IsTransportChannelIdValid function
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                              || Test config                                    || Expected values                  ||
 * |----------|-----------------|-----------------------------|---------------------------|---------------------|--------------|--------------------|
 * |          | Red channel id  | Tr channel id to clear flag | Tr channel id to set flag | Tr channel valid    | Expect Fatal | Test Result        |
 * | 0        | 0               | 0                           | 0                         | true                | false        | Normal operation   |
 * | 1        | 0               | 1                           | 1                         | true                | false        | Normal operation   |
 * | 2        | 0               | 1                           | 0                         | true                | false        | Normal operation   |
 * | 3        | 0               | 0                           | 1                         | true                | false        | Normal operation   |
 * | 4        | 1               | 3                           | 3                         | true                | false        | Normal operation   |
 * | 5        | 1               | 2                           | 2                         | true                | false        | Normal operation   |
 * | 6        | 1               | 2                           | 3                         | true                | false        | Normal operation   |
 * | 7        | 1               | 3                           | 2                         | true                | false        | Normal operation   |
 * | 8        | 2               | 3                           | 3                         | false               | true         | Fatal error raised |
 * | 9        | 1               | 4                           | 3                         | false               | true         | Fatal error raised |
 * | 10       | 0               | 2                           | 0                         | false               | true         | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(ClearMessagePendingFlagTestWithParam, redcorTest012VerifyClearMessagePendingFlag)
{
  // expect calls for configuration validation and init of both redundancy channel
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  // expect calls for init of each redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(default_config.number_of_redundancy_channels);

  if(redundancy_channel_id <= UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX)
  {
    if(transport_channel_id_valid)
    {
      // expect call for transport channel validation on set and clear
      EXPECT_CALL(reddia_mock, reddia_IsTransportChannelIdValid(redundancy_channel_id, _)).Times(2).WillRepeatedly(Return(transport_channel_id_valid));
    }
    else
    {
      // expect call for transport channel validation only on get
      EXPECT_CALL(reddia_mock, reddia_IsTransportChannelIdValid(redundancy_channel_id, transport_channel_id_to_clear_pending)).WillOnce(Return(transport_channel_id_valid));
    }
  }

  // init the module
  redcor_Init(&default_config);

  if(transport_channel_id_valid)
  {
    // set pending flag and verify
    EXPECT_NO_THROW(redcor_SetMessagePendingFlag(redundancy_channel_id, transport_channel_id_to_set_pending));
    EXPECT_TRUE(redcor_redundancy_channels[redundancy_channel_id].received_data_pending[transport_channel_id_to_set_pending]);
  }

  // perform the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));
    EXPECT_THROW(redcor_ClearMessagePendingFlag(redundancy_channel_id, transport_channel_id_to_clear_pending), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcor_ClearMessagePendingFlag(redundancy_channel_id, transport_channel_id_to_clear_pending));
    EXPECT_FALSE(redcor_redundancy_channels[redundancy_channel_id].received_data_pending[transport_channel_id_to_clear_pending]);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyClearMessagePendingFlag,
  ClearMessagePendingFlagTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN,       UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN,       true,  false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), true,  false),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), true,  false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN,       UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN,       true,  false),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX,       UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       true,  false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), true,  false),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), true,  false),
    std::make_tuple((UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX - 1), UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX,       UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       true,  false),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       (UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       false, true),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MAX + 1), false, true),
    std::make_tuple(UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN,       UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN,       (UT_REDCOR_TRANSPORT_CHANNEL_ID_MIN + 2), false, true)
  )
);

/**
 * @test        @ID{redcorTest014} Verify the ClearSendBufferMessagePendingFlag function.
 *
 * @details     This test verifies the correct behaviour of the ClearSendBufferMessagePendingFlag function.
 *
 * Test steps:
 * - Init module with default config
 * - Write message payload to sendBuffer and verify message in buffer flag set to true
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call ClearSendBufferMessagePendingFlag function for given redundancy channel
 * - Verify message_in_buffer flag set to false
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter       | Test config      | Expected values    |
 * |----------|-----------------------|------------------|--------------------|
 * |          | Redundancy channel id | Expect Fatal     | Test Result        |
 * | 0        | 0U                    | false            | Normal operation   |
 * | 1        | 1U                    | false            | Normal operation   |
 * | 2        | 2U                    | true             | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(ClearSendBufferMessagePendingFlagTestWithParam, redcorTest014VerifyClearSendBufferMessagePendingFlag)
{
  // test variables
  uint8_t payload[UT_REDCOR_DATA_LEN_MAX] = {};
  uint16_t payload_size = UT_REDCOR_DATA_LEN_MAX;

  // expect calls for configuration validation and init of both redundancy channel
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
    EXPECT_THROW(redcor_ClearSendBufferMessagePendingFlag(redundancy_channel_id), std::invalid_argument);
  }
  else
  {
    // Add message and verify
    EXPECT_NO_THROW(redcor_WriteMessagePayloadToSendBuffer(redundancy_channel_id, payload_size, payload));
    EXPECT_TRUE(redcor_redundancy_channels[redundancy_channel_id].send_buffer.message_in_buffer);

    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcor_ClearSendBufferMessagePendingFlag(redundancy_channel_id));

    EXPECT_FALSE(redcor_redundancy_channels[redundancy_channel_id].send_buffer.message_in_buffer);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyClearSendBufferMessagePendingFlag,
  ClearSendBufferMessagePendingFlagTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, false),
    std::make_tuple((UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), true)
  )
);

/**
 * @test        @ID{redcorTest016} Verify the ClearInputBufferMessagePendingFlag function.
 *
 * @details     This test verifies the correct behaviour of the ClearInputBufferMessagePendingFlag function.
 *
 * Test steps:
 * - Init module with default config
 * - Set message in buffer flag on input buffer to true and verify
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Call ClearInputBufferMessagePendingFlag function for given redundancy channel
 * - Verify input_buffer.message_in_buffer flag set to false
 * .
 *
 * @testParameter
 * - Redundancy channel id: Redundancy channel identification
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter       | Expected values                      ||
 * |----------|-----------------------|------------------|--------------------|
 * |          | Redundancy channel id | Expect Fatal     | Test Result        |
 * | 0        | 0U                    | false            | Normal operation   |
 * | 1        | 1U                    | false            | Normal operation   |
 * | 2        | 2U                    | true             | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(ClearInputBufferMessagePendingFlagTestWithParam, redcorTest016VerifyClearInputBufferMessagePendingFlag)
{
  // test variables
  uint8_t payload[UT_REDCOR_DATA_LEN_MAX] = {};
  uint16_t payload_size = UT_REDCOR_DATA_LEN_MAX;

  // expect calls for configuration validation and init of both redundancy channel
  EXPECT_CALL(reddia_mock, reddia_IsConfigurationValid(_)).WillOnce(Return(true));
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyLayerDiagnostics(_)).Times(1);
  EXPECT_CALL(redmsg_mock, redmsg_Init(_)).Times(1);

  // expect calls for init of each redundancy channel
  EXPECT_CALL(reddfq_mock, reddfq_InitDeferQueue(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(redrbf_mock, redrbf_InitBuffer(_)).Times(default_config.number_of_redundancy_channels);
  EXPECT_CALL(reddia_mock, reddia_InitRedundancyChannelDiagnostics(_)).Times(default_config.number_of_redundancy_channels);

  // init the module
  redcor_Init(&default_config);

  if(redundancy_channel_id <= UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX)
  {
    // set flag to true and verify
    redcor_redundancy_channels[redundancy_channel_id].input_buffer.message_in_buffer = true;
    EXPECT_TRUE(redcor_redundancy_channels[redundancy_channel_id].input_buffer.message_in_buffer);
  }

  // perform the test
  if(expect_fatal_error)
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).Times(1).WillOnce(Invoke(this, &redcorTest::invalidArgumentException));
    EXPECT_THROW(redcor_ClearInputBufferMessagePendingFlag(redundancy_channel_id), std::invalid_argument);
  }
  else
  {
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcor_ClearInputBufferMessagePendingFlag(redundancy_channel_id));

    EXPECT_FALSE(redcor_redundancy_channels[redundancy_channel_id].input_buffer.message_in_buffer);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyClearInputBufferMessagePendingFlag,
  ClearInputBufferMessagePendingFlagTestWithParam,
  ::testing::Values(
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MIN, false),
    std::make_tuple(UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX, false),
    std::make_tuple((UT_REDCOR_REDUNDANCY_CHANNEL_ID_MAX + 1), true)
  )
);

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
