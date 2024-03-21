/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srdia.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the diagnostics module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srdia.hh"

using ::testing::_;
using ::testing::InSequence;
using ::testing::SaveArg;
using ::testing::Sequence;

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define UT_SRDIA_NR_OF_BINS 5U         ///< number of bins
#define UT_SRDIA_B0_MIN 0U             ///< bin 0 min interval value
#define UT_SRDIA_B0_MAX 199U           ///< bin 0 max interval value
#define UT_SRDIA_B1_MIN 200U           ///< bin 1 min interval value
#define UT_SRDIA_B1_MAX 399U           ///< bin 1 max interval value
#define UT_SRDIA_B2_MIN 400U           ///< bin 2 min interval value
#define UT_SRDIA_B2_MAX 599U           ///< bin 2 max interval value
#define UT_SRDIA_B3_MIN 600U           ///< bin 3 min interval value
#define UT_SRDIA_B3_MAX 799U           ///< bin 3 max interval value
#define UT_SRDIA_B4_MIN 800U           ///< bin 4 min interval value
#define UT_SRDIA_B4_MAX UT_SRDIA_T_MAX          ///< bin 4 max interval value

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
 * - configured connections (uint32_t)
 * - configured t max (uint32_t)
 * - configured nDiagWindow (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class InitTestWithParam : public srdiaTest,
    public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, bool>> {
public:
  const uint32_t configured_connections = std::get<0>(GetParam()); ///< number of configured connections
  const uint32_t configured_t_max = std::get<1>(GetParam()); ///< configured t max
  const uint32_t configured_n_diag_window = std::get<2>(GetParam()); ///< configured size of diagnostic window
  const bool expect_fatal_error = std::get<3>(GetParam()); ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized init connection diagnostics test
 *
 * Parameter order:
 * - configured connection ID (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class InitConnectionDiagnosticsTestWithParam : public srdiaTest,
    public testing::WithParamInterface<std::tuple<uint32_t, bool>> {
public:
  const uint32_t configured_connection_id = std::get<0>(GetParam()); ///< the connection index to use
  const bool expect_fatal_error = std::get<1>(GetParam()); ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized update connection diagnostics test
 *
 * Parameter order:
 * - configured connection ID (uint32_t)
 * - number of updates (uint32_t)
 * - round trip delay (uint32_t)
 * - round trip delay bin (uint32_t)
 * - alive time (uint32_t)
 * - alive time bin (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class UpdateConnectionDiagnosticsTestWithParam : public srdiaTest,
    public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool>> {
public:
  const uint32_t configured_connection_id = std::get<0>(GetParam()); ///< the connection index to use
  const uint32_t nr_of_updates = std::get<1>(GetParam()); ///< number of updates
  const uint32_t round_trip_delay = std::get<2>(GetParam()); ///< the round trip delay in ms
  const uint32_t round_trip_delay_bin = std::get<3>(GetParam()); ///< in which bin the given round trip delay lands
  const uint32_t alive_time = std::get<4>(GetParam()); ///< the alive time in ms
  const uint32_t alive_time_bin = std::get<5>(GetParam()); ///< in which bin the given alive time lands
  const bool expect_fatal_error = std::get<6>(GetParam()); ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized send diagnostic notification test
 *
 * Parameter order:
 * - configured connection ID (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class SendDiagnosticNotificationTestWithParam : public srdiaTest,
    public testing::WithParamInterface<std::tuple<uint32_t, bool>> {
public:
  const uint32_t configured_connection_id = std::get<0>(GetParam()); ///< the connection index to use
  const bool expect_fatal_error = std::get<1>(GetParam()); ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - t max (uint32_t)
 * - diagnostic timing distribution interval 1 (uint32_t)
 * - diagnostic timing distribution interval 2 (uint32_t)
 * - diagnostic timing distribution interval 3 (uint32_t)
 * - diagnostic timing distribution interval 4 (uint32_t)
 * - expected return value (bool)
 * - expect fatal error (bool)
 * .
 */
class AreDiagnosticTimingIntervalsValidTestWithParam : public srdiaTest,
    public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, bool, bool>> {
public:
  const uint32_t t_max = std::get<0>(GetParam()); ///< t max
  const uint32_t timings_distribution_interval_1 = std::get<1>(GetParam()); ///< diagnostic timing distribution interval 1
  const uint32_t timings_distribution_interval_2 = std::get<2>(GetParam()); ///< diagnostic timing distribution interval 2
  const uint32_t timings_distribution_interval_3 = std::get<3>(GetParam()); ///< diagnostic timing distribution interval 3
  const uint32_t timings_distribution_interval_4 = std::get<4>(GetParam()); ///< diagnostic timing distribution interval 4
  const bool expect_to_be_valid = std::get<5>(GetParam()); ///< if the diagnostics timing intervals are expected to be valid
  const bool expect_fatal_error = std::get<6>(GetParam()); ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_diagnostics
 * @{
 */

/**
 * @test        @ID{srdiaTest001} Verify init function
 *
 * @details     This test verifies that the init function sets the
 *              number_of_connections and the n_diag_window parameter correctly.
 *
 * Test steps:
 * - Set the fatal error expectation for radef_kInvalidParameter based on the test parameters
 * - Call the init function with given test parameter
 *   - Check that a correct configuration does not cause the init function to throw
 *   - Check that an incorrect configuration causes the init function to throw
 * - Verify if the number_of_connections parameter is set correctly
 * - Verify that the n_diag_window parameter is set correctly
 * - Verify that the module is set to the initialized state
 * .
 *
 * @testParameter
 * - Number of connections: Number of configured RaSTA connections
 * - T Max: Configured maximal accepted age of a message in miliseconds (valid range: 0 - 2000)
 * - N diag window: Configured diagnosis window size (messages) (valid range: 100 - 10000)
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                             ||| Expected values                        ||
 * |----------|-----------------------|-------|---------------|--------------------|--------------------|
 * |          | Number of connections | T Max | N diag window | Expect fatal error | Test Result        |
 * | 0        | 0                     | 750   | 100           | True               | Fatal error raised |
 * | 1        | 1                     | 749   | 100           | True               | Fatal error raised |
 * | 2        | 1                     | 750   | 99            | True               | Fatal error raised |
 * | 3        | 1                     | 750   | 100           | false              | Normal operation   |
 * | 4        | 2                     | 2000  | 10000         | false              | Normal operation   |
 * | 5        | 2                     | 2000  | 10001         | True               | Fatal error raised |
 * | 6        | 2                     | 2001  | 10000         | True               | Fatal error raised |
 * | 7        | 3                     | 2000  | 10000         | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-438} N diagWindow
 * @verifyReq{RASW-642} Init sr_diagnostics Function
 */
TEST_P(InitTestWithParam, srdiaTest001VerifyInitFunction) {
  // set expectations /////////////////////////////
  if (expect_fatal_error) {
    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .WillOnce(Invoke(this, &srdiaTest::invalidArgumentException))
    ;
  }

  // perform test /////////////////////////////////
  if (expect_fatal_error) {
    // call the init function and expect a throw
    EXPECT_THROW(srdia_Init(configured_connections, configured_t_max, configured_n_diag_window, diag_timings_distribution_intervals), std::invalid_argument);
  }
  else {
    extern uint32_t srdia_number_of_connections;
    extern uint32_t srdia_n_diag_window;

    // call the init function and verify if parameter are set correctly
    EXPECT_NO_THROW(srdia_Init(configured_connections, configured_t_max, configured_n_diag_window, diag_timings_distribution_intervals));

    EXPECT_EQ(srdia_number_of_connections, configured_connections);
    EXPECT_EQ(srdia_n_diag_window, configured_n_diag_window);
    EXPECT_TRUE(srdia_initialized);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitFunction,
  InitTestWithParam,
  ::testing::Values(
//                    Number of connections
//                    |                                         T max
//                    |                                         |                   N diag window
//                    |                                         |                   |                                Expect fatal error
//                    |                                         |                   |                                |
      std::make_tuple(UT_SRDIA_CONFIGURED_CONNECTIONS_MIN - 1U, UT_SRDIA_T_MIN,     UT_SRDIA_N_DIAG_WINDOW_MIN,      true),
      std::make_tuple(UT_SRDIA_CONFIGURED_CONNECTIONS_MIN,      UT_SRDIA_T_MIN - 1, UT_SRDIA_N_DIAG_WINDOW_MIN,      true),
      std::make_tuple(UT_SRDIA_CONFIGURED_CONNECTIONS_MIN,      UT_SRDIA_T_MIN,     UT_SRDIA_N_DIAG_WINDOW_MIN - 1U, true),
      std::make_tuple(UT_SRDIA_CONFIGURED_CONNECTIONS_MIN,      UT_SRDIA_T_MIN,     UT_SRDIA_N_DIAG_WINDOW_MIN,      false),
      std::make_tuple(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX,      UT_SRDIA_T_MAX,     UT_SRDIA_N_DIAG_WINDOW_MAX,      false),
      std::make_tuple(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX,      UT_SRDIA_T_MAX,     UT_SRDIA_N_DIAG_WINDOW_MAX + 1U, true),
      std::make_tuple(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX,      UT_SRDIA_T_MAX + 1, UT_SRDIA_N_DIAG_WINDOW_MIN,      true),
      std::make_tuple(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX + 1U, UT_SRDIA_T_MAX,     UT_SRDIA_N_DIAG_WINDOW_MAX,      true)
  )
);

/**
 * @test        @ID{srdiaTest002} Verify init connection diagnostics function
 *
 * @details     This test verifies that the init connection diagnostics functions
 *              resets all the connection diagnostics values
 *
 * Test steps:
 * - Set the fatal error expectation for radef_kInvalidParameter based on the test parameters
 * - Call the init function
 * - Increment all diagnostic counters
 * - Produce a diagnostic notification to check that all diagnostic counters were incremented
 * - Increment all diagnostic counters
 * - Init the connection diagnostics
 * - Produce a diagnostic notification to check that all diagnostic counters were reset
 * .
 *
 * @testParameter
 * - Configured connection Id: RaSTA connection identification
 * - Expect fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter          | Expected values                        ||
 * |----------|--------------------------|--------------------|--------------------|
 * |          | Configured connection Id | Expect fatal Error | Test Result        |
 * | 0        | 0                        | False              | Normal operation   |
 * | 1        | 1                        | False              | Normal operation   |
 * | 2        | 2                        | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-321} Connection Diagnostic Data
 * @verifyReq{RASW-326} Connection Identification
 * @verifyReq{RASW-327} SafRetL Diagnostic Notification Structure
 * @verifyReq{RASW-433} Diagnostic Timing Interval
 * @verifyReq{RASW-468} EC safety
 * @verifyReq{RASW-470} Struct Connection Diagnostic Data Structure
 * @verifyReq{RASW-477} Alive Time Distribution
 * @verifyReq{RASW-478} Round Trip Delay Time Distribution
 * @verifyReq{RASW-479} EC CSN
 * @verifyReq{RASW-480} EC SN
 * @verifyReq{RASW-481} EC type
 * @verifyReq{RASW-482} EC address
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-556} Sr Diagnostic Notification
 * @verifyReq{RASW-636} Component sr_diagnostics Overview
 * @verifyReq{RASW-637} Inc Address Error Counter Function
 * @verifyReq{RASW-638} Inc Confirmed Sequence Number Error Counter Function
 * @verifyReq{RASW-639} Inc Safety Code Error Counter Function
 * @verifyReq{RASW-640} Inc Sequence Number Error Counter Function
 * @verifyReq{RASW-641} Inc Type Error Counter Function
 * @verifyReq{RASW-643} Init Connection Diagnostics Function
 * @verifyReq{RASW-644} Send Diagnostic Notification Function
 * @verifyReq{RASW-645} Update Connection Diagnostics Function
 */
TEST_P(InitConnectionDiagnosticsTestWithParam, srdiaTest002VerifyInitConnectionDiagnosticsFunction) {
  // set expectations /////////////////////////////
  if (expect_fatal_error) {
    const uint32_t nr_of_function_call = 17U;
    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .Times(nr_of_function_call)
        .WillRepeatedly(Invoke(this, &srdiaTest::invalidArgumentException))
    ;
    EXPECT_CALL(srnot_mock, srnot_SrDiagnosticNotification)
        .Times(0)
    ;
  }
  else {
    EXPECT_CALL(srnot_mock, srnot_SrDiagnosticNotification(configured_connection_id, _))
        .WillOnce(SaveArg<1>(&diag_noti))
        .WillOnce(SaveArg<1>(&diag_noti))
    ;
  }

  // perform test /////////////////////////////////
  srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, diag_timings_distribution_intervals);

  memset(&diag_noti, 0U, sizeof(diag_noti));

  if (expect_fatal_error) {
    EXPECT_THROW(srdia_IncSafetyCodeErrorCounter(configured_connection_id), std::invalid_argument);
    EXPECT_THROW(srdia_IncAddressErrorCounter(configured_connection_id), std::invalid_argument);
    EXPECT_THROW(srdia_IncTypeErrorCounter(configured_connection_id), std::invalid_argument);
    EXPECT_THROW(srdia_IncSequenceNumberErrorCounter(configured_connection_id), std::invalid_argument);
    EXPECT_THROW(srdia_IncConfirmedSequenceNumberErrorCounter(configured_connection_id), std::invalid_argument);
    EXPECT_THROW(srdia_UpdateConnectionDiagnostics(configured_connection_id, UT_SRDIA_T_MIN, UT_SRDIA_T_MIN), std::invalid_argument);
    EXPECT_THROW(srdia_UpdateConnectionDiagnostics(configured_connection_id, UT_SRDIA_T_MAX, UT_SRDIA_T_MAX), std::invalid_argument);

    EXPECT_THROW(srdia_SendDiagnosticNotification(configured_connection_id), std::invalid_argument);
    EXPECT_EQ(diag_noti.ec_safety, 0U);
    EXPECT_EQ(diag_noti.ec_address, 0U);
    EXPECT_EQ(diag_noti.ec_type, 0U);
    EXPECT_EQ(diag_noti.ec_sn, 0U);
    EXPECT_EQ(diag_noti.ec_csn, 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[0], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[1], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[2], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[3], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[4], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[0], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[1], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[2], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[3], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[4], 0U);

    EXPECT_THROW(srdia_IncSafetyCodeErrorCounter(configured_connection_id), std::invalid_argument);
    EXPECT_THROW(srdia_IncAddressErrorCounter(configured_connection_id), std::invalid_argument);
    EXPECT_THROW(srdia_IncTypeErrorCounter(configured_connection_id), std::invalid_argument);
    EXPECT_THROW(srdia_IncSequenceNumberErrorCounter(configured_connection_id), std::invalid_argument);
    EXPECT_THROW(srdia_IncConfirmedSequenceNumberErrorCounter(configured_connection_id), std::invalid_argument);
    EXPECT_THROW(srdia_UpdateConnectionDiagnostics(configured_connection_id, UT_SRDIA_T_MIN, UT_SRDIA_T_MIN), std::invalid_argument);
    EXPECT_THROW(srdia_UpdateConnectionDiagnostics(configured_connection_id, UT_SRDIA_T_MAX, UT_SRDIA_T_MAX), std::invalid_argument);

    EXPECT_THROW(srdia_InitConnectionDiagnostics(configured_connection_id), std::invalid_argument);

    EXPECT_THROW(srdia_SendDiagnosticNotification(configured_connection_id), std::invalid_argument);
    EXPECT_EQ(diag_noti.ec_safety, 0U);
    EXPECT_EQ(diag_noti.ec_address, 0U);
    EXPECT_EQ(diag_noti.ec_type, 0U);
    EXPECT_EQ(diag_noti.ec_sn, 0U);
    EXPECT_EQ(diag_noti.ec_csn, 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[0], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[1], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[2], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[3], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[4], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[0], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[1], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[2], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[3], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[4], 0U);
  }
  else {
    EXPECT_NO_THROW(srdia_IncSafetyCodeErrorCounter(configured_connection_id));
    EXPECT_NO_THROW(srdia_IncAddressErrorCounter(configured_connection_id));
    EXPECT_NO_THROW(srdia_IncTypeErrorCounter(configured_connection_id));
    EXPECT_NO_THROW(srdia_IncSequenceNumberErrorCounter(configured_connection_id));
    EXPECT_NO_THROW(srdia_IncConfirmedSequenceNumberErrorCounter(configured_connection_id));
    EXPECT_NO_THROW(srdia_UpdateConnectionDiagnostics(configured_connection_id, UT_SRDIA_T_MAX, UT_SRDIA_T_MAX));

    EXPECT_NO_THROW(srdia_SendDiagnosticNotification(configured_connection_id));
    EXPECT_EQ(diag_noti.ec_safety, 1U);
    EXPECT_EQ(diag_noti.ec_address, 1U);
    EXPECT_EQ(diag_noti.ec_type, 1U);
    EXPECT_EQ(diag_noti.ec_sn, 1U);
    EXPECT_EQ(diag_noti.ec_csn, 1U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[0], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[1], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[2], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[3], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[4], 1U); // from UT_SRDIA_T_MAX
    EXPECT_EQ(diag_noti.t_alive_distribution[0], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[1], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[2], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[3], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[4], 1U); // from UT_SRDIA_T_MAX

    EXPECT_NO_THROW(srdia_IncSafetyCodeErrorCounter(configured_connection_id));
    EXPECT_NO_THROW(srdia_IncAddressErrorCounter(configured_connection_id));
    EXPECT_NO_THROW(srdia_IncTypeErrorCounter(configured_connection_id));
    EXPECT_NO_THROW(srdia_IncSequenceNumberErrorCounter(configured_connection_id));
    EXPECT_NO_THROW(srdia_IncConfirmedSequenceNumberErrorCounter(configured_connection_id));
    EXPECT_NO_THROW(srdia_UpdateConnectionDiagnostics(configured_connection_id, UT_SRDIA_T_MIN, UT_SRDIA_T_MIN));
    EXPECT_NO_THROW(srdia_UpdateConnectionDiagnostics(configured_connection_id, UT_SRDIA_T_MAX, UT_SRDIA_T_MAX));

    EXPECT_NO_THROW(srdia_InitConnectionDiagnostics(configured_connection_id));

    EXPECT_NO_THROW(srdia_SendDiagnosticNotification(configured_connection_id));
    EXPECT_EQ(diag_noti.ec_safety, 0U);
    EXPECT_EQ(diag_noti.ec_address, 0U);
    EXPECT_EQ(diag_noti.ec_type, 0U);
    EXPECT_EQ(diag_noti.ec_sn, 0U);
    EXPECT_EQ(diag_noti.ec_csn, 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[0], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[1], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[2], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[3], 0U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[4], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[0], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[1], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[2], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[3], 0U);
    EXPECT_EQ(diag_noti.t_alive_distribution[4], 0U);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitConnectionDiagnosticsFunction,
  InitConnectionDiagnosticsTestWithParam,
  ::testing::Values(
//                    Configured connection ID
//                    |   Expect fatal error
//                    |   |
      std::make_tuple(0U, false),
      std::make_tuple(1U, false),
      std::make_tuple(2U, true)
  )
);

/**
 * @test        @ID{srdiaTest003} Verify second init fails
 *
 * @details     This test verifies that if the init function is called twice, an exception
 *              is thrown.
 *
 * Test steps:
 * - Call the init function
 * - Call the init function again
 * - Check that an exception is thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-642} Init sr_diagnostics Function
 */
TEST_F(srdiaTest, srdiaTest003VerifySecondInitFails) {
  // set expectations /////////////////////////////
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized))
      .Times(1)
      .WillRepeatedly(Invoke(this, &srdiaTest::invalidArgumentException))
  ;

  // perform test /////////////////////////////////
  EXPECT_NO_THROW(srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, diag_timings_distribution_intervals));
  EXPECT_THROW(srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, diag_timings_distribution_intervals), std::invalid_argument);
}

/**
 * @test        @ID{srdiaTest004} Verify initialisation check
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialisation of the module.
 *
 * Test steps:
 * - Call the srdia_IncSafetyCodeErrorCounter function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - Call the srdia_IncAddressErrorCounter function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - Call the srdia_IncTypeErrorCounter function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - Call the srdia_IncSequenceNumberErrorCounter function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - Call the srdia_IncConfirmedSequenceNumberErrorCounter function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - Call the srdia_UpdateConnectionDiagnostics function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - Call the srdia_SendDiagnosticNotification function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-636} Component sr_diagnostics Overview
 * @verifyReq{RASW-637} Inc Address Error Counter Function
 * @verifyReq{RASW-638} Inc Confirmed Sequence Number Error Counter Function
 * @verifyReq{RASW-639} Inc Safety Code Error Counter Function
 * @verifyReq{RASW-640} Inc Sequence Number Error Counter Function
 * @verifyReq{RASW-641} Inc Type Error Counter Function
 * @verifyReq{RASW-644} Send Diagnostic Notification Function
 * @verifyReq{RASW-645} Update Connection Diagnostics Function
 * @verifyReq{RASW-643} Init Connection Diagnostics Function
 */
TEST_F(srdiaTest, srdiaTest004VerifyInitialisationCheck) {
  // set expectations /////////////////////////////
  const uint32_t nr_of_function_calls = 8U;
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
      .Times(nr_of_function_calls)
      .WillRepeatedly(Invoke(this, &srdiaTest::invalidArgumentException))
  ;

  // perform test /////////////////////////////////
  /* call the functions without initializing the module */
  const uint32_t connection_id = 0U;
  EXPECT_THROW(srdia_IncSafetyCodeErrorCounter(connection_id), std::invalid_argument);
  EXPECT_THROW(srdia_IncAddressErrorCounter(connection_id), std::invalid_argument);
  EXPECT_THROW(srdia_IncTypeErrorCounter(connection_id), std::invalid_argument);
  EXPECT_THROW(srdia_IncSequenceNumberErrorCounter(connection_id), std::invalid_argument);
  EXPECT_THROW(srdia_IncConfirmedSequenceNumberErrorCounter(connection_id), std::invalid_argument);
  EXPECT_THROW(srdia_UpdateConnectionDiagnostics(connection_id, 0U, 0U), std::invalid_argument);
  EXPECT_THROW(srdia_SendDiagnosticNotification(connection_id), std::invalid_argument);
  EXPECT_THROW(srdia_InitConnectionDiagnostics(connection_id), std::invalid_argument);
}

/**
 * @test        @ID{srdiaTest010} Verify update connection diagnostics function
 *
 * @details     This test verifies that the update connection diagnostics function
 *              sorts the round trip and alive time into the correct bins.
 *
 * Test steps:
 * - Call the init function with distribution intervals set to: 199, 399, 599, 799
 * - Update the connection diagnostics 10 times with a fixed round trip/alive time
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Produce a diagnostic notification to check that all bins were set correctly
 * .
 *
 * @testParameter
 * - Configured connection Id: RaSTA connection identification
 * - Round trip delay: Round trip delay (T_rtd) of a received message in ms (valid range: 0 - 2000)
 * - Alive time: Alive time (T_alive) of a received message in ms (valid range: 0 - 2000)
 * - Nr of updates: Number of updates
 * - Round trip delay bin: In which bin the given round trip delay lands
 * - Alive time bin: In which bin the given alive time lands
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                        ||| Test config   | Expected values                                                              ||||
 * |----------|--------------------------|------------------|------------|---------------|----------------------|----------------|--------------------|--------------------|
 * |          | Configured connection Id | Round trip delay | Alive time | Nr of updates | Round trip delay bin | Alive time bin | Expect fatal error | Test Result        |
 * |          | vary configured_connection_id                                                                                                   |||||||                    |
 * | 0        | 0                        | 0                | 0          | 10            | 0                    | 0              | False              | Normal operation   |
 * | 1        | 1                        | 0                | 0          | 10            | 0                    | 0              | False              | Normal operation   |
 * | 2        | 2                        | 0                | 0          | 10            | 0                    | 0              | True               | Fatal error raised |
 * |          | vary nr_of_updates                                                                                                              |||||||                    |
 * | 3        | 0                        | 0                | 0          | 10000         | 0                    | 0              | False              | Normal operation   |
 * |          | vary round_trip_delay                                                                                                           |||||||                    |
 * | 4        | 0                        | 200              | 0          | 10            | 1                    | 0              | False              | Normal operation   |
 * | 5        | 0                        | 399              | 0          | 10            | 1                    | 0              | False              | Normal operation   |
 * | 6        | 0                        | 400              | 0          | 10            | 2                    | 0              | False              | Normal operation   |
 * | 7        | 0                        | 599              | 0          | 10            | 2                    | 0              | False              | Normal operation   |
 * | 8        | 0                        | 600              | 0          | 10            | 3                    | 0              | False              | Normal operation   |
 * | 9        | 0                        | 799              | 0          | 10            | 3                    | 0              | False              | Normal operation   |
 * | 10       | 0                        | 800              | 0          | 10            | 4                    | 0              | False              | Normal operation   |
 * | 11       | 0                        | 2000             | 0          | 10            | 4                    | 0              | False              | Normal operation   |
 * | 12       | 0                        | 2001             | 0          | 10            | 4                    | 0              | True               | Fatal error raised |
 * |          | vary alive_time                                                                                                                 |||||||                    |
 * | 13       | 0                        | 0                | 200        | 10            | 0                    | 1              | False              | Normal operation   |
 * | 14       | 0                        | 0                | 399        | 10            | 0                    | 1              | False              | Normal operation   |
 * | 15       | 0                        | 0                | 400        | 10            | 0                    | 2              | False              | Normal operation   |
 * | 16       | 0                        | 0                | 599        | 10            | 0                    | 2              | False              | Normal operation   |
 * | 17       | 0                        | 0                | 600        | 10            | 0                    | 3              | False              | Normal operation   |
 * | 18       | 0                        | 0                | 799        | 10            | 0                    | 3              | False              | Normal operation   |
 * | 19       | 0                        | 0                | 800        | 10            | 0                    | 4              | False              | Normal operation   |
 * | 20       | 0                        | 0                | 2000       | 10            | 0                    | 4              | False              | Normal operation   |
 * | 21       | 0                        | 0                | 2001       | 10            | 0                    | 4              | True               | Fatal error raised |
 * |          | vary round_trip_delay and alive_time together                                                                                   |||||||                    |
 * | 22       | 0                        | 200              | 200        | 10            | 1                    | 1              | False              | Normal operation   |
 * | 23       | 0                        | 399              | 399        | 10            | 1                    | 1              | False              | Normal operation   |
 * | 24       | 0                        | 400              | 400        | 10            | 2                    | 2              | False              | Normal operation   |
 * | 25       | 0                        | 599              | 599        | 10            | 2                    | 2              | False              | Normal operation   |
 * | 26       | 0                        | 600              | 600        | 10            | 3                    | 3              | False              | Normal operation   |
 * | 27       | 0                        | 799              | 799        | 10            | 3                    | 3              | False              | Normal operation   |
 * | 28       | 0                        | 800              | 800        | 10            | 4                    | 4              | False              | Normal operation   |
 * | 29       | 0                        | 2000             | 2000       | 10            | 4                    | 4              | False              | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-321} Connection Diagnostic Data
 * @verifyReq{RASW-326} Connection Identification
 * @verifyReq{RASW-327} SafRetL Diagnostic Notification Structure
 * @verifyReq{RASW-433} Diagnostic Timing Interval
 * @verifyReq{RASW-470} Struct Connection Diagnostic Data Structure
 * @verifyReq{RASW-477} Alive Time Distribution
 * @verifyReq{RASW-478} Round Trip Delay Time Distribution
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-556} Sr Diagnostic Notification
 * @verifyReq{RASW-636} Component sr_diagnostics Overview
 * @verifyReq{RASW-644} Send Diagnostic Notification Function
 * @verifyReq{RASW-645} Update Connection Diagnostics Function
 */
TEST_P(UpdateConnectionDiagnosticsTestWithParam, srdiaTest010VerifyUpdateConnectionDiagnosticsFunction) {
  // test variables /////////////////////////////
  const uint32_t timings_distribution_intervals[4] = {
    199,
    399,
    599,
    799,
  };

  // set expectations /////////////////////////////
  const uint32_t clipped_connection_id = configured_connection_id <= UT_SRDIA_CONFIGURED_CONNECTIONS_MAX - 1U ? configured_connection_id : UT_SRDIA_CONFIGURED_CONNECTIONS_MAX - 1U;
  bool expect_diagnosis_window_overflow = nr_of_updates >= UT_SRDIA_N_DIAG_WINDOW_MAX ? true : false;

  if (expect_fatal_error) {
    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .Times(nr_of_updates)
        .WillRepeatedly(Invoke(this, &srdiaTest::invalidArgumentException))
    ;
  }
  else
  {
    if(expect_diagnosis_window_overflow)
    {
      EXPECT_CALL(srnot_mock, srnot_SrDiagnosticNotification(configured_connection_id, _)).Times(2)
        .WillRepeatedly(SaveArg<1>(&diag_noti))
      ;
    }
    else
    {
      EXPECT_CALL(srnot_mock, srnot_SrDiagnosticNotification(configured_connection_id, _))
        .WillOnce(SaveArg<1>(&diag_noti))
      ;
    }
  }

  // perform test /////////////////////////////////
  srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, timings_distribution_intervals);

  memset(&diag_noti, 0U, sizeof(diag_noti));

  if (expect_fatal_error) {
    for (uint32_t update_index = 0U; update_index < nr_of_updates; ++update_index) {
      EXPECT_THROW(srdia_UpdateConnectionDiagnostics(configured_connection_id, round_trip_delay, alive_time), std::invalid_argument);
    }
  }
  else {
    for (uint32_t update_index = 0U; update_index < nr_of_updates; ++update_index) {
      EXPECT_NO_THROW(srdia_UpdateConnectionDiagnostics(configured_connection_id, round_trip_delay, alive_time));
    }

    srdia_SendDiagnosticNotification(clipped_connection_id);
    for (uint32_t bin_index = 0U; bin_index < UT_SRDIA_NR_OF_BINS; ++bin_index) {
      if (bin_index == round_trip_delay_bin && !expect_diagnosis_window_overflow) {
        EXPECT_EQ(diag_noti.t_rtd_distribution[bin_index], nr_of_updates);
      }
      else {
        EXPECT_EQ(diag_noti.t_rtd_distribution[bin_index], 0U);
      }

      if (bin_index == alive_time_bin && !expect_diagnosis_window_overflow) {
        EXPECT_EQ(diag_noti.t_alive_distribution[bin_index], nr_of_updates);
      }
      else {
        EXPECT_EQ(diag_noti.t_alive_distribution[bin_index], 0U);
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyUpdateConnectionDiagnosticsFunction,
  UpdateConnectionDiagnosticsTestWithParam,
  ::testing::Values(
//                    Configured connection ID
//                    |   Number of updates
//                    |   |       Round trip delay
//                    |   |       |                     Round trip delay bin
//                    |   |       |                     |       Alive time
//                    |   |       |                     |       |                    Alive time bin
//                    |   |       |                     |       |                    |       Expect fatal error
//                    |   |       |                     |       |                    |       |
      // vary configured_connection_id:
      std::make_tuple(0U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B0_MIN,      0U,     false),
      std::make_tuple(1U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B0_MIN,      0U,     false),
      std::make_tuple(2U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B0_MIN,      0U,     true),

      // vary nr_of_updates:
      std::make_tuple(0U, 10000U, UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B0_MIN,      0U,     false),

      // vary round_trip_delay:
      std::make_tuple(0U, 10U,    UT_SRDIA_B1_MIN,      1U,     UT_SRDIA_B0_MIN,      0U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B1_MAX,      1U,     UT_SRDIA_B0_MIN,      0U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B2_MIN,      2U,     UT_SRDIA_B0_MIN,      0U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B2_MAX,      2U,     UT_SRDIA_B0_MIN,      0U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B3_MIN,      3U,     UT_SRDIA_B0_MIN,      0U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B3_MAX,      3U,     UT_SRDIA_B0_MIN,      0U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B4_MIN,      4U,     UT_SRDIA_B0_MIN,      0U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B4_MAX,      4U,     UT_SRDIA_B0_MIN,      0U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B4_MAX + 1U, 4U,     UT_SRDIA_B0_MIN,      0U,     true),

      // vary alive_time:
      std::make_tuple(0U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B1_MIN,      1U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B1_MAX,      1U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B2_MIN,      2U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B2_MAX,      2U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B3_MIN,      3U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B3_MAX,      3U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B4_MIN,      4U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B4_MAX,      4U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B0_MIN,      0U,     UT_SRDIA_B4_MAX + 1U, 4U,     true),

      // vary round_trip_delay and alive_time together:
      std::make_tuple(0U, 10U,    UT_SRDIA_B1_MIN,      1U,     UT_SRDIA_B1_MIN,      1U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B1_MAX,      1U,     UT_SRDIA_B1_MAX,      1U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B2_MIN,      2U,     UT_SRDIA_B2_MIN,      2U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B2_MAX,      2U,     UT_SRDIA_B2_MAX,      2U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B3_MIN,      3U,     UT_SRDIA_B3_MIN,      3U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B3_MAX,      3U,     UT_SRDIA_B3_MAX,      3U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B4_MIN,      4U,     UT_SRDIA_B4_MIN,      4U,     false),
      std::make_tuple(0U, 10U,    UT_SRDIA_B4_MAX,      4U,     UT_SRDIA_B4_MAX,      4U,     false)
  )
);

/**
 * @test        @ID{srdiaTest011} Verify send diagnostics notification function
 *
 * @details     This test verifies that the send diagnostics notification function
 *              triggers a notification
 *
 * Test steps:
 * - Call the init function
 * - Increment the safety code error counter
 * - Expect a radef_kInvalidParameter when fatal error gets thrown
 * - Set data to connection diagnostics
 * - Produce a diagnostic notification
 * - Verify diagnostic data reset
 * .
 *
 * @testParameter
 * - Configured connection Id: RaSTA connection identification
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter          | Expected values                        ||
 * |----------|--------------------------|--------------------|--------------------|
 * |          | Configured connection Id | Expect fatal error | Test Result        |
 * | 0        | 0                        | False              | Normal operation   |
 * | 1        | 1                        | False              | Normal operation   |
 * | 2        | 2                        | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-321} Connection Diagnostic Data
 * @verifyReq{RASW-326} Connection Identification
 * @verifyReq{RASW-327} SafRetL Diagnostic Notification Structure
 * @verifyReq{RASW-433} Diagnostic Timing Interval
 * @verifyReq{RASW-468} EC safety
 * @verifyReq{RASW-470} Struct Connection Diagnostic Data Structure
 * @verifyReq{RASW-477} Alive Time Distribution
 * @verifyReq{RASW-478} Round Trip Delay Time Distribution
 * @verifyReq{RASW-479} EC CSN
 * @verifyReq{RASW-480} EC SN
 * @verifyReq{RASW-481} EC type
 * @verifyReq{RASW-482} EC address
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-556} Sr Diagnostic Notification
 * @verifyReq{RASW-636} Component sr_diagnostics Overview
 * @verifyReq{RASW-639} Inc Safety Code Error Counter Function
 * @verifyReq{RASW-644} Send Diagnostic Notification Function
 */
TEST_P(SendDiagnosticNotificationTestWithParam, srdiaTest011VerifySendDiagnosticNotificationFunction) {
  // set expectations /////////////////////////////
  const uint32_t clipped_connection_id = configured_connection_id <= UT_SRDIA_CONFIGURED_CONNECTIONS_MAX - 1U ? configured_connection_id : UT_SRDIA_CONFIGURED_CONNECTIONS_MAX - 1U;

  if (expect_fatal_error) {
    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .WillOnce(Invoke(this, &srdiaTest::invalidArgumentException))
    ;
  }
  else {
    EXPECT_CALL(srnot_mock, srnot_SrDiagnosticNotification(clipped_connection_id, _))
        .WillOnce(SaveArg<1>(&diag_noti))
    ;
  }

  // perform test /////////////////////////////////
  srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, diag_timings_distribution_intervals);

  memset(&diag_noti, 0U, sizeof(diag_noti));

  srdia_IncSafetyCodeErrorCounter(clipped_connection_id);

  if (expect_fatal_error) {
    EXPECT_THROW(srdia_SendDiagnosticNotification(configured_connection_id), std::invalid_argument);
  }
  else {
    // Set data to connection diagnostics
    srdia_connection_diagnostics[configured_connection_id].message_counter = 1U;
    srdia_connection_diagnostics[configured_connection_id].diagnostic_data.ec_safety = 1U;
    srdia_connection_diagnostics[configured_connection_id].diagnostic_data.ec_address = 1U;
    srdia_connection_diagnostics[configured_connection_id].diagnostic_data.ec_type = 1U;
    srdia_connection_diagnostics[configured_connection_id].diagnostic_data.ec_sn = 1U;
    srdia_connection_diagnostics[configured_connection_id].diagnostic_data.ec_csn = 1U;

    for (uint32_t index = 0U; index < UT_SRDIA_DIAG_TIMINGS_DIST_INTERVALS; ++index) {
      srdia_connection_diagnostics[configured_connection_id].diagnostic_data.t_alive_distribution[index] = 1U;
      srdia_connection_diagnostics[configured_connection_id].diagnostic_data.t_rtd_distribution[index] = 1U;
    }

    EXPECT_NO_THROW(srdia_SendDiagnosticNotification(configured_connection_id));

    EXPECT_EQ(diag_noti.ec_safety, 1U);
    EXPECT_EQ(diag_noti.ec_address, 1U);
    EXPECT_EQ(diag_noti.ec_type, 1U);
    EXPECT_EQ(diag_noti.ec_sn, 1U);
    EXPECT_EQ(diag_noti.ec_csn, 1U);

    EXPECT_EQ(diag_noti.t_rtd_distribution[0], 1U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[1], 1U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[2], 1U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[3], 1U);
    EXPECT_EQ(diag_noti.t_rtd_distribution[4], 1U);
    EXPECT_EQ(diag_noti.t_alive_distribution[0], 1U);
    EXPECT_EQ(diag_noti.t_alive_distribution[1], 1U);
    EXPECT_EQ(diag_noti.t_alive_distribution[2], 1U);
    EXPECT_EQ(diag_noti.t_alive_distribution[3], 1U);
    EXPECT_EQ(diag_noti.t_alive_distribution[4], 1U);

    // Verify diagnostic data reset
    EXPECT_EQ(srdia_connection_diagnostics[configured_connection_id].message_counter, 0U);
    EXPECT_EQ(srdia_connection_diagnostics[configured_connection_id].diagnostic_data.ec_safety, 0U);
    EXPECT_EQ(srdia_connection_diagnostics[configured_connection_id].diagnostic_data.ec_address, 0U);
    EXPECT_EQ(srdia_connection_diagnostics[configured_connection_id].diagnostic_data.ec_type, 0U);
    EXPECT_EQ(srdia_connection_diagnostics[configured_connection_id].diagnostic_data.ec_sn, 0U);
    EXPECT_EQ(srdia_connection_diagnostics[configured_connection_id].diagnostic_data.ec_csn, 0U);

    for (uint32_t index = 0U; index < UT_SRDIA_DIAG_TIMINGS_DIST_INTERVALS; ++index) {
      EXPECT_EQ(srdia_connection_diagnostics[configured_connection_id].diagnostic_data.t_alive_distribution[index], 0U);
      EXPECT_EQ(srdia_connection_diagnostics[configured_connection_id].diagnostic_data.t_rtd_distribution[index], 0U);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifySendDiagnosticNotificationFunction,
  SendDiagnosticNotificationTestWithParam,
  ::testing::Values(
//                    Configured connection ID
//                    |   Expect fatal error
//                    |   |
      // vary configured_connection_id:
      std::make_tuple(0U, false),
      std::make_tuple(1U, false),
      std::make_tuple(2U, true)
  )
);

/**
 * @test        @ID{srdiaTest012} Verify the AreDiagnosticTimingIntervalsValid function
 *
 * @details     This test verifies the AreDiagnosticTimingIntervalsValid function
 *
 * Test steps:
 * - Set the fatal error expectation for radef_kInvalidParameter based on the test parameters
 * - Call the AreDiagnosticTimingIntervalsValid function with given test parameter and verify its return value
 * .
 *
 * @testParameter
 * - T max: Configured maximal accepted age of a message in miliseconds (valid range: 750 - 2000)
 * - timing distr interval 1: Configured value for timing distribution interval 1 in miliseconds
 * - timing distr interval 2: Configured value for timing distribution interval 2 in miliseconds
 * - timing distr interval 3: Configured value for timing distribution interval 3 in miliseconds
 * - timing distr interval 4: Configured value for timing distribution interval 4 in miliseconds
 * - Expected return value: Indicates if the diagnostics timing intervals are expected to be valid
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                                                                           ||||| Expected values                                               |||
 * |----------|-------|-------------------------|-------------------------|-------------------------|-------------------------|-----------------------|--------------------|--------------------|
 * |          | T max | timing distr interval 1 | timing distr interval 2 | timing distr interval 3 | timing distr interval 4 | Expected return value | Expect fatal error | Test Result        |
 * | 0        | 750   | 0                       | 747           	        | 748                     | 749                     | false                 | false              | Normal operation   |
 * | 1        | 750   | 746                     | 747         	          | 748                     | 749                     | true                  | false              | Normal operation   |
 * | 2        | 2000  | 1996       	            | 1997         	          | 1998          	        | 1999          	        | true                  | false              | Normal operation   |
 * | 3        | 2000  | 400         	          | 500         	          | 667          	          | 1000          	        | true                  | false              | Normal operation   |
 * | 4        | 2000  | 1996        	          | 1997         	          | 1998          	        | 2000          	        | false                 | false              | Normal operation   |
 * | 5        | 2000  | 1996        	          | 1997         	          | 1999         	          | 1999          	        | false                 | false              | Normal operation   |
 * | 6        | 2000  | 1996        	          | 1998         	          | 1998          	        | 1999         	          | false                 | false              | Normal operation   |
 * | 7        | 2000  | 1997        	          | 1997         	          | 1998          	        | 1999          	        | false                 | false              | Normal operation   |
 * | 8        | 749   | 745         	          | 746          	          | 747          	          | 748         	          | false                 | true               | Fatal error raised |
 * | 9        | 2001  | 1996         	          | 1997         	          | 1998          	        | 1999          	        | false                 | true               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-817} Are Diagnostic Timing Intervals Valid Function
 */
TEST_P(AreDiagnosticTimingIntervalsValidTestWithParam, srdiaTest012VerifyAreDiagnosticTimingIntervalsValidFunction) {
  const uint32_t timings_distribution_intervals[UT_SRDIA_DIAG_TIMINGS_DIST_INTERVALS] = {
    timings_distribution_interval_1,
    timings_distribution_interval_2,
    timings_distribution_interval_3,
    timings_distribution_interval_4
  };

  // set expectations /////////////////////////////
  if (expect_fatal_error) {
    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .WillOnce(Invoke(this, &srdiaTest::invalidArgumentException))
    ;
  }

  // perform test /////////////////////////////////
  if (expect_fatal_error) {
    // call the init function and expect a throw
    EXPECT_THROW(srdia_AreDiagnosticTimingIntervalsValid(t_max, timings_distribution_intervals), std::invalid_argument);
  }
  else {
    // call the init function and verify if parameter are set correctly
    EXPECT_EQ(expect_to_be_valid, srdia_AreDiagnosticTimingIntervalsValid(t_max, timings_distribution_intervals));
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyAreDiagnosticTimingIntervalsValidFunction,
  AreDiagnosticTimingIntervalsValidTestWithParam,
  ::testing::Values(
//                  T max
//                  |                     Timing distribution interval 1
//                  |                     |                     Timing distribution interval 2
//                  |                     |                     |                     Timing distribution interval 3
//                  |                     |                     |                     |                           Timing distribution interval 4
//                  |                     |                     |                     |                           |                     Expect to be valid
//                  |                     |                     |                     |                           |                     |      Expect fatal error
//                  |                     |                     |                     |                           |                     |      |
    std::make_tuple(UT_SRDIA_T_MIN,       0U,                   (UT_SRDIA_T_MIN - 3), (UT_SRDIA_T_MIN - 2),       (UT_SRDIA_T_MIN - 1), false, false),
    std::make_tuple(UT_SRDIA_T_MIN,       (UT_SRDIA_T_MIN - 4), (UT_SRDIA_T_MIN - 3), (UT_SRDIA_T_MIN - 2),       (UT_SRDIA_T_MIN - 1), true,  false),
    std::make_tuple(UT_SRDIA_T_MAX,       (UT_SRDIA_T_MAX - 4), (UT_SRDIA_T_MAX - 3), (UT_SRDIA_T_MAX - 2),       (UT_SRDIA_T_MAX - 1), true,  false),
    std::make_tuple(UT_SRDIA_T_MAX,       (UT_SRDIA_T_MAX / 5), (UT_SRDIA_T_MAX / 4), ((UT_SRDIA_T_MAX + 1) / 3), (UT_SRDIA_T_MAX / 2), true,  false),
    std::make_tuple(UT_SRDIA_T_MAX,       (UT_SRDIA_T_MAX - 4), (UT_SRDIA_T_MAX - 3), (UT_SRDIA_T_MAX - 2),       UT_SRDIA_T_MAX,       false, false),
    std::make_tuple(UT_SRDIA_T_MAX,       (UT_SRDIA_T_MAX - 4), (UT_SRDIA_T_MAX - 3), (UT_SRDIA_T_MAX - 1),       (UT_SRDIA_T_MAX - 1), false, false),
    std::make_tuple(UT_SRDIA_T_MAX,       (UT_SRDIA_T_MAX - 4), (UT_SRDIA_T_MAX - 2), (UT_SRDIA_T_MAX - 2),       (UT_SRDIA_T_MAX - 1), false, false),
    std::make_tuple(UT_SRDIA_T_MAX,       (UT_SRDIA_T_MAX - 3), (UT_SRDIA_T_MAX - 3), (UT_SRDIA_T_MAX - 2),       (UT_SRDIA_T_MAX - 1), false, false),
    std::make_tuple((UT_SRDIA_T_MIN - 1), (UT_SRDIA_T_MIN - 5), (UT_SRDIA_T_MIN - 4), (UT_SRDIA_T_MIN - 3),       (UT_SRDIA_T_MIN - 2), false, true),
    std::make_tuple((UT_SRDIA_T_MAX + 1), (UT_SRDIA_T_MAX - 4), (UT_SRDIA_T_MAX - 3), (UT_SRDIA_T_MAX - 2),       (UT_SRDIA_T_MAX - 1), false, true)
  )
);

/**
 * @test        @ID{srdiaTest013} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - call the srdia_Init function with invalid message pointer and verify that a radef_kInvalidParameter error gets thrown
 * - call the srdia_AreDiagnosticTimingIntervalsValid function with invalid message pointer and verify that a radef_kInvalidParameter error gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-642} Init sr_diagnostics Function
 * @verifyReq{RASW-817} Are Diagnostic Timing Intervals Valid Function
 */
TEST_F(srdiaTest, srdiaTest013VerifyNullPointerCheck) {
  // set expectations /////////////////////////////
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
      .Times(2)
      .WillRepeatedly(Invoke(this, &srdiaTest::invalidArgumentException))
  ;

  // perform test /////////////////////////////////
  EXPECT_THROW(srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, (uint32_t *)NULL), std::invalid_argument);

  EXPECT_NO_THROW(srdia_Init(UT_SRDIA_CONFIGURED_CONNECTIONS_MAX, UT_SRDIA_T_MAX, UT_SRDIA_N_DIAG_WINDOW_MAX, diag_timings_distribution_intervals));
  EXPECT_THROW(srdia_AreDiagnosticTimingIntervalsValid(UT_SRDIA_T_MAX, (uint32_t *)NULL), std::invalid_argument);
}

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

